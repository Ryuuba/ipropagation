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

package informationpropagation.simulations;
import informationpropagation.node.WirelessBroadcastNode;
import informationpropagation.observer.ConnectivityObserver;
import inet.physicallayer.contract.packetlevel.IRadioMedium;
//
// TODO select a realistic radio medium
//
network NeighborCacheTest
{
    parameters:
        @display("bgb=180,180;bgg=,,grey10");
        int hostNumber = default(2);
    submodules:
        radioMedium: <> like IRadioMedium {
            @display("p=20,20;is=s");
        }
        node[hostNumber]: WirelessBroadcastNode {
            @display("p=100,100");
        }
        connectivityobserver: ConnectivityObserver {
            @display("p=20,160;is=s");
            hostNumber = hostNumber;
        }
    connections allowunconnected:
}
