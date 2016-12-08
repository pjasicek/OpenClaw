#ifndef __ACTOR_TEMPLATES_H__
#define __ACTOR_TEMPLATES_H__

#include "../SharedDefines.h"
#include "Actor.h"

namespace ActorTemplates
{
    // Exposed Xml data generating functions.
    TiXmlElement* CreateXmlData_CrateActor(std::string imageSet, Point position, const std::vector<PickupType>& loot, uint32 health, int32 zCoord);
    TiXmlElement* CreateXmlData_PowderKegActor(std::string imageSet, Point position, int32 damage, int32 zCoord);
    TiXmlElement* CreateXmlData_CrumblingPeg(std::string imageSet, Point position, int32 zCoord);

    // This is used by the game itself
    StrongActorPtr CreateActorPickup(PickupType pickupType, Point position, bool isStatic = false);
    StrongActorPtr CreatePowerupSparkleActor();
    StrongActorPtr CreateClawProjectile(AmmoType ammoType, Direction direction, Point position);
    StrongActorPtr CreateProjectile(std::string imageSet, Direction direction, Point position);
    StrongActorPtr CreateExplosion(Point position, Point size, int32 damage, std::string imageSet = "", int32 zCoord = 0);
};

#endif