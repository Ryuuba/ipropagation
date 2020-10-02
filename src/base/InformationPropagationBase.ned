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

simple InformationPropagationBase like IApp{
    parameters:
        @display("i=block/infection_app");
        double unitTime @unit(s);
        double recoveryProbability;  //a.k.a. mu
        int lambda;          //a.k.a. lambda
        double initialInfectionProbability; //a.k.a rho0
        @signal[srcSet](type=SourceNotification?);
        @signal[sentMessage](type = unsigned long);
        @statistic[sentMessage](title = "number of sent infectious messages"; record = vector);
        @signal[recvMessage](type = unsigned long);
        @statistic[recvMessage](title = "number of received infectious messages"; record = vector);
        @signal[infectionTime](type = simtime_t);
        @statistic[infectionTime](title = "infection time"; record = vector);
        @signal[lastStatus](type = long);
        @statistic[lastStatus](title = "last recorded status"; record = vector);
    gates:
        input inputSocket @labels(from TransportLayerDispatcher);
        output outputSocket @labels(to TransportLayerDispatcher);
}