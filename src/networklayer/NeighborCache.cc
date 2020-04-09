#include "NeighborCache.h"

Define_Module(NeighborCache);

omnetpp::simsignal_t NeighborCache::neighborhood_signal = 
  registerSignal("neighborhood");

std::ostream& operator<<(std::ostream& os, const NeighborCache& cache) {
  for (auto&& neighbor : cache)
    std::cout << neighbor.netw_address.getId() << ' '
              << neighbor.mac_address  << ' '
              << neighbor.last_contact_time
              << '\n';
  return os;
}

void NeighborCache::initialize(int stage) {
  if (stage == inet::INITSTAGE_LOCAL) {
    timer = new omnetpp::cMessage("send state");
    WATCH_LIST(*cache);
  }
}

void NeighborCache::handleMessage(omnetpp::cMessage* msg) {
  if (msg->isSelfMessage()) {

  }
  else 
    throw omnetpp::cRuntimeError(
      "NeighborCache: This module does not process any message (name: %s)\n", 
      msg->getName()
    );
}

void NeighborCache::push_register(CacheRegister&& entry) {
  Enter_Method("NeighborCache: push register");
  cache_it it = std::find_if(
    cache->begin(),
    cache->end(), 
    [entry](const cache_register & entry_) -> bool {
      return entry.netw_address.getId() == entry_.netw_address.getId();
    }
  );
  if (it == cache->end()) {
    cache->push_back(entry);
    EV_INFO <<  "NeighborCache: entry: <" << entry.netw_address.getId() << ", " 
             << entry.mac_address << ", "
             << entry.last_contact_time << "> is pushed back\n";
  }
  else
    EV_INFO <<  "NeighborCache: a host "
            << entry.netw_address.getId() << "is already in cache\n";
}

void NeighborCache::erase_register(const inet::MacAddress& neighbor_mac) {
  Enter_Method_Silent();
  cache_it it = std::find_if(
    cache->begin(),
    cache->end(), 
    [neighbor_mac](const cache_register & entry) -> bool {
      return entry.mac_address == neighbor_mac;
    }
  );
  if (it != cache->end()) {
    EV_INFO <<  "NeighborCache: entry: <" << it->netw_address.getId() << ", " 
             << it->mac_address << ", "
             << it->last_contact_time << "> is erased\n";
    cache->erase(it);
  }
  else
    EV_ERROR << "NeighborCache: there is not an entry that matches the MAC\
     address " << neighbor_mac << '\n';
}

void NeighborCache::erase_register(int neighbor_id) {
  Enter_Method_Silent();
  cache_it it = std::find_if(
    cache->begin(),
    cache->end(), 
    [neighbor_id](const cache_register & entry) -> bool {
      return entry.netw_address.getId() == neighbor_id;
    }
  );
  if (it != cache->end()) {
    EV_INFO <<  "NeighborCache: entry: <" << it->netw_address.getId() << ", " 
             << it->mac_address << ", "
             << it->last_contact_time << "> is erased\n";
    cache->erase(it);
  }
  else
    EV_ERROR << "NeighborCache: there is not an entry that matches the host ID\
     address " << neighbor_id << '\n';
}

void NeighborCache::update_last_contact_time(int id, omnetpp::simtime_t time) {
  Enter_Method_Silent();
  cache_it it = std::find_if(
    cache->begin(),
    cache->end(), 
    [id](const cache_register & entry) -> bool {
      return entry.netw_address.getId() == id;
    }
  );
  if (it != cache->end()) {
    it->last_contact_time = time;
    it->still_connected = true;
    EV_INFO <<  "NeighborCache: entry: <" << it->netw_address.getId() << ", " 
             << it->mac_address << ", "
             << it->last_contact_time << "> has been updated\n";
  }
  else
    EV_ERROR << "NeighborCache: there is not an entry that matches the host ID\
     address " << id << '\n';
}

bool NeighborCache::is_in_cache(int id) {
  bool result = false;
  cache_it it = std::find_if(
    cache->begin(),
    cache->end(), 
    [id](const cache_register & entry) -> bool {
      return entry.netw_address.getId() == id;
    }
  );
  if (it != cache->end()) {
    result = true;
    EV_INFO <<  "NeighborCache: entry: <" << it->netw_address.getId() << ", " 
             << it->mac_address << ", "
             << it->last_contact_time << "> is in cache\n";
  }
  return result;
}

void NeighborCache::invalid_cache() {
  Enter_Method_Silent();
  for (auto&& entry : *cache)
    entry.still_connected = false;
}

void NeighborCache::flush_cache() {
  Enter_Method_Silent();
  auto it = cache->begin();
  while (it != cache->end()) {
    if (!it->still_connected)
      it = cache->erase(it);
    else
      ++it;
  }
}

void NeighborCache::emit() {
  Enter_Method_Silent();
  NeighborhoodNotificacion notification(get());
  omnetpp::cSimpleModule::emit(neighborhood_signal, &notification);
}