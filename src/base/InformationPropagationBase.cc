//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "../base/InformationPropagationBase.h"

Register_Abstract_Class(InformationPropagationBase);

omnetpp::simsignal_t InformationPropagationBase::sent_message_signal = 
  registerSignal("sentMessage");
omnetpp::simsignal_t InformationPropagationBase::received_message_signal = 
  registerSignal("receivedMessage");
omnetpp::simsignal_t InformationPropagationBase::last_status_signal = 
  registerSignal("lastStatus");
omnetpp::simsignal_t InformationPropagationBase::infection_time_signal = 
  registerSignal("infectionTime");

InformationPropagationBase::InformationPropagationBase()
  : destination_list(std::make_shared<inet::DestinationList>())
  , mu(0.0)
  , eta(0.0)
  , lambda(0)
  , step_time(0.0)
  , trial_num(0)
  , diff_time(0.0)
  , infection_time(0.0)
  , sent_messages(0)
  , received_messages(0)
  , step_timer(nullptr)
{
  
}

InformationPropagationBase::~InformationPropagationBase()
{
  if (step_timer)
    cancelAndDelete(step_timer);
  if (information_timer)
    cancelAndDelete(information_timer);
  if (socket) {
    delete socket;
    socket = nullptr;
  }
}

void InformationPropagationBase::initialize(int stage) {
  if (stage == inet::INITSTAGE_LOCAL) {
    input_gate_id = gate("inputSocket")->getId();
    output_gate_id = gate("outputSocket")->getId();
    step_timer = new omnetpp::cMessage("step timer");
    information_timer = new omnetpp::cMessage("information timer");
    auto cache_module = getSimulation()->getSystemModule()->
      getSubmodule("node", inet::getContainingNode(this)->getIndex())->
      getSubmodule("net")->getSubmodule("cache");
    neighbor_cache = static_cast<NeighborCache*>(cache_module);
    netw_protocol = &inet::Protocol::probabilistic;
  }
  else if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
    auto id = inet::getContainingNode(this)->getIndex() + 1; //unicast address
    src_address = std::make_unique<inet::ModuleIdAddress>(id);
    socket = new inet::L3Socket(netw_protocol, gate(output_gate_id));
    socket->bind(&inet::Protocol::information, inet::ModuleIdAddress(id));
    socket->setCallback(this);
  }
}

void InformationPropagationBase::draw_neighbor(size_t max) {
  destination_list->clear();
  if (max > 0) {
    std::list<cache_register> neighborhood = *(neighbor_cache->get());
    if (max < neighborhood.size())
      while (destination_list->size() < max) {
        auto it = neighborhood.begin();
        std::advance(it, intuniform(0, neighborhood.size() - 1));
        destination_list->push_back( it->netw_address );
        neighborhood.erase(it);
      }
    else
      for (auto&& entry : neighborhood)
        destination_list->push_back(entry.netw_address);
  }
}

void InformationPropagationBase::socketDataArrived(
  inet::L3Socket* socket, 
  inet::Packet* pkt
) {

  auto pkt_protocol = pkt->getTag<inet::PacketProtocolTag>()->getProtocol();
  if (pkt_protocol == &inet::Protocol::information) {
    auto pkt_header = inet::dynamicPtrCast<inet::InfoPacket>(
      pkt->popAtFront<inet::InfoPacket>()->dupShared()
    );
    auto it = std::find(
      pkt_header->getDestination_list_ptr()->begin(),
      pkt_header->getDestination_list_ptr()->end(),
      inet::L3Address(*src_address)
    );
    if (it != pkt_header->getDestination_list_ptr()->end()){
      EV_INFO << "InformationPropagationBase: Node "<< src_address->getId() 
              << " receives packet with name " << pkt->getName() 
              << " from " << pkt_header->getSrc() << '\n';
          process_packet(pkt_header);
    }
    else 
      EV_INFO << "Discarting packet from " << pkt_header->getSrc() << '\n';
  }
  else
    throw omnetpp::cRuntimeError(
      "InformationPropagationBase: Unaccepted packet %s(%s)", pkt->getName(), pkt->getClassName()
    );
  delete pkt;
}

void InformationPropagationBase::socketClosed(inet::L3Socket* s) {
  EV_INFO << "InformationPropagationBase: socket has been closed\n";
  if (s == socket) {
    socket->close();
    delete socket;
    socket = nullptr;
  }
}

const char* InformationPropagationBase::status_to_string(Status s) {
  switch (s)
  {
  case INFECTED:
    return "INFECTED";
    break;
  case NOT_INFECTED:
    return "NOT_INFECTED";
    break;
  default:
    throw omnetpp::cRuntimeError(
      "InformationPropagationBase: invalid conversion to string from status %d", s);
    break;
  } 
}