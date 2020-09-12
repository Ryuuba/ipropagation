#include "ConnectivityObserver.h"
#include "inet/common/ModuleAccess.h"

Define_Module(ConnectivityObserver);

omnetpp::simsignal_t ConnectivityObserver::forwarding_list_signal
  = registerSignal("fwdList");
omnetpp::simsignal_t ConnectivityObserver::src_id_signal
  = registerSignal("srcId");

void ConnectivityObserver::initialize(int stage) {
  if (stage == inet::INITSTAGE_LOCAL) {
    host_number = par("hostNumber").intValue();
    r_matrix = std::make_unique<SquareMatrix<uint64_t>>(host_number, 0);
    e_matrix = std::make_unique<SquareMatrix<uint64_t>>(host_number, 0);
    getSimulation()->getSystemModule()->subscribe(
      forwarding_list_signal,
      this
    );
    getSimulation()->getSystemModule()->subscribe(
      src_id_signal,
      this
    );
  }
}

void ConnectivityObserver::handleMessage(omnetpp::cMessage* msg) {
  throw omnetpp::cRuntimeError(
    "ConnectivityObserver: This module does not receive any messages\
    (name = %s)\n", msg->getName()
  );
}

// The random process the network layer performs determines r_ij
void ConnectivityObserver::receiveSignal(
  omnetpp::cComponent* src,   //@param the module emitting the signal 
  omnetpp::simsignal_t id,    //@param the signal id
  omnetpp::cObject* obj,      //@param the object carried by the signal
  omnetpp::cObject* details   //@param details about the object
) {
  auto notification_ptr = dynamic_cast<ForwardingListNotification*>(obj);
  auto forwarding_list = notification_ptr->forwarding_list;
  auto netw_layer = dynamic_cast<omnetpp::cModule*>(src)->getParentModule();
  size_t i = inet::getContainingNode(netw_layer)->getIndex(); //host_id
  Enter_Method("Receiving forwarding list from host[%d]", i);
  for (auto&& id : *forwarding_list) {
    auto j = id.toModuleId().getId();
    (*r_matrix)(i, j-1)++; // decrement j because netw addresses start at 1
  }
}

void ConnectivityObserver::receiveSignal(
  omnetpp::cComponent* src,   //@param the module emitting the signal 
  omnetpp::simsignal_t id,    //@param the signal id
  long src_id,                //@param the object the signal carries
  omnetpp::cObject* details   //@param details about the object
) {
  auto netw_layer = dynamic_cast<omnetpp::cModule*>(src)->getParentModule();
  size_t recv_id = inet::getContainingNode(netw_layer)->getIndex(); //host_id
  --src_id; // decrements source address because netw addresses start at 1
  Enter_Method("Receiving source id %d from [%d]", src_id, recv_id);
  (*e_matrix)(src_id, recv_id)++;
}

void ConnectivityObserver::finish() {
  std::string result_file {
    omnetpp::getEnvir()->getConfig()->substituteVariables("${resultdir}/${configname}-${seedset}")
  };
  std::ofstream ofs( result_file + "-rij" + ".mat" );
  //Computes the number of attempts of transmissions per node
  std::vector<uint64_t> msg_tx(host_number, 0);
  for (size_t i = 0; i < r_matrix->size(); i++)
    for (size_t j = 0; j < r_matrix->size(); j++)
      msg_tx[i] += (*r_matrix)(i, j);
  //Writes rij (attempt)
  try {
    for (size_t i = 0; i < r_matrix->size(); i++) {
      for (size_t j = 0; j < r_matrix->size(); j++)
        ofs << std::fixed << std::setprecision(2) 
            << double((*r_matrix)(i, j))/msg_tx[i] <<  ' ';
      ofs << '\n';
    }
  }
  catch(const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
  ofs.close();
  //Writes the effective connectivity ratio (receptor view)
  ofs.open(result_file + "-eff" + ".mat");
  try {
    for (size_t i = 0; i < e_matrix->size(); i++) {
      for (size_t j = 0; j < e_matrix->size(); j++)
        ofs << std::fixed << std::setprecision(2) 
            << double((*e_matrix)(i, j))/msg_tx[i] << ' ';
      ofs << '\n';
    }
  }
  catch(const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
  ofs.close();
}