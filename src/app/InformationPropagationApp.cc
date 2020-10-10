#include "InformationPropagationApp.h"

Define_Module(InformationPropagationApp);

void InformationPropagationApp::initialize(int stage)
{
  InformationPropagationBase::initialize(stage);
  if (stage == inet::INITSTAGE_LOCAL) {
    payload = par("payload");
    packet_name = par("packetName").stringValue();
    recovery_timer->setKind(InformationPropagationBase::TimerKind::RECOVERY);
    stat_timer->setKind(InformationPropagationBase::TimerKind::SEND_STATS);
    transmission_timer->setKind(
      InformationPropagationBase::TimerKind::TRANSMISSION
    );
    // Recovery is most relevant than sending information
    recovery_timer->setSchedulingPriority(1);
    transmission_timer->setSchedulingPriority(2);
    stat_timer->setSchedulingPriority(3);
    WATCH(sent_msg);
    WATCH(recv_msg); //from IApp
    WATCH(status);;
  }
  else if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
    auto t0 = omnetpp::simTime() + getSimulation()->getWarmupPeriod();
    auto t1 = t0 + step_time;
    scheduleAt(t0 + unit_time, transmission_timer);
    scheduleAt(t1, recovery_timer);
    scheduleAt(t0, stat_timer);
  }
}

void InformationPropagationApp::handleMessage(omnetpp::cMessage* msg)
{
  if (msg->isSelfMessage()) {
    if (msg->getKind() == RECOVERY) {
      if (hasGUI())
        getParentModule()->bubble("Recovery");
      if (status == INFECTED)
        try_recovery();
      scheduleAt(omnetpp::simTime() + step_time, msg);
    }
    else if (msg->getKind() == TRANSMISSION) {
      if (hasGUI())
        getParentModule()->bubble("Transmission");
      if (status == INFECTED)
        send_message(msg);
      scheduleAt(omnetpp::simTime() + unit_time, msg);
    }
    else if (msg->getKind() == SEND_STATS) {
      if (hasGUI())
        getParentModule()->bubble("Send stats");
      round_num++;
      EV_INFO << "The status of host " << src_address->getId() 
              << " is " << status_to_string(status) 
              << " at round " << round_num << '\n';
      emit(last_status_signal, status);
      SourceNotification notification(src_set);
      emit(src_set_signal, &notification);
      src_set->clear();
      // It's assumed the maximum delay is constrained
      if (omnetpp::simTime() == getSimulation()->getWarmupPeriod())
        scheduleAt(omnetpp::simTime() + step_time + max_bcast_delay, msg);
      else
        scheduleAt(omnetpp::simTime() + step_time, msg);
    }
    else
      throw omnetpp::cRuntimeError(
        "App: Invalid message kind %s", msg->getName()
      );
  }
  else if(socket->belongsToSocket(msg)) {
    socket->processMessage(msg);
  }
  else
    throw omnetpp::cRuntimeError("App: Invalid message %s", msg->getName());
}


void InformationPropagationApp::send_message(omnetpp::cMessage* msg)
{
    encapsulate();
    // App layer requests the L3 layer to draw a random destination
    socket->sendTo(information, inet::L3Address(unspecified_address));
    emit(sent_message_signal, ++sent_msg);
    EV_INFO << "InformationPropagationApp: Sending infectious message\n"; 
}

void InformationPropagationApp::encapsulate() {
  information = new inet::Packet(packet_name);
  auto data = inet::makeShared<inet::ByteCountChunk>(inet::B(payload));
  const auto& header = inet::makeShared<inet::InfoPacket>();
  header->setSrc(*src_address);
  header->setChunkLength(inet::B(payload));
  header->setType(inet::VIRUS);
  header->setIdentifer(555);
  information->insertAtBack(data);
  information->insertAtFront(header);
  information->addTagIfAbsent<inet::L3AddressInd>()->setSrcAddress(*src_address);
  information->addTagIfAbsent<inet::L3AddressInd>()->setDestAddress(inet::ModuleIdAddress()); //unspecified address
  information->addTagIfAbsent<inet::PacketProtocolTag>()->
    setProtocol(&inet::Protocol::information);
  auto addressReq = information->addTag<inet::L3AddressReq>();
  addressReq->setSrcAddress(*src_address);
  addressReq->setDestAddress(inet::ModuleIdAddress());
}

void InformationPropagationApp::try_recovery()
{
  EV_INFO << "Status of node " << src_address->getId()
          << " is " << status_to_string(status) << '\n';
  if (bernoulli(mu)) {
    EV_INFO << "Host " << src_address->getId() << " recovers from infection\n";
    infection_time = omnetpp::simTime() - infection_time;
    emit(infection_time_signal, infection_time);
    status = InformationPropagationBase::NOT_INFECTED;
    if (hasGUI())
      getParentModule()->bubble("Get not infected!");
  }
  else
    EV_INFO << "Host " << src_address->getId() << " fails to recover from infection\n";

}

void InformationPropagationApp::process_packet(inet::Ptr<inet::InfoPacket> pkt)
{
  if (status == InformationPropagationBase::NOT_INFECTED) {
    EV_INFO << "Host " << src_address->getId() 
            << " received an infectious message from " 
            << pkt->getSrc() << "\n";
    infection_time = omnetpp::simTime() - infection_time;
    status = InformationPropagationBase::INFECTED;
    emit(infection_time_signal, infection_time);
    emit(last_status_signal, status);
    if (hasGUI())
      getParentModule()->bubble("Get infected!");
  }
  else 
    EV_INFO << "App: Host " << src_address->getId()
            << " is already infected\n";
  int src_id = pkt->getSrc().toModuleId().getId();
  src_set->insert(src_id);
  emit(recv_msg_signal, ++recv_msg);
}

void InformationPropagationApp::refreshDisplay() const
{
  auto parent_module = getParentModule();
  omnetpp::cDisplayString& display_str = parent_module->getDisplayString();
  if (status == InformationPropagationBase::INFECTED)
    display_str.setTagArg("i", 1, "red");
  else 
    display_str.setTagArg("i", 1, "");
}

void InformationPropagationApp::finish()
{
  infection_time = omnetpp::simTime() - infection_time;
  emit(infection_time_signal, infection_time);
  emit(last_status_signal, status);
}

