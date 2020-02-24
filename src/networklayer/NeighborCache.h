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
//NEIGHBOR

#if !defined(NEIGHBOR_CACHE_H)
#define NEIGHBOR_CACHE_H


#include <list>
#include <omnetpp.h>
#include "inet/linklayer/common/MacAddress.h"

class NeighborCache : public omnetpp::cSimpleModule {
protected:
  std::list<inet::MacAddress> cache;
public:
  NeighborCache() {}
  ~NeighborCache() {}
  /**
   * @brief This module does not receive messages
   * */
  virtual void handleMessage(omnetpp::cMessage* msg) {
    throw omnetpp::cRuntimeError("NeighborCache: This module does not receive any message (name: %s)\n", msg->getName());
  }
  /**  
   * @brief Returns the number of stages needed to initalize an INET node
   */
  virtual int numInitStages() const override {
    return inet::NUM_INIT_STAGES;
  }
  /** @brief Initializes the module parameters from the NED file */
  virtual void initialize(int) override;
};

#endif // NEIGHBOR_CACHE_H





