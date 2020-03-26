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
import informationpropagation.networklayer.ExtendedNetworkLayer;
import informationpropagation.contract.IApp;
import inet.common.lifecycle.NodeStatus;
import inet.mobility.contract.IMobility;
import inet.linklayer.contract.IWirelessInterface;
import inet.common.MessageDispatcher;
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
        status.initialStatus = "NOT_INFO";
        int numWlanInterfaces = default(1);
        mobility.subjectModule = default("^");
        wlan[*].mgmt.typename = default("Ieee80211MgmtAdhoc");
        wlan[*].agent.typename = default("");
        wlan[*].radio.antenna.mobilityModule = default("^.^.^.mobility");
        wlan[*].radio.antenna.mobility.typename = "";
        *.interfaceTableModule = default(absPath(".interfaceTable"));
    gates:
        input radioIn[numWlanInterfaces] @directIn;
    submodules:
        status: NodeStatus if hasStatus {
            parameters:
                @display("p=65,50;is=s");
        }
        mobility: <default("StationaryMobility")> like IMobility {
            parameters:
                @display("p=65,100;is=s");
        }
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=65,150;is=s");
        }
        wlan[numWlanInterfaces]: <default("Ieee80211Interface")> like IWirelessInterface {
            parameters:
                @display("p=226,296,row,150;q=queue");
        }
        nl: MessageDispatcher {
            parameters:
                @display("p=226,229;b=200,5,,,,1");
        }
        net: ExtendedNetworkLayer {
            @display("p=226,167");
        }
        appl: MessageDispatcher {
            parameters:
                @display("p=226,103;b=200,5,,,,1");
        }
        app: <> like IApp {
            @display("p=226,40");
        }
    connections allowunconnected:
        for i=0..sizeof(radioIn)-1 {
            radioIn[i] --> { @display("m=s"); } --> wlan[i].radioIn;
        }
        for i=0..sizeof(radioIn)-1 {
            wlan[i].upperLayerOut --> nl.in++;
            wlan[i].upperLayerIn <-- nl.out++;
        }
        app.outputSocket --> appl.in++;
        appl.out++ --> app.inputSocket;
        net.ifOut --> nl.in++;
        nl.out++ --> net.ifIn;
        net.transportOut --> appl.in++;
        appl.out++ --> net.transportIn;
}
