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
#include "inet/common/INETDefs.h"
#include "../common/SquareMatrix.h"
#include "../signal/NeighborNotification.h"
#include <iomanip>

class ConnectivityObserver
  : public omnetpp::cSimpleModule
  , public omnetpp::cListener
{
protected:
  struct Cell
  {
    bool still_connected;
    omnetpp::simtime_t lifetime;
    omnetpp::simtime_t start_time;
    omnetpp::simtime_t last_contact_time;
    Cell() 
      : still_connected(false)
      , lifetime(0.0)
      , start_time(0.0)
      , last_contact_time(0.0)
    { }
    friend std::ostream& operator << (std::ostream& os, const Cell& cell_) {
      os << '[' << cell_.lifetime << ", " << cell_.last_contact_time << ']'; 
      return os;
    }
  };
  typedef Cell cell_t;
protected:
  size_t host_number;
  std::shared_ptr< SquareMatrix<cell_t> > adjacency_matrix;
  static omnetpp::simsignal_t neighborhood_notification_signal;
protected:
  void write_matrix(const char*, const std::function<double(size_t, size_t)>&);
public:
  virtual int numInitStages() const override {
    return inet::NUM_INIT_STAGES;
  }
  /** @brief Default constructor */
  ConnectivityObserver()
    : host_number(0)
    , adjacency_matrix(nullptr)
  { }
  ~ConnectivityObserver() 
  { 
    getSimulation()->getSystemModule()->unsubscribe(
      neighborhood_notification_signal,
      this
    );
  }
  /** @brief Initializes the module state */
  virtual void initialize(int) override;
  /** @brief This module does not process messages */
  virtual void handleMessage(omnetpp::cMessage*) override;
  /** @brief Write the adjacency matrix in a file */
  virtual void finish() override;
  /** @brief Receives the one-hop neighborhood and updates the adjacency matrix */
  virtual void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t,  
    omnetpp::cObject*, omnetpp::cObject*);
};



#endif // CONNECTIVITY_OBSERVER_H
