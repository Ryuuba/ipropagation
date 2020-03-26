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

#if !defined(PROPAGATION_SERVICE_H)
#define PROPAGATION_SERVICE_H

#include "inet/common/packet/Packet.h"
#include "inet/networklayer/base/NetworkProtocolBase.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/contract/INetworkProtocol.h"

class PropagationService
  : public inet::NetworkProtocolBase
  , public inet::INetworkProtocol
{
private:
  /* data */
public:
  PropagationService(/* args */);
  ~PropagationService();
  const inet::Protocol& getProtocol() const override { 
    return inet::Protocol::probabilistic;
  }
  // OperationalBase:
  virtual void handleStartOperation(inet::LifecycleOperation *operation) override {}    //TODO implementation
  virtual void handleStopOperation(inet::LifecycleOperation *operation) override {}    //TODO implementation
  virtual void handleCrashOperation(inet::LifecycleOperation *operation) override {}    //TODO implementation
};

PropagationService::PropagationService(/* args */)
{
}

PropagationService::~PropagationService()
{
}


#endif // PROPAGATION_SERVICE_H
