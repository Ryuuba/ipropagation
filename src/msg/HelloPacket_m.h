//
// Generated file, do not edit! Created by nedtool 5.6 from networklayer/../msg/HelloPacket.msg.
//

#ifndef __INET_HELLOPACKET_M_H
#define __INET_HELLOPACKET_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0506
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif


namespace inet {

class HelloPacket;
} // namespace inet

#include "inet/common/INETDefs_m.h" // import inet.common.INETDefs

#include "inet/common/packet/chunk/Chunk_m.h" // import inet.common.packet.chunk.Chunk

#include "inet/linklayer/common/MacAddress_m.h" // import inet.linklayer.common.MacAddress


namespace inet {

/**
 * Enum generated from <tt>networklayer/../msg/HelloPacket.msg:26</tt> by nedtool.
 * <pre>
 * enum HelloPacketType
 * {
 *     REQ = 0;
 *     REP = 1;
 * }
 * </pre>
 */
enum HelloPacketType {
    REQ = 0,
    REP = 1
};

/**
 * Class generated from <tt>networklayer/../msg/HelloPacket.msg:32</tt> by nedtool.
 * <pre>
 * class HelloPacket extends FieldsChunk
 * {
 *     chunkLength = B(25);
 *     HelloPacketType type;
 *     int hostId;
 *     int sequenceNum;
 *     MacAddress srcMacAddress;
 *     MacAddress dstMacAddress;
 * }
 * </pre>
 */
class HelloPacket : public ::inet::FieldsChunk
{
  protected:
    inet::HelloPacketType type = static_cast<inet::HelloPacketType>(-1);
    int hostId = 0;
    int sequenceNum = 0;
    MacAddress srcMacAddress;
    MacAddress dstMacAddress;

  private:
    void copy(const HelloPacket& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const HelloPacket&);

  public:
    HelloPacket();
    HelloPacket(const HelloPacket& other);
    virtual ~HelloPacket();
    HelloPacket& operator=(const HelloPacket& other);
    virtual HelloPacket *dup() const override {return new HelloPacket(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual inet::HelloPacketType getType() const;
    virtual void setType(inet::HelloPacketType type);
    virtual int getHostId() const;
    virtual void setHostId(int hostId);
    virtual int getSequenceNum() const;
    virtual void setSequenceNum(int sequenceNum);
    virtual const MacAddress& getSrcMacAddress() const;
    virtual MacAddress& getSrcMacAddressForUpdate() { handleChange();return const_cast<MacAddress&>(const_cast<HelloPacket*>(this)->getSrcMacAddress());}
    virtual void setSrcMacAddress(const MacAddress& srcMacAddress);
    virtual const MacAddress& getDstMacAddress() const;
    virtual MacAddress& getDstMacAddressForUpdate() { handleChange();return const_cast<MacAddress&>(const_cast<HelloPacket*>(this)->getDstMacAddress());}
    virtual void setDstMacAddress(const MacAddress& dstMacAddress);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const HelloPacket& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, HelloPacket& obj) {obj.parsimUnpack(b);}

} // namespace inet

#endif // ifndef __INET_HELLOPACKET_M_H

