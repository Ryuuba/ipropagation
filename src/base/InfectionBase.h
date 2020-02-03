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


class InfectionBase : public omnetpp::cSimpleModule
{
protected:
  /** @brief The possible node status for this infection app. This enum could 
   *  be extended if the first you override it */
  enum Status {
    INFECTED,          //Nodes broadcasted infectious messages to its neighbors
    NOT_INFECTED       //Nodes listen to the wireless channel
  };
  /** @brief The current status of nodes running the infection app */
  Status status;
  /** @brief The rate at which hosts report their status */
  omnetpp::simtime_t status_report_interval;
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
  /** @brief Timer to set the next message */
  omnetpp::cMessage* message_timer;
  /** @brief Timer to set the next recovery attempt */
  omnetpp::cMessage* recovery_timer;
  /** @brief The duration of the infection period */
  omnetpp::cMessage* status_timer;
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
                              status_signal;
protected:
  /** @brief Sends infectious messages to nodes in N(x) following a 
   *  transferring communication methos (unicast, broadcast, multicast, anycast)
   * */
  virtual void send_message(omnetpp::cMessage*) = 0;
  /** @brief Tries to recovery from an infection */
  virtual void try_recovery(omnetpp::cMessage*) = 0;
  /** @brief Process the received packet */
  virtual void process_packet(omnetpp::cMessage*) = 0;
  /** @brief Emits the host status */
  virtual void emit_status(omnetpp::cMessage*) = 0;
public:
  /** @brief Default constructor, initializes all attributes */
  InfectionBase();
  /** @brief Default destructor, derived classes must cancel and delete 
   *  self-messages */
  virtual ~InfectionBase();
};

#endif /* INFECTION_BASE_H */
