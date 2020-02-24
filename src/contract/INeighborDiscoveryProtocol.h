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

#if !defined(I_NEIGHBOR_DISCOVERY_PROTOCOL_H)
#define I_NEIGHBOR_DISCOVERY_PROTOCOL_H

#include <omnetpp.h>

class INeighborDiscoveryProtocol : public omnetpp::cSimpleModule {
protected:
  /** @brief Time between two consecutive discovery operations */
  omnetpp::simtime_t discovery_time;
  /** @brief Timer to trigger a discovery operation */
  omnetpp::cMessage* discovery_timer;
  /** @brief The ID of the input gate of this module */
  int input_gate_id;
  /** @brief The ID of the output gate of this module */
  int output_gate_id;
protected:
  /** @brief Encapsulates a hello message into an INET packet */
  virtual void send_packet() = 0;
  /** @briefModifies the neighbor cache with the hello information */
  virtual void process_packet(omnetpp::cMessage*) = 0;
public:
  INeighborDiscoveryProtocol() : 
    discovery_timer(nullptr),
    discovery_time(0.0),
    input_gate_id(0), output_gate_id(0)
    { }
  virtual ~INeighborDiscoveryProtocol() {
    cancelAndDelete(discovery_timer);
  }
};

Register_Abstract_Class(INeighborDiscoveryProtocol);

#endif // I_NEIGHBOR_DISCOVERY_PROTOCOL_H
