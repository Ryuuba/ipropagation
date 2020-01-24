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

package infection.contract;

moduleinterface InfectionBase {
    parameters:
        @display("i=block/cogwheel"); //TODO: Update this icon
        volatile double unicastInterval @unit(s); //Time between two consecutive broadcasts
        volatile double recoveryInterval @unit(s); //Time between two consecutive recovery attemps
        double recoveryProbability; //a.k.a. mu
        double infectionProbability; //a.k.a. eta
        int initialStatus @enum(staringOperation=1, infected, notInfected);
        @signal[sentMessage](type=long);
        @statistic[sentMessage](
          title="Number of sent messages";
          record=histogram,vector
        );
        @signal[receivedMessage](type=long);
        @statistic[receivedMessage](
          title="Number of received messages";
          record=histogram,vector
        );
        @signal[infectionTime](type=long);
        @statistic[infectionTime](
          title="Duration of infection periods";
          record=histogram,vector;
          unit=s
        );
}