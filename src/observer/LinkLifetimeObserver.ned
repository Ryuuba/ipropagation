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

package informationpropagation.observer;

//
// This module receives the neighborhood of each mobile host and then it 
// computes the adjacency matrix. The actualization of this module is done 
// periodically, each time the neighbor cache in the network layer emits
// a neighborhood notification
//
// This observer is particularly useful when nodes move
//

simple LinkLifetimeObserver
{
    parameters:
        @display("i=observer/connectivity_observer");
        int hostNumber;
}
