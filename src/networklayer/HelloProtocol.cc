#include "HelloProtocol.h"

Define_Module(HelloProtocol);

void HelloProtocol::initialize(int stage) {
  NeighborDiscoveryProtocolBase::initialize(stage);
  if (stage == inet::INITSTAGE_LOCAL) {
    bcast_delay = par("broadcastDelay");
    max_attemps = par("maxAttemptNumber").intValue();
    //gets pointer to the neighbor cache
    node_id = getSimulation()->getSystemModule()->getSubmodule("node")->getIndex();
    auto cache_module = getSimulation()->getSystemModule()->getSubmodule("node")->getSubmodule("net")->getSubmodule("cache");
    neighbor_cache = static_cast<NeighborCache*>(cache_module);
    auto delay = bcast_delay < discovery_time ? 
                 uniform(0.0, bcast_delay) + discovery_time : 
                 discovery_time;
    scheduleAt(omnetpp::simTime() + delay, discovery_timer);
  }
}

void HelloProtocol::handleMessage(omnetpp::cMessage* msg) {
  if (msg->isSelfMessage()) {
    //Says hello
    send_hello_packet();
    //Mitigate somehow packet collisions
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
  hello_info->setType = inet::HelloPacketType::REQ;
  hello_info->setSequenceNum(sequence_number++);
  hello_info->setHostId(node_id);
  //hello_info->setSrcMacAddress();
  hello_info->setDstMacAddress(inet::MacAddress::BROADCAST_ADDRESS);
  hello_pkt->addTagIfAbsent<inet::DispatchProtocolReq>()->setProtocol(&inet::Protocol::neighborDiscovery);
}

void HelloProtocol::process_hello_packet(omnetpp::cMessage* msg) {

}