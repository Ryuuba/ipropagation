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
    only_bcast = par("onlyBroadcast").boolValue();
    infection_degree = par("infectionDegree").intValue();
    payload = par("payload");
    status_report_interval = par("statusReportInterval");
    recovery_probability = par("recoveryProbability");
    infection_probability = par("infectionProbability");
    status_timer->setKind(TimerKind::STATUS);
    status_timer->setSchedulingPriority(1);
    information_timer->setKind(TimerKind::BROADCAST);
    information_timer->setSchedulingPriority(1);
    recovery_timer->setKind(TimerKind::RECOVERY);
    recovery_timer->setSchedulingPriority(1);
    WATCH(sent_messages);
    WATCH(received_messages);
    WATCH(status);
  }
  else if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
    auto coin = uniform(0.0, 1.0);
    EV_INFO << "Host " << src_address->toModuleId().getId() << " gets " << coin << '\n';
    if (
      coin < 
      par("initialInfectionProbability").doubleValue()
    )
      status = InfectionBase::INFECTED;
    else
      status = InfectionBase::NOT_INFECTED;
    EV_INFO << "Status of host " << src_address->toModuleId().getId() 
            << " is " << status << '\n';
    if (status == InfectionBase::INFECTED) {
      scheduleAt(
        omnetpp::simTime() + par("sentInterval"), 
        information_timer
      );
      EV_INFO << "BroadcastInfectionApp: schedule information pkt at " << omnetpp::simTime() + par("sentInterval") << '\n';
    }
    scheduleAt(omnetpp::simTime() + status_report_interval, status_timer);
    EV_INFO << "BroadcastInfectionApp: schedule status report at " << omnetpp::simTime() + status_report_interval << '\n';
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
  else if(socket->belongsToSocket(msg))
    socket->processMessage(msg);
  else
    throw omnetpp::cRuntimeError("App: Invalid message %s", msg->getName());
}

void BroadcastInfectionApp::encapsulate(const inet::L3Address& dst_addr) {
  information = new inet::Packet("virus");
  auto data = inet::makeShared<inet::ByteCountChunk>(inet::B(payload));
  const auto& header = inet::makeShared<inet::InfoPacket>();
  header->setChunkLength(inet::B(payload));
  header->setType(inet::VIRUS);
  header->setIdentifer(555);
  header->setHost_id(src_address->toModuleId().getId());
  information->insertAtBack(data);
  information->insertAtFront(header);
  information->addTagIfAbsent<inet::L3AddressInd>()->setSrcAddress(*src_address);
  information->addTagIfAbsent<inet::L3AddressInd>()->setDestAddress(dst_addr);
  information->addTagIfAbsent<inet::PacketProtocolTag>()->
    setProtocol(&inet::Protocol::information);
  auto addressReq = information->addTag<inet::L3AddressReq>();
  addressReq->setSrcAddress(*src_address);
  addressReq->setDestAddress(dst_addr);
}

void BroadcastInfectionApp::send_message(omnetpp::cMessage* msg)
{
  if (status == InfectionBase::INFECTED) {
    if (!neighbor_cache->get_neighbor_cache()->empty()) {
      auto neighbor_address = draw_neighbor();
      encapsulate(neighbor_address);
      EV_INFO << "BroadcastInfectionApp: Sending infectious message to host " 
              << neighbor_address << "\n";
      socket->sendTo(information, neighbor_address);
      emit(sent_message_signal, ++sent_messages);
      scheduleAt(omnetpp::simTime() + par("sentInterval"), msg);
    }
    else
      EV_INFO << "BroadcastInfectionApp: This node doesn't have any neighbor\n";
  }
  else
    throw omnetpp::cRuntimeError(
      "BroadcastInfectionApp: not infected host %d tries to send a message", src_address->toModuleId().getId()
    );
}

void BroadcastInfectionApp::try_recovery(omnetpp::cMessage* msg)
{
  if (status == InfectionBase::INFECTED) {
    if (uniform(0.0, 1.0) < recovery_probability) {
      EV_INFO << "Host " << src_address->toModuleId().getId() << " recovers from infection\n";
      cancelEvent(information_timer);
      status = InfectionBase::NOT_INFECTED;
    }
    else {
      EV_INFO << "Host " << src_address->toModuleId().getId() << " fails to recover from infection\n";
      scheduleAt(omnetpp::simTime() + par("recoveryInterval"), msg);
    }
  }
  else
    throw omnetpp::cRuntimeError(
      "APP: not infected host %d tries to recover", src_address->toModuleId().getId()
    );
}

void BroadcastInfectionApp::process_packet(inet::Packet* pkt)
{
  if (status == InfectionBase::NOT_INFECTED) {
    auto pkt_header = inet::dynamicPtrCast<inet::InfoPacket>(
      pkt->popAtFront<inet::InfoPacket>()->dupShared()
    );
    if (bernoulli(infection_probability)) {
      EV_INFO << "Host " << src_address->toModuleId().getId() 
              << " received an infectious message from " 
              << pkt_header->getIdentifer() << "\n";
      scheduleAt(omnetpp::simTime() + par("sentInterval"), information_timer);
      status = InfectionBase::INFECTED;
    }
    else 
      EV_INFO << "Host " << src_address->toModuleId().getId() 
              << " dropped an infectious message from " 
              << pkt_header->getHost_id() << '\n';
  }
  else 
    EV_INFO << "Host " << src_address->toModuleId().getId()
            << " is already infected\n";
  delete pkt;
  emit(received_message_signal, ++received_messages);
}

void BroadcastInfectionApp::emit_status(omnetpp::cMessage* msg) {
  EV_INFO << "The status of host " << src_address->toModuleId().getId() << " is " << status << '\n';
  emit(status_signal, status);
  scheduleAt(omnetpp::simTime() + status_signal, msg);
}

void BroadcastInfectionApp::refreshDisplay() const
{

}

