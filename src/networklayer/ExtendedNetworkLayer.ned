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

package informationpropagation.networklayer;
import inet.networklayer.common.SimpleNetworkLayer;
import informationpropagation.contract.INeighborDiscoveryProtocol;
import informationpropagation.contract.INeighborCache;
import informationpropagation.networklayer.ProbabilisticCast;

//
// This class extends the INET simple network layer with two modules:
// a neighbor Discovery Protocol and a Neighbor Cache.
//
module ExtendedNetworkLayer extends SimpleNetworkLayer
{
    parameters:
        @display("i=block/broadcast");
        np.typename = "ProbabilisticCast";
    submodules:
        dp: <default("")> like INeighborDiscoveryProtocol if typename != "" {
            @display("p=450,300");
        }
        cache: <default("")> like INeighborCache if typename != "" {
            @display("p=41,76");
        }
    connections allowunconnected:
        dp.indicationPort <-- lp.out++ if exists(dp);
        dp.protocolPort --> lp.in++ if exists(dp);
}
