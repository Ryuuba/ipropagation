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
  : mu(0.0)
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
  // TODO Auto-generated constructor stub
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
  if (src_address)
    delete src_address;
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
    //Sets Socket
    auto module_index = inet::getContainingNode(this)->getIndex();
    src_address = new inet::L3Address( inet::ModuleIdAddress(module_index) );
    socket = new inet::L3Socket(netw_protocol, gate(output_gate_id));
    socket->bind(&inet::Protocol::information, inet::ModuleIdAddress(module_index));
    socket->setCallback(this);
  }
}

inet::L3Address InformationPropagationBase::draw_neighbor() {
  auto cache = neighbor_cache->get();
  inet::L3Address neighbor_address;
  if (!cache->empty()) {
    auto it = cache->begin();
    std::advance(it, intuniform(0, cache->size() - 1));
    neighbor_address.set(it->netw_address);
  }
  return neighbor_address;
}

std::list<inet::L3Address> InformationPropagationBase::draw_neighbor(size_t max) {
  std::list<inet::L3Address> neighbor_list;
  if (max > 0) {
    std::list<cache_register> neighborhood = *(neighbor_cache->get());
    if (max < neighborhood.size())
      while (neighbor_list.size() < max) {
        auto it = neighborhood.begin();
        std::advance(it, intuniform(0, neighborhood.size() - 1));
        neighbor_list.push_back( it->netw_address );
        neighborhood.erase(it);
      }
    else
      for (auto&& entry : neighborhood)
        neighbor_list.push_back(entry.netw_address);
  }
  return neighbor_list;
}

void InformationPropagationBase::socketDataArrived(
  inet::L3Socket* socket, 
  inet::Packet* pkt
) {
  auto pkt_protocol = pkt->getTag<inet::PacketProtocolTag>()->getProtocol();
  EV_INFO << "InformationPropagationBase: Receiving packet with name " 
          << pkt->getName() << '\n';
  if (pkt_protocol == &inet::Protocol::information)
    process_packet(pkt);
  else
    throw omnetpp::cRuntimeError(
      "InformationPropagationBase: Unaccepted packet %s(%s)", pkt->getName(), pkt->getClassName()
    );
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
  case NOT_INFECTED:
    return "NOT_INFECTED";
  default:
    throw omnetpp::cRuntimeError(
      "InformationPropagationBase: invalid conversion to string from status %d", s);
    break;
  } 
}