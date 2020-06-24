#include "InformationPropagationApp.h"

Define_Module(InformationPropagationApp);

void InformationPropagationApp::initialize(int stage)
{
  InformationPropagationBase::initialize(stage);
  if (stage == inet::INITSTAGE_LOCAL) {
    InformationPropagationBase::mu = par("recoveryProbability");
    InformationPropagationBase::eta = par("infectionProbability");
    InformationPropagationBase::lambda = par("numberOfTrials").intValue();
    InformationPropagationBase::step_time = par("step");
    payload = par("payload");
    packet_name = par("packetName").stringValue();
    auto mode = par("communicationMode").stringValue();
    comm_mode 
      = strcmp(mode, "BROADCAST") == 0 ? BROADCAST
      : strcmp(mode, "MULTICAST") == 0 ? MULTICAST
      : UNICAST; //The default mode is unicast
    step_timer->setKind(InformationPropagationBase::TimerKind::STEP);
    information_timer->setKind(
      InformationPropagationBase::TimerKind::SEND_INFORMATION
    );
    // Step is most relevant than sending information
    step_timer->setSchedulingPriority(1);
    information_timer->setSchedulingPriority(2);
    diff_time = step_time / double(lambda + 1);
    WATCH(sent_messages);
    WATCH(received_messages);
    WATCH(status);
    WATCH(diff_time);
    WATCH(trial_num);
  }
  else if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
    compute_initial_state();
    EV_INFO << "The status of host " << src_address->getId() 
            << " is " << status_to_string(status) << '\n';
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
      EV_INFO << "The status of host " << src_address->getId() 
              << " is " << status_to_string(status) 
              << " at round " << round_num++ << '\n';
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

void InformationPropagationApp::draw_neighbor(COMM_MODE mode) 
{
  auto cache_size = neighbor_cache->size();
  if (cache_size > 0) {
    if (mode == BROADCAST) {
      InformationPropagationBase::draw_neighbor(cache_size);
    }
    else if (mode == MULTICAST) {
      size_t list_size 
        = neighbor_cache->size() == 1 ? 1
        : neighbor_cache->size() == 2 ? 2
        : neighbor_cache->size()  > 2 ? intuniform(1, cache_size-1)
        : 0;
      InformationPropagationBase::draw_neighbor(list_size);
    }
    else // UNICAST case
      InformationPropagationBase::draw_neighbor(1);
  }
  else 
    destination_list->clear();
}


void InformationPropagationApp::send_message(omnetpp::cMessage* msg)
{
  draw_neighbor(comm_mode);
  if (!destination_list->empty()) {
    encapsulate();
    socket->sendTo(information, *src_address);//L2 broadcast is used
    emit(sent_message_signal, ++sent_messages);
    EV_INFO << "InformationPropagationApp: Sending infectious message to: "; 
    for (auto& neighbor_address : (*destination_list))
      EV_INFO << neighbor_address << ' ';
    EV_INFO << '\n';
  }
  else
    EV_INFO << "InformationPropagationApp: This node can't send any msg since it doesn't have any neighbor\n";
}

void InformationPropagationApp::encapsulate() {
  information = new inet::Packet(packet_name);
  auto data = inet::makeShared<inet::ByteCountChunk>(inet::B(payload));
  const auto& header = inet::makeShared<inet::InfoPacket>();
  header->setSrc(*src_address);
  header->setChunkLength(inet::B(payload));
  header->setType(inet::VIRUS);
  header->setIdentifer(555);
  header->setDestination_list_ptr(destination_list);
  information->insertAtBack(data);
  information->insertAtFront(header);
  information->addTagIfAbsent<inet::L3AddressInd>()->setSrcAddress(*src_address);
  information->addTagIfAbsent<inet::L3AddressInd>()->setDestAddress(inet::ModuleIdAddress());
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
  // else if (status == InformationPropagationBase::NOT_INFECTED)
  //   EV_INFO << "Host " << src_address->getId() << " is not infected\n";
  // else
  //   throw omnetpp::cRuntimeError(
  //     "APP: not infected host %d tries to recover", src_address->getId()
  //   );
}

void InformationPropagationApp::process_packet(inet::Ptr<inet::InfoPacket> pkt)
{
  if (status == InformationPropagationBase::NOT_INFECTED) {
    if (bernoulli(eta)) {
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
      EV_INFO << "Host " << src_address->getId() 
              << " dropped an infectious message from " 
              << pkt->getSrc() << '\n';
  }
  else 
    EV_INFO << "Host " << src_address->getId()
            << " is already infected\n";
  emit(received_message_signal, ++received_messages);
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

void InformationPropagationApp::finish() {
  infection_time = omnetpp::simTime() - infection_time;
  emit(infection_time_signal, infection_time);
  emit(last_status_signal, status);
}

void InformationPropagationApp::compute_initial_state() {
    auto coin = uniform(0.0, 1.0);
    EV_INFO << "Host " << src_address->getId() << " gets " 
            << coin << '\n';
    if (
      coin < 
      par("initialInfectionProbability").doubleValue()
    ) {
      status = InformationPropagationBase::INFECTED;
      if (hasGUI())
        getParentModule()->bubble("Get infected!");
    }
    else
      status = InformationPropagationBase::NOT_INFECTED;
    EV_INFO << "Initial status of host " << src_address->getId() 
            << " is " << status << '\n';
}