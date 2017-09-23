#ifndef __CLAW_LEVEL_UTIL_H__
#define __CLAW_LEVEL_UTIL_H__

#include "../SharedDefines.h"

// This is meant to be a single point of entry to all stuff which is
// required to be defined in code when new level is added so that 
// it is not scattered throughout 10 source files
// (not counting implementing new components etc)
class LevelData;
namespace ClawLevelUtil
{
    // Actor logic name + level number -> actor prototype
    ActorPrototype ActorLogicToActorPrototype(int levelNumber, const std::string& logic);

    // Checkpoint number + level number -> Claw (re)spawn location
    Point GetClawSpawnLocation(int checkpointNumber, int levelNumber);

    // When directly loading some level it will use this data
    shared_ptr<LevelData> GetDebugLoadLevelData();

    // Certain levels spawn various death effects (water splash, tar splash, etc)
    // - Returns nullptr if no special death effect is in specified level (e.g. death by spikes)
    StrongActorPtr CreateSpecialDeathEffect(const Point& location, int levelNumber);

    // Ladders contain a fixture on top so that Claw can stand on top of the ladders
    // - Returns false if specified tile ID and level number combination is NOT ladder endtile
    // - Returns true if specified tile ID and level number combinaton IS ladder endtile and sets @fixtureOffset
    bool TryGetTopLadderInfo(int levelNumber, int tileId, Point& fixtureOffset);
}

#endif