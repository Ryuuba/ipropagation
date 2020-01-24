#include "InfectionApp.h"

Define_Module(InfectionApp);

omnetpp::simsignal_t InfectionBase::sent_message_signal = 
  registerSignal("sentMessage");
omnetpp::simsignal_t InfectionBase::received_message_signal = 
  registerSignal("receivedMessage");
omnetpp::simsignal_t InfectionBase::infection_time_signal = 
  registerSignal("infectionTime");

InfectionApp::InfectionApp()
  : packet_size(0)
{ }

InfectionApp::~InfectionApp()
{

}

void InfectionApp::initialize(int stage)
{
  if (stage == inet::INITSTAGE_LOCAL) {
    WATCH(sent_messages);
    WATCH(received_messages);
    WATCH(infection_time);
    packet_size = par("packetSize");
    unicast_timer = new omnetpp::cMessage("unicast", InfectionApp::BROADCAST);
    recovery_timer = new omnetpp::cMessage("recovery", InfectionApp::RECOVERY);
    //TODO: Get access to the observer that computes N(x)
    //TODO: Get the initial operational status from network
    status = static_cast<Status>(par("initialStatus").intValue());
    //TODO: Schedule self-messages
}
