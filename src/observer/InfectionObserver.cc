#include "InfectionObserver.h"

Define_Module(InfectionObserver);

omnetpp::simsignal_t InfectionObserver::status_signal = 
  registerSignal("lastStatus");

omnetpp::simsignal_t InfectionObserver::infected_node_stat =
  registerSignal("infectedNodes");

void InfectionObserver::initialize(int stage) {
  
}