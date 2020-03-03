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
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.probabilistic.ProbabilisticBroadcast;
//
// TODO auto-generated type
//
module WirelessBroadcastNode
{
    parameters:
        @display("i=device/palm2,,4");
        @labels(node,ad-hoc-node);
        @networkNode;
        bool hasStatus = default(false);
        string canvasImage = default("");
        string canvasImageColor = default("");
        string interfaceTableModule = "interfaceTable";
        string mobilityModule = "mobility";
        mobility.typename = default("");
        status.initialStatus = "NOT_INFO";
        int numWlanInterfaces = default(1);
        wlan[*].mgmt.typename = default("Ieee80211MgmtAdhoc");
        wlan[*].agent.typename = default("");
        wlan[*].radio.antenna.mobilityModule = default(absPath(".mobility"));
        *.interfaceTableModule = default(absPath(".interfaceTable"));
    gates:
        input radioIn[numWlanInterfaces] @directIn;
    submodules:
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=65,150;is=s");
        }
        status: NodeStatus if hasStatus {
            parameters:
                @display("p=65,50;is=s");
        }
        mobility: <default("")> like IMobility if typename != "" {
            parameters:
                @display("p=65,100;is=s");
        }
        app: <> like IApp {
            @display("p=226,40");
        }
        appl: MessageDispatcher {
            parameters:
                @display("p=226,103;b=200,5,,,,1");
        }
        //TODO: Fix the interface table initialization in ProbabilisticBroadcast
        net: ExtendedNetworkLayer {
            @display("p=226,167");
        }
        nl: MessageDispatcher {
            parameters:
                @display("p=226,229;b=200,5,,,,1");
        }
        wlan[numWlanInterfaces]: <default("Ieee80211Interface")> like IWirelessInterface {
            parameters:
                @display("p=226,296,row,150;q=queue");
        }
    connections allowunconnected:
        for i=0..sizeof(radioIn)-1 {
            radioIn[i] --> { @display("m=s"); } --> wlan[i].radioIn;
        }
        for i=0..sizeof(radioIn)-1 {
            wlan[i].upperLayerOut --> nl.in++;
            wlan[i].upperLayerIn <-- nl.out++;
        }
        app.outputPort --> appl.in++;
        appl.out++ --> app.inputPort;
        net.ifOut --> nl.in++;
        nl.out++ --> net.ifIn;
        net.transportOut --> appl.in++;
        appl.out++ --> net.transportIn;
}
