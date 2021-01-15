#include "HelloProtocol.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"

Define_Module(HelloProtocol);

void HelloProtocol::initialize(int stage) {
  NeighborDiscoveryProtocolBase::initialize(stage);
  if (stage == inet::INITSTAGE_LOCAL) {
    is_one_way = par("isOneWay").boolValue();
    if (is_one_way)
      pkt_type = inet::HelloPacketType::ADV;
    else
      pkt_type = inet::HelloPacketType::REP;
    is_static_network = par("isStaticNetwork").boolValue();
    bcast_delay_max = par("maximumBcastDelay");
    max_attemps = par("maxAttemptNumber").intValue();
    flush_delay = par("flushDelay");
    discovery_timer = new omnetpp::cMessage(
      "discoverytimer", 
      TimerKind::DISCOVERY
    );
    if (bcast_delay_max >= discovery_time)
      throw omnetpp::cRuntimeError(
        "HelloProtocol: Maximum broadcast delay must be less than discovery time"
      );
    else {
      backoff_timer = bcast_delay_max >= 0
                    ? new omnetpp::cMessage("backofftimer", TimerKind::BACKOFF)
                    : nullptr;
    }
    scheduleAt(omnetpp::simTime(), discovery_timer);
    flush_timer = new omnetpp::cMessage("flushtimer", TimerKind::FLUSH);
    scheduleAt(omnetpp::simTime() + flush_delay, flush_timer);
    WATCH(*neighbor_cache);
  }
}

void HelloProtocol::handleMessage(omnetpp::cMessage* msg) {
  if (
    !is_static_network && omnetpp::simTime() > getSimulation()->getWarmupPeriod()
  )
    return;
  if (msg->isSelfMessage()) {
    switch (msg->getKind()) {
      case TimerKind::DISCOVERY :
        scheduleAt(omnetpp::simTime() + discovery_time, discovery_timer);
        neighbor_cache->invalid_cache();
        if (bcast_delay_max > 0.0) {
          EV_INFO << "Node[" << getParentModule()->getParentModule()->getIndex()
                  << "] schedules BACKOFF timer\n";
          scheduleAt(omnetpp::simTime() + backoff(), backoff_timer);
        }
        else {
          EV_INFO << "Node[" << getParentModule()->getParentModule()->getIndex()
          << "] sends hello\n";
          scheduleAt(omnetpp::simTime() + backoff(), backoff_timer);
        }
          send_hello_packet(pkt_type); //Says hello
        break;
      case TimerKind::BACKOFF :
        EV_INFO << "Node[" << getParentModule()->getParentModule()->getIndex()
        << "] sends hello\n";
        send_hello_packet(pkt_type); //Says hello
        break;
      case TimerKind::FLUSH :
        neighbor_cache->emit();
        neighbor_cache->flush_cache();
        scheduleAt(omnetpp::simTime() + flush_delay, flush_timer);
        break;
      default:
        throw omnetpp::cRuntimeError(
          "HelloProtocol: unknown timer kind (msg_name: %d)\n",
          msg->getKind()
        );
        break;
    }
  }
  else if (msg->arrivedOn(input_gate_id))
    process_hello_packet(msg);
  else
    throw omnetpp::cRuntimeError(
      "HelloProtocol: unknown event (msg_name: %s)\n",
      msg->getName()
    );
}

void HelloProtocol::send_hello_packet(inet::HelloPacketType type) {
  inet::Packet* hello_pkt = new inet::Packet("hello");
  auto hello_info = inet::makeShared<inet::HelloPacket>();
  hello_info->setChunkLength(inet::B(packet_size));
  hello_info->setType(type);
  hello_info->setSequenceNum(sequence_number++);
  hello_info->setNodeIndex(node_index);
  hello_info->setSrcNetwAddress(inet::L3Address(*netw_address));
  hello_info->setSrcMacAddress(mac);
  hello_info->setDstMacAddress(inet::MacAddress::BROADCAST_ADDRESS);
  hello_pkt->insertAtFront(hello_info);
  hello_pkt->addTagIfAbsent<inet::MacAddressReq>()->setSrcAddress(mac);
  hello_pkt->addTagIfAbsent<inet::MacAddressReq>()->
    setDestAddress(inet::MacAddress::BROADCAST_ADDRESS);
  hello_pkt->addTagIfAbsent<inet::InterfaceReq>()->
    setInterfaceId(interface_id);
  hello_pkt->addTagIfAbsent<inet::PacketProtocolTag>()->
    setProtocol(&inet::Protocol::neighborDiscovery);
  send(hello_pkt, output_gate_id);
  EV_INFO << "HelloProtocol: hello pkt has been sent\n";
}

void HelloProtocol::process_hello_packet(omnetpp::cMessage* msg) {
  auto hello_pkt = dynamic_cast<inet::Packet*>(msg);
  auto hello_header = 
    inet::dynamicPtrCast<inet::HelloPacket>(
      hello_pkt->popAtFront<inet::HelloPacket>()->dupShared()
    );//
  EV_INFO << "Node[" << getParentModule()->getParentModule()->getIndex() << ']'
          << " receives hello packet from:\n" 
          << "index: " << hello_header->getNodeIndex() << '\n'
          << "mac: " << hello_header->getSrcMacAddress() << '\n'
          << "module ID: " << hello_header->getSrcNetwAddress().toModuleId()
          << '\n';
  cache_register cache_reg;
  cache_reg.start_time = omnetpp::simTime();
  cache_reg.node_index = hello_header->getNodeIndex();
  cache_reg.netw_address = hello_header->getSrcNetwAddress().toModuleId();
  cache_reg.mac_address = hello_header->getSrcMacAddress();
  cache_reg.last_contact_time = omnetpp::simTime();
  cache_reg.still_connected = true;
  if (
    neighbor_cache->is_in_cache(hello_header->getSrcNetwAddress().toModuleId())
  )
    neighbor_cache->update_last_contact_time(
      hello_header->getSrcNetwAddress().toModuleId(),
      omnetpp::simTime()
    );
  else
    neighbor_cache->push_register(std::move(cache_reg));
  if (hello_header->getType() == inet::HelloPacketType::REQ)
    send_hello_packet(inet::HelloPacketType::REP);
  delete msg;
}

omnetpp::simtime_t HelloProtocol::backoff()  {
  auto delay =  bcast_delay_max >= 0
             ? uniform(0.0, bcast_delay_max)
             : 0.0;
  EV_INFO << "HelloProtocol: hello message will be delayed " << delay << '\n';
  return delay;
}
