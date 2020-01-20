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

#include <omnetpp.h>

#include "inet/common/INETDefs.h"

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"

class Infection : public ApplicationBase, public UdpSocket::ICallback
{
  protected:
    /** @brief A UDP socket*/
    inet::UdpSocket socket;
    /** @brief A timer to try recovering the infection */
    omnetpp::cMessage* recovey_timer;
    /** @brief The packet containing the infection */
    const char *packet_name;
    /** @brief The duration of the infection period */
    long infection_time;
    /** @brief the size of the packet containing the infection */
    int packet_size;
    /** @brief The rate at which the infectious packet in broadcasted */
    double send_interval;
  public:
    /** @brief Default constructor initializing the class parameters */
    Infection();
    /** @brief Cancels self-messages when destructing this */
    virtual ~Infection();
    /** @brief Handles the action this performs when the timer expires according to
     *  the status of the node */
    virtual void handleMessage(omnetpp::cMessage*) override;
    /** @brief INET applications need 12 states to initialize all simulation modules */
    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    /** @brief Initializes the app state getting N(x) and the initial node status*/
    virtual void initialize(int stage) override;

    /** These member functions are overridden from the ICallBack abstract class */
    /** @brief Notifies about data arrival, packet ownership is transferred to the callee */
    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    /** @brief Notifies about error indication arrival, indication ownership is transferred to the callee. */
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    /** @brief Notifies about socket closed, indication ownership is transferred to the callee. */
    virtual void socketClosed(UdpSocket *socket) override;


};

#endif // INFECTION_APP_H
