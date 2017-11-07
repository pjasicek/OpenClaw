#include "ClawLevelUtil.h"
#include "../GameApp/BaseGameApp.h"
#include "../GameApp/BaseGameLogic.h"

namespace ClawLevelUtil
{
    

    // Every logic in CaptainClaw is determined by its ID (logic string, e.g. "Soldier") and level number (e.g. 2)
    // This way same logics for different levels can be used, e.g. Soldier in level 1 and Soldier in level 2
    // - they differ by various things, most commonly image sets and health
    // This is a convinience function just to have it all at one place
    ActorPrototype ActorLogicToActorPrototype(int levelNumber, const std::string& logic)
    {
        const shared_ptr<LevelMetadata> pLevelMetadata = g_pApp->GetLevelMetadata(levelNumber);
        assert(pLevelMetadata != nullptr);

        auto findIt = pLevelMetadata->logicToActorPrototypeMap.find(logic);
        if (findIt == pLevelMetadata->logicToActorPrototypeMap.end())
        {
            return ActorPrototype_None;
        }

        return findIt->second;
    }

    Point GetClawSpawnLocation(int checkpointNumber, int levelNumber)
    {
        shared_ptr<LevelMetadata> pLevelMetadata = g_pApp->GetLevelMetadata(levelNumber);
        assert(pLevelMetadata != nullptr);

        return GetValueFromMap(checkpointNumber, pLevelMetadata->checkpointNumberToSpawnPositionMap);

        /*switch (levelNumber)
        {
            case 11:
                if (checkpointNumber == 0) { return Point(3372, 1159); }
                else if (checkpointNumber == 1) { return Point(17168, 1417); }
                else if (checkpointNumber == 2) { return Point(10897, 3912); }
                break;
            case 12:
                if (checkpointNumber == 0) { return Point(1709, 2258); }
                else if (checkpointNumber == 1) { return Point(11519, 3144); }
                else if (checkpointNumber == 2) { return Point(31537, 3596); }
                break;
            case 13:
                if (checkpointNumber == 0) { return Point(712, 2387); }
                else if (checkpointNumber == 1) { return Point(13428, 1672); }
                else if (checkpointNumber == 2) { return Point(24496, 1992); }
                break;
            case 14:
                if (checkpointNumber == 0) { return Point(1723, 3173); }
                else if (checkpointNumber == 1) { return Point(9845, 3037); }
                else if (checkpointNumber == 2) { return Point(19892, 3549); }
                break;
            default:
            assert(false && "Not implemented yet");
        }

        LOG_ERROR("Conflicting LevelNumber: " + ToStr(levelNumber) + ", conflicting CheckpointNumber: " + ToStr(checkpointNumber));
        assert(false && "Unknown level / checkpoint number.");
        return Point();*/
    }

    shared_ptr<LevelData> GetDebugLoadLevelData()
    {
        int debugLevelNumber = g_pApp->GetDebugOptions()->skipMenuToLevel;

        shared_ptr<LevelData> pLevelData(new LevelData(debugLevelNumber, false, 0));
        return pLevelData;
    }

    StrongActorPtr CreateSpecialDeathEffect(const Point& location, int levelNumber)
    {
        const shared_ptr<LevelMetadata> pLevelMetadata = g_pApp->GetLevelMetadata(levelNumber);
        assert(pLevelMetadata != nullptr);

        if (pLevelMetadata->tileDeathEffectType == "LiquidSplash")
        {
            return ActorTemplates::CreateSingleAnimation(
                location + pLevelMetadata->tileDeathEffectOffset, 
                AnimationType_TarSplash);
        }

        return nullptr;
    }

    bool TryGetTopLadderInfo(int levelNumber, int tileId, Point& fixtureOffset)
    {
        const shared_ptr<LevelMetadata> pLevelMetadata = g_pApp->GetLevelMetadata(levelNumber);
        assert(pLevelMetadata != nullptr);

        auto findIt = pLevelMetadata->tileIdToTopLadderEndMap.find(tileId);
        if (findIt != pLevelMetadata->tileIdToTopLadderEndMap.end())
        {
            fixtureOffset = findIt->second;
            return true;
        }

        return false;
    }
}
