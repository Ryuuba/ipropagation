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

#if !defined(INFECTION_APP_H)
#define INFECTION_APP_H

#include <string>
#include <omnetpp.h>

#include "../contract/InfectionBase.h"

#include "inet/common/INETDefs.h"

#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/networklayer/common/FragmentationTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"

class InfectionApp : public InfectionBase, public inet::UdpSocket::ICallback
{
  protected: //App attributes
    /** @brief A UDP socket to send infectious packets */
    inet::UdpSocket socket;
    /** @brief List of IPs of hosts in N(x), separated by spaces */
    std::string neighbor_ip;
    /** @brief The IP address of this host */
    std::string ip;
    /** @brief The name of the infectious packet */
    std::string packet_name;
    /** @brief The length in bytes of the infectious packet */
    int packet_length;
    /** @brief the sort of timers this app reacts */
    enum timer_kind {
      RECOVERY = 100, //Indicates the node will try to recover from an infection
      BROADCAST,      //Indicates the host will broadcast a packet
      EXPERIMENTAL    //Use this enum element to extend this timer
    };
  protected: //App signals to carry statistics
  /** @brief 1) Signal carrying the number of sent messages 
   *         2) Signal carrying the number of received messages
   *         3) Signal carrying the duration of an infection period
   */
  omnetpp::simsignal_t sent_message_signal, 
                       received_message_signal, 
                       infection_time_signal;
  /** @brief Signal carrying the number of received messages */
  protected: //App member functions
    /** @brief Broadcasts infectious messages to nodes in N(x) at a rate of 
     *  broadcast_rate per broadcast_timer (in seconds), overriden from 
     *  InfectionBase class */
    virtual void broadcast() override;
    /** @brief choose random destination address */

    /** @brief Handle the timers of this application */
    virtual void handleMessage(omnetpp::cMessage*) override;
    /** @brief INET applications need 12 states to initialize all simulation 
     *  modules. TODO: check this whether this condition is needed or not */
    virtual int numInitStages() const override {return inet::NUM_INIT_STAGES;}
    /** @brief Initializes the app state getting N(x), the initial node 
     *  status, probability distributuions, etc.  */
    virtual void initialize(int stage) override;
    /** @brief Changes the icon of the app to indicate the host status */
    virtual void refreshDisplay() const override;
  protected: //Member functions overridden from the ICallBack abstract class
    /** @brief Notifies about data arrival, packet ownership is transferred to 
     *  the callee */
    virtual void socketDataArrived(
      inet::UdpSocket* socket, inet::Packet* packet
    ) override;
    /** @brief Notifies about error indication arrival, indication ownership 
     *  is transferred to the callee. */
    virtual void socketErrorArrived(
      inet::UdpSocket* socket, inet::Indication* indication
    ) override;
    /** @brief Notifies about socket closed, indication ownership is 
     *  transferred to the callee. */
    virtual void socketClosed(inet::UdpSocket* socket) override;
  public:
    /** @brief Default constructor initializing the class parameters */
    InfectionApp();
    /** @brief Cancels self-messages (timers) when destructing *this */
    virtual ~InfectionApp();

};

#endif // INFECTION_APP_H
