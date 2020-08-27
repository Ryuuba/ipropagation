#include "DynamicConnectivityObserver.h"
#include "inet/common/ModuleAccess.h"

Define_Module(DynamicConnectivityObserver);

omnetpp::simsignal_t DynamicConnectivityObserver::neighborhood_notification_signal =
  registerSignal("neighborhood");

void DynamicConnectivityObserver::initialize(int stage) {
  if (stage == inet::INITSTAGE_LOCAL) {
    host_number = par("hostNumber").intValue();
    adjacency_matrix = std::make_shared< SquareMatrix<cell_t> >(host_number);
    getSimulation()->getSystemModule()->subscribe(
      neighborhood_notification_signal,
      this
    );
  }
}

void DynamicConnectivityObserver::handleMessage(omnetpp::cMessage* msg) {
  throw omnetpp::cRuntimeError(
    "DynamicConnectivityObserver: This module does not receive any messages\
    (name = %s)\n", msg->getName()
  );
}

void DynamicConnectivityObserver::receiveSignal(
  omnetpp::cComponent* src,   //@param the module emitting the signal 
  omnetpp::simsignal_t id,    //@param the signal id
  omnetpp::cObject* obj,      //@param the object carried by the signal
  omnetpp::cObject* details   //@param details about the object
) {
  static unsigned counter = 0;
  auto notification_ptr = dynamic_cast<NeighborhoodNotificacion*>(obj);
  auto neighborhood = notification_ptr->neighborhood;
  auto netw_layer = dynamic_cast<omnetpp::cModule*>(src)->getParentModule();
  size_t i = inet::getContainingNode(netw_layer)->getIndex(); //host_id
  Enter_Method("Receiving neighborhood from host[%d]", i);
  // Updates the neighborhood of host host_id
  for (auto&& entry : *neighborhood) {
    size_t j = entry.netw_address.getId(); //neighbor_id
    if (!entry.still_connected) //j is not more neighbor of i
      (*adjacency_matrix)(i, j).lifetime += 
        entry.last_contact_time - entry.start_time;
    (*adjacency_matrix)(i, j).last_contact_time = entry.last_contact_time;
    (*adjacency_matrix)(i, j).start_time = entry.start_time;
    (*adjacency_matrix)(i, j).still_connected = entry.still_connected;
  }
  if (counter%(host_number) == (host_number-1))
    EV_INFO << *adjacency_matrix << '\n';
  counter++;
}

void DynamicConnectivityObserver::finish() {
  std::string result_file {
    omnetpp::getEnvir()->getConfig()->substituteVariables("${resultdir}/${configname}-${seedset}")
  };
  //Updates lifetime of still connected neighbors
  for (size_t i = 0; i < adjacency_matrix->size(); i++)
    for (size_t j = 0; j < adjacency_matrix->size(); j++)
      if ((*adjacency_matrix)(i, j).still_connected)
        (*adjacency_matrix)(i, j).lifetime += 
          (*adjacency_matrix)(i, j).last_contact_time - 
          (*adjacency_matrix)(i, j).start_time;
  std::ofstream ofs( result_file + ".mat" );
  //Writes lifetime
  try {
    for (size_t i = 0; i < adjacency_matrix->size(); i++) {
      for (size_t j = 0; j < adjacency_matrix->size(); j++)
        ofs << std::fixed << std::setprecision(2) << (*adjacency_matrix)(i, j).lifetime.dbl() << ' ';
      ofs << '\n';
    }
  }
  catch(const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
  ofs.close();
  //Writes connectivity ratio
  ofs.open(result_file + "-ratio" + ".mat");
  try {
    for (size_t i = 0; i < adjacency_matrix->size(); i++) {
      for (size_t j = 0; j < adjacency_matrix->size(); j++)
        ofs << std::fixed << std::setprecision(2) << (*adjacency_matrix)(i, j).lifetime.dbl()/omnetpp::simTime().dbl() << ' ';
      ofs << '\n';
    }
  }
  catch(const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
  ofs.close();
}
