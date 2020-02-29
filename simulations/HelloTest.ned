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
import inet.physicallayer.unitdisk.UnitDiskRadioMedium;
//
// TODO select a realistic radio medium
//
network HelloTest
{
    parameters:
        @display("bgb=200,200;bgg=,,grey10");
    submodules:
        radioMedium: UnitDiskRadioMedium {
            parameters:
                @display("p=53,24;is=s");
        }
        nodeA: WirelessBroadcastNode {
            @display("p=100,100");
        }
        nodeB: WirelessBroadcastNode {
            @display("p=100,130");
        }
    connections allowunconnected:
}
