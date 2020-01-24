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

#include "InfectionBase.h"

Register_Abstract_Class(InfectionBase);

InfectionBase::InfectionBase() : 
  operational_status(InfectionBase::Status::STARTING_OPERATION),
  last_change(0.0),
  recovery_probability(0.0),
  infection_probability(0.0),
  unicast_interval(0.0),
  unicast_timer(nullptr),
  recovery_timer(nullptr),
  infection_time(0.0),
  sent_messages(0),
  received_messages(0)
{
  // TODO Auto-generated constructor stub
}

InfectionBase::~InfectionBase()
{
  cancelAndDelete(unicast_timer);
  cancelAndDelete(recovery_timer);
}

