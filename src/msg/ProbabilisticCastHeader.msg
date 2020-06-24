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

import inet.common.INETDefs;
import inet.common.Units;
import inet.networklayer.common.IpProtocolId;
import inet.networklayer.probabilistic.ProbabilisticBroadcastHeader;

cplusplus {{
#include <list>
#include <memory>
#include "inet/common/ProtocolGroup.h"
typedef std::list<inet::L3Address> DestinationList;
typedef std::shared_ptr<const DestinationList> DestinationListPtr;
}}

class DestinationList { @existingClass; }

class DestinationListPtr { @existingClass; }

namespace inet;

class ProbabilisticCastHeader extends ProbabilisticBroadcastHeader
{
    long hopCount;
    DestinationListPtr forwardingList;
}

