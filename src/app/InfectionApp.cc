#include "Infection.h"

Define_Module(Infection);

Infection::Infection()
  : recovey_timer(nullptr), packet_name("infection"), infection_time(0), 
    packet_size(0)
{ }

Infection::~Infection()
{
  cancelAndDelete(selfMsg);
}

void Infection::initialize(int stage)
{
  ApplicationBase::initialize(stage);
  if (stage == inet::INITSTAGE_LOCAL) {
    WATCH(infection_time);
    packet_size = par("packetSize");
    
  }
}
