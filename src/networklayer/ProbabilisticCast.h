/*
 *  A routing protocol for ad hoc networks that take advantage of L2 broadcast
 *  It perform ramdom unicast, multicast as well as broadcast communications
 *  
 *  Based on: ProbabilisticBroadcast.h
 *      by: Damien Piguet
 *  Modified on: Apr, 2020
 *      By: Adán G. Medrano-Chávez
 * 
 */

#if !defined(PROBABILISTIC_CAST_H)
#define PROBABILISTIC_CAST_H


#include <list>
#include <map>
#include <set>
#include <cassert>

#include <omnetpp.h>

#include "inet/common/packet/Packet.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolGroup.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/networklayer/base/NetworkProtocolBase.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/networklayer/contract/IL3AddressType.h"
#include "inet/networklayer/contract/INetworkProtocol.h"
#include "inet/linklayer/common/MacAddress.h"
#include "inet/linklayer/common/MacAddressTag_m.h"

#include "NeighborCache.h"
#include "../msg/ProbabilisticCastHeader_m.h"


class INET_API ProbabilisticCast
  : public inet::NetworkProtocolBase
  , public inet::INetworkProtocol
{
protected:
  enum messagesTypes {
    UNKNOWN = 0,
    BROADCAST_TIMER,
    NEIGHBOR_TIMER,
    BETA_TIMER,
  };
  enum COMM_MODE {
    UNICAST = 10,
    MULTICAST,
    BROADCAST
  };

  // @brief Data structure to store the ID of received messages
  typedef std::set<long> MsgIdSet;
  // @brief Data structure to store pairs <time, msg>
  typedef std::multimap<omnetpp::simtime_t, std::unique_ptr<inet::Packet> > TimeMsgMap;
protected:
  // @brief Enables L3 forwading
  bool forwarding;
  //@brief Routing communicatin scheme
  COMM_MODE communication_mode;
  //@brief The number of times a packet can be forwarded
  int hop_limit;
  // @brief Length of the NetwPkt header
  int header_length;
  // @brief Probability of each broadcast attempt.
  double beta;
  // @brief The probability of passing a packet to the App layer
  double eta;
  // @brief Period (in sim time) between two broadcast attempts.
  omnetpp::simtime_t broadcast_period;
  // @brief Maximal back-off before first broadcast attempt [seconds].
  omnetpp::simtime_t max_first_bcast_backoff;
  // TODO: find the reason of this parameter
  // @brief the number of times a message has arrived to this module
  static long id_counter;
  // @brief The L3 address of this module (node index + 1)
  std::unique_ptr<inet::ModuleIdAddress> src_address;
  // @brief rings when it's time to perform broadcast
  omnetpp::cMessage* broadcast_timer;
  // @brief The list of possible forwarding nodes
  std::shared_ptr<DestinationList> forwarding_list;
  // @brief A pointer to access the neighbor cache
  NeighborCache* cache;
  // @brief Stores the ID of received messages
  MsgIdSet msg_id_set;
  /** @brief Protocol queue that let messages be dropped if they are kept by 
   *  more than drop_time seconds */
  TimeMsgMap msg_queue;
  // variables for statistics
  // @brief Number of received packets from lower layer
  long recv_pkt_num;
  // @brief Number of sent packets from lower layer
  long sent_pkt_num;
  // @brief Number of forwarded packets
  long fwd_pkt_num;
  // @brief Number of hops of forwarded packets
  long hop_count;
  // @brief Number of dropped packets
  long drp_pkt_num;
  //@brief The time a packet is kept in queue
  omnetpp::simtime_t queueing_time;
  //signal to carry statistics
  static omnetpp::simsignal_t recv_pkt_signal;
  static omnetpp::simsignal_t sent_pkt_signal;
  static omnetpp::simsignal_t fwd_pkt_signal;
  static omnetpp::simsignal_t hop_count_signal;
  static omnetpp::simsignal_t drp_pkt_signal;
  static omnetpp::simsignal_t qtime_signal;
protected:
  static long getNextID()
  {
      long nextID = id_counter;
      id_counter++;
      return nextID;
  }
  // @brief The number of stage required to initialized an INET module
  virtual int numInitStages() const { return inet::NUM_INIT_STAGES; }
  // @brief Return the kind of protocol
  const inet::Protocol& getProtocol() const override {
    return inet::Protocol::probabilistic;
  }
  // @brief Handle messages from upper layer
  virtual void handleUpperPacket(inet::Packet *packet) override;
  // @brief Handle messages from lower layer 
  virtual void handleLowerPacket(inet::Packet *packet) override;
  // @brief Handle self messages
  virtual void handleSelfMessage(omnetpp::cMessage *msg) override;
  /// @brief Checks whether a message is already in queue or not
  virtual bool is_in_queue(unsigned int); //knowMessage}
  // @brief Checks whether a message is known (= kept in memory) or not */
  // virtual bool debugMessageKnown(unsigned int msgId);
  /** @brief Inserts a message in both known ID list and message queue. If the  
   *  message comes in front of the queue (i.e., it will be the next one to be 
   *  broadcasted, the broadcast_timer is reset accordingly.
   *  @param bcast_delay relative (to now) simulator time of next broadcast 
   *  attempt.
   *  @param msg descriptor of the message to insert in the queue.
   **/
  virtual void insert_msg(inet::Packet*, omnetpp::simtime_t_cref);
  /** @brief Returns the first packet in the queue,
   *  then remove its pointer from the queue and its id from the known IDs 
   *  list. Then re-schedule the broadcast_timer to the broadcast instant of 
   *  the new first element in the list.
   **/
  virtual inet::Packet* pop_msg();
  /** @brief Returns a network layer packet which encapsulates the upper layer 
   * packet passed to the function. **/
  virtual void encapsulate(inet::Packet*);
  /** @brief extracts and returns the application layer packet which is 
   *  encapsulated in the network layer packet given in argument, delete
   *  network layer packet. **/
  virtual void decapsulate(inet::Packet*);
  /**
   * @brief Attaches a "control info" (NetwToMac) structure (object) to the 
   *  message pMsg. This is most useful when passing packets between protocol 
   *  layers of a protocol stack, the control info will contain the destination 
   *  MAC address. The "control info" object will be deleted when the message 
   *  is deleted. Only one "control info" structure can be attached (the second
   *  setL3ToL2ControlInfo() call throws an error).
   *
   * @param pMsg      The message where the "control info" shall be attached.
   * @param pDestAddr The MAC address of the message receiver.
   */
  virtual void set_ctrl_info(inet::Packet* const, const inet::MacAddress&);
  // OperationalBase: TODO: implement someday...
  virtual void handleStartOperation(inet::LifecycleOperation*) override {}
  virtual void handleStopOperation(inet::LifecycleOperation*) override {}
  virtual void handleCrashOperation(inet::LifecycleOperation*) override {}
  // @brief Initializes the parameters of this module
  virtual void initialize(int) override;
  /** @brief Computes de size of the set of possible forwarding neighbors */
  virtual void compute_forwarding_list();
  /** @brief Draws randomly neighbors from the neighbor cache */
  virtual void draw_neighbor(size_t);
  //** TODO: Drops packet */
  // virtual void drop_packet(inet::Packet*);
public:
  ProbabilisticCast();
  virtual ~ProbabilisticCast();

};

#endif // PROBABILISTIC_CAST_H

