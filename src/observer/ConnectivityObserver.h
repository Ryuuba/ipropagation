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

#if !defined(CONNECTIVITY_OBSERVER_H)
#define CONNECTIVITY_OBSERVER_H

#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <omnetpp.h>
#include <string>
#include <iomanip>
#include <cstdint>
#include "inet/common/INETDefs.h"
#include "../common/SquareMatrix.h"
#include "../signal/ForwardingListNotification.h"
#include "../signal/DestinationNotification.h"
#include "../common/ConnectivityObserverCell.h"

class ConnectivityObserver
  : public omnetpp::cSimpleModule
  , public omnetpp::cListener
{
protected:
  /** @brief The number of host in a simulation */
  size_t host_number;
  /** @brief The number of times a node with ID 'i' is infected */
  std::unique_ptr<std::vector<uint64_t>> contact_cnt;
  /** @brief This adjacency matrix records the number of times a link is drawn 
   *  to forward a packet */
  std::unique_ptr<SquareMatrix<uint64_t>> r_matrix;
  /** @brief This matrix records the number of times node i contacts node j */
  std::unique_ptr<SquareMatrix<uint64_t>> c_matrix;
  /** @brief the signal carrying the destination list from the netw. layer */
  static omnetpp::simsignal_t forwarding_list_signal;
  /** @brief This signal carries the source of app-layer received messages */
  static omnetpp::simsignal_t contact_list_signal;
protected:
  /** @brief Writes the adjacency matrix when finish is invoked at the end of a 
   * simulation*/
  void write_matrix(const char*, const std::function<double(size_t, size_t)>&);
public:
  virtual int numInitStages() const override {
    return inet::NUM_INIT_STAGES;
  }
  /** @brief Default constructor */
  ConnectivityObserver()
    : host_number(0)
    , contact_cnt(nullptr)
    , r_matrix(nullptr)
    , c_matrix(nullptr)
  { }
  ~ConnectivityObserver() 
  { 
    getSimulation()->getSystemModule()->unsubscribe(
      forwarding_list_signal,
      this
    );
    getSimulation()->getSystemModule()->unsubscribe(
      contact_list_signal,
      this
    );
  }
  /** @brief Initializes the module state */
  virtual void initialize(int) override;
  /** @brief This module does not process messages */
  virtual void handleMessage(omnetpp::cMessage*) override;
  /** @brief Writes the r matrix, the f matrix and the difference between such matrices in a file when the simulation ends */
  virtual void finish() override;
  /** @brief Receives the forwarding list the routing protocol computes. */
  virtual void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t,  
    omnetpp::cObject*, omnetpp::cObject*) override;
};



#endif // DYNAMIC_CONNECTIVITY_OBSERVER_H