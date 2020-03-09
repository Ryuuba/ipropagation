//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#if !defined(BROADCAST_INFECTION_APP_H)
#define BROADCAST_INFECTION_APP_H

#include <omnetpp.h>

#include "inet/common/INETDefs.h"
#include "inet/common/packet/Packet.h"
#include "../base/InfectionBase.h"
#include "../msg/InfoPacket_m.h"

class BroadcastInfectionApp : public InfectionBase
{
  protected: //App attributes
    /** @brief The ID of the gate where broadcast messages arrive */
    int input_gate_id;
    /** @brief The ID of the gate where broadcast messages are sent */
    int output_gate_id;
    /** @brief The simulation ID of the host */
    int host_id;
    /** @brief The name of the infectious packet */
    const char* packet_name = "infection";
    /** @brief The length in bytes of the infectious packet */
    int packet_size;
    /** @brief the sort of timers this app reacts */
    enum timer_kind {
      RECOVERY = 100, //Triggers an attempt to recover from an infection
      BROADCAST,      //Triggers the broadcasting of a packet
      STATUS          //Triggers the emittion of the host status
    };
  protected: //App member functions
    /** @brief Broadcasts infectious messages to nodes in N(x) at a rate of 
     *  broadcast_rate per broadcast_timer (in seconds), overridden from
     *  InfectionBase class */
    virtual void send_message(omnetpp::cMessage*) override;
    /** @brief Tries to recovery from an infection */
    virtual void try_recovery(omnetpp::cMessage*) override;
    /** @brief Emits the host status */
    virtual void emit_status(omnetpp::cMessage*) override;
    /** @brief Process the received packet */
    virtual void process_packet(omnetpp::cMessage*) override;
    /** @brief Handle the timers of this application */
    virtual void handleMessage(omnetpp::cMessage*) override;
    /** @brief INET applications need 12 states to initialize all simulation 
     *  modules. TODO: check this whether this condition is needed or not */
    virtual int numInitStages() const override {return inet::NUM_INIT_STAGES;}
    /** @brief Initializes the app state getting N(x), the initial node 
     *  status, probability distributions, etc.  */
    virtual void initialize(int stage) override;
    /** @brief Changes the icon of the app to indicate the host status */
    virtual void refreshDisplay() const override;
  public:
    /** @brief Default constructor initializing the class parameters */
    BroadcastInfectionApp();
    /** @brief Cancels self-messages (timers) when destructing *this */
    virtual ~BroadcastInfectionApp();

};

#endif // BROADCAST_INFECTION_APP_H
