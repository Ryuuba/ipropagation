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

#if !defined(INFORMTATION_PROPAGATION_APP_H)
#define INFORMTATION_PROPAGATION_APP_H

#include "../base/InformationPropagationBase.h"
#include "../msg/InfoPacket_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/linklayer/common/MacAddress.h"
#include "inet/common/IProtocolRegistrationListener.h"

class InformationPropagationApp : public InformationPropagationBase
{
public:
  enum COMM_MODE {
    UNICAST = 0,
    MULTICAST,
    BROADCAST
  };
protected: //App attributes
  /** @brief The name of the infectious packet */
  const char* packet_name;
  /** @brief The length in bytes of the data within an infectious packet */
  int payload;
  /** @brief Communication scheme */
  COMM_MODE comm_mode;
protected: //App member functions
  /** @brief Draws randomly a neighbor according to a communication scheme */
  virtual std::list<inet::L3Address> draw_neighbor(COMM_MODE);
  /** @brief Computes whether this node starts infected or not */
  virtual void compute_initial_state();
  /** @brief Encapsulates a message to send it via a L3 socket */
  virtual void encapsulate(const inet::L3Address&);
  /** @brief Sends an infectious message according to a communication scheme
   *  In case the communication scheme is unicast or multicast, then 
   *  destinations are randomly drawn */
  virtual void send_message(omnetpp::cMessage*);
  /** @brief Tries to recovery from an infection */
  virtual void try_recovery(omnetpp::cMessage*) override;
  /** @brief Process the received packet */
  virtual void process_packet(inet::Packet*) override;
  /** @brief Handle the timers of this application */
  virtual void handleMessage(omnetpp::cMessage*) override;
  /** @brief INET applications need 12 states to initialize all simulation 
   *  modules. */
  virtual int numInitStages() const override {return inet::NUM_INIT_STAGES;}
  /** @brief Initializes the app state getting N(x), the initial node 
   *  status, probability distributions, etc.  */
  virtual void initialize(int stage) override;
  /** @brief Sends not emitted statistics */
  virtual void finish();
  /** @brief Changes the icon of the app to indicate the host status */
  virtual void refreshDisplay() const override;
public:
  /** @brief Default constructor initializing the class parameters */
InformationPropagationApp()
  : packet_name("")
  , payload(0)
  , comm_mode(UNICAST)
  { }
  /** @brief Default destructor */
  ~InformationPropagationApp() { }
};

#endif // INFORMTATION_PROPAGATION_APP_H
