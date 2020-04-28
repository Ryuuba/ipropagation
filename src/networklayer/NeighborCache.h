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
#include <memory>
#include <omnetpp.h>

#include "CacheRegister.h"
#include "../signal/NeighborNotification.h"

class NeighborCache : public omnetpp::cSimpleModule {
public:
  /** @brief An iterator */
  typedef std::list<cache_register>::iterator cache_it;
  typedef std::list<cache_register>::const_iterator cache_const_it;
protected:
  /** @brief Data structure storing the one-hop neighborhood N(x) of a node x*/
  std::shared_ptr< std::list<cache_register> > cache;
  /** @brief A timer to send the neighborhood to an observer that computes the
   *  the adjacency matrix
   * */
  omnetpp::cMessage* timer;
  /** @brief Signal conveying N(x) */
  static omnetpp::simsignal_t neighborhood_signal;
  /** @brief Signal conveying |N(x)| */
  static omnetpp::simsignal_t degree_signal;
public:
  NeighborCache()
    : cache{std::make_shared< std::list<CacheRegister> >()}
    , timer{nullptr}
  { }
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
  virtual std::shared_ptr<const std::list<cache_register> > get() const {
    std::shared_ptr< const std::list<cache_register> > ptr = cache;
    return ptr;
  }
  /** @brief Indicates if an neighbor is in cache */
  bool is_in_cache(int);
  /** @brief Inserts a neighbor in cache, it is assumed that the entry will
   *  not be used by the calle
   */
  virtual void push_register(cache_register&&);
  /** @brief Removes a neighbor from cache by mac_address */
  virtual void erase_register(const inet::MacAddress&);
  /** @brief Removes a neighbor from cache by host_id*/
  virtual void erase_register(int);
  /** @brief Updates the last time a neighbor has been seen */
  virtual void update_last_contact_time(int, omnetpp::simtime_t);
  /** @brief Sets value is_updated of all entries to false */
  virtual void invalid_cache();
  /** @brief Flushes stale entries */
  virtual void flush_cache();
  /** @brief Returns the numbers of entries in the cache */
  virtual size_t size() {
    return cache->size();
  }
  /** @brief Returns the numbers of entries in the cache */
  virtual size_t size() const {
    return cache->size();
  }
  /** @brief Erases an entry in the cache */
  virtual cache_const_it erase(cache_const_it it) {
    return cache->erase(it);
  }
  /** @brief Returns the begin of the cache for loop-range iteration */
  virtual cache_it begin() {
    return cache->begin();
  }
  /** @brief Returns the end of the cache for loop-range iteration */
  virtual cache_it end() {
    return cache->end();
  }
  /** @brief Returns the begin of the cache for loop-range iteration */
  virtual cache_const_it begin() const{
    return cache->begin();
  }
  /** @brief Returns the end of the cache for loop-range iteration */
  virtual cache_const_it end() const {
    return cache->end();
  }
  virtual void emit();
  /** @brief Allows a WATCH macro to display the neighbor cache of a node */
  friend std::ostream& operator<<(std::ostream&, const NeighborCache&);
};

#endif // NEIGHBOR_CACHE_H