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

network RadioTest {
    parameters:
        int hostNumber;
        @display("bgb=400,400;bgg=,,grey10");
        node[0].mobility.initialX = 100m;
        node[0].mobility.initialY = 100m;
        node[1].mobility.initialX = 200m;
        node[1].mobility.initialY = 100m;
        node[2].mobility.initialX = 300m;
        node[2].mobility.initialY = 100m;
        node[3].mobility.initialX = 100m;
        node[3].mobility.initialY = 200m;
        node[4].mobility.initialX = 200m;
        node[4].mobility.initialY = 200m;
        node[5].mobility.initialX = 300m;
        node[5].mobility.initialY = 200m;
        node[6].mobility.initialX = 100m;
        node[6].mobility.initialY = 300m;
        node[7].mobility.initialX = 200m;
        node[7].mobility.initialY = 300m;
        node[8].mobility.initialX = 300m;
        node[8].mobility.initialY = 300m;
    submodules:
        radioMedium: Ieee80211ScalarRadioMedium {
            @display("p=50,25;is=s");
        }
        node[hostNumber]: WirelessBroadcastNode {
            @display("p=100,100");
        }
    connections allowunconnected:
}