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

//
// TODO auto-generated type
//
module ExtendedNetworkLayer extends SimpleNetworkLayer
{
    parameters:
        @display("i=block/broadcast");
    submodules:
        dp: <default("")> like INeighborDiscoveryProtocol if typename != "" {
            @display("p=450,300");
        }
        cache: <default("")> like INeighborCache if typename != "" {
            @display("p=500,300");
        }
    connections allowunconnected:
        dp.inputPort <-- lp.out++ if exists(dp);
        dp.outputPort --> lp.in++ if exists(cache);
        
}
