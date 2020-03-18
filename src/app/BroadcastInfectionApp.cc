#include "BroadcastInfectionApp.h"
#include "inet/common/IProtocolRegistrationListener.h"

Define_Module(BroadcastInfectionApp);

omnetpp::simsignal_t InfectionBase::sent_message_signal = 
  registerSignal("sentMessage");
omnetpp::simsignal_t InfectionBase::received_message_signal = 
  registerSignal("receivedMessage");
omnetpp::simsignal_t InfectionBase::status_signal = 
  registerSignal("infectionTime");

BroadcastInfectionApp::BroadcastInfectionApp()
  : payload(0)
{ }

BroadcastInfectionApp::~BroadcastInfectionApp() { }

void BroadcastInfectionApp::initialize(int stage)
{
  InfectionBase::initialize(stage);
  if (stage == inet::INITSTAGE_LOCAL) {
    host_id = inet::getContainingNode(this)->getIndex();
    payload = par("payload");
    status_report_interval = par("statusReportInterval");
    recovery_probability = par("recoveryProbability");
    infection_probability = par("infectionProbability");
    status_timer = new omnetpp::cMessage(
      "status", 
      BroadcastInfectionApp::STATUS
    );
    status_timer->setSchedulingPriority(1);
    information_timer->setKind(BroadcastInfectionApp::BROADCAST);
    information_timer->setSchedulingPriority(1);
    recovery_timer = new omnetpp::cMessage(
      "recovery",
      BroadcastInfectionApp::RECOVERY
    );
    recovery_timer->setSchedulingPriority(1);
    auto coin = uniform(0.0, 1.0);
    EV_INFO << "Host " << host_id << " gets " << coin << '\n';
    if (
      coin < 
      par("initialInfectionProbability").doubleValue()
    )
      status = InfectionBase::INFECTED;
    else
      status = InfectionBase::NOT_INFECTED;
    EV_INFO << "Status of host " << host_id << " is " << status << '\n';
    if (status == InfectionBase::INFECTED) {
      scheduleAt(
        omnetpp::simTime() + par("sentInterval"), 
        information_timer
      );
      EV_INFO << "BroadcastInfectionApp: schedule information pkt at " << omnetpp::simTime() + par("sentInterval") << '\n';
    }
    scheduleAt(omnetpp::simTime() + status_report_interval, status_timer);
    EV_INFO << "BroadcastInfectionApp: schedule status report at " << omnetpp::simTime() + status_report_interval << '\n';
    WATCH(sent_messages);
    WATCH(received_messages);
    WATCH(status);
  }
}

void BroadcastInfectionApp::handleMessage(omnetpp::cMessage* msg)
{
  if (msg->isSelfMessage()) {
    switch (msg->getKind())
    {
    case RECOVERY: try_recovery(msg); break;
    case BROADCAST: send_message(msg); break;
    case STATUS: emit_status(msg); break;
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
    information->setName("virus");
    const auto& content = inet::makeShared<inet::InfoPacket>();
    content->setChunkLength(inet::B(payload));
    content->setType(inet::VIRUS);
    content->setIdentifer(555);
    content->setHost_id(host_id);
    information->insertAtBack(content);
    send_down(information);
    emit(sent_message_signal, ++sent_messages);
    scheduleAt(omnetpp::simTime() + par("sentInterval"), msg);
  }
  else
    throw omnetpp::cRuntimeError(
      "APP: not infected host %d tries to send a message", host_id
    );
}

void BroadcastInfectionApp::try_recovery(omnetpp::cMessage* msg)
{
  if (status == InfectionBase::INFECTED) {
    if (uniform(0.0, 1.0) < recovery_probability) {
      EV_INFO << "Host " << host_id << " recovers from infection\n";
      cancelEvent(information_timer);
      status = InfectionBase::NOT_INFECTED;
    }
    else {
      EV_INFO << "Host " << host_id << " fails to recover from infection\n";
      scheduleAt(omnetpp::simTime() + par("recoveryInterval"), msg);
    }
  }
  else
    throw omnetpp::cRuntimeError(
      "APP: not infected host %d tries to recover", host_id
    );
}

void BroadcastInfectionApp::process_packet(omnetpp::cMessage* msg)
{
  if (status == InfectionBase::NOT_INFECTED) {
    auto pkt = dynamic_cast<inet::Packet*>(msg);
    auto pkt_content = inet::dynamicPtrCast<inet::InfoPacket>(
      pkt->popAtFront<inet::InfoPacket>()->dupShared()
    );
    if (bernoulli(infection_probability)) {
      EV_INFO << "Host " << host_id << " received an infectious message from " 
              << pkt_content->getIdentifer() << "\n";
      scheduleAt(omnetpp::simTime() + par("sentInterval"), information_timer);
      status = InfectionBase::INFECTED;
    }
    else 
      EV_INFO << "Host " << host_id << " dropped an infectious message from " 
              << pkt_content->getHost_id() << "\n";
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
