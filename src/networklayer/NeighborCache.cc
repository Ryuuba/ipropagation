#include "NeighborCache.h"

Define_Module(NeighborCache);

omnetpp::simsignal_t NeighborCache::neighborhood_signal = 
  registerSignal("neighborhood");

std::ostream& operator<<(std::ostream& os, const NeighborCache& cache) {
  for (auto &&neighbor : cache)
    std::cout << neighbor.host_id << ' ' 
              << neighbor.mac_address
              << neighbor.last_contact_time
              << '\n';
}

void NeighborCache::initialize(int stage) {
  if (stage == inet::INITSTAGE_LOCAL) {
    timer = new omnetpp::cMessage("send state");
    signaling_time = par("signalingTime");
    scheduleAt(omnetpp::simTime() + signaling_time, timer);
  }
}

void NeighborCache::handleMessage(omnetpp::cMessage* msg) {
  if (msg->isSelfMessage()) {
    const std::list<cache_entry>* ptr = get_neighbor_cache();
    NeighborhoodNotificacion notification(ptr);
    emit(neighborhood_signal, &notification);
    scheduleAt(omnetpp::simTime() + signaling_time, timer);
  }
  else 
    throw omnetpp::cRuntimeError(
      "NeighborCache: This module does not process any message (name: %s)\n", 
      msg->getName()
    );
}

void NeighborCache::push_register(NeighborCache::cache_entry&& entry) {
  cache.push_back(entry);
}

void NeighborCache::erase_register(const inet::MacAddress& neighbor_mac) {
  cache_it it = std::find_if(
    cache.begin(),
    cache.end(), 
    [neighbor_mac](const cache_entry & entry) -> bool {
      return entry.mac_address == neighbor_mac;
    }
  );
  if (it != cache.end()) {
    EV_INFO <<  "NeighborCache: entry: <" << it->host_id << ", " 
             << it->mac_address << ", "
             << it->last_contact_time << "> is erased\n";
    cache.erase(it);
  }
  else
    EV_ERROR << "NeighborCache: there is not an entry that matches the MAC\
     address " << neighbor_mac << '\n';
}

void NeighborCache::erase_register(int neighbor_id) {
  cache_it it = std::find_if(
    cache.begin(),
    cache.end(), 
    [neighbor_id](const cache_entry & entry) -> bool {
      return entry.host_id == neighbor_id;
    }
  );
  if (it != cache.end()) {
    EV_INFO <<  "NeighborCache: entry: <" << it->host_id << ", " 
             << it->mac_address << ", "
             << it->last_contact_time << "> is erased\n";
    cache.erase(it);
  }
  else
    EV_ERROR << "NeighborCache: there is not an entry that matches the host ID\
     address " << neighbor_id << '\n';
}