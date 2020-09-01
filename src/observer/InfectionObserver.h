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

#include <numeric>
#include <vector>
#include <omnetpp.h>
#include <memory>
#include "inet/common/INETDefs.h"
#include "inet/networklayer/probabilistic/ProbabilisticBroadcast.h"
#include "../app/InformationPropagationApp.h"
#include "../signal/NeighborhoodNotification.h"
#include "../networklayer/CacheRegister.h"


class InfectionObserver
  : public omnetpp::cSimpleModule
  , public omnetpp::cListener
{
protected:
  // @brief The number of rounds to be observed
  long long round_num;
  // @brief The error
  double epsilon;
  // @brief The expected infection density
  double rho;
  // @brief Transmission probability from network layer
  double beta;
  // @brief Recovery probability from network layer
  double mu;
  // @brief The number of hosts
  size_t host_num;
  // @brief The total of infected nodes
  size_t infected_num;
  // @brief The actual number of rounds
  long long round_counter;
  // @brief The node status
  static omnetpp::simsignal_t status_signal;
  // @brief The number of infected nodes
  static omnetpp::simsignal_t infected_node_stat;
  // @brief The expected infection density
  static omnetpp::simsignal_t rho_stat;
  // @brief The neighborhood of a node
  static omnetpp::simsignal_t neighborhood_notification_signal;
  // @brief node status, indeces match the node id
  std::unique_ptr < std::vector<InformationPropagationApp::Status> > p;
  // @brief probability of node i does not get infected by a neighbor
  std::unique_ptr < std::vector<double> > q;
  // @brief probability of node i gets infected at t+1
  std::unique_ptr < std::vector<double> > next_p;
  // @brief adjacency matrix at time t
  std::unique_ptr <std::vector<std::shared_ptr< const std::list<cache_register> > > >adjacency_matrix;
  // @brief The duration of a round
  omnetpp::simtime_t step_time;
  // @brief Timer
  omnetpp::cMessage* step_timer;
protected:
  // @brief Computes the probability a node i gets infected
  virtual double compute_rho();
public:
  // @brief Default constructor
  InfectionObserver()
    : round_num(0)
    , epsilon(0.0)
    , rho(0.0)
    , beta(0.0)
    , mu(0.0)
    , host_num(0)
    , infected_num(0)
    , round_counter(0)
    , p(nullptr)
    , q(nullptr)
    , next_p(nullptr)
    , adjacency_matrix(nullptr)
    , step_time(0.0)
    , step_timer(nullptr)
  { }
  // @brief Default destructor
  virtual ~InfectionObserver();
  virtual int numInitStages() const override {
    return inet::NUM_INIT_STAGES;
  }
  // @brief Initializes the module state
  virtual void initialize(int) override;
  // @brief This module only process self-messages
  virtual void handleMessage(omnetpp::cMessage*);
  /** @brief Receives the node status and updates the average of infected nodes 
    * rho */
  virtual void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t,  
    long, omnetpp::cObject*);
  // @brief Receives one-hop neighborhoods
  virtual void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t,  
    omnetpp::cObject*, omnetpp::cObject*);
};

#endif // INFECTION_OBSERVER_H
