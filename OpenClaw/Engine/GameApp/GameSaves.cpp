#include "GameSaves.h"
#include "../Util/ClawLevelUtil.h"

Point GetSpawnPosition(uint32 levelNumber, uint32 checkpointNumber)
{
    return ClawLevelUtil::GetClawSpawnLocation(checkpointNumber, levelNumber);
}