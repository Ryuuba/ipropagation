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
#include <utility>
#include <ostream>
#include <omnetpp.h>
#include "inet/linklayer/common/MacAddress.h"

class NeighborCache : public omnetpp::cSimpleModule {
protected:
  /** @brief A pair (host_id, macaddress) makes up a cache entry */
  typedef struct {
    int host_id;
    inet::MacAddress mac_address;
  } cache_entry;
  typedef std::list<cache_entry>::iterator cache_it;
  typedef std::list<cache_entry>::const_iterator cache_const_it;
  /** @brief Data structure storing the one-hop neighborhood N(x) of a node x*/
  std::list<cache_entry> cache;
public:
  NeighborCache() {}
  virtual ~NeighborCache() {}
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
  /** @brief Returns a const pointer to access the neighbor cache */
  virtual const std::list<cache_entry>* get_neighbor_cache() {
    const std::list<cache_entry>* ptr = &cache;
    return ptr;
  }
  /** @brief inserts a neighbor in cache */
  virtual void insert_neighbor(cache_entry&);
  /** @brief Returns the begin of the cache for loop-range iteration */
  cache_it begin() {
    return cache.begin();
  }
  /** @brief Returns the end of the cache for loop-range iteration */
  cache_it end() {
    return cache.end();
  }
  /** @brief Returns the begin of the cache for loop-range iteration */
  cache_const_it begin() const{
    return cache.begin();
  }
  /** @brief Returns the end of the cache for loop-range iteration */
  cache_const_it end() const {
    return cache.end();
  }
  /** @brief Allows a WATCH macro to display the neighbor cache of a node */
  friend std::ostream& operator<<(std::ostream&, const NeighborCache&);
};

Define_Module(NeighborCache);

std::ostream& operator<<(std::ostream& os, const NeighborCache& cache) {
  for (auto &&neighbor : cache)
    std::cout << neighbor.host_id << ' ' << neighbor.mac_address << '\n';
}

#endif // NEIGHBOR_CACHE_H





