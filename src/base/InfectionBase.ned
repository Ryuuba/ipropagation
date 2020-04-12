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

package informationpropagation.base;
import informationpropagation.contract.IApp;

simple InfectionBase like IApp{
    parameters:
        @display("i=block/infection_app");
        volatile double sentInterval @unit(s);
        volatile double recoveryInterval @unit(s);
        double statusReportInterval @unit(s) = default (1s);
        double recoveryProbability; //a.k.a. mu
        double infectionProbability; //a.k.a. eta
        @signal[sentMessage](type = long);
        @statistic[sentMessage](title = "number of sent infectios messages"; record=histogram,vector);
        @signal[receivedMessage](type = long);
        @statistic[receivedMessage](title = "number of received infectious messages"; record=histogram,vector);
        @signal[infectionTime](type = simtime_t);
        @statistic[infectionTime](title = "infection time"; record=histogram,vector);
        @signal[lastStatus](type = long);
        @statistic[lastStatus](title = "last recorded status"; record=vector);
    gates:
        input inputSocket @labels(from TransportLayerDispatcher);
        output outputSocket @labels(to TransportLayerDispatcher);
}