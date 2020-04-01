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
import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;
import slaw.tripmanager.TripManager;

network MobilityTest
{
    parameters:
        int hostNumber;
        @display("bgb=2000,2000;bgg=100,2,grey50");
    submodules:
        radioMedium: Ieee80211ScalarRadioMedium {
            @display("p=200,250;is=s");
        }
        tripmanager: TripManager {
            @display("p=200,450;is=s");
            walkermodel.numOfWalker = hostNumber;
            walkermodel.hurstParameter = 0.75; //?
        }
        node[hostNumber]: WirelessBroadcastNode {
            @display("p=1000,500;is=s");
        }
    connections allowunconnected:
}
