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
import informationpropagation.observer.InfectionObserver;
import inet.physicallayer.contract.packetlevel.IRadioMedium;
import slaw.mobility.SlawMobility;

network MobilityTest
{
    parameters:
        @display("bgb=300,300;bgg=,,grey10");
        int hostNumber = default(2);
        int lambda;
        bool isSLAW = default(false);
        double unitTime @unit(s);
        double maxBroadcastDelay @unit(s);
    submodules:
        radioMedium: <> like IRadioMedium {
            @display("p=480,20;is=s");
        }
        node[hostNumber]: WirelessBroadcastNode {
            @display("p=250,250;is=s");
            mobility.numHosts = hostNumber;
            net.np.maxFirstBcastBackoff = maxBroadcastDelay;
            app.unitTime = unitTime;
            app.lambda = lambda;
            app.maxBcastDelay = maxBroadcastDelay;
        }
        connectivityobserver: ConnectivityObserver {
            @display("p=480,80;is=s");
            hostNumber = hostNumber;
        }
        infectionobserver: InfectionObserver {
            @display("p=480,140;is=s");
            hostNumber = hostNumber;
            lambda = lambda;
            unitTime = unitTime;
            maxBcastDelay = maxBroadcastDelay;
        }
    connections allowunconnected:
}
