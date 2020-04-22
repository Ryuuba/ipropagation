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

#if !defined(INFECTION_OBSERVER_H)
#define INFECTION_OBSERVER_H

#include <vector>
#include <omnetpp.h>
#include "inet/common/INETDefs.h"
#include "../common/SquareMatrix.h"
#include "../app/BroadcastInfectionApp.h"

class InfectionObserver
  : public omnetpp::cSimpleModule
  , public omnetpp::cListener
{
protected:
  // @brief The expected infection density
  double rho;
  // @brief The number of hosts
  unsigned host_num;
  // @brief The total of infected nodes
  unsigned infected_num;
  // @brief The node status
  static omnetpp::simsignal_t status_signal;
  // @brief The number of infected nodes
  static omnetpp::simsignal_t infected_node_stat;
  // @brief Data structure to save the status of each nodes and their N(x)
  SquareMatrix<BroadcastInfectionApp::Status> adjacency_matrix;
public:
  // @brief Default constructor
  InfectionObserver()
    : rho(0.0)
    , infected_num(0)
  { }
  // @brief Default destructor
  virtual ~InfectionObserver() { }
  virtual int numInitStages() const override {
    return inet::NUM_INIT_STAGES;
  }
  // @brief Initializes the module state
  virtual void initialize(int) override;
  // @brief This module does not process messages 
  virtual void handleMessage(omnetpp::cMessage*) override {
    throw omnetpp::cRuntimeError(
      "InfectionObserver: this module does not receive any message\n"
    );
  }
  /** @brief Receives the node status and updates the average of infected nodes 
    * rho */
  virtual void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t,  
    omnetpp::cObject*, omnetpp::cObject*);
};

#endif // INFECTION_OBSERVER_H
