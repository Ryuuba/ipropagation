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
import informationpropagation.contract.IApp;
import inet.common.MessageDispatcher;
//
// TODO auto-generated type
//
network AppTest
{
    parameters:
        @display("bgb=250,250");
        string appKind = "BroadcastInfectionApp";
        int appNumber = default(100);
    submodules:
        app[appNumber]: <appKind> like IApp {
            parameters:
                @display("p=129,47");
        }
        bus: MessageDispatcher {
            parameters:
                @display("p=129,197;b=200,5,,,,1");
        }
    connections:
        for i=0..appNumer-1 {
            app[i].outputPort --> bus.in++;
            app[i].inputPort <-- bus.out++;
        }
}
