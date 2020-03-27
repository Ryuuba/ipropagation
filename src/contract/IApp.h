#if !defined(I_APP_H)
#define I_APP_H

#include <vector>
#include <omnetpp.h>
#include "inet/common/INETDefs.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/contract/L3Socket.h"
#include "inet/networklayer/common/L3Address.h"
#include "../networklayer/NeighborCache.h"
#include "../msg/InfoPacket_m.h"

class IApp
  : public omnetpp::cSimpleModule
  , public inet::L3Socket::ICallback {
protected: //IAPP attributes
  /** @brief the packet containing the information to be broadcasted */
  inet::Packet* information;
  /** @brief the timer indicating when to send an information packet */
  omnetpp::cMessage* information_timer;
  /** @brief the ID of the input gate */
  int input_gate_id;
  /** @brief the ID of the output gate */
  int output_gate_id;
  /** @brief The source of the information */
  inet::L3Address* src_address;
  /** @brief The socket to send information packets */
  inet::L3Socket* socket;
  /** @brief A pointer to access the neighbor cache */
  const NeighborCache* neighbor_cache;
  /** @brief The network protocol */
  const inet::Protocol* netw_protocol;
protected: //Member functions inherited from INetworkSocket::ICallBack
  virtual void socketDataArrived(inet::L3Socket*, inet::Packet*) = 0;
  virtual void socketClosed(inet::L3Socket*) = 0;
public:
  IApp ()
    : information(nullptr)
    , information_timer(nullptr)
    , input_gate_id(-1)
    , output_gate_id(-1)
    , src_address(nullptr)
    , socket(nullptr)
    , neighbor_cache(nullptr)
    , netw_protocol(nullptr)
  { }
  virtual ~IApp () { }
};

#endif // I_APP_H