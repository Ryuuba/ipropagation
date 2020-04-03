#include "ConnectivityObserver.h"

Define_Module(ConnectivityObserver);

omnetpp::simsignal_t ConnectivityObserver::neighborhood_notification_signal =
  registerSignal("neighborhood");

void ConnectivityObserver::initialize(int stage) {
  if (stage == inet::INITSTAGE_LOCAL) {
    size_t size = par("hostNumber").intValue();
    adjacency_matrix = std::make_unique< SquareMatrix<cell_t> >(size);
    getSimulation()->getSystemModule()->subscribe(
      neighborhood_notification_signal,
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

void ConnectivityObserver::receiveSignal(
  omnetpp::cComponent* src, 
  omnetpp::simsignal_t id, 
  omnetpp::cObject* obj, 
  omnetpp::cObject* details
) {
  auto notification_ptr = dynamic_cast<NeighborhoodNotificacion*>(obj);
  auto neighborhood = notification_ptr->neighborhood;
  auto host_id = dynamic_cast<omnetpp::cModule*>(src)->
    getParentModule()->getIndex();
  // Updates the neighborhood of host host_id
  for (auto&& entry : *neighborhood) {
    size_t neighbor_id = entry.netw_address.getId();
    auto diff_time = entry.last_contact_time -
      (*adjacency_matrix)(host_id, neighbor_id).last_contact_time;
    (*adjacency_matrix)(host_id, neighbor_id).accum += diff_time;
    (*adjacency_matrix)(host_id, neighbor_id).last_contact_time =
      entry.last_contact_time;
  }
  EV_INFO << "ConnectivityObserver: Update neighborhood of host "
          << host_id << '\n';
}

void ConnectivityObserver::finish() {
  std::string result_file {
    omnetpp::getEnvir()->getConfig()->substituteVariables("${resultdir}/${configname}-${runnumber}")
  };
  std::ofstream ofs(result_file + ".mat");
  try {
    for (size_t i = 0; i < adjacency_matrix->size(); i++) {
      for (size_t j = 0; j < adjacency_matrix->size(); j++)
        ofs << (*adjacency_matrix)(i, j).accum << ' ';
      ofs << '\n';
    }
  }
  catch(const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
  ofs.close();
  ofs.open(result_file + "-ratio" + ".mat");
  try {
    for (size_t i = 0; i < adjacency_matrix->size(); i++) {
      for (size_t j = 0; j < adjacency_matrix->size(); j++)
        ofs << (*adjacency_matrix)(i, j).accum/omnetpp::simTime() << ' ';
      ofs << '\n';
    }
  }
  catch(const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
  ofs.close();
}