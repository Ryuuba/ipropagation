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
    information_timer->setKind(InformationPropagationBase::TimerKind::SEND_DATA);
    // Step is most relevant than sending information
    step_timer->setSchedulingPriority(1);
    information_timer->setSchedulingPriority(2);
    diff_time = step_time / double(lambda + 1);
    WATCH(sent_messages);
    WATCH(received_messages);
    WATCH(status);
    WATCH(diff_time);
  }
  else if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
    compute_initial_state();
    auto t0 = omnetpp::simTime() + getSimulation()->getWarmupPeriod();
    scheduleAt(t0 + diff_time, information_timer);
  }
}

void InformationPropagationApp::handleMessage(omnetpp::cMessage* msg)
{
  if (msg->isSelfMessage())
    switch (msg->getKind()) {
      case STEP:
        EV_INFO << "The status of host " << src_address->toModuleId().getId() 
                << " is " << status << '\n';
        emit(last_status_signal, status);
        try_recovery(msg);
        scheduleAt(omnetpp::simTime() + diff_time, information_timer);
        scheduleAt(omnetpp::simTime() + step_time, step_timer);
        break;
      case SEND_DATA: 
        send_message(msg);
        if (trial_num < lambda) {
          scheduleAt(omnetpp::simTime() + diff_time, information_timer);
          trial_num++;
        }
        else
          trial_num = 0;
        break;
      default:
        throw omnetpp::cRuntimeError("App: Unknown timer %d", msg->getKind());
        break;
    }
  else if(socket->belongsToSocket(msg)) {
    socket->processMessage(msg);
  }
  else
    throw omnetpp::cRuntimeError("App: Invalid message %s", msg->getName());
}

std::list<inet::L3Address> InformationPropagationApp::draw_neighbor(COMM_MODE mode) 
{
  std::list<inet::L3Address> neighbor_list;
  if (mode == BROADCAST) {
    inet::L3Address broadcast_address;
    broadcast_address.set(inet::MacAddress::BROADCAST_ADDRESS);
    neighbor_list.push_back(broadcast_address);
  }
  else if (mode == MULTICAST) {
    size_t list_size 
      = neighbor_cache->size() == 1 ? 1
      : neighbor_cache->size() == 2 ? 2
      : neighbor_cache->size()  > 2 ? intuniform(1, neighbor_cache->size()-1)
      : 0;
    neighbor_list = std::move(InformationPropagationBase::draw_neighbor(list_size));
  }
  else 
    neighbor_list = std::move(InformationPropagationBase::draw_neighbor(1));
  return neighbor_list;
}


void InformationPropagationApp::send_message(omnetpp::cMessage* msg)
{
  std::list<inet::L3Address> neighbor_list(draw_neighbor(comm_mode));
  if (status == InformationPropagationBase::INFECTED) {
    if (!neighbor_list.empty()) {
      for (auto& neighbor_address : neighbor_list) {
        encapsulate(neighbor_address);
        EV_INFO << "InformationPropagationApp: Sending infectious message to " 
                << neighbor_address << "\n";
        socket->sendTo(information, neighbor_address);
        emit(sent_message_signal, ++sent_messages);
      }
    }
    else
      EV_INFO << "InformationPropagationApp: This node doesn't have any neighbor\n";
  }
  else if (status == InformationPropagationBase::NOT_INFECTED)
    EV_INFO << "This node is healthy\n";
  else
    throw omnetpp::cRuntimeError(
      "InformationPropagationApp: not infected host %d tries to send a message", src_address->toModuleId().getId()
    );
}

void InformationPropagationApp::encapsulate(const inet::L3Address& dst_addr) {
  information = new inet::Packet(packet_name);
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

void InformationPropagationApp::try_recovery(omnetpp::cMessage* msg)
{
  std::cout << "Status of node " << src_address->toModuleId().getId()
            << " is " << status_to_string(status) << '\n';
  if (status == InformationPropagationBase::INFECTED) {
    if (bernoulli(mu)) {
      EV_INFO << "Host " << src_address->toModuleId().getId() << " recovers from infection\n";
      infection_time = omnetpp::simTime() - infection_time;
      emit(infection_time_signal, infection_time);
      emit(last_status_signal, status);
      cancelEvent(information_timer);
      status = InformationPropagationBase::NOT_INFECTED;
      if (hasGUI())
        getParentModule()->bubble("Get not infected!");
    }
    else
      EV_INFO << "Host " << src_address->toModuleId().getId() << " fails to recover from infection\n";
  }
  else if (status == InformationPropagationBase::NOT_INFECTED)
    EV_INFO << "Host " << src_address->toModuleId().getId() << " is not infected\n";
  else
    throw omnetpp::cRuntimeError(
      "APP: not infected host %d tries to recover", src_address->toModuleId().getId()
    );
}

void InformationPropagationApp::process_packet(inet::Packet* pkt)
{
  if (status == InformationPropagationBase::NOT_INFECTED) {
    auto pkt_header = inet::dynamicPtrCast<inet::InfoPacket>(
      pkt->popAtFront<inet::InfoPacket>()->dupShared()
    );
    if (bernoulli(eta)) {
      EV_INFO << "Host " << src_address->toModuleId().getId() 
              << " received an infectious message from " 
              << pkt_header->getIdentifer() << "\n";
      infection_time = omnetpp::simTime() - infection_time;
      emit(infection_time_signal, infection_time);
      emit(last_status_signal, status);
      status = InformationPropagationBase::INFECTED;
      if (hasGUI())
       getParentModule()->bubble("Get infected!");
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
    EV_INFO << "Host " << src_address->toModuleId().getId() << " gets " 
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
    EV_INFO << "Initial status of host " << src_address->toModuleId().getId() 
            << " is " << status << '\n';
}