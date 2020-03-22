#if !defined(I_APP_H)
#define I_APP_H

#include <omnetpp.h>
#include "inet/common/INETDefs.h"
#include "inet/common/packet/Packet.h"

class IApp : public omnetpp::cSimpleModule {
protected:
  /** @brief the packet containing the information to be broadcasted */
  inet::Packet* information;
  omnetpp::cMessage* information_timer;
  int input_gate_id;
  int output_gate_id;
protected:
  virtual void send_down(inet::Packet*) = 0;
public:
  IApp ()
    : information(nullptr)
    , information_timer(nullptr)
    , input_gate_id(-1)
    , output_gate_id(-1)
  { }
  virtual ~IApp () {
    if (information_timer)
      cancelAndDelete(information_timer);
  }
};

#endif // I_APP_H
