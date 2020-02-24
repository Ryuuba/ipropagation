#include "HelloProtocol.h"

Define_Module(HelloProtocol);

void HelloProtocol::initialize(int stage) {
  if (stage == inet::INITSTAGE_NETWORK_LAYER)
    NeighborDiscoveryProtocolBase::initialize(stage);
  if (stage == inet::INITSTAGE_LOCAL) {
    //gets pointer to the neighbor cache

  }
}