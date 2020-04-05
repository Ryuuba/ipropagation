#include "ConnectivityObserver.h"
#include "inet/common/ModuleAccess.h"

Define_Module(ConnectivityObserver);

omnetpp::simsignal_t ConnectivityObserver::neighborhood_notification_signal =
  registerSignal("neighborhood");

void ConnectivityObserver::initialize(int stage) {
  if (stage == inet::INITSTAGE_LOCAL) {
    host_number = par("hostNumber").intValue();
    adjacency_matrix = std::make_unique< SquareMatrix<cell_t> >(host_number);
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
  static unsigned counter = 0;
  auto notification_ptr = dynamic_cast<NeighborhoodNotificacion*>(obj);
  auto neighborhood = notification_ptr->neighborhood;
  auto netw_layer = dynamic_cast<omnetpp::cModule*>(src)->getParentModule();
  size_t i = inet::getContainingNode(netw_layer)->getIndex(); //host_id
  Enter_Method("Receiving signal from %d", i);
  // Updates the neighborhood of host host_id
  for (auto&& entry : *neighborhood) {
    size_t j = entry.netw_address.getId(); //neighbor_id
    if (!entry.still_updated) //j is not more neighbor of i
      (*adjacency_matrix)(i, j).lifetime += 
        entry.last_contact_time - entry.start_time;
    (*adjacency_matrix)(i, j).last_contact_time = entry.last_contact_time;
  }
  std::cout << "ConnectivityObserver: Update neighborhood of host "
          << i << '\n';
  if (counter%(host_number) == (host_number-1))
    std::cout << *adjacency_matrix << '\n';
  counter++;
}

void ConnectivityObserver::finish() {
  std::string result_file {
    omnetpp::getEnvir()->getConfig()->substituteVariables("${resultdir}/${configname}-${runnumber}")
  };
  std::ofstream ofs(result_file + ".mat");
  try {
    for (size_t i = 0; i < adjacency_matrix->size(); i++) {
      for (size_t j = 0; j < adjacency_matrix->size(); j++)
        ofs << (*adjacency_matrix)(i, j).lifetime << ' ';
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
        ofs << (*adjacency_matrix)(i, j).lifetime/omnetpp::simTime() << ' ';
      ofs << '\n';
    }
  }
  catch(const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
  ofs.close();
}