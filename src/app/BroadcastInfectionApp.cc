#include "BroadcastInfectionApp.h"

Define_Module(BroadcastInfectionApp);

omnetpp::simsignal_t InfectionBase::sent_message_signal = 
  registerSignal("sentMessage");
omnetpp::simsignal_t InfectionBase::received_message_signal = 
  registerSignal("receivedMessage");
omnetpp::simsignal_t InfectionBase::status_signal = 
  registerSignal("infectionTime");

BroadcastInfectionApp::BroadcastInfectionApp()
  : pkt(nullptr), packet_size(0)
{ 

}

BroadcastInfectionApp::~BroadcastInfectionApp()
{

}

void BroadcastInfectionApp::initialize(int stage)
{
  if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
    host_id = getParentModule()->getIndex();
    input_gate_id = gate("inputPort")->getId();
    output_gate_id = gate("outputPort")->getId();
    WATCH(sent_messages);
    WATCH(received_messages);
    WATCH(status);
    packet_size = par("packetSize");
    status_report_interval = par("statusReportInterval");
    recovery_probability = par("recoveryProbabily");
    infection_probability = par("infectionProbability");
    status_timer = new omnetpp::cMessage("status", BroadcastInfectionApp::STATUS);
    status_timer->setSchedulingPriority(1);
    message_timer = new omnetpp::cMessage("broadcast", BroadcastInfectionApp::BROADCAST);
    message_timer->setSchedulingPriority(1);
    recovery_timer = new omnetpp::cMessage("recovery", BroadcastInfectionApp::RECOVERY);
    recovery_timer->setSchedulingPriority(1);
    //TODO: Get the initial operational status from network
    status = static_cast<Status>(par("initialStatus").intValue());
    //TODO: Schedule self-messages
    if (status == InfectionBase::INFECTED)
      scheduleAt(
        omnetpp::simTime() + par("sentInterval"), 
        message_timer
      );
    scheduleAt(omnetpp::simTime() + status_report_interval, status_timer);
  }
}

void BroadcastInfectionApp::handleMessage(omnetpp::cMessage* msg)
{
  if (msg->isSelfMessage()) {
    switch (msg->getKind())
    {
    case RECOVERY:
      try_recovery(msg);
      break;
    case BROADCAST:
      send_message(msg);
    case STATUS:
      emit_status(msg);
    default:
      throw omnetpp::cRuntimeError("App: Unknown timer %d", msg->getKind());
      break;
    }
  }
  else if(msg->arrivedOn(input_gate_id)) {
    process_packet(msg);
  }
  else
    throw omnetpp::cRuntimeError("App: Invalid message %s", msg->getName());
}

void BroadcastInfectionApp::send_message(omnetpp::cMessage* msg)
{
  if (status == InfectionBase::INFECTED) {
    pkt = new Info(packet_name);
    pkt->setByteLength(packet_size);
    pkt->setSchedulingPriority(10);
    pkt->setHost_id(host_id);
    send(pkt, output_gate_id);
    emit(sent_message_signal, ++sent_messages);
    scheduleAt(omnetpp::simTime() + par("sentInterval"), msg);
  }
  else
    throw omnetpp::cRuntimeError(
      "App: not infected host %d tries to send a message", host_id
    );
}

void BroadcastInfectionApp::try_recovery(omnetpp::cMessage* msg)
{
  if (status == InfectionBase::INFECTED) {
    if (uniform(0.0, 1.0) < recovery_probability) {
      EV_INFO << "Host " << host_id << " recovers from infection\n";
      cancelEvent(message_timer);
      status = InfectionBase::NOT_INFECTED;
    }
    else {
      EV_INFO << "Host " << host_id << " fails to recover from infection\n";
      scheduleAt(omnetpp::simTime() + par("recoveryInterval"), msg);
    }
  }
  else
    throw omnetpp::cRuntimeError(
      "App: not infected host %d tries to recover", host_id
    );
}

void BroadcastInfectionApp::process_packet(omnetpp::cMessage* msg)
{
  if (status == InfectionBase::NOT_INFECTED){
    auto infected_pkt = dynamic_cast<Info*>(msg);
    if (uniform(0.0, 1.0) < infection_probability) {
      EV_INFO << "Host " << host_id << " received an infectious message from " 
      << infected_pkt->getHost_id() << "\n";
      scheduleAt(omnetpp::simTime() + par("sentInterval"), message_timer);
      status = InfectionBase::INFECTED;
    }
    else 
      EV_INFO << "Host " << host_id << " dropped an infectious message from " 
      << infected_pkt->getHost_id() << "\n";
  }
  delete msg;
  emit(received_message_signal, ++received_messages);
}

void BroadcastInfectionApp::emit_status(omnetpp::cMessage* msg) {
  EV_INFO << "The status of host " << host_id << " is " << status << '\n';
  emit(status_signal, status);
  scheduleAt(omnetpp::simTime() + status_signal, msg);
}

void BroadcastInfectionApp::refreshDisplay() const
{

}