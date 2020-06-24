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

#if !defined(NEIGHBORHOOD_NOTIFICATION_H)
#define NEIGHBORHOOD_NOTIFICATION_H

#include <memory>
#include <omnetpp.h>
#include "../networklayer/CacheRegister.h"

class NeighborhoodNotificacion : public omnetpp::cObject, omnetpp::noncopyable {
public:
  std::shared_ptr< const std::list<cache_register> > neighborhood;
public:
  NeighborhoodNotificacion() : neighborhood(nullptr) {}
  NeighborhoodNotificacion(
    const std::shared_ptr< const std::list<cache_register> > neighborhood_
  )
    : neighborhood(neighborhood_)
  { }
};

Register_Class(NeighborhoodNotificacion);


#endif // NEIGHBORHOOD_NOTIFICATION_H