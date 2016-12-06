#ifndef __ACTOR_TEMPLATES_H__
#define __ACTOR_TEMPLATES_H__

#include "../SharedDefines.h"
#include "Actor.h"

namespace ActorTemplates
{
    StrongActorPtr CreateActorPickup(PickupType pickupType, Point position, bool isStatic = false);
    StrongActorPtr CreatePowerupSparkleActor();
    StrongActorPtr CreateClawProjectile(AmmoType ammoType, Direction direction, Point position);
    StrongActorPtr CreateProjectile(std::string imageSet, Direction direction, Point position);
};

#endif