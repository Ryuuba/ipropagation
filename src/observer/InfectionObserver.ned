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
// This observer keeps track the number of infected nodes receiving the status
// signal from them. In addition, this module finishes the simulation when 
// the system reaches its stationary state, that is, whem the average of 
// infected nodes is stable
//
simple InfectionObserver
{
    int roundNumber; // Number of rounds
    int hostNumber;
    double step @unit(s); //the duration of a round
    @display("i=observer/infection_observer");
    @signal[infectedNodes](type = unsigned long);
    @statistic[infectedNodes](
        title= "Number of infected nodes"; 
        record = vector
    );
    @signal[rho](type = double);
    @statistic[rho](
        title = "Expected infection density";
        recor = vector
    );
}

