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

#ifndef INFECTION_BASE_H
#define INFECTION_BASE_H

#include <omnetpp.h>
#include "../contract/IApp.h"
#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/common/L3AddressTag_m.h"

class InfectionBase : public IApp
{
public:
  /** @brief The possible node status for this infection app. This enum could 
   *  be extended if the first you override it */
  enum Status {
    NOT_INFECTED = 0, //Nodes listen to the wireless channel
    INFECTED = 1      //Nodes broadcasted infectious messages to its neighbors
  };
protected:
  /** @brief The current status of nodes running the infection app */
  Status status;
  /** @brief The rate at which hosts report their status */
  omnetpp::simtime_t status_report_interval;
  /** @brief The maximum number of neighbors to be infected */
  int infection_degree;
  /** @brief The probability of recovery of infection (mu) */
  double recovery_probability;
  /** @brief The probability of getting infected after receiving a msg (eta) */
  double infection_probability;
  /** @brief The rate at which infectious packets are sent per second. This 
   *  time interval could be drawn from a probability distribution. By default, 
   *  this time is constant */
  omnetpp::simtime_t sent_interval;
  /** @brief The rate at which infectious packets are sent per second */
  omnetpp::simtime_t recovery_interval;
  /** @brief Timer to set the next recovery attempt */
  omnetpp::cMessage* recovery_timer;
  /** @brief The duration of the infection period */
  omnetpp::simtime_t infection_time;
  /** @brief The number of sent messages */
  long sent_messages;
  /** @brief The number of received messages */
  long received_messages;
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
  /** @brief Returns a string indicating the host status */
  const char* status_to_string(Status);
  /** @brief Draws a neighbor from neighbor cache if it is possible */
  inet::L3Address draw_neighbor();
  /** @brief Encapsulates a message to send it via a L3 socket */
  virtual void encapsulate(const inet::L3Address&) = 0;
  /** @brief Sends infectious messages to nodes in N(x) */
  virtual void send_message(omnetpp::cMessage*) = 0;
  /** @brief Tries to recovery from an infection */
  virtual void try_recovery(omnetpp::cMessage*) = 0;
  /** @brief Emits the host status */
  virtual void emit_status(omnetpp::cMessage*) = 0;
  /** @brief Process the received packet */
  virtual void process_packet(inet::Packet*) = 0;
protected: //Member functions inherited from INetworkSocket::ICallBack
  /** @brief This memebr function is called back when data arrives thu the 
   *  socket, it process the received data */
  virtual void socketDataArrived(inet::L3Socket*, inet::Packet*) override;
  /** @brief This callback is execute when the socket is closed */
  virtual void socketClosed(inet::L3Socket*) override;
public:
  /** @brief Default constructor, initializes all attributes */
  InfectionBase();
  /** @brief Default destructor, derived classes must cancel and delete 
   *  self-messages */
  virtual ~InfectionBase();
  /**  
   * @brief Returns the number of stages needed to initalize an INET node
   */
  virtual int numInitStages() const override {
    return inet::NUM_INIT_STAGES;
  }
  /** @brief Initializes the module */
  virtual void initialize(int) override;
};

#endif /* INFECTION_BASE_H */
