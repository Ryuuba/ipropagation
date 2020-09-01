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

#if !defined(FORWARDING_LIST_NOTIFICATION_H)
#define FORWARDING_LIST_NOFITICATION_H

#include <memory>
#include <omnetpp.h>
#include "../msg/ProbabilisticCastHeader_m.h"

class ForwardingListNotification 
  : public omnetpp::cObject
  , omnetpp::noncopyable
{
public:
  DestinationListPtr forwarding_list;
public:
  ForwardingListNotification() 
    : forwarding_list(nullptr)
  { }
  
  ForwardingListNotification(
    const std::shared_ptr<const DestinationList> _forwarding_list
  )
    : forwarding_list(_forwarding_list)
  { }
};



#endif // FORWARDING_LIST_NOTIFICATION_H