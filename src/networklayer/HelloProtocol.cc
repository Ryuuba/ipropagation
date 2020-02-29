#include "HelloProtocol.h"

Define_Module(HelloProtocol);

void HelloProtocol::initialize(int stage) {
  NeighborDiscoveryProtocolBase::initialize(stage);
  if (stage == inet::INITSTAGE_LOCAL) {
    bcast_delay = par("broadcastDelay");
    max_attemps = par("maxAttemptNumber").intValue();
    //gets pointer to the neighbor cache
    node_id = getSimulation()->getSystemModule()->getSubmodule("node")->getIndex();
    auto delay = bcast_delay < discovery_time ? 
                 uniform(0.0, bcast_delay) + discovery_time : 
                 discovery_time;
    scheduleAt(omnetpp::simTime() + delay, discovery_timer);
  }
}

void HelloProtocol::handleMessage(omnetpp::cMessage* msg) {
  if (msg->isSelfMessage()) {
    send_hello_packet(); //Says hello
    //Schedule the next broadcast considering a backoff
    auto delay = bcast_delay < discovery_time ? 
                 uniform(0.0, bcast_delay) + discovery_time : 
                 discovery_time;
    scheduleAt(omnetpp::simTime() + delay, discovery_timer);
  }
  else if (msg->arrivedOn(input_gate_id))
    process_hello_packet(msg);
  else
    throw omnetpp::cRuntimeError(
      "HelloProtocol: unknown event (msg_name: %s)\n",
      msg->getName()
    );
}

void HelloProtocol::send_hello_packet() {
  inet::Packet* hello_pkt = new::inet::Packet("hello");
  const auto& hello_info = inet::makeShared<inet::HelloPacket>();
  hello_info->setChunkLength(inet::B(packet_size));
  hello_info->setType(inet::HelloPacketType::REQ);
  hello_info->setSequenceNum(sequence_number++);
  hello_info->setHostId(node_id);
  hello_info->setSrcMacAddress(mac);
  hello_info->setDstMacAddress(inet::MacAddress::BROADCAST_ADDRESS);
  hello_pkt->insertAtBack(hello_info);
  auto mac_address_request = hello_pkt->addTag<inet::MacAddressReq>();
  mac_address_request->setSrcAddress(mac);
  mac_address_request->setDestAddress(inet::MacAddress::BROADCAST_ADDRESS);
  auto interface_req = hello_pkt->addTag<inet::InterfaceReq>();
  interface_req->setInterfaceId(interface_id);
  auto pktProtocoloTag = hello_pkt->addTagIfAbsent<inet::PacketProtocolTag>();
  pktProtocoloTag->setProtocol(&inet::Protocol::neighborDiscovery);
  send(hello_pkt, output_gate_id);
}

void HelloProtocol::process_hello_packet(omnetpp::cMessage* msg) {
  auto hello_pkt = dynamic_cast<inet::Packet*>(msg);
  auto hello_info = inet::dynamicPtrCast<inet::HelloPacket>(hello_pkt->popAtBack<inet::HelloPacket>()->dupShared());//???
  hello_pkt->trim();//???
  auto macAddressInd = hello_pkt->getTag<inet::MacAddressInd>();//???
  delete hello_pkt->removeControlInfo(); //???
  //Read data packet
  EV_INFO << "Receive hello packet from " 
          << hello_info->getSrcMacAddress() << ' '
          << hello_info->getHostId() << ' '
          << '\n';
  //Determine action
  delete msg;
}