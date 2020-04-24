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
  getSimulation()->getSystemModule()->unsubscribe(
    neighborhood_notification_signal,
    this
  );
  getSimulation()->getSystemModule()->unsubscribe(status_signal, this);
}

void InfectionObserver::initialize(int stage) {
  if (stage == inet::INITSTAGE_LOCAL) {
    epsilon = par("epsilon").doubleValue();
    beta = par("beta").doubleValue();
    mu = par("mu").doubleValue();
    host_num = par("hostNumber").intValue();
    p = std::make_unique< std::vector<unsigned> >(host_num);
    q = std::make_unique < std::vector<double> >(host_num);
    next_p = std::make_unique < std::vector<double> >(host_num);
    adjacency_matrix = std::make_unique <
      std::vector<std::shared_ptr< const std::list<cache_register> > > 
    >(host_num);
    getSimulation()->getSystemModule()->subscribe(status_signal, this);
    getSimulation()->getSystemModule()->subscribe(
      neighborhood_notification_signal,
      this
    );
    WATCH(infected_num);
    WATCH(rho);
  }
}

void InfectionObserver::receiveSignal(
  omnetpp::cComponent* src,   //@param the module emitting the signal 
  omnetpp::simsignal_t id,    //@param the signal id
  long value,                 //@param the node status {INFECTED, NOT_INFECTED}
  omnetpp::cObject* details   //@param details about the object
) {
  static unsigned counter = 0;
  unsigned host_id = (dynamic_cast<omnetpp::cModule*>(src))->getIndex();
  BroadcastInfectionApp::Status status = static_cast<BroadcastInfectionApp::Status>(value);
  p->at(host_id) = status;
  if (status == BroadcastInfectionApp::Status::INFECTED)
    infected_num++;
  if (counter < host_num - 1) //number of received signals is lt host_num - 1
    counter++;
  else
  {
    double new_rho = compute_rho();
    emit(infected_node_stat, infected_num);
    emit(rho_stat, new_rho);
    if (rho != 0.0 && fabs(rho - new_rho)/rho < epsilon){
      endSimulation();
    }
    else {
      rho = new_rho;
      counter = 0;
      infected_num = 0;
    }
  }
  
}

void InfectionObserver::receiveSignal(
  omnetpp::cComponent* src,   //@param the module emitting the signal 
  omnetpp::simsignal_t id,    //@param the signal id
  omnetpp::cObject* obj,      //@param the object carried by the signal
  omnetpp::cObject* details   //@param details about the object
) {
  unsigned host_id = (dynamic_cast<omnetpp::cModule*>(src))->getIndex();
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
      (*q)[i] *= (1 - beta * (*p)[entry.netw_address.getId()]);

    (*next_p)[i]                         //p(t+1)
      = (1 - (*q)[i]) * (1 - (*p)[i])    //not infected & infected by nbr
      + (1 - mu) * (*p)[i]               //not recovery & infected
      + mu * (1 - (*q)[i]) * (*p)[i];    //infected after recovery
    
  }
  return std::accumulate(next_p->begin(), next_p->end(), 0.0) / host_num; //rho
}