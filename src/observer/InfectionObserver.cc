#include "InfectionObserver.h"

Define_Module(InfectionObserver);

omnetpp::simsignal_t InfectionObserver::status_signal = 
  registerSignal("lastStatus");

omnetpp::simsignal_t InfectionObserver::infected_node_stat =
  registerSignal("infectedNodes");

omnetpp::simsignal_t InfectionObserver::rho_stat =
  registerSignal("rho");

omnetpp::simsignal_t InfectionObserver::neighborhood_notification_signal =
  registerSignal("neighborhood");


InfectionObserver::~InfectionObserver() {
  cancelAndDelete(recovery_timer);
  getSimulation()->getSystemModule()->unsubscribe(
    neighborhood_notification_signal,
    this
  );
  getSimulation()->getSystemModule()->unsubscribe(status_signal, this);
}

void InfectionObserver::initialize(int stage) {
  static inet::ProbabilisticBroadcast* net_protocol(nullptr);
  static InformationPropagationApp* app_module(nullptr);
  if (stage == inet::INITSTAGE_LOCAL) {
    epsilon = par("epsilon");
    round_num = par("roundNumber").intValue();
    trial_num = par("lambda");
    step_time = par("unitTime"); //TODO: update round
    step_time *= trial_num;
    host_num = par("hostNumber").intValue();
    p = std::make_unique< std::vector<InformationPropagationApp::Status> >(host_num, InformationPropagationApp::NOT_INFECTED);
    q = std::make_unique < std::vector<double> >(host_num, 1.0);
    next_p = std::make_unique < std::vector<double> >(host_num, InformationPropagationApp::NOT_INFECTED);
    adjacency_matrix = std::make_unique <
      std::vector<std::shared_ptr< const std::list<cache_register> > > 
    >(host_num);
    getSimulation()->getSystemModule()->subscribe(status_signal, this);
    getSimulation()->getSystemModule()->subscribe(
      neighborhood_notification_signal,
      this
    );
    net_protocol = (inet::ProbabilisticBroadcast*) getSimulation()
      ->getSystemModule()->getSubmodule("node", 0)->getSubmodule("net")->getSubmodule("np");
    app_module = (InformationPropagationApp*) getSimulation()
      ->getSystemModule()->getSubmodule("node", 0)->getSubmodule("app");
    recovery_timer = new omnetpp::cMessage("Step Timer");
    // Nodes emit their status before the observer compute probabilities
    recovery_timer->setSchedulingPriority(10);
    WATCH(infected_num);
    WATCH(rho);
    WATCH(mu);
    WATCH(beta);
  }
  else if(stage == inet::INITSTAGE_APPLICATION_LAYER) {
    beta = net_protocol->par("beta").doubleValue();
    mu = app_module->par("recoveryProbability").doubleValue();
    auto t0 = omnetpp::simTime() + getSimulation()->getWarmupPeriod();
    scheduleAt(t0, recovery_timer);
  }
}

void InfectionObserver::receiveSignal(
  omnetpp::cComponent* src,   //@param the module emitting the signal 
  omnetpp::simsignal_t id,    //@param the signal id
  long value,                 //@param the node status {INFECTED, NOT_INFECTED}
  omnetpp::cObject* details   //@param details about the object
) {
  unsigned host_id = src->getParentModule()->getIndex();
  InformationPropagationApp::Status status =  
    static_cast<InformationPropagationApp::Status>(value);
  p->at(host_id) = status;
  
  Enter_Method(
    "Receiving status %s from host[%d]", InformationPropagationApp::status_to_string(status),
    host_id
  );
  infected_num = std::accumulate(p->begin(), p->end(), 0.0);
  rho = double(infected_num) / host_num;
}

void InfectionObserver::receiveSignal(
  omnetpp::cComponent* src,   //@param the module emitting the signal 
  omnetpp::simsignal_t id,    //@param the signal id
  omnetpp::cObject* obj,      //@param the node neighborhood
  omnetpp::cObject* details   //@param details about the object
) {
  //hello->netlayer->node
  unsigned host_id = src->getParentModule()->getParentModule()->getIndex();
  auto notification_ptr = dynamic_cast<NeighborhoodNotificacion*>(obj);
  auto neighborhood = notification_ptr->neighborhood;
  adjacency_matrix->at(host_id) = neighborhood;
  Enter_Method("Receiving neighborhood from host[%d]", host_id);
}

double InfectionObserver::compute_rho() {
  //(1−q_i(t))(1−p_i(t)) + (1−μ)p_i(t) + μ(1−q_i(t))p_i(t)
  //q_i (t)= ∏24_(j=1)^N▒(1−βr_ij p_j (t)) 

  for (size_t i = 0; i < host_num; i++) {
    (*q)[i] = 1.0; //not infected by a neighbor
    for (auto&& entry : *(adjacency_matrix->at(i)))
      (*q)[i] *= (1 - beta * (*p)[entry.node_index]);
    (*next_p)[i]                         //p(t+1)
      = (1 - (*q)[i]) * (1 - (*p)[i])    //infected by nbr & not infected
      + (1 - mu) * (*p)[i]               //not recovery & infected
      + mu * (1 - (*q)[i]) * (*p)[i];    //infected after recovery
    EV_INFO << "next_p[" << i << "] = " << (*next_p)[i] << '\n';
  }
  return std::accumulate(next_p->begin(), next_p->end(), 0.0) / host_num; //rho
}

void InfectionObserver::handleMessage(omnetpp::cMessage* msg) {
  if (msg->isSelfMessage()) {
    infected_num = std::accumulate(p->begin(), p->end(), 0.0);
    double new_rho = double(infected_num) / host_num;
    if (fabs(new_rho - rho) < epsilon) {
      round_counter++; // Increments rounds in possible stationaty state
      EV_INFO << "Round number (steady state): " << round_counter << '\n';
    }
    rho = new_rho;
    emit(infected_node_stat, infected_num);
    emit(rho_stat, rho);
    EV_INFO << "Number of infected nodes: " << infected_num << '\n';
    EV_INFO << "Expected infection density: " << rho << '\n';
    scheduleAt(omnetpp::simTime() + step_time, recovery_timer);
    // bool stop_condition = (round_counter > round_num) || (rho == 0.0);
    if (round_counter > round_num)
      endSimulation();
  }
  else 
    throw omnetpp::cRuntimeError(
      "InfectionObserver: This module does not process messages from any module\n"
    );
}