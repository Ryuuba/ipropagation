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

package informationpropagation.node;
import inet.common.lifecycle.NodeStatus;
import inet.mobility.contract.IMobility;
import inet.linklayer.contract.IWirelessInterface;
import inet.common.MessageDispatcher;
import informationpropagation.networklayer.ExtendedNetworkLayer;
import informationpropagation.contract.IApp;

//
// TODO auto-generated type
//
module WirelessBroadcastNode
{
    parameters:
        @display("i=device/palm2,,4");
        @labels(node,ad-hoc-node);
        string canvasImage = default("");
        string canvasImageColor = default("");
        mobility.typename = default("");
        status.initialStatus = "NOT_INFO";
        int numWlanInterfaces = default(1);
    gates:
        input radioIn[numWlanInterfaces] @directIn;
    submodules:
        status: NodeStatus {
            parameters:
                @display("p=65,32;is=s");
        }
        mobility: <default("")> like IMobility if typename != "" {
            parameters:
                @display("p=65,90;is=s");
        }
        app: <> like IApp {
            @display("p=226,98");
        }
        net: ExtendedNetworkLayer {
            @display("p=226,188");
        }
        nl: MessageDispatcher {
            parameters:
                @display("p=226,264;b=200,5,,,,1");
        }
        wlan[numWlanInterfaces]: <default("Ieee80211Interface")> like IWirelessInterface {
            parameters:
                @display("p=226,341,row,150;q=queue");
        }
    connections allowunconnected:
        for i=0..sizeof(radioIn)-1 {
            radioIn[i] --> { @display("m=s"); } --> wlan[i].radioIn;
        }
        for i=0..sizeof(radioIn)-1 {
            wlan[i].upperLayerOut --> nl.in++;
            wlan[i].upperLayerIn <-- nl.out++;
        }
        nl.out++ --> wlan[0].upperLayerIn;
        net.ifOut --> nl.in++;
        nl.out++ --> net.ifIn;
        app.outputPort --> net.transportIn;
        net.transportOut --> app.inputPort;
}
