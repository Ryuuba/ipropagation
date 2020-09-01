#include "InformationPropagationApp.h"

Define_Module(InformationPropagationApp);

void InformationPropagationApp::initialize(int stage)
{
  InformationPropagationBase::initialize(stage);
  if (stage == inet::INITSTAGE_LOCAL) {
    payload = par("payload");
    packet_name = par("packetName").stringValue();
    step_timer->setKind(InformationPropagationBase::TimerKind::STEP);
    information_timer->setKind(
      InformationPropagationBase::TimerKind::SEND_INFORMATION
    );
    // Step is most relevant than sending information
    step_timer->setSchedulingPriority(1);
    information_timer->setSchedulingPriority(2);
    diff_time = step_time / double(lambda + 1);
    WATCH(sent_msg);
    WATCH(recv_msg); //from IApp
    WATCH(status);
    WATCH(diff_time);
    WATCH(trial_num);
  }
  else if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
    auto t0 = omnetpp::simTime() + getSimulation()->getWarmupPeriod();
    scheduleAt(t0, step_timer);
  }
}

void InformationPropagationApp::handleMessage(omnetpp::cMessage* msg)
{
  if (msg->isSelfMessage()) {
    if (msg->getKind() == STEP) {
      if (status == INFECTED) {
        scheduleAt(omnetpp::simTime() + diff_time, information_timer);
        if (round_num > 0)
          try_recovery(msg);
      }
      round_num++;
      EV_INFO << "The status of host " << src_address->getId() 
              << " is " << status_to_string(status) 
              << " at round " << round_num << '\n';
      emit(last_status_signal, status);
      scheduleAt(omnetpp::simTime() + step_time, step_timer);
    }
    else if (msg->getKind() == SEND_INFORMATION) {
      send_message(msg);
      trial_num++;
      if (trial_num < lambda)
        scheduleAt(omnetpp::simTime() + diff_time, information_timer);
      else
        trial_num = 0;
    }
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

void InformationPropagationApp::try_recovery(omnetpp::cMessage* msg)
{
  EV_INFO << "Status of node " << src_address->getId()
          << " is " << status_to_string(status) << '\n';
  if (status == InformationPropagationBase::INFECTED) {
    if (bernoulli(mu)) {
      EV_INFO << "Host " << src_address->getId() << " recovers from infection\n";
      infection_time = omnetpp::simTime() - infection_time;
      emit(infection_time_signal, infection_time);
      cancelEvent(information_timer);
      status = InformationPropagationBase::NOT_INFECTED;
      if (hasGUI())
        getParentModule()->bubble("Get not infected!");
    }
    else
      EV_INFO << "Host " << src_address->getId() << " fails to recover from infection\n";
  }
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
  emit(src_id_signal, pkt->getSrc().toModuleId().getId());
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

