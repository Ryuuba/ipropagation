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
  cancelAndDelete(recovery_timer);
  cancelAndDelete(status_timer);
}

void InfectionBase::initialize(int stage) {
  if (stage == inet::INITSTAGE_LOCAL) {
    input_gate_id = gate("inputPort")->getId();
    output_gate_id = gate("outputPort")->getId();
    information_timer = new omnetpp::cMessage("information timer");
    recovery_timer = new omnetpp::cMessage("recovery timer");
    status_timer = new omnetpp::cMessage("status timer");
  }
  else if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
    inet::registerService(
      inet::Protocol::information,
      nullptr,
      gate(input_gate_id)
    );
    inet::registerProtocol(
      inet::Protocol::information,
      nullptr,
      gate(output_gate_id)
    );
  }
}

void InfectionBase::send_down(inet::Packet* pkt) {
  // sets a SimpleNetworkProtocol as destination protocol
  std::cout << "InfectionBase: sending packet to dispatcher\n";
  pkt->addTagIfAbsent<inet::DispatchProtocolReq>()->
    setProtocol(&inet::Protocol::probabilistic);
  send(pkt, output_gate_id);
}
