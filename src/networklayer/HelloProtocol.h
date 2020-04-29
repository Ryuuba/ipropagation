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
#include "CacheRegister.h"

class HelloProtocol : public NeighborDiscoveryProtocolBase {
protected:
  enum TimerKind {
    DISCOVERY = 0,
    BACKOFF,
    FLUSH
  };
  /** @brief Maximum number of attempts a node must perform to consider a
   * a neighbor is out of range */
  int max_attemps;
  /** @brief Maximum delay before broadcasting a hello packet */
  omnetpp::simtime_t bcast_delay_max;
  /** @brief Delay to flush stale entries in the neighbor cache */
  omnetpp::simtime_t flush_delay;
  /** @brief Timer to trigger the emision of a hello message */
  omnetpp::cMessage* backoff_timer;
  /** @brief Timer to flush the neighbor cache */
  omnetpp::cMessage* flush_timer;
protected:
  /** @brief Sends a hello packet */
  virtual void send_hello_packet(inet::HelloPacketType) override;
  /** @brief Modifies the neighbor cache with the hello information */
  virtual void process_hello_packet(omnetpp::cMessage*) override;
  /** @brief Computes a random delay to avoid signal collisions */
  virtual omnetpp::simtime_t backoff();
public:
  /** @brief Default constructor */
  HelloProtocol()
    : NeighborDiscoveryProtocolBase()
    , max_attemps(0)
    , bcast_delay_max(0.0)
    , flush_delay(0.0)
    , backoff_timer(nullptr)
    , flush_timer(nullptr)
  { }
  /** @brief Default desconstructor: cancels and deletes the hello timer */
  virtual ~HelloProtocol() {
    cancelAndDelete(discovery_timer);
    cancelAndDelete(backoff_timer);
    cancelAndDelete(flush_timer);
  }
  /** @brief Initializes the module state from a NED file */
  virtual void initialize(int) override;
  /** @brief Handles simulation events (timers and msg) */
  virtual void handleMessage(omnetpp::cMessage*) override;
  /** @brief Sends a hello before the timer in order to update the neighbor 
   *  cache */
  virtual void refreshNeighborCache() { 
    send_hello_packet(inet::HelloPacketType::REQ);
  }
};

#endif