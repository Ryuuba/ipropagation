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
import inet.node.inet.INetworkNode;
import inet.common.lifecycle.NodeStatus;
import inet.mobility.contract.IMobility;
import inet.linklayer.contract.IWirelessInterface;
import inet.networklayer.common.InterfaceTable;
import inet.common.MessageDispatcher;
import inet.networklayer.contract.INetworkLayer;
import inet.transportlayer.contract.IUdp;
import inet.applications.contract.IApp;
import inet.networklayer.common.InterfaceTable;
//
// TODO auto-generated type
//
module WirelessUnicastNode like INetworkNode
{
    parameters:
        @display("i=device/palm2,,4");
        @labels(node,ad-hoc-node);
        @display("bgb=1256,672");
        string canvasImage = default("");
        string canvasImageColor = default("");
        string mobilityType = default("");
        *.interfaceTableModule = default(absPath(".interfaceTable"));
        wlan[*].radio.antenna.mobilityModule = default("^.^.^.mobility");
        @figure[linkLayer](type=rectangle; pos=250,456; size=1000,210; fillColor=#0000ff; lineColor=#808080; cornerRadius=5; fillOpacity=0.1);
        @figure[linkLayer.title](type=text; pos=1245,461; anchor=ne; text="link layer");
        @figure[networkLayer](type=rectangle; pos=250,306; size=1000,130; fillColor=#00ff00; lineColor=#808080; cornerRadius=5; fillOpacity=0.1);
        @figure[networkLayer.title](type=text; pos=1245,311; anchor=ne; text="network layer");
        @figure[transportLayer](type=rectangle; pos=250,156; size=1000,130; fillColor=#ff0000; lineColor=#808080; cornerRadius=5; fillOpacity=0.1);
        @figure[transportLayer.title](type=text; pos=1245,161; anchor=ne; text="transport layer");
        @figure[applicationLayer](type=rectangle; pos=250,6; size=1000,130; lineColor=#808080; cornerRadius=5; fillColor=#ffff00; fillOpacity=0.1);
        @figure[applicationLayer.title](type=text; pos=1245,11; anchor=ne; text="application layer");
    gates:
        input radioIn[numWlanInterfaces] @directIn;
    submodules:
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=125,240;is=s");
        }
        status: NodeStatus {
            parameters:
                @display("p=65,32;is=s");
        }
        mobility: <mobilityType> like IMobility if typename != "" {
            parameters:
                @display("p=65,90;is=s");
        }
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=65,151;is=s");
        }
        wlan[numWlanInterfaces]: <default("Ieee80211Interface")> like IWirelessInterface {
            parameters:
                @display("p=217,437,row,150;q=queue");
        }
        ipv4: <default("Ipv4NetworkLayer")> like INetworkLayer {
            parameters:
                @display("p=217,303;q=queue");
        }
        nl: MessageDispatcher {
            parameters:
                @display("p=217,370;b=200,5,,,,1");
        }
        udp: <default("Udp")> like IUdp {
            parameters:
                @display("p=217,159");
        }
        tn: MessageDispatcher {
            parameters:
                @display("p=217,233;b=200,5,,,,1");
        }
        app: <default("BroadcastApp")> like IApp {
            parameters:
                @display("p=217,32,row,150");
        }
        at: MessageDispatcher {
            parameters:
                @display("p=217,89;b=200,5,,,,1");
        }
    connections allowunconnected:
        for i=0..sizeof(radioIn)-1 {
            radioIn[i] --> { @display("m=s"); } --> wlan[i].radioIn;
        }
        for i=0..sizeof(radioIn)-1 {
            wlan[i].upperLayerOut --> nl.in++;
            wlan[i].upperLayerIn <-- nl.out++;
        }
        ipv4.ifIn <-- nl.out++;
        ipv4.ifOut --> nl.in++;
        udp.ipOut --> tn.in++;
        udp.ipIn <-- tn.out++;
        app.socketOut --> at.in++;
        app.socketIn <-- at.out++;
        tn.out++ --> ipv4.transportIn;
        tn.in++ <-- ipv4.transportOut;
        at.out++ --> udp.appIn;
        at.in++ <-- udp.appOut;
}
