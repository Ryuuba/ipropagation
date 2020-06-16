/*
 *  Based on: ProbabilisticBroadcast.h
 *      by: Damien Piguet
 *  Modified on: Apr, 2020
 *      By: Adán G. Medrano-Chávez
 * 
 */
#include "ProbabilisticCast.h"


using std::make_pair;
using std::endl;

long ProbabilisticCast::id_counter = 0;
omnetpp::simsignal_t ProbabilisticCast::recv_pkt_signal 
  = registerSignal("recvPktNum");
omnetpp::simsignal_t ProbabilisticCast::sent_pkt_signal 
  = registerSignal("sentPktNum");
omnetpp::simsignal_t ProbabilisticCast::fwd_pkt_signal 
  = registerSignal("fwdPktNum");
omnetpp::simsignal_t ProbabilisticCast::hop_count_signal 
  = registerSignal("hopCount");
omnetpp::simsignal_t ProbabilisticCast::drp_pkt_signal 
  = registerSignal("drpPktNum");
omnetpp::simsignal_t ProbabilisticCast::qtime_signal 
  = registerSignal("qTime");

Define_Module(ProbabilisticCast);

ProbabilisticCast::ProbabilisticCast()
  : forwarding(false)
  , communication_mode()
  , hop_limit(0)
  , header_length(0)
  , beta(1.0)
  , eta(1.0)
  , broadcast_period(0.0)
  , max_first_bcast_backoff(0.0)
  , src_address(nullptr)
  , broadcast_timer(nullptr)
  , forwarding_list(std::make_shared<DestinationList>())
  , cache(nullptr)
  , recv_pkt_num(0)
  , sent_pkt_num(0)
  , fwd_pkt_num(0)
  , hop_count(0)
  , drp_pkt_num(0)
  , queueing_time(0.0)
{ }

ProbabilisticCast::~ProbabilisticCast() {
  cancelAndDelete(broadcast_timer);
  // if some messages are still in the queue, delete them.
  for (auto&& item : msg_queue)
    if (item.second)
      delete item.second;
  msg_queue.clear();
}


void ProbabilisticCast::initialize(int stage)
{
  NetworkProtocolBase::initialize(stage);
  if (stage == inet::INITSTAGE_LOCAL) {
    forwarding = par("forwarding").boolValue();
    auto comm_mode_str = par("communicationMode").stringValue();
    communication_mode
      = strcmp(comm_mode_str, "BROADCAST") == 0 ? BROADCAST
      : strcmp(comm_mode_str, "MULTICAST") == 0 ? MULTICAST
      : UNICAST; //The default mode is unicast
    hop_limit = par("hopLimit").intValue();
    header_length = par("headerLength").intValue();
    beta = par("beta").doubleValue();
    eta = par("eta").doubleValue();
    broadcast_period = par("bcperiod");
    max_first_bcast_backoff = par("maxFirstBcastBackoff");
    broadcast_timer = new omnetpp::cMessage("broadcastTimer");
    auto cache_module = getSimulation()->getSystemModule()->
      getSubmodule("node", inet::getContainingNode(this)->getIndex())->
      getSubmodule("net")->getSubmodule("cache");
    cache = static_cast<NeighborCache*>(cache_module);
  }
  else if (stage == inet::INITSTAGE_NETWORK_LAYER) {
    // unicast address
    auto id = inet::getContainingNode(getParentModule())->getIndex() + 1;
    src_address = std::make_unique<inet::ModuleIdAddress>(id);
    // TODO get network_address in interface table
  }
}

void ProbabilisticCast::handleUpperPacket(inet::Packet *packet)
{
  // encapsulate message in a network layer packet.
  encapsulate(packet);
  auto netw_header = packet->peekAtFront<inet::ProbabilisticCastHeader>();
  EV_INFO << "ProbabilisticCast: at " << omnetpp::simTime() << " host " 
          << *src_address << "calls handleUpperPacket(): Pkt ID = " 
          << netw_header->getId() << " hop count = " 
          << netw_header->getHopCount() << endl;
  omnetpp::simtime_t bcast_delay 
    = broadcast_period < max_first_bcast_backoff ? broadcast_period
    : uniform(0, max_first_bcast_backoff);
  insert_msg(packet, bcast_delay);
}

void ProbabilisticCast::handleLowerPacket(inet::Packet *packet)
{
  inet::MacAddress macSrcAddr;
  auto netw_header = 
    inet::dynamicPtrCast<inet::ProbabilisticCastHeader>(
      packet->peekAtFront<inet::ProbabilisticCastHeader>()->dupShared()
    );
  auto macAddressInd = packet->getTag<inet::MacAddressInd>();
  netw_header->setHopCount(netw_header->getHopCount() + 1);
  macSrcAddr = macAddressInd->getSrcAddress();
  delete packet->removeControlInfo();
  hop_count = netw_header->getHopCount();
  // emit statistic
  if (netw_header->getHopCount() < hop_limit) {
    if (is_in_queue(netw_header->getId())) {
      // we got this message already, ignore it.
      EV_INFO << "ProbabilisticCast: at " << omnetpp::simTime() << " host " 
              << *src_address 
              << " calls handleLowerPacket(): Dead or seen message ID = " 
              << netw_header->getId() << " from node "
              << macSrcAddr << " hop count = " << netw_header->getHopCount() 
              << endl;
      delete packet;
    }
    else { //hop limit is not superado
      // Finds whether this node is in the destination list
      auto it = std::find(
        netw_header->getForwardingList()->begin(),
        netw_header->getForwardingList()->end(),
        inet::L3Address(*src_address)
      );
      if (it != netw_header->getForwardingList()->end()) {
        if (bernoulli(eta)) {
          // This host is in the forwarding list
          EV_INFO << "ProbabilisticCast: at " << omnetpp::simTime() << " host " 
                  << *src_address
                  << " receives unseen message with ID = " 
                  << netw_header->getId() << " from node "
                  << macSrcAddr << endl;
          std::cout << "Test\n";
          auto pkt_copy = packet->dup();
          std::cout << "Packet size: " << pkt_copy->getDataLength() << "\n";
          decapsulate(pkt_copy);
          std::cout << "OK1\n";
          sendUp(pkt_copy); //Pass packet to the application layer
          std::cout << "OK2\n";
          recv_pkt_num++;
        }
        else{
          EV_INFO << "ProbabilisticCast: at " << omnetpp::simTime() 
                  << " host " << *src_address
                  << " discards message by a Bernoulli process\n"
                  << " ID = " << netw_header->getId() << " from node "
                  << macSrcAddr << endl;
          drp_pkt_num++;
          delete packet;
        }
      }
      else {
        if (forwarding) {
          EV_INFO << "ProbabilisticCast: at " << omnetpp::simTime() 
                  << " host " << *src_address
                  << " receices unseen message to forward it\n"
                  << "ID = " << netw_header->getId() << " from node "
                  << macSrcAddr << endl;
          // Unseen message. Insert message in queue with random backoff 
          // broadcast delay. Because we got the message from lower layer, we 
          // need to create and add a new control info with the MAC destination 
          // address = broadcast address.
          set_ctrl_info(packet, inet::MacAddress::BROADCAST_ADDRESS);
          // before inserting message, update source address
          // increment the hop count
          netw_header->setSourceAddress(*src_address);
          netw_header->setHopCount(netw_header->getHopCount() + 1);
          packet->insertAtFront(netw_header);
          omnetpp::simtime_t bcast_delay 
            = broadcast_period < max_first_bcast_backoff ? broadcast_period
            : uniform(0, max_first_bcast_backoff);
          insert_msg(packet, bcast_delay);
          recv_pkt_num++;
        }
        else {
          EV_INFO << "ProbabilisticCast: at " << omnetpp::simTime() 
                  << " host " << *src_address
                  << " discards message ID = " 
                  << netw_header->getId() << " from node "
                  << macSrcAddr << endl;
          drp_pkt_num++;
          delete packet;
        }
      }
    }
  }
  else {
    EV_INFO << "ProbabilisticCast: at " << omnetpp::simTime() << " host " 
            << *src_address
            << " discards message with ID " << netw_header->getId() << '\n'
            << " hop count " << netw_header->getHopCount()  << '\n'
            << " from node " << macSrcAddr << endl;
    drp_pkt_num++;
    delete packet;
  }
}

void ProbabilisticCast::handleSelfMessage(omnetpp::cMessage *msg)
{
  if (msg == broadcast_timer) {
    auto packet = pop_msg();
    if (bernoulli(beta)) {
      auto netwHeader = packet->peekAtFront<inet::ProbabilisticCastHeader>();
      if (netwHeader->getHopCount() == 0)
        fwd_pkt_num++;
      else
        sent_pkt_num++;
      packet->setTimestamp();
      sendDown(packet);
      EV_INFO << "ProbabilisticCast: at " << omnetpp::simTime() << " host " 
              << *src_address 
              << " pass the Bernoulli test. Send packet down." << endl;
    }
    else {
      EV << "ProbabilisticCast: at " << omnetpp::simTime() << " host " 
      << *src_address << "does not pass the Benoulli test." << endl;
      delete packet;
    }
    if (!msg_queue.empty())
      scheduleAt(msg_queue.begin()->first, broadcast_timer);
  }
  else
    EV_ERROR << "ProbabilisticCast: at " << omnetpp::simTime() 
             << " node " << *src_address << " received unexpected self message" 
             << endl;
}

bool ProbabilisticCast::is_in_queue(unsigned int msgId)
{
  return msg_id_set.find(msgId) != msg_id_set.end();
}

void ProbabilisticCast::insert_msg(
  inet::Packet* pkt,
  omnetpp::simtime_t_cref bcast_delay
) {
  omnetpp::simtime_t bcast_time = omnetpp::simTime() + bcast_delay;
  EV_INFO << "ProbabilisticCast: at " << omnetpp::simTime() << " host " 
          << *src_address << "inserts message assign it a broadcast delay\n" 
          << " Msg ID = " << pkt->getId() << endl;
  // insert message ID in ID list.
  auto netw_header = pkt->peekAtFront<inet::ProbabilisticCastHeader>();
  msg_id_set.insert(netw_header->getId());
  // insert key value pair <broadcast time, pointer to message> in message queue.
  auto pos = msg_queue.insert(make_pair(bcast_time, pkt));
  // if the message has been inserted in the front of the list, it means that 
  // it will be the next message to be broadcasted, therefore we have to 
  // re-schedule the broadcast timer to the message's broadcast instant.
  if (pos == msg_queue.begin()) {
    EV_INFO << "ProbabilisticCast: at " << omnetpp::simTime() << " host " 
            << *src_address << "inserts at front, reschedule it." << endl;
    cancelEvent(broadcast_timer);
    scheduleAt(bcast_time, broadcast_timer);
  }
}

inet::Packet* ProbabilisticCast::pop_msg( )
{
  inet::Packet* pkt(nullptr);
  if (!msg_queue.empty()) {
    auto it = msg_queue.begin();
    pkt = it->second;
    // remove first message from message queue and from ID list
    msg_queue.erase(it);
    auto netw_header = pkt->peekAtFront<inet::ProbabilisticCastHeader>();
    msg_id_set.erase(netw_header->getId());

    EV_INFO << "ProbabilisticCast: at " << omnetpp::simTime() << " host " 
            << *src_address << " pops msg with ID" 
            << netw_header->getId() << endl;
  }
  else
    throw omnetpp::cRuntimeError(
      "ProbabilisticCast: trying pop when queue is empty"
    );
  return pkt;
}

void ProbabilisticCast::encapsulate(inet::Packet *packet)
{
  auto netw_header = inet::makeShared<inet::ProbabilisticCastHeader>();
  compute_forwarding_list();
  cObject *controlInfo = packet->removeControlInfo();
  netw_header->setInitialSrcAddr(*src_address);
  netw_header->setSourceAddress(*src_address);
  netw_header->setForwardingList(forwarding_list);
  netw_header->setId(getNextID());
  netw_header->setHopCount(0);
  netw_header->setChunkLength(inet::B(header_length));
  netw_header->setProtocol(packet->getTag<inet::PacketProtocolTag>()->getProtocol());
  netw_header->setPayloadLengthField(packet->getDataLength());
  // cleans-up
  delete controlInfo;
  //encapsulates the application packet
  packet->insertAtFront(netw_header);
  set_ctrl_info(packet, inet::MacAddress::BROADCAST_ADDRESS);
}

void ProbabilisticCast::decapsulate(inet::Packet* packet)
{
  //packet->peekAtFront<inet::ProbabilisticCastHeader>()->dupShared()
  auto network_header = inet::dynamicPtrCast<inet::ProbabilisticCastHeader>(packet->popAtFront<inet::ProbabilisticCastHeader>()->dupShared());
  packet->trim();
  auto payload_length = network_header->getPayloadLengthField();
  if (packet->getDataLength() < payload_length)
    throw omnetpp::cRuntimeError("Data error: illegal payload length");
  if (packet->getDataLength() > payload_length)
    packet->setBackOffset(packet->getFrontOffset() + payload_length);
  packet->addTagIfAbsent<inet::NetworkProtocolInd>()->setProtocol(&getProtocol());
  packet->addTagIfAbsent<inet::NetworkProtocolInd>()
    ->setNetworkProtocolHeader(network_header);
  auto payload_protocol = network_header->getProtocol();
  packet->addTagIfAbsent<inet::DispatchProtocolReq>()
    ->setProtocol(payload_protocol);
  packet->addTagIfAbsent<inet::PacketProtocolTag>()
    ->setProtocol(payload_protocol);
  packet->addTagIfAbsent<inet::L3AddressInd>()
    ->setSrcAddress(network_header->getSourceAddress());
}

/**
 * Attaches a "control info" structure (object) to the down message pMsg.
 */
void ProbabilisticCast::set_ctrl_info(
  inet::Packet *const pMsg, 
  const inet::MacAddress& pDestAddr
) {
  pMsg->addTagIfAbsent<inet::MacAddressReq>()->setDestAddress(pDestAddr);
  pMsg->addTagIfAbsent<inet::PacketProtocolTag>()
    ->setProtocol(&inet::Protocol::probabilistic);
  pMsg->addTagIfAbsent<inet::DispatchProtocolInd>()
    ->setProtocol(&inet::Protocol::probabilistic);
}


void ProbabilisticCast::compute_forwarding_list() {
  auto cache_size = cache->size();
  if (cache_size > 0) {
    if (communication_mode == BROADCAST) {
      draw_neighbor(cache_size); //Effectively, it modifies the fwd list
    }
    else if (communication_mode == MULTICAST) {
      size_t list_size 
        = cache->size() == 1 ? 1
        : cache->size() == 2 ? 2
        : cache->size()  > 2 ? intuniform(1, cache_size-1)
        : 0;
      draw_neighbor(list_size); //Effectively, it modifies the fwd list
    }
    else 
      draw_neighbor(1); //Effectively, it modifies the fwd list
  }
  else 
    forwarding_list->clear();
}

void ProbabilisticCast::draw_neighbor(size_t max) {
  forwarding_list->clear();
  if (max > 0) {
    std::list<cache_register> neighborhood = *(cache->get());
    if (max < neighborhood.size())
      while (forwarding_list->size() < max) {
        auto it = neighborhood.begin();
        std::advance(it, intuniform(0, neighborhood.size() - 1));
        forwarding_list->push_back( it->netw_address );
        neighborhood.erase(it);
      }
    else
      for (auto&& entry : neighborhood)
        forwarding_list->push_back(entry.netw_address);
  }
}