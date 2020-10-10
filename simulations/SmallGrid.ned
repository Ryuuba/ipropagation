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

//
// TODO select a realistic radio medium
//
network SmallGrid
{
    parameters:
        @display("bgb=250,160;bgg=,,grey10");
        double unitTime @unit(s);
        int lambda;
        double maxBroadcastDelay @unit(s);
    submodules:
        node[16]: WirelessBroadcastNode {
            @display("p=200,200;is=s");
            mobility.numHosts = 16;
            net.np.maxFirstBcastBackoff = maxBroadcastDelay;
            app.unitTime = unitTime;
            app.lambda = lambda;
            app.maxBcastDelay = maxBroadcastDelay;
        }
        radioMedium: <> like IRadioMedium {
            @display("p=200,20;is=s");
        }
        connectivityobserver: ConnectivityObserver {
            @display("p=200,60;is=s");
            hostNumber = 16;
            unitTime = unitTime;
            lambda = lambda;
            maxBcastDelay = maxBroadcastDelay;
        }
        infectionobserver: InfectionObserver {
            @display("p=200,100;is=s");
            hostNumber = 16;
            unitTime = unitTime;
            lambda = lambda;
            maxBcastDelay = maxBroadcastDelay;
        }
    connections allowunconnected:
}
