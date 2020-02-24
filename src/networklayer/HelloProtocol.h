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

#include "../base/NeighborDiscoveryProtocolBase.h"
#include "../msg/HelloPacket_m.h"
#include "NeighborCache.h"

class HelloProtocol : public NeighborDiscoveryProtocolBase {
protected:
  /** @brief A pointer to access the neighbor cache */
  NeighborCache* neighbor_cache;
protected:
  /** @brief Encapsulates a hello message into an INET packet */
  virtual void send_packet();
  /** @brief Modifies the neighbor cache with the hello information */
  virtual void process_packet(omnetpp::cMessage*);
public:
  /** @brief Default constructor */
  HelloProtocol() : neighbor_cache(nullptr) {}
  /** @brief Default desconstructor: cancels and deletes the hello timer */
  virtual ~HelloProtocol() {}
  /** @brief Initializes the module state from a NED file */
  virtual void initialize(int) override;
  /** @brief Handles simulation events (timers and msg) */
  virtual void handleMessage(omnetpp::cMessage*) override;
};

#endif