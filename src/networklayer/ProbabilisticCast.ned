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
import inet.networklayer.base.NetworkProtocolBase;
import inet.networklayer.contract.INetworkProtocol;;


// Author: Adán G. Medrano-Chávez
// This module transmitwith probability beta a packet via L2 broadcast.
// Destinations could be a single node, several nodes or all nodes in 
// the range of coverage. In this way, this protocol is able to perfom unicast,
// multicast or broadcast communications taking advantage of the natural
// L2 broadcast. The communication scheme is configurable. Destinations are
// drawn in a random way. This protocol forwarding packets only if the option
// is set. In this case, its behavior is similar to a probabilistic flooding.
// This protocol is useful to evaluate ad hoc networks
//
// This module is based on the ProbabilisticBroadcast of Damien Piguet
// inet.networklayer.probabilistic.ProbabilisticBroadcast.ned
//


simple ProbabilisticCast extends NetworkProtocolBase like INetworkProtocol {
    @class(ProbabilisticCast);
    // Enable L3 forwarding
    bool forwading = default(false);
    // Routing communication scheme
    string communicationMode @enum("unicast", "multicast", "broadcast") = default("unicast");
    // The number of hops to discard a packet
    int hopLimit = default(5);
    // length of the network packet header (in bits)
    // (took from ProbabilisticBroadcast)
    int headerLength @unit(b) = default(96b);
    // probability of broadcast for each attempt
    // (took from ProbabilisticBroadcast)
    double beta = default(0.8);}
    // How many seconds the message should be kept in queue after its died.
    // That way the message is known if the node receives one of its
    // copy that isn't dead because of TTL de-synchronization due to
    // MAC backoff, propagation delay and clock drift.
    // (took from ProbabilisticBroadcast)
    double timeInQueueAfterDeath @unit(s) = default(60 s);
    // period between two probabilistic broadcast
    // (took from ProbabilisticBroadcast)
    double bcperiod @unit(s) = default(2 s);
    // maximal back-off before first broadcast attempt [seconds].
    // if don't want to use this parameter, simply set it
    // to a value that is greater than bcperiod.
    // (took from ProbabilisticBroadcast)
    double maxFirstBcastBackoff @unit(s) = default(1 s);
}