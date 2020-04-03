#if !defined(CACHE_REGISTER_H)
#define CACHE_REGISTER_H

#include <iostream>
#include <omnetpp.h>
#include "inet/linklayer/common/MacAddress.h"
#include "inet/networklayer/common/ModuleIdAddress.h"

struct CacheRegister {
  inet::ModuleIdAddress netw_address;
  inet::MacAddress mac_address;
  omnetpp::simtime_t last_contact_time;
  friend std::ostream& operator <<(std::ostream& os, const CacheRegister& c) {
    os << '[' << c.netw_address.getId() << ", "
       << c.mac_address << ", "
       << c.last_contact_time << ']';
    return os;
  }
};

typedef CacheRegister cache_register;

#endif // CACHE_REGISTER_H
