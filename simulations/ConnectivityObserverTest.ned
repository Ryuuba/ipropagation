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
import inet.physicallayer.contract.packetlevel.IRadioMedium;
import slaw.tripmanager.TripManager;
import informationpropagation.observer.ConnectivityObserver;

network ConnectivityObserverTest
{
    parameters:
        bool isSlaw = default(false);
        int hostNumber;
        connectivityobserver.hostNumber = hostNumber;
        node[*].mobility.numHosts = hostNumber;
        @display("bgb=200,200;bgg=100,2,grey50");
    submodules:
        radioMedium: <> like IRadioMedium{
            @display("p=200,50;is=s");
        }
        connectivityobserver: ConnectivityObserver {
            @display("p=200,100;is=s");
        }
        node[hostNumber]: WirelessBroadcastNode {
            @display("p=100,100;is=s");
        }
        tripmanager: TripManager if isSlaw {
            @display("p=200,;is=s");
        }
    connections allowunconnected:
}
