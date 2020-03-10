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

#include <algorithm>
#include <list>
#include <utility>
#include <ostream>
#include <omnetpp.h>
#include "inet/linklayer/common/MacAddress.h"

class NeighborCache : public omnetpp::cSimpleModule {
public:
  /** @brief A pair (host_id, macaddress) makes up a cache entry */
  typedef struct {
    int host_id;
    inet::MacAddress mac_address;
    omnetpp::simtime_t last_contact_time;
  } cache_entry;
  /** @brief An iterator */
  typedef std::list<cache_entry>::iterator cache_it;
  typedef std::list<cache_entry>::const_iterator cache_const_it;
protected:
  /** @brief Data structure storing the one-hop neighborhood N(x) of a node x*/
  std::list<cache_entry> cache;
  /** @brief A timer to send the neighborhood to an observer that computes the
   *  the adjacency matrix
   * */
  omnetpp::cMessage* timer;
  /** @brief The time between signal transmissions that conveys the N(x) */
  omnetpp::simtime_t signaling_time;
  /** @brief Signal conveying N(x) */
  static omnetpp::simsignal_t neighborhood_signal;
public:
  NeighborCache() : timer(nullptr) {}
  virtual ~NeighborCache() {
    cancelAndDelete(timer);
  }
  /**
   * @brief This module does not receive messages, but process a timer to
   * emit the neighborhood to an observer module
   * */
  virtual void handleMessage(omnetpp::cMessage*);
  /**  
   * @brief Returns the number of stages needed to initalize an INET node
   */
  virtual int numInitStages() const override {
    return inet::NUM_INIT_STAGES;
  }
  /** @brief Initializes the module parameters from the NED file */
  virtual void initialize(int) override;
  /** 
   * @brief Returns a const pointer to access the neighbor cache in const mode */
  virtual const std::list<cache_entry>* get_neighbor_cache() {
    const std::list<cache_entry>* ptr = &cache;
    return ptr;
  }
  /** @brief Inserts a neighbor in cache, it is assumed that the entry will
   *  not be used by the calle
   */
  virtual void push_register(cache_entry&&);
  /** @brief Removes a neighbor from cache by mac_address */
  virtual void erase_register(const inet::MacAddress&);
  /** @brief Removes a neighbor from cache by host_id*/
  virtual void erase_register(int);
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

class NeighborhoodNotificacion : public omnetpp::cObject, omnetpp::noncopyable {
public:
  const std::list<NeighborCache::cache_entry>* neighborhood;
public:
  NeighborhoodNotificacion() : neighborhood(nullptr) {}
  NeighborhoodNotificacion(const std::list<NeighborCache::cache_entry>* n_)
    : neighborhood(n_)
  { }
};

Register_Class(NeighborhoodNotificacion);

#endif // NEIGHBOR_CACHE_H