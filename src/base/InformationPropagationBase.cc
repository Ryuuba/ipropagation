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

inet::ModuleIdAddress InformationPropagationBase::unspecified_address(0);


omnetpp::simsignal_t InformationPropagationBase::sent_message_signal 
  = registerSignal("sentMessage");
omnetpp::simsignal_t InformationPropagationBase::recv_msg_signal
  = registerSignal("recvMessage");
omnetpp::simsignal_t InformationPropagationBase::last_status_signal
  = registerSignal("lastStatus");
omnetpp::simsignal_t InformationPropagationBase::infection_time_signal
  = registerSignal("infectionTime");
omnetpp::simsignal_t InformationPropagationBase::src_id_signal
  = registerSignal("srcId");

InformationPropagationBase::InformationPropagationBase()
  : mu(0.0)
  , lambda(0)
  , step_time(0.0)
  , trial_num(0)
  , round_num(0)
  , diff_time(0.0)
  , infection_time(0.0)
  , sent_msg(0)
  , recv_msg(0)
  , step_timer(nullptr)
{
  
}

InformationPropagationBase::~InformationPropagationBase()
{
  cancelAndDelete(step_timer);
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
    netw_protocol = &inet::Protocol::probabilistic;
    mu = par("recoveryProbability");
    lambda = par("numberOfTrials").intValue();
    step_time = par("step");
  }
  else if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
    auto id = inet::getContainingNode(this)->getIndex() + 1; //unicast address
    src_address = std::make_unique<inet::ModuleIdAddress>(id);
    socket = new inet::L3Socket(netw_protocol, gate(output_gate_id));
    socket->bind(&inet::Protocol::information, inet::ModuleIdAddress(id));
    socket->setCallback(this);
    compute_initial_state();
    EV_INFO << "The status of host " << src_address->getId() 
            << " is " << status_to_string(status) << '\n';
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
    EV_INFO << "InformationPropagationBase: Node "<< src_address->getId() 
            << " receives packet with name " << pkt->getName() 
            << " from " << pkt_header->getSrc() << '\n';
    process_packet(pkt_header);
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

void InformationPropagationBase::compute_initial_state() {
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