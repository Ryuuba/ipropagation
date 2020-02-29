//
// Generated file, do not edit! Created by nedtool 5.5 from msg/InfoPacket.msg.
//

#ifndef __INET_INFOPACKET_M_H
#define __INET_INFOPACKET_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0505
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif


namespace inet {

class InfoPacket;
} // namespace inet

#include "inet/common/INETDefs_m.h" // import inet.common.INETDefs

#include "inet/common/packet/chunk/Chunk_m.h" // import inet.common.packet.chunk.Chunk


namespace inet {

/**
 * Enum generated from <tt>msg/InfoPacket.msg:25</tt> by nedtool.
 * <pre>
 * enum InfoType
 * {
 *     VIRUS = 0;
 *     DATA = 1;
 *     OTHER = 2;
 * }
 * </pre>
 */
enum InfoType {
    VIRUS = 0,
    DATA = 1,
    OTHER = 2
};

/**
 * Class generated from <tt>msg/InfoPacket.msg:32</tt> by nedtool.
 * <pre>
 * class InfoPacket extends FieldsChunk
 * {
 *     InfoType type;
 *     int identifer;
 *     int host_id;
 * }
 * </pre>
 */
class InfoPacket : public ::inet::FieldsChunk
{
  protected:
    inet::InfoType type = static_cast<inet::InfoType>(-1);
    int identifer = 0;
    int host_id = 0;

  private:
    void copy(const InfoPacket& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const InfoPacket&);

  public:
    InfoPacket();
    InfoPacket(const InfoPacket& other);
    virtual ~InfoPacket();
    InfoPacket& operator=(const InfoPacket& other);
    virtual InfoPacket *dup() const override {return new InfoPacket(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual inet::InfoType getType() const;
    virtual void setType(inet::InfoType type);
    virtual int getIdentifer() const;
    virtual void setIdentifer(int identifer);
    virtual int getHost_id() const;
    virtual void setHost_id(int host_id);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const InfoPacket& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, InfoPacket& obj) {obj.parsimUnpack(b);}

} // namespace inet

#endif // ifndef __INET_INFOPACKET_M_H

