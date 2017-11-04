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
        static std::map<std::pair<int, std::string>, ActorPrototype> s_LevelLogicToActorProtoMap =
        {
            // Level 1
            { { 1, "Soldier" }, ActorPrototype_Level1_Soldier },
            { { 1, "Officer" }, ActorPrototype_Level1_Officer },
            { { 1, "Rat" }, ActorPrototype_Level1_Rat },
            { { 1, "Elevator" }, ActorPrototype_BaseElevator },
            { { 1, "StartElevator" }, ActorPrototype_BaseElevator },
            { { 1, "TriggerElevator" }, ActorPrototype_BaseElevator },
            { { 1, "CrumblingPeg" }, ActorPrototype_Level1_CrumblingPeg },
            { { 1, "TogglePeg" }, ActorPrototype_Level1_TogglePeg },
            { { 1, "TogglePeg2" }, ActorPrototype_Level1_TogglePeg },
            { { 1, "TogglePeg3" }, ActorPrototype_Level1_TogglePeg },
            { { 1, "TogglePeg4" }, ActorPrototype_Level1_TogglePeg },

            // Level 2
            { { 2, "Soldier" }, ActorPrototype_Level2_Soldier },
            { { 2, "Officer" }, ActorPrototype_Level2_Officer },
            { { 2, "PunkRat" }, ActorPrototype_Level2_PunkRat },
            { { 2, "Raux" }, ActorPrototype_Level2_LaRaux },
            { { 2, "BossStager" }, ActorPrototype_Level2_BossStager },
            { { 2, "TogglePeg" }, ActorPrototype_Level2_TogglePeg },
            { { 2, "TogglePeg2" }, ActorPrototype_Level2_TogglePeg },
            { { 2, "TogglePeg3" }, ActorPrototype_Level2_TogglePeg },
            { { 2, "TogglePeg4" }, ActorPrototype_Level2_TogglePeg },
            { { 2, "Elevator" }, ActorPrototype_BaseElevator },
            { { 2, "OneWayTriggerElevator" }, ActorPrototype_BaseElevator },
            { { 2, "OneWayStartElevator" }, ActorPrototype_BaseElevator },
            { { 2, "TriggerElevator" }, ActorPrototype_BaseElevator },
            { { 2, "TowerCannonLeft" }, ActorPrototype_Level2_TowerCannonLeft },
            { { 2, "TowerCannonRight" }, ActorPrototype_Level2_TowerCannonRight },

            // Level 3
            { { 3, "CutThroat" }, ActorPrototype_Level3_CutThroat },
            { { 3, "RobberThief" }, ActorPrototype_Level3_RobberThief },
            { { 3, "Rat" }, ActorPrototype_Level3_Rat },
            { { 3, "Elevator" }, ActorPrototype_BaseElevator },
            { { 3, "OneWayTriggerElevator" }, ActorPrototype_BaseElevator },
            { { 3, "TriggerElevator" }, ActorPrototype_BaseElevator },
            { { 3, "PathElevator" }, ActorPrototype_BasePathElevator },
            { { 3, "CrumblingPeg" }, ActorPrototype_Level3_CrumblingPeg }, 

            // Level 4
            { { 4, "CutThroat" }, ActorPrototype_Level4_CutThroat },
            { { 4, "RobberThief" }, ActorPrototype_Level4_RobberThief },
            { { 4, "Rat" }, ActorPrototype_Level4_Rat },
            { { 4, "Katherine" }, ActorPrototype_Level4_Katherine },
            { { 4, "Elevator" }, ActorPrototype_BaseElevator },
            { { 4, "CrumblingPeg" }, ActorPrototype_Level4_CrumblingPeg },
            { { 4, "BossStager" }, ActorPrototype_Level4_BossStager },
            { { 4, "SpringBoard" }, ActorPrototype_Level4_SpringBoard },
            { { 4, "StartSteppingStone" }, ActorPrototype_Level4_SteppingGround },
            { { 4, "SteppingStone" }, ActorPrototype_Level4_SteppingGround },
            { { 4, "SteppingStone2" }, ActorPrototype_Level4_SteppingGround },
            { { 4, "SteppingStone3" }, ActorPrototype_Level4_SteppingGround },
            { { 4, "SteppingStone4" }, ActorPrototype_Level4_SteppingGround },

            // Level 5
            { { 5, "CrumblingPeg" }, ActorPrototype_Level5_CrumblingPeg },
            { { 5, "Elevator" }, ActorPrototype_BaseElevator },
            { { 5, "TriggerElevator" }, ActorPrototype_BaseElevator },
            { { 5, "BehindStatue" }, ActorPrototype_Level5_LootStatue },
            { { 5, "FrontStatue" }, ActorPrototype_Level5_LootStatue },
            { { 5, "Seagull" }, ActorPrototype_Level5_Seagull },
            { { 5, "TogglePeg" }, ActorPrototype_Level5_TogglePeg },
            { { 5, "TogglePeg2" }, ActorPrototype_Level5_TogglePeg },
            { { 5, "TogglePeg3" }, ActorPrototype_Level5_TogglePeg },
            { { 5, "TogglePeg4" }, ActorPrototype_Level5_TogglePeg },
            { { 5, "TownGuard1" }, ActorPrototype_Level5_TownGuard1 },
            { { 5, "TownGuard2" }, ActorPrototype_Level5_TownGuard2 },

            // Level 6
            { { 6, "BossStager" }, ActorPrototype_Level6_BossStager },
            { { 6, "CrumblingPeg" }, ActorPrototype_Level6_CrumblingPeg },
            { { 6, "Elevator" }, ActorPrototype_BaseElevator },
            { { 6, "OneWayTriggerElevator" }, ActorPrototype_BaseElevator },
            { { 6, "OneWayStartElevator" }, ActorPrototype_BaseElevator },
            { { 6, "TriggerElevator" }, ActorPrototype_BaseElevator },
            { { 6, "GooVent" }, ActorPrototype_Level6_GooVent },
            { { 6, "GroundBlower" }, ActorPrototype_Level6_GroundBlower },
            { { 6, "Rat" }, ActorPrototype_Level6_Rat },
            { { 6, "Seagull" }, ActorPrototype_Level6_Seagull },
            { { 6, "StartSteppingStone" }, ActorPrototype_Level6_SteppingGround },
            { { 6, "TownGuard1" }, ActorPrototype_Level6_TownGuard1 },
            { { 6, "TownGuard2" }, ActorPrototype_Level6_TownGuard2 },
            { { 6, "Wolvington" }, ActorPrototype_Level6_Wolvington },

            // Level 7
            { { 7, "BearSailor" }, ActorPrototype_Level7_BearSailor },
            { { 7, "CrumblingPeg" }, ActorPrototype_Level7_CrumblingPeg },
            { { 7, "Elevator" }, ActorPrototype_BaseElevator },
            { { 7, "PathElevator" }, ActorPrototype_Level7_PathElevator },
            { { 7, "RedTailPirate" }, ActorPrototype_Level7_RedTailPirate },
            { { 7, "Seagull" }, ActorPrototype_Level7_Seagull },
            { { 7, "SpringBoard" }, ActorPrototype_Level7_SpringBoard },
            { { 7, "StartSteppingStone" }, ActorPrototype_Level7_SteppingGround },
            { { 7, "SteppingStone" }, ActorPrototype_Level7_SteppingGround },
            { { 7, "HermitCrab" }, ActorPrototype_Level7_HermitCrab },
            { { 7, "CrabNest" }, ActorPrototype_Level7_CrabNest },

            // Level 8
            { { 8, "BearSailor" }, ActorPrototype_Level8_BearSailor },
            { { 8, "BossStager" }, ActorPrototype_Level8_BossStager },
            { { 8, "Elevator" }, ActorPrototype_BaseElevator },
            { { 8, "OneWayTriggerElevator" }, ActorPrototype_BaseElevator },
            { { 8, "PunkRat" }, ActorPrototype_Level8_PunkRat },
            { { 8, "RedTailPirate" }, ActorPrototype_Level8_RedTailPirate },
            { { 8, "Seagull" }, ActorPrototype_Level8_Seagull },
            { { 8, "StartSteppingStone" }, ActorPrototype_Level8_SteppingGround },
            { { 8, "TogglePeg" }, ActorPrototype_Level8_TogglePeg },
            { { 8, "TogglePeg2" }, ActorPrototype_Level8_TogglePeg },
            { { 8, "TogglePeg3" }, ActorPrototype_Level8_TogglePeg },
            { { 8, "TogglePeg4" }, ActorPrototype_Level8_TogglePeg },

            // Level 9
            { { 9, "Elevator" }, ActorPrototype_BaseElevator },
            { { 9, "OneWayTriggerElevator" }, ActorPrototype_BaseElevator },
            { { 9, "OneWayStartElevator" }, ActorPrototype_BaseElevator },
            { { 9, "PathElevator" }, ActorPrototype_BasePathElevator },
            { { 9, "Seagull" }, ActorPrototype_Level9_Seagull },
            { { 9, "SpringBoard" }, ActorPrototype_Level9_SpringBoard },
            { { 9, "TProjectile" }, ActorPrototype_Level9_PuffDartSpawner },
            { { 9, "SawBlade" }, ActorPrototype_Level9_SawBlade },
            { { 9, "SawBlade2" }, ActorPrototype_Level9_SawBlade },
            { { 9, "SawBlade3" }, ActorPrototype_Level9_SawBlade },
            { { 9, "SawBlade4" }, ActorPrototype_Level9_SawBlade },
            { { 9, "SkullCannon" }, ActorPrototype_Level9_SkullCannon },
            { { 9, "CrazyHook" }, ActorPrototype_Level9_CrazyHook },
        };

        auto key = std::make_pair(levelNumber, logic);
        auto findIt = s_LevelLogicToActorProtoMap.find(key);
        if (findIt == s_LevelLogicToActorProtoMap.end())
        {
            return ActorPrototype_None;
        }

        return findIt->second;
    }

    Point GetClawSpawnLocation(int checkpointNumber, int levelNumber)
    {
        switch (levelNumber)
        {
            case 1:
                if (checkpointNumber == 0) { return Point(689, 4723); }
                else if (checkpointNumber == 1) { return Point(6873, 4970); }
                else if (checkpointNumber == 2) { return Point(14227, 2281); }
                break;
            case 2:
                if (checkpointNumber == 0) { return Point(420, 4789); }
                else if (checkpointNumber == 1) { return Point(8235, 4848); }
                else if (checkpointNumber == 2) { return Point(15657, 4650); }
                break;
            case 3:
                if (checkpointNumber == 0) { return Point(4600, 6270); }
                else if (checkpointNumber == 1) { return Point(15358, 6868); }
                else if (checkpointNumber == 2) { return Point(21712, 3464); }
                break;
            case 4:
                if (checkpointNumber == 0) { return Point(1439, 4760); }
                else if (checkpointNumber == 1) { return Point(9300, 4248); }
                else if (checkpointNumber == 2) { return Point(7081, 5254); }
                break;
            case 5:
                if (checkpointNumber == 0) { return Point(1895, 2775); }
                else if (checkpointNumber == 1) { return Point(14022, 3915); }
                else if (checkpointNumber == 2) { return Point(23430, 5513); }
                break;
            case 6:
                if (checkpointNumber == 0) { return Point(425, 4749); }
                else if (checkpointNumber == 1) { return Point(9165, 4710); }
                else if (checkpointNumber == 2) { return Point(19831, 5999); }
                break;
            case 7:
                if (checkpointNumber == 0) { return Point(1665, 7200); }
                else if (checkpointNumber == 1) { return Point(9360, 7370); }
                else if (checkpointNumber == 2) { return Point(25210, 7180); }
                break;
            case 8:
                if (checkpointNumber == 0) { return Point(1046, 5520); }
                else if (checkpointNumber == 1) { return Point(12121, 4995);  }
                else if (checkpointNumber == 2) { return Point(22557, 4996); }
                break;
            case 9:
                if (checkpointNumber == 0) { return Point(1594, 7750); }
                else if (checkpointNumber == 1) { return Point(8721, 7759); }
                else if (checkpointNumber == 2) { return Point(14289, 7756); }
                break;
            case 10:
                if (checkpointNumber == 0) { return Point(740, 7744); }
                else if (checkpointNumber == 1) { return Point(6939, 9159); }
                else if (checkpointNumber == 2) { return Point(22679, 8067); }
                break;
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
        return Point();
    }

    shared_ptr<LevelData> GetDebugLoadLevelData()
    {
        int debugLevelNumber = g_pApp->GetDebugOptions()->skipMenuToLevel;

        shared_ptr<LevelData> pLevelData(new LevelData(debugLevelNumber, false, 0));
        return pLevelData;
    }

    StrongActorPtr CreateSpecialDeathEffect(const Point& location, int levelNumber)
    {
        // Any special effect linked to the death and level
        switch (levelNumber)
        {
            // Liquid
            case 2:
            {
                Point splashPosition(
                    location.x,
                    location.y - 42);
                return ActorTemplates::CreateSingleAnimation(splashPosition, AnimationType_TarSplash);
            }
            case 4:
            {
                Point splashPosition(
                    location.x,
                    location.y - 30);
                return ActorTemplates::CreateSingleAnimation(splashPosition, AnimationType_TarSplash);
            }
            case 6:
            {
                Point splashPosition(
                    location.x,
                    location.y - 8);
                return ActorTemplates::CreateSingleAnimation(splashPosition, AnimationType_TarSplash);
            }
            case 7:
            {
                Point splashPosition(
                    location.x,
                    location.y - 20);
                return ActorTemplates::CreateSingleAnimation(splashPosition, AnimationType_TarSplash);
            }
            case 8:
            case 11:
            case 12:
            case 13:
            case 14:
            assert(false && "Unsupported at the moment");
            break;

            default: return nullptr;
        }

        return nullptr;
    }

    bool TryGetTopLadderInfo(int levelNumber, int tileId, Point& fixtureOffset)
    {
        static std::map<std::pair<int, int>, Point> s_EndLadderInfoMap =
        {
            // Level 1
            { { 1, 310 }, Point(0, 0) },

            // Level 2
            { { 2, 16 }, Point(0, 50) },

            // Level 3
            { { 3, 667 }, Point(0, 0) },
            { { 3, 668 }, Point(0, 5) },

            // Level 4
            { { 4, 181 }, Point(0, 7) },

            // Level 5
            { { 5, 215 }, Point(0, -14) },
            { { 5, 221 }, Point(0, -14) },
            { { 5, 516 }, Point(0, 0) },

            // Level 6
            { { 6, 16 }, Point(0, 0) },
            { { 6, 139 }, Point(0, 0) },

            // Level 7
            { { 7, 110 }, Point(0, 24) },
            { { 7, 113 }, Point(0, 8) },
            { { 7, 119 }, Point(0, 8) },
            { { 7, 394 }, Point(0, 8) },

            // Level 8
            { { 8, 62 }, Point(0, 0) },
            { { 8, 534 }, Point(0, -30) },
            { { 8, 535 }, Point(0, 0) },
            { { 8, 536 }, Point(0, 44) },
            { { 8, 540 }, Point(0, 20) },
            { { 8, 543 }, Point(0, 0) },
            { { 8, 548 }, Point(0, 39) },
            { { 8, 583 }, Point(0, 0) },
        };

        auto key = std::make_pair(levelNumber, tileId);
        auto findIt = s_EndLadderInfoMap.find(key);
        if (findIt == s_EndLadderInfoMap.end())
        {
            return false;
        }

        fixtureOffset = findIt->second;
        return true;
    }
}
