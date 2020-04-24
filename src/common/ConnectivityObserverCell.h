#if !defined(CONNECTIVITY_OBSERVER_CELL_H)
#define CONNECTIVITY_OBSERVER_CELL_H

#include <iostream>
#include <omnetpp.h>

struct Cell
{
  bool still_connected;
  omnetpp::simtime_t lifetime;
  omnetpp::simtime_t start_time;
  omnetpp::simtime_t last_contact_time;
  Cell() 
    : still_connected(false)
    , lifetime(0.0)
    , start_time(0.0)
    , last_contact_time(0.0)
  { }
  friend std::ostream& operator << (std::ostream& os, const Cell& cell_) {
    os << '[' << cell_.lifetime << ", " << cell_.last_contact_time << ']'; 
    return os;
  }
};
typedef Cell cell_t;

#endif // CONNECTIVITY_OBSERVER_CELL_H
