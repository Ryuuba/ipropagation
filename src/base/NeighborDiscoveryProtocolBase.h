//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#if !defined(NEIGHBOR_DISCOVERY_PROTOCOL_BASE_H)
#define NEIGHBOR_DISCOVERY_PROTOCOL_BASE_H

#include "../contract/INeighborDiscoveryProtocol.h"
#include "../networklayer/NeighborCache.h"
#include "inet/common/INETDefs.h"
#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/common/ModuleAccess.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/linklayer/common/MacAddressTag_m.h"
#include "inet/common/ProtocolTag_m.h"
//TODO: Design an abstract class for NeighborDiscoveryProtocol packets

class NeighborDiscoveryProtocolBase : public INeighborDiscoveryProtocol {
protected:
  /** @brief The MAC address of this host */
  inet::MacAddress mac;
  /** @brief A pointer to access the neighbor cache */
  NeighborCache* neighbor_cache;
  /** @brief The size of the packet in bytes */
  int packet_size;
  /** @brief The sequence number of the hello messages */
  int sequence_number;
  /** @brief A pointer to access the interface table */
  inet::IInterfaceTable *interface_table;
  /** @brief The ID of the wlan interface */
  int interface_id;
protected:
  /** @brief Encapsulates a hello message into an INET packet */
  virtual void send_hello_packet() = 0;
  /** @briefModifies the neighbor cache with the hello information */
  virtual void process_hello_packet(omnetpp::cMessage*) = 0;
public:
  NeighborDiscoveryProtocolBase()
    : packet_size(0)
    , interface_id(0)
    , sequence_number(0)
    , mac( )
    , neighbor_cache(nullptr)
    , interface_table(nullptr)
    { }
  virtual ~NeighborDiscoveryProtocolBase(){}
  /**  
   * @brief Returns the number of stages needed to initalize an INET node
   */
  virtual int numInitStages() const override {
    return inet::NUM_INIT_STAGES;
  }
  /** @brief initializes the packer size
   *  @param the INET number of stages
   */
  virtual void initialize(int) override;
};

Register_Abstract_Class(NeighborDiscoveryProtocolBase);

void NeighborDiscoveryProtocolBase::initialize(int stage) {
  if (stage == inet::INITSTAGE_LOCAL) {
    input_gate_id = gate("inputPort")->getId();
    output_gate_id = gate("outputPort")->getId();
    packet_size = par("packetSize").intValue();
    discovery_time = par("discoveryTime");
    discovery_timer = new omnetpp::cMessage("discovery timer");
    auto cache_module = getSimulation()->getSystemModule()->getSubmodule("node")->getSubmodule("net")->getSubmodule("cache");
    neighbor_cache = static_cast<NeighborCache*>(cache_module);
  }
  else if (stage == inet::INITSTAGE_NETWORK_LAYER) {
    interface_table = inet::getModuleFromPar<inet::IInterfaceTable>(par("interfaceTableModule"), this);
    interface_id = interface_table->getInterfaceByName("wlan")->getId();
    mac = interface_table->getInterfaceByName("wlan")->getMacAddress();
    inet::registerService(inet::Protocol::neighborDiscovery, nullptr, gate("inputPort"));
    inet::registerProtocol(inet::Protocol::neighborDiscovery, gate("outputPort"), nullptr);
  }
}

#endif