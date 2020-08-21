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

#ifndef INFORMATION_PROPAGATION_BASE_H
#define INFORMATION_PROPAGATION_BASE_H

#include <omnetpp.h>
#include "../contract/IApp.h"
#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/common/L3AddressTag_m.h"

class InformationPropagationBase : public IApp
{
public:
  /** @brief The possible node status for this infection app. This enum could 
   *  be extended if the first you override it */
  enum Status {
    NOT_INFECTED = 0, //Nodes listen to the wireless channel
    INFECTED          //Nodes broadcasted infectious messages to its neighbors
  };
  /** @brief the sort of timers this app reacts */
  enum TimerKind {
    STEP = 100,         //Starts a new round
    SEND_INFORMATION    //Send a message trying to infect a neighbor
  };
  /** @brief Returns a string indicating the host status */
  static const char* status_to_string(Status);
protected:
  /** L3 broadcast address */
  static inet::ModuleIdAddress unspecified_address;
  /** @brief The current status of nodes running the infection app */
  Status status;
  /** @brief The probability of recovery of an infection */
  double mu;
  /** @brief the number of trials per time step */
  int lambda;
  /** @brief The duration of a round */
  omnetpp::simtime_t step_time;
  /** @brief The number of data sending trials during a step */
  int trial_num;
  // @brief The number of rounds
  long long round_num;
  /** @brief The time between two data sending trials */
  omnetpp::simtime_t diff_time;
  /** @brief Statistic: The duration of the infection period */
  omnetpp::simtime_t infection_time;
  /** @brief Statistic: The number of sent messages */
  long sent_messages;
  /** @brief Statistic: The number of received messages */
  long received_messages;
  /** @brief Timer defining when a round starts */
  omnetpp::cMessage* step_timer;
protected: //App signals that carry statistics
  /** @brief 1) Signal carrying the number of sent messages 
   *         2) Signal carrying the number of received messages
   *         3) Signal carrying the host status
   */
  static omnetpp::simsignal_t sent_message_signal, 
                              received_message_signal,
                              last_status_signal,
                              infection_time_signal;
protected:
  /** @brief Computes whether this node starts infected or not */
  virtual void compute_initial_state();
  /** @brief Encapsulates a message to send it via a L3 socket */
  virtual void encapsulate() = 0;
  /** @brief Sends infectious messages to nodes in N(x) */
  virtual void send_message(omnetpp::cMessage*) = 0;
  /** @brief Tries to recovery from an infection */
  virtual void try_recovery(omnetpp::cMessage*) = 0;
  /** @brief Process the received packet */
  virtual void process_packet(inet::Ptr<inet::InfoPacket>) = 0;
protected: //Member functions inherited from INetworkSocket::ICallBack
  /** @brief This memebr function is called back when data arrives thu the 
   *  socket, it process the received data */
  virtual void socketDataArrived(inet::L3Socket*, inet::Packet*) override;
  /** @brief This callback is execute when the socket is closed */
  virtual void socketClosed(inet::L3Socket*) override;
public:
  /** @brief Default constructor, initializes all attributes */
  InformationPropagationBase();
  /** @brief Default destructor, derived classes must cancel and delete 
   *  self-messages */
  virtual ~InformationPropagationBase();
  /**  
   * @brief Returns the number of stages needed to initalize an INET node
   */
  virtual int numInitStages() const override {
    return inet::NUM_INIT_STAGES;
  }
  /** @brief Initializes the module */
  virtual void initialize(int) override;
};

#endif /* INFORMATION_PROPAGATION_BASE_H */
