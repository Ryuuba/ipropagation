//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package informationpropagation.networklayer;
import informationpropagation.base.NeighborDiscoveryProtocolBase;

//
// TODO auto-generated type
//
simple HelloProtocol extends NeighborDiscoveryProtocolBase
{
    parameters:
        @class(HelloProtocol);
        @display("i=block/network");
        // Maximum number of contact attempts a node must perform to consider a 
        // neighbor is out of range
        int maxAttemptNumber;
        // backoff time to send a hello message
        double maximumBcastDelay @unit(s);
        // The amount of time from which a node is considered unreachable
        double flushTime @unit(s) = default(2*discoveryTime);
        // Allows the module to emit hello message after the warm-up period
        bool isStaticNetwork = default(false);
        // Sets this protocol to work as a one-way protocol
        bool isOneWay = default(false);
}