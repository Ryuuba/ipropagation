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

#include "../base/InfectionBase.h"

Register_Abstract_Class(InfectionBase);

InfectionBase::InfectionBase() : 
  recovery_probability(0.0),
  infection_probability(0.0),
  sent_interval(0.0),
  recovery_timer(nullptr),
  status_timer(nullptr),
  sent_messages(0),
  received_messages(0)
{
  // TODO Auto-generated constructor stub
}

InfectionBase::~InfectionBase()
{
  if (information_timer)
    cancelAndDelete(information_timer);
  if (recovery_timer)
    cancelAndDelete(recovery_timer);
  if (status_timer)
    cancelAndDelete(status_timer);
  if (socket) {
    delete socket;
    socket = nullptr;
  }
  if (src_address)
    delete src_address;
}

void InfectionBase::initialize(int stage) {
  if (stage == inet::INITSTAGE_LOCAL) {
    input_gate_id = gate("inputSocket")->getId();
    output_gate_id = gate("outputSocket")->getId();
    information_timer = new omnetpp::cMessage("information timer");
    recovery_timer = new omnetpp::cMessage("recovery timer");
    status_timer = new omnetpp::cMessage("status timer");
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

inet::L3Address InfectionBase::draw_neighbor() {
  auto cache = neighbor_cache->get_neighbor_cache();
  inet::L3Address neighbor_address;
  if (!cache->empty()) {
    auto it = cache->begin();
    std::advance(it, intuniform(0, cache->size() - 1));
    neighbor_address.set(it->netw_address);
  }
  return neighbor_address;
}

void InfectionBase::socketDataArrived(
  inet::L3Socket* socket, 
  inet::Packet* pkt
) {
  auto pkt_protocol = pkt->getTag<inet::PacketProtocolTag>()->getProtocol();
  EV_INFO << "InfectionBase: Receiving packet with name " 
          << pkt->getName() << '\n';
  if (pkt_protocol == &inet::Protocol::information)
    process_packet(pkt);
  else
    throw omnetpp::cRuntimeError(
      "InfectionBase: Unaccepted packet %s(%s)", pkt->getName(), pkt->getClassName()
    );
}

void InfectionBase::socketClosed(inet::L3Socket* s) {
  std::cout << "InfectionBase: socket has been closed\n";
  if (s == socket) {
    socket->close();
    delete socket;
    socket = nullptr;
  }
}