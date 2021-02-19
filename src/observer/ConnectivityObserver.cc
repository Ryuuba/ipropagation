#include "ConnectivityObserver.h"
#include "inet/common/ModuleAccess.h"

Define_Module(ConnectivityObserver);

omnetpp::simsignal_t ConnectivityObserver::forwarding_list_signal
  = registerSignal("fwdList");
omnetpp::simsignal_t ConnectivityObserver::contact_list_signal
  = registerSignal("contactList");

void ConnectivityObserver::initialize(int stage) {
  if (stage == inet::INITSTAGE_LOCAL) {
    host_number = par("hostNumber").intValue();
    contact_cnt = std::make_unique<std::vector<uint64_t>>(host_number, 0);
    r_matrix = std::make_unique<SquareMatrix<uint64_t>>(host_number, 0);
    c_matrix = std::make_unique<SquareMatrix<uint64_t>>(host_number, 0);
    getSimulation()->getSystemModule()->subscribe(
      forwarding_list_signal,
      this
    );
    getSimulation()->getSystemModule()->subscribe(
      contact_list_signal,
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
  auto netw_layer = dynamic_cast<omnetpp::cModule*>(src)->getParentModule();
  size_t host_id = inet::getContainingNode(netw_layer)->getIndex();
  if (id == forwarding_list_signal) {
    Enter_Method("Receiving forwarding list from host[%d]", host_id);
    auto notification_ptr = dynamic_cast<ForwardingListNotification*>(obj);
    auto forwarding_list = notification_ptr->forwarding_list;
    for (auto&& id : *forwarding_list) {
      auto j = id.toModuleId().getId();
      (*r_matrix)(host_id, j-1)++; // decrement j because netw addresses start at 1
    }
  }
  else if (id == contact_list_signal) {
    Enter_Method("Receiving destination set from host[%d]", host_id);
    auto notification_ptr = dynamic_cast<DestinationNotification*>(obj);
    auto contact_list = notification_ptr->contact_list;
    (*contact_cnt)[host_id]++;
    for(auto&& dest_id : *contact_list)
      (*c_matrix)(host_id, dest_id.first)++; //decrement src_id
  }
}

void ConnectivityObserver::finish() {
  std::string result_file {
    omnetpp::getEnvir()->getConfig()->substituteVariables("${resultdir}/${configname}_${seedset}")
  };
  std::ofstream ofs( result_file + "-rij" + ".mat" );
  //Computes the number of attempts of transmissions per node
  std::vector<uint64_t> msg_tx(host_number, 0);
  double r_ij;
  for (size_t i = 0; i < r_matrix->size(); i++)
    for (size_t j = 0; j < r_matrix->size(); j++)
      msg_tx[i] += (*r_matrix)(i, j);
  //Writes rij (attempt)
  try {
    for (size_t i = 0; i < r_matrix->size(); i++) {
      std::cout << "Connectivity Observer: Transmitted messages of node[" 
                << i << "]: " << msg_tx[i] << '\n'; 
      for (size_t j = 0; j < r_matrix->size(); j++) {
        r_ij = (msg_tx[i] == 0) ? 0.0 : double((*r_matrix)(i, j));
        // ofs << std::fixed << std::setprecision(2) << r_ij <<  ' ';
        ofs << r_ij <<  ' ';
        }
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
    for (size_t i = 0; i < c_matrix->size(); i++) {
      for (size_t j = 0; j < c_matrix->size(); j++) {
        r_ij = ((*contact_cnt)[i] == 0) 
             ? 0.0 
             : double((*c_matrix)(i, j))/(*contact_cnt)[i];
        ofs << std::fixed << std::setprecision(2) << r_ij << ' ';
        }
      ofs << '\n';
    }
  }
  catch(const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
  ofs.close();
}