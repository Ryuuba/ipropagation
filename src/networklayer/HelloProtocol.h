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

#if !defined(HELLO_PROTOCOL_H)
#define HELLO_PROTOCOL_H

#include <omnetpp.h>
#include "inet/common/INETDefs.h"
#include "inet/common/packet/Packet.h"
#include "../msg/HelloPacket_m.h"

class HelloProtocol : public omnetpp::cSimpleModule {
protected:
  /** @brief Timer to send hello messages */
  omnetpp::cMessage* hello_timer;
  /** @brief Time between two consecutive hello transmissions */
  omnetpp::simtime_t hello_delay;
  /** @brief The ID of the input gate of this module */
  int input_gate_id;
  /** @brief The ID of the output gate of this module */
  int output_gate_id;
  /** @brief The size of the packet in bytes */
  int packet_size;
  /** @brief The sequence number of the hello messages */
  int sequence_number;
protected:
  /** @brief Encapsulates a hello message into an INET packet */
  virtual void send_packet();
  /** @briefModifies the neighbor cache with the hello information */
  virtual void process_packet(omnetpp::cMessage*);
public:
  /** @brief Default constructor */
  HelloProtocol();
  /** @brief Default desconstructor: cancels and deletes the hello timer */
  virtual ~HelloProtocol();
  /** @brief Returns the number of stages needed to initalize an INET node */
  virtual int numInitStages() const override {
    return inet::NUM_INIT_STAGES;
  }
  /** @brief Initializes the module state from a NED file */
  virtual void initialize(int) override;
  /** @brief Handles simulation events (timers and msg) */
  virtual void handleMessage(omnetpp::cMessage*) override;
};

#endif