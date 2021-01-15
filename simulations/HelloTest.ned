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
import informationpropagation.node.HelloNode;
import inet.physicallayer.contract.packetlevel.IRadioMedium;
import slaw.map.SelfSimilarWaypointMap;
//
// 
//
network HelloTest
{
    parameters:
        @display("bgb=200,200;bgg=,,grey10");
        int hostNumber = default(2);
        bool hasSelfSimilarMap = default(false);
        double unitTime @unit(s);
        double maxBroadcastDelay @unit(s);
    submodules:
        radioMedium: <> like IRadioMedium {
            @display("p=200,200;is=s");
        }
        node[hostNumber]: HelloNode {
            @display("p=150,150;is=s");
            mobility.numHosts = hostNumber;
            net.np.maxFirstBcastBackoff = maxBroadcastDelay;
        }
    connections allowunconnected:
}
