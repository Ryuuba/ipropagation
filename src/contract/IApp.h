#if !defined(I_APP_H)
#define I_APP_H

#include <vector>
#include <omnetpp.h>
#include <cstdint>
#include "inet/common/INETDefs.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/contract/L3Socket.h"
#include "inet/networklayer/common/L3Address.h"
#include "../networklayer/NeighborCache.h"
#include "../msg/InfoPacket_m.h"

class IApp
  : public omnetpp::cSimpleModule
  , public inet::L3Socket::ICallback
  , public omnetpp::cListener
{
protected: //IAPP attributes
  /** @brief the signal carrying the destination list from the netw. layer */
  static omnetpp::simsignal_t forwarding_list_signal;
  /** @brief the index of the node containing this module */
  int module_index;
  /** @brief the packet containing the information to be broadcasted */
  inet::Packet* information;
  /** @brief the timer indicating when to send an information packet */
  omnetpp::cMessage* transmission_timer;
  /** @brief the ID of the input gate */
  int input_gate_id;
  /** @brief the ID of the output gate */
  int output_gate_id;
  /** @brief The source of the information */
  std::unique_ptr<inet::ModuleIdAddress> src_address;
  /** @brief The socket to send information packets */
  inet::L3Socket* socket;
  /** @brief The network protocol */
  const inet::Protocol* netw_protocol;
protected: //Member functions inherited from INetworkSocket::ICallBack
  virtual void socketDataArrived(inet::L3Socket*, inet::Packet*) = 0;
  virtual void socketClosed(inet::L3Socket*) = 0;
protected: //Member function from cListener
    /** @brief Receives the forwarding list the routing protocol computes. */
  virtual void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t,  
    omnetpp::cObject*, omnetpp::cObject*) = 0;
public:
  IApp ()
    : module_index(0)
    , information(nullptr)
    , transmission_timer(nullptr)
    , input_gate_id(-1)
    , output_gate_id(-1)
    , src_address(nullptr)
    , socket(nullptr)
    , netw_protocol(nullptr)
  { }
  virtual ~IApp () { }
};


#endif // I_APP_H
