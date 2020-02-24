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

void NeighborCache::erase_register(NeighborCache::cache_entry& entry) {
  cache_it it = std::find(cache.begin(), cache.end(), entry);
  if (it != cache.end()) {
    cache.erase(it);
    EV_INFO <<  "NeighborCache: entry: <" << entry.host_id << ", " 
             << entry.mac_address << ", "
             << entry.last_contact_time << "> is erased\n";
  }
  else
    EV_ERROR << "NeighborCache: entry: <" << entry.host_id << ", " 
             << entry.mac_address << ", "
             << entry.last_contact_time << "> is not found\n";
}