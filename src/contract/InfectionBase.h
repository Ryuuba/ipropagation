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
   *  be extended if the first new status equals EXTRA_STATUS */
  enum Status {
    STARTING_OPERATION, //Nodes are performing initialization tasks, e.g., obtaining N(x)
    INFECTED,           //Nodes broadcasted infectious messages to its neighbors
    NOT_INFECTED,       //Nodes listen to the wireless channel
    EXTRA_STATUS        //First element from an extended status equals this
  };
  /** @brief The current status of nodes running the infection app */
  Status operational_status;
  /** @brief The time of the last change of status */
  omnetpp::simtime_t last_change;
  /** @brief The probability of recovery of infection (mu) */
  double recovery_probability;
  /** @brief The probability of getting infected after receiving a msg (eta) */
  double infection_probability;
  /** @brief The rate at which infectious packets are sent per second */
  omnetpp::simtime_t broadcast_interval;
  /** @brief The rate at which infectious packets are sent per second */
  omnetpp::simtime_t recovery_interval;
  /** @brief Timer to set the next broadcast */
  omnetpp::cMessage* broadcast_timer;
  /** @brief Timer to set the next recovery attempt */
  omnetpp::cMessage* recovery_timer;
  /** @brief The duration of the infection period */
  long infection_time;
  /** @brief The number of sent messages */
  long sent_messages;
  /** @brief The number of received messages */
  long received_messages;
protected:
  /** @brief Broadcasts infectious messages to nodes in N(x) at a rate of 
   *  broadcast_rate per broadcast_timer (in seconds) */
  virtual void broadcast() = 0;
public:
  /** @brief Default constructor, initializes all attributes */
  InfectionBase();
  /** @brief Default destructor, derived classes must cancel and delete 
   *  self-messages */
  virtual ~InfectionBase();
};

#endif /* INFECTION_BASE_H */
