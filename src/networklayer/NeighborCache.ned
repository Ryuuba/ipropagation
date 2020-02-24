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
import informationpropagation.contract.INeighborCache;

//
// TODO auto-generated type
//
simple NeighborCache like INeighborCache
{
    parameters:
        //TODO: implement an algorithm to erase the cache
        //double removeNeighborTimer
        //The time to emit the cache to a global observer
        double signalingTime;
        @display("i=block/table");
        @signal[neighborhood](type=neighbor_cache?);
}
