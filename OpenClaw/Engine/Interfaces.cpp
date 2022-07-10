#include "Interfaces.h"
#include "Logger/Logger.h"
#include "Util/StringUtil.h"

//=============================================================================
// ActorPrototype
//=============================================================================

std::string EnumToString_ActorPrototype(ActorPrototype actorProto)
{
    static std::map<ActorPrototype, std::string> actorPrototypeEnumToStringMap =
    {
        // Actor enemies
        { ActorPrototype_Null, "ActorPrototype_Null" },

        { ActorPrototype_BaseEnemy, "ActorPrototype_BaseEnemy" },

        { ActorPrototype_Level1_Soldier, "ActorPrototype_Level1_Soldier" },
        { ActorPrototype_Level1_Officer, "ActorPrototype_Level1_Officer" },
        { ActorPrototype_Level1_Rat, "ActorPrototype_Level1_Rat" },
        { ActorPrototype_Level2_Soldier, "ActorPrototype_Level2_Soldier" },
        { ActorPrototype_Level2_Officer, "ActorPrototype_Level2_Officer" },
        { ActorPrototype_Level2_PunkRat, "ActorPrototype_Level2_PunkRat" },

        { ActorPrototype_Level3_Rat, "ActorPrototype_Level3_Rat" },
        { ActorPrototype_Level3_CutThroat, "ActorPrototype_Level3_CutThroat" },
        { ActorPrototype_Level3_RobberThief, "ActorPrototype_Level3_RobberThief" },

        { ActorPrototype_Level4_Rat, "ActorPrototype_Level4_Rat" },
        { ActorPrototype_Level4_CutThroat, "ActorPrototype_Level4_CutThroat" },
        { ActorPrototype_Level4_RobberThief, "ActorPrototype_Level4_RobberThief" },

        { ActorPrototype_Level5_TownGuard1, "ActorPrototype_Level5_TownGuard1" },
        { ActorPrototype_Level5_TownGuard2, "ActorPrototype_Level5_TownGuard2" },
        { ActorPrototype_Level5_Seagull, "ActorPrototype_Level5_Seagull" },

        { ActorPrototype_Level6_TownGuard1, "ActorPrototype_Level6_TownGuard1" },
        { ActorPrototype_Level6_TownGuard2, "ActorPrototype_Level6_TownGuard2" },
        { ActorPrototype_Level6_Rat, "ActorPrototype_Level6_Rat" },
        { ActorPrototype_Level6_Seagull, "ActorPrototype_Level6_Seagull" },

        // Elevators
        { ActorPrototype_BaseElevator, "ActorPrototype_BaseElevator" },

        { ActorPrototype_BasePathElevator, "ActorPrototype_BasePathElevator" },

        // Toggle pegs
        { ActorPrototype_BaseTogglePeg, "ActorPrototype_BaseTogglePeg" },

        { ActorPrototype_Level1_TogglePeg, "ActorPrototype_Level1_TogglePeg" },
        { ActorPrototype_Level2_TogglePeg, "ActorPrototype_Level2_TogglePeg" },

        // Crumbling pegs
        { ActorPrototype_BaseCrumblingPeg, "ActorPrototype_BaseCrumblingPeg" },

        { ActorPrototype_Level1_CrumblingPeg, "ActorPrototype_Level1_CrumblingPeg" },
        { ActorPrototype_Level3_CrumblingPeg, "ActorPrototype_Level3_CrumblingPeg" },
        { ActorPrototype_Level4_CrumblingPeg, "ActorPrototype_Level4_CrumblingPeg" },
        { ActorPrototype_Level5_CrumblingPeg, "ActorPrototype_Level5_CrumblingPeg" },
        { ActorPrototype_Level6_CrumblingPeg, "ActorPrototype_Level6_CrumblingPeg" },

        // Projectile spawners
        { ActorPrototype_Level2_TowerCannonLeft, "ActorPrototype_Level2_TowerCannonLeft" },
        { ActorPrototype_Level2_TowerCannonRight, "ActorPrototype_Level2_TowerCannonRight" },

        // Projectiles
        { ActorPrototype_Level1_RatBomb, "ActorPrototype_Level1_RatBomb" },

        { ActorPrototype_Level2_CannonBall, "ActorPrototype_Level2_CannonBall" },
        { ActorPrototype_Level2_RatProjectile, "ActorPrototype_Level2_RatProjectile" },

        { ActorPrototype_Level3_RatBomb, "ActorPrototype_Level3_RatBomb" },
        { ActorPrototype_Level3_ArrowProjectile, "ActorPrototype_Level3_ArrowProjectile" },
        { ActorPrototype_Level3_KnifeProjectile, "ActorPrototype_Level3_KnifeProjectile" },

        { ActorPrototype_Level4_RatBomb, "ActorPrototype_Level4_RatBomb" },
        { ActorPrototype_Level4_ArrowProjectile, "ActorPrototype_Level4_ArrowProjectile" },
        { ActorPrototype_Level4_KnifeProjectile, "ActorPrototype_Level4_KnifeProjectile" },

        { ActorPrototype_Level6_WolvingtonMagic, "ActorPrototype_Level6_WolvingtonMagic" },

        // Boss Stagers
        { ActorPrototype_Level2_BossStager, "ActorPrototype_Level2_BossStager" },
        { ActorPrototype_Level4_BossStager, "ActorPrototype_Level4_BossStager" },
        { ActorPrototype_Level6_BossStager, "ActorPrototype_Level6_BossStager" },

        // Bosses
        { ActorPrototype_Level2_LaRaux, "ActorPrototype_Level2_LaRaux" },
        { ActorPrototype_Level4_Katherine, "ActorPrototype_Level4_Katherine" },
        { ActorPrototype_Level6_Wolvington, "ActorPrototype_Level6_Wolvington" },

        // Stepping stones
        { ActorPrototype_Level4_SteppingGround, "ActorPrototype_Level4_SteppingGround" },
        { ActorPrototype_Level6_SteppingGround, "ActorPrototype_Level6_SteppingGround" },

        // Spring board
        { ActorPrototype_Level4_SpringBoard, "ActorPrototype_Level4_SpringBoard" },
        { ActorPrototype_Level6_GroundBlower, "ActorPrototype_Level6_GroundBlower" },

        // Loot containers
        { ActorPrototype_BaseLootContainer, "ActorPrototype_BaseLootContainer" },
        { ActorPrototype_Level5_LootStatue, "ActorPrototype_Level5_LootStatue" },

        { ActorPrototype_Level6_GooVent, "ActorPrototype_Level6_GooVent" },

        //=======================================
        // General
        //=======================================

        { ActorPrototype_BaseProjectile, "ActorPrototype_BaseProjectile" },
        { ActorPrototype_FireSwordProjectile, "ActorPrototype_FireSwordProjectile" },
        { ActorPrototype_FrostSwordProjectile, "ActorPrototype_FrostSwordProjectile" },
        { ActorPrototype_LightningSwordProjectile, "ActorPrototype_LightningSwordProjectile" },

        { ActorPrototype_BaseProjectileSpawner, "ActorPrototype_BaseProjectileSpawner" },

        // Sound
        { ActorPrototype_LocalAmbientSound, "ActorPrototype_LocalAmbientSound" },

        // Stepping stones
        { ActorPrototype_BaseSteppingGround, "ActorPrototype_BaseSteppingGround" },

        // Spring board
        { ActorPrototype_BaseSpringBoard, "ActorPrototype_BaseSpringBoard" },

        { ActorPrototype_BaseBossStager, "ActorPrototype_BaseBossStager" },

        // Floor Spike
        { ActorPrototype_BaseFloorSpike, "ActorPrototype_BaseFloorSpike" },
        { ActorPrototype_Level3_FloorSpike, "ActorPrototype_Level3_FloorSpike" },
        { ActorPrototype_Level4_FloorSpike, "ActorPrototype_Level4_FloorSpike" },
        { ActorPrototype_Level12_FloorSpike, "ActorPrototype_Level12_FloorSpike" },
        { ActorPrototype_Level13_FloorSpike, "ActorPrototype_Level13_FloorSpike" },

        // Rope
        { ActorPrototype_BaseRope, "ActorPrototype_BaseRope" },
        { ActorPrototype_BaseActorSpawner, "ActorPrototype_BaseActorSpawner" },

        // Level 2
        { ActorPrototype_Level2_PunkRatCannon, "ActorPrototype_Level2_PunkRatCannon" },
        
        // Level 7
        { ActorPrototype_Level7_CrumblingPeg, "ActorPrototype_Level7_CrumblingPeg" },
        { ActorPrototype_Level7_Seagull, "ActorPrototype_Level7_Seagull" },
        { ActorPrototype_Level7_SpringBoard, "ActorPrototype_Level7_SpringBoard" },
        { ActorPrototype_Level7_SteppingGround, "ActorPrototype_Level7_SteppingGround" },
        { ActorPrototype_Level7_PathElevator, "ActorPrototype_Level7_PathElevator" },
        { ActorPrototype_Level7_BearSailor, "ActorPrototype_Level7_BearSailor" },
        { ActorPrototype_Level7_RedTailPirate, "ActorPrototype_Level7_RedTailPirate" },
        { ActorPrototype_Level7_HermitCrab, "ActorPrototype_Level7_HermitCrab" },
        { ActorPrototype_Level7_CrabNest, "ActorPrototype_Level7_CrabNest" },

        // Level 8
        { ActorPrototype_Level8_Seagull, "ActorPrototype_Level8_Seagull" },
        { ActorPrototype_Level8_BossStager, "ActorPrototype_Level8_BossStager" },
        { ActorPrototype_Level8_SteppingGround, "ActorPrototype_Level8_SteppingGround" },
        { ActorPrototype_Level8_BearSailor, "ActorPrototype_Level8_BearSailor" },
        { ActorPrototype_Level8_RedTailPirate, "ActorPrototype_Level8_RedTailPirate" },
        { ActorPrototype_Level8_PunkRat, "ActorPrototype_Level8_PunkRat" },
        { ActorPrototype_Level8_TogglePeg, "ActorPrototype_Level8_TogglePeg" },
        { ActorPrototype_Level8_TogglePeg_2, "ActorPrototype_Level8_TogglePeg_2" },
        { ActorPrototype_Level8_CannonBall, "ActorPrototype_Level8_CannonBall" },
        { ActorPrototype_Level8_PunkRatCannon, "ActorPrototype_Level8_PunkRatCannon" },
        { ActorPrototype_Level8_GabrielButton, "ActorPrototype_Level8_GabrielButton" },
        { ActorPrototype_Level8_GabrielCannon, "ActorPrototype_Level8_GabrielCannon" },
        { ActorPrototype_Level8_Gabriel, "ActorPrototype_Level8_Gabriel" },
        { ActorPrototype_Level8_GabrielPirate, "ActorPrototype_Level8_GabrielPirate" },
        { ActorPrototype_Level8_GabrielBomb, "ActorPrototype_Level8_GabrielBomb" },
        { ActorPrototype_Level8_BossGem, "ActorPrototype_Level8_BossGem" },

        // Level 9
        { ActorPrototype_Level9_Seagull, "ActorPrototype_Level9_Seagull" },
        { ActorPrototype_Level9_SpringBoard, "ActorPrototype_Level9_SpringBoard" },
        { ActorPrototype_Level9_PuffDartSpawner, "ActorPrototype_Level9_PuffDartSpawner" },
        { ActorPrototype_Level9_DartProjectile_Up, "ActorPrototype_Level9_DartProjectile_Up" },
        { ActorPrototype_Level9_DartProjectile_Down, "ActorPrototype_Level9_DartProjectile_Down" },
        { ActorPrototype_Level9_DartProjectile_Right, "ActorPrototype_Level9_DartProjectile_Right" },
        { ActorPrototype_Level9_DartProjectile_Left, "ActorPrototype_Level9_DartProjectile_Left" },
        { ActorPrototype_Level9_SawBlade, "ActorPrototype_Level9_SawBlade" },
        { ActorPrototype_Level9_SkullCannon, "ActorPrototype_Level9_SkullCannon" },
        { ActorPrototype_Level9_CannonBall, "ActorPrototype_Level9_CannonBall" },
        { ActorPrototype_Level9_CrazyHook, "ActorPrototype_Level9_CrazyHook" },
        { ActorPrototype_Level9_PegLeg, "ActorPrototype_Level9_PegLeg" },
        { ActorPrototype_Level9_BulletProjectile, "ActorPrototype_Level9_BulletProjectile" },
        { ActorPrototype_Level9_ConveyorBelt, "ActorPrototype_Level9_ConveyorBelt" },

        // Level 10
        { ActorPrototype_Level10_Seagull, "ActorPrototype_Level10_Seagull" },
        { ActorPrototype_Level10_PuffDartSpawner, "ActorPrototype_Level10_PuffDartSpawner" },
        { ActorPrototype_Level10_DartProjectile_Up, "ActorPrototype_Level10_DartProjectile_Up" },
        { ActorPrototype_Level10_DartProjectile_Down, "ActorPrototype_Level10_DartProjectile_Down" },
        { ActorPrototype_Level10_DartProjectile_Right, "ActorPrototype_Level10_DartProjectile_Right" },
        { ActorPrototype_Level10_DartProjectile_Left, "ActorPrototype_Level10_DartProjectile_Left" },
        { ActorPrototype_Level10_CrazyHook, "ActorPrototype_Level10_CrazyHook" },
        { ActorPrototype_Level10_PegLeg, "ActorPrototype_Level10_PegLeg" },
        { ActorPrototype_Level10_CrumblingPeg, "ActorPrototype_Level10_CrumblingPeg" },
        { ActorPrototype_Level10_TogglePeg, "ActorPrototype_Level10_TogglePeg" },
        { ActorPrototype_Level10_BossStager, "ActorPrototype_Level10_BossStager" },
        { ActorPrototype_Level10_Marrow, "ActorPrototype_Level10_Marrow" },
        { ActorPrototype_Level10_MarrowFloor, "ActorPrototype_Level10_MarrowFloor" },
        { ActorPrototype_Level10_MarrowParrot, "ActorPrototype_Level10_MarrowParrot" },
        { ActorPrototype_Level10_BossGem, "ActorPrototype_Level10_BossGem" },

        // Level 11
        { ActorPrototype_Level11_Mercat, "ActorPrototype_Level11_Mercat" },
        { ActorPrototype_Level11_Siren, "ActorPrototype_Level11_Siren" },
        { ActorPrototype_Level11_Fish, "ActorPrototype_Level11_Fish" },
        { ActorPrototype_Level11_TogglePeg, "ActorPrototype_Level11_TogglePeg" },
        { ActorPrototype_Level11_BreakPlank, "ActorPrototype_Level11_BreakPlank" },
        { ActorPrototype_Level11_ConveyorBelt, "ActorPrototype_Level11_ConveyorBelt" },
        { ActorPrototype_Level11_Laser, "ActorPrototype_Level11_Laser" },
        { ActorPrototype_Level11_SirenProjectile, "ActorPrototype_Level11_SirenProjectile" },
        { ActorPrototype_Level11_TridentProjectile, "ActorPrototype_Level11_TridentProjectile" },

        // Level 12
        { ActorPrototype_Level12_Mercat, "ActorPrototype_Level12_Mercat" },
        { ActorPrototype_Level12_CrumblingPeg, "ActorPrototype_Level12_CrumblingPeg" },
        { ActorPrototype_Level12_Siren, "ActorPrototype_Level12_Siren" },
        { ActorPrototype_Level12_Fish, "ActorPrototype_Level12_Fish" },
        { ActorPrototype_Level12_TogglePeg, "ActorPrototype_Level12_TogglePeg" },
        { ActorPrototype_Level12_Aquatis, "ActorPrototype_Level12_Aquatis" },
        { ActorPrototype_Level12_RockSpring, "ActorPrototype_Level12_RockSpring" },
        { ActorPrototype_Level12_PathElevator, "ActorPrototype_Level12_PathElevator" },
        { ActorPrototype_Level12_BossStager, "ActorPrototype_Level12_BossStager" },
        { ActorPrototype_Level12_BossGem, "ActorPrototype_Level12_BossGem" },

        // Level 13
        { ActorPrototype_Level13_BearSailor, "ActorPrototype_Level13_BearSailor" },
        { ActorPrototype_Level13_RedTailPirate, "ActorPrototype_Level13_RedTailPirate" },
        { ActorPrototype_Level13_WaterRock, "ActorPrototype_Level13_WaterRock" },
        { ActorPrototype_Level13_SpringBoard, "ActorPrototype_Level13_SpringBoard" },
        { ActorPrototype_Level13_PathElevator, "ActorPrototype_Level13_PathElevator" },
        { ActorPrototype_Level13_TogglePeg, "ActorPrototype_Level13_TogglePeg" },
        { ActorPrototype_Level13_CrumblingPeg, "ActorPrototype_Level13_CrumblingPeg" },
        { ActorPrototype_Level13_SteppingGround, "ActorPrototype_Level13_SteppingGround" },
        { ActorPrototype_Level13_PuffDartSpawner, "ActorPrototype_Level13_PuffDartSpawner" },
        { ActorPrototype_Level13_RedTail, "ActorPrototype_Level13_RedTail" },
        { ActorPrototype_Level13_BossGem, "ActorPrototype_Level13_BossGem" },
        { ActorPrototype_Level13_BossStager, "ActorPrototype_Level13_BossStager" },

        //=======================================
        // Menu
        //=======================================

        { ActorPrototype_StaticImage, "ActorPrototype_StaticImage" },
        { ActorPrototype_StaticAnimatedImage, "ActorPrototype_StaticAnimatedImage" },
    };

    auto findIt = actorPrototypeEnumToStringMap.find(actorProto);
    if (findIt == actorPrototypeEnumToStringMap.end())
    {
        LOG_ERROR("Could not find actor enum: " + ToStr((int)actorProto));
        assert(false && "Could not convert ActorPrototype enum to string");
    }

    return findIt->second;
}

ActorPrototype StringToEnum_ActorPrototype(const std::string& actorProtoStr)
{
    static std::map<std::string, ActorPrototype> actorPrototypeStringToEnumMap =
    {
        // Actor enemies
        { "ActorPrototype_Null", ActorPrototype_Null },

        { "ActorPrototype_BaseEnemy", ActorPrototype_BaseEnemy },

        { "ActorPrototype_Level1_Soldier", ActorPrototype_Level1_Soldier },
        { "ActorPrototype_Level1_Officer", ActorPrototype_Level1_Officer },
        { "ActorPrototype_Level1_Rat", ActorPrototype_Level1_Rat },
        { "ActorPrototype_Level2_Soldier", ActorPrototype_Level2_Soldier },
        { "ActorPrototype_Level2_Officer", ActorPrototype_Level2_Officer },
        { "ActorPrototype_Level2_PunkRat", ActorPrototype_Level2_PunkRat },

        { "ActorPrototype_Level3_Rat", ActorPrototype_Level3_Rat },
        { "ActorPrototype_Level3_CutThroat", ActorPrototype_Level3_CutThroat },
        { "ActorPrototype_Level3_RobberThief", ActorPrototype_Level3_RobberThief },

        { "ActorPrototype_Level4_Rat", ActorPrototype_Level4_Rat },
        { "ActorPrototype_Level4_CutThroat", ActorPrototype_Level4_CutThroat },
        { "ActorPrototype_Level4_RobberThief", ActorPrototype_Level4_RobberThief },
        
        { "ActorPrototype_Level5_TownGuard1", ActorPrototype_Level5_TownGuard1 },
        { "ActorPrototype_Level5_TownGuard2", ActorPrototype_Level5_TownGuard2 },
        { "ActorPrototype_Level5_Seagull", ActorPrototype_Level5_Seagull },

        { "ActorPrototype_Level6_TownGuard1", ActorPrototype_Level6_TownGuard1 },
        { "ActorPrototype_Level6_TownGuard2", ActorPrototype_Level6_TownGuard2 },
        { "ActorPrototype_Level6_Rat", ActorPrototype_Level6_Rat },
        { "ActorPrototype_Level6_Seagull", ActorPrototype_Level6_Seagull },

        // Elevators
        { "ActorPrototype_BaseElevator", ActorPrototype_BaseElevator },

        { "ActorPrototype_BasePathElevator", ActorPrototype_BasePathElevator },

        // Toggle pegs
        { "ActorPrototype_BaseTogglePeg", ActorPrototype_BaseTogglePeg },

        { "ActorPrototype_Level1_CrumblingPeg", ActorPrototype_Level1_CrumblingPeg },
        { "ActorPrototype_Level3_CrumblingPeg", ActorPrototype_Level3_CrumblingPeg },
        { "ActorPrototype_Level4_CrumblingPeg", ActorPrototype_Level4_CrumblingPeg },
        { "ActorPrototype_Level5_CrumblingPeg", ActorPrototype_Level5_CrumblingPeg },
        { "ActorPrototype_Level6_CrumblingPeg", ActorPrototype_Level6_CrumblingPeg },

        { "ActorPrototype_Level1_TogglePeg", ActorPrototype_Level1_TogglePeg },
        { "ActorPrototype_Level2_TogglePeg", ActorPrototype_Level2_TogglePeg },
        { "ActorPrototype_Level5_TogglePeg", ActorPrototype_Level5_TogglePeg },

        // Crumbling pegs
        { "ActorPrototype_BaseCrumblingPeg", ActorPrototype_BaseCrumblingPeg },

        // Projectile spawners
        { "ActorPrototype_Level2_TowerCannonLeft", ActorPrototype_Level2_TowerCannonLeft },
        { "ActorPrototype_Level2_TowerCannonRight", ActorPrototype_Level2_TowerCannonRight },

        // Projectiles
        { "ActorPrototype_Level1_RatBomb", ActorPrototype_Level1_RatBomb },

        { "ActorPrototype_Level2_CannonBall", ActorPrototype_Level2_CannonBall },
        { "ActorPrototype_Level2_RatProjectile", ActorPrototype_Level2_RatProjectile },

        { "ActorPrototype_Level3_RatBomb", ActorPrototype_Level3_RatBomb },
        { "ActorPrototype_Level3_ArrowProjectile", ActorPrototype_Level3_ArrowProjectile },
        { "ActorPrototype_Level3_KnifeProjectile", ActorPrototype_Level3_KnifeProjectile },

        { "ActorPrototype_Level4_RatBomb", ActorPrototype_Level4_RatBomb },
        { "ActorPrototype_Level4_ArrowProjectile", ActorPrototype_Level4_ArrowProjectile },
        { "ActorPrototype_Level4_KnifeProjectile", ActorPrototype_Level4_KnifeProjectile },

        { "ActorPrototype_Level6_WolvingtonMagic", ActorPrototype_Level6_WolvingtonMagic },

        // Boss Stagers
        { "ActorPrototype_Level2_BossStager", ActorPrototype_Level2_BossStager },
        { "ActorPrototype_Level4_BossStager", ActorPrototype_Level4_BossStager },
        { "ActorPrototype_Level6_BossStager", ActorPrototype_Level6_BossStager },

        // Bosses
        { "ActorPrototype_Level2_LaRaux", ActorPrototype_Level2_LaRaux },
        { "ActorPrototype_Level4_Katherine", ActorPrototype_Level4_Katherine },
        { "ActorPrototype_Level6_Wolvington", ActorPrototype_Level6_Wolvington },

        // Stepping stones
        { "ActorPrototype_Level4_SteppingGround", ActorPrototype_Level4_SteppingGround },
        { "ActorPrototype_Level6_SteppingGround", ActorPrototype_Level6_SteppingGround },

        // Spring board
        { "ActorPrototype_Level4_SpringBoard", ActorPrototype_Level4_SpringBoard },
        { "ActorPrototype_Level6_GroundBlower", ActorPrototype_Level6_GroundBlower },

        // Loot containers
        { "ActorPrototype_BaseLootContainer", ActorPrototype_BaseLootContainer },
        { "ActorPrototype_Level5_LootStatue", ActorPrototype_Level5_LootStatue },

        { "ActorPrototype_Level6_GooVent", ActorPrototype_Level6_GooVent },

        //=======================================
        // General
        //=======================================

        { "ActorPrototype_BaseProjectile", ActorPrototype_BaseProjectile },
        { "ActorPrototype_FireSwordProjectile", ActorPrototype_FireSwordProjectile },
        { "ActorPrototype_FrostSwordProjectile", ActorPrototype_FrostSwordProjectile },
        { "ActorPrototype_LightningSwordProjectile", ActorPrototype_LightningSwordProjectile },

        { "ActorPrototype_BaseProjectileSpawner", ActorPrototype_BaseProjectileSpawner },

        // Sound
        { "ActorPrototype_LocalAmbientSound", ActorPrototype_LocalAmbientSound },

        // Stepping Stone
        { "ActorPrototype_BaseSteppingGround", ActorPrototype_BaseSteppingGround },

        // Spring board
        { "ActorPrototype_BaseSpringBoard", ActorPrototype_BaseSpringBoard },

        { "ActorPrototype_BaseBossStager", ActorPrototype_BaseBossStager },

        // Floor Spike
        { "ActorPrototype_BaseFloorSpike", ActorPrototype_BaseFloorSpike },
        { "ActorPrototype_Level3_FloorSpike", ActorPrototype_Level3_FloorSpike },
        { "ActorPrototype_Level4_FloorSpike", ActorPrototype_Level4_FloorSpike },
        { "ActorPrototype_Level12_FloorSpike", ActorPrototype_Level12_FloorSpike },
        { "ActorPrototype_Level13_FloorSpike", ActorPrototype_Level13_FloorSpike },

        // Rope
        { "ActorPrototype_BaseRope", ActorPrototype_BaseRope },
        { "ActorPrototype_BaseActorSpawner", ActorPrototype_BaseActorSpawner },

        // Level 2
        { "ActorPrototype_Level2_PunkRatCannon", ActorPrototype_Level2_PunkRatCannon },

        // Level 7
        { "ActorPrototype_Level7_CrumblingPeg", ActorPrototype_Level7_CrumblingPeg },
        { "ActorPrototype_Level7_PathElevator", ActorPrototype_Level7_PathElevator },
        { "ActorPrototype_Level7_Seagull", ActorPrototype_Level7_Seagull },
        { "ActorPrototype_Level7_SpringBoard", ActorPrototype_Level7_SpringBoard },
        { "ActorPrototype_Level7_SteppingGround", ActorPrototype_Level7_SteppingGround },
        { "ActorPrototype_Level7_BearSailor", ActorPrototype_Level7_BearSailor },
        { "ActorPrototype_Level7_RedTailPirate", ActorPrototype_Level7_RedTailPirate },
        { "ActorPrototype_Level7_HermitCrab", ActorPrototype_Level7_HermitCrab },
        { "ActorPrototype_Level7_CrabNest", ActorPrototype_Level7_CrabNest },

        // Level 8
        { "ActorPrototype_Level8_Seagull", ActorPrototype_Level8_Seagull },
        { "ActorPrototype_Level8_BossStager", ActorPrototype_Level8_BossStager },
        { "ActorPrototype_Level8_SteppingGround", ActorPrototype_Level8_SteppingGround },
        { "ActorPrototype_Level8_BearSailor", ActorPrototype_Level8_BearSailor },
        { "ActorPrototype_Level8_RedTailPirate", ActorPrototype_Level8_RedTailPirate },
        { "ActorPrototype_Level8_PunkRat", ActorPrototype_Level8_PunkRat },
        { "ActorPrototype_Level8_TogglePeg", ActorPrototype_Level8_TogglePeg },
        { "ActorPrototype_Level8_TogglePeg_2", ActorPrototype_Level8_TogglePeg_2 },
        { "ActorPrototype_Level8_CannonBall", ActorPrototype_Level8_CannonBall },
        { "ActorPrototype_Level8_PunkRatCannon", ActorPrototype_Level8_PunkRatCannon },
        { "ActorPrototype_Level8_GabrielButton", ActorPrototype_Level8_GabrielButton },
        { "ActorPrototype_Level8_GabrielCannon", ActorPrototype_Level8_GabrielCannon },
        { "ActorPrototype_Level8_Gabriel", ActorPrototype_Level8_Gabriel },
        { "ActorPrototype_Level8_GabrielPirate", ActorPrototype_Level8_GabrielPirate },
        { "ActorPrototype_Level8_GabrielBomb", ActorPrototype_Level8_GabrielBomb },
        { "ActorPrototype_Level8_BossGem", ActorPrototype_Level8_BossGem },

        // Level 9
        { "ActorPrototype_Level9_Seagull", ActorPrototype_Level9_Seagull },
        { "ActorPrototype_Level9_SpringBoard", ActorPrototype_Level9_SpringBoard },
        { "ActorPrototype_Level9_PuffDartSpawner", ActorPrototype_Level9_PuffDartSpawner },
        { "ActorPrototype_Level9_DartProjectile_Up", ActorPrototype_Level9_DartProjectile_Up },
        { "ActorPrototype_Level9_DartProjectile_Down", ActorPrototype_Level9_DartProjectile_Down },
        { "ActorPrototype_Level9_DartProjectile_Right", ActorPrototype_Level9_DartProjectile_Right },
        { "ActorPrototype_Level9_DartProjectile_Left", ActorPrototype_Level9_DartProjectile_Left },
        { "ActorPrototype_Level9_SawBlade", ActorPrototype_Level9_SawBlade },
        { "ActorPrototype_Level9_SkullCannon", ActorPrototype_Level9_SkullCannon },
        { "ActorPrototype_Level9_CannonBall", ActorPrototype_Level9_CannonBall },
        { "ActorPrototype_Level9_CrazyHook", ActorPrototype_Level9_CrazyHook },
        { "ActorPrototype_Level9_PegLeg", ActorPrototype_Level9_PegLeg },
        { "ActorPrototype_Level9_BulletProjectile", ActorPrototype_Level9_BulletProjectile },
        { "ActorPrototype_Level9_ConveyorBelt", ActorPrototype_Level9_ConveyorBelt },

        // Level 10
        { "ActorPrototype_Level10_Seagull", ActorPrototype_Level10_Seagull },
        { "ActorPrototype_Level10_PuffDartSpawner", ActorPrototype_Level10_PuffDartSpawner },
        { "ActorPrototype_Level10_DartProjectile_Up", ActorPrototype_Level10_DartProjectile_Up },
        { "ActorPrototype_Level10_DartProjectile_Down", ActorPrototype_Level10_DartProjectile_Down },
        { "ActorPrototype_Level10_DartProjectile_Right", ActorPrototype_Level10_DartProjectile_Right },
        { "ActorPrototype_Level10_DartProjectile_Left", ActorPrototype_Level10_DartProjectile_Left },
        { "ActorPrototype_Level10_CrazyHook", ActorPrototype_Level10_CrazyHook },
        { "ActorPrototype_Level10_PegLeg", ActorPrototype_Level10_PegLeg },
        { "ActorPrototype_Level10_CrumblingPeg", ActorPrototype_Level10_CrumblingPeg },
        { "ActorPrototype_Level10_TogglePeg", ActorPrototype_Level10_TogglePeg },
        { "ActorPrototype_Level10_BossStager", ActorPrototype_Level10_BossStager },
        { "ActorPrototype_Level10_Marrow", ActorPrototype_Level10_Marrow },
        { "ActorPrototype_Level10_MarrowFloor", ActorPrototype_Level10_MarrowFloor },
        { "ActorPrototype_Level10_MarrowParrot", ActorPrototype_Level10_MarrowParrot },
        { "ActorPrototype_Level10_BossGem", ActorPrototype_Level10_BossGem },
        
        // Level 11
        { "ActorPrototype_Level11_Mercat", ActorPrototype_Level11_Mercat },
        { "ActorPrototype_Level11_Siren", ActorPrototype_Level11_Siren },
        { "ActorPrototype_Level11_Fish", ActorPrototype_Level11_Fish },
        { "ActorPrototype_Level11_TogglePeg", ActorPrototype_Level11_TogglePeg },
        { "ActorPrototype_Level11_BreakPlank", ActorPrototype_Level11_BreakPlank },
        { "ActorPrototype_Level11_ConveyorBelt", ActorPrototype_Level11_ConveyorBelt },
        { "ActorPrototype_Level11_Laser", ActorPrototype_Level11_Laser },
        { "ActorPrototype_Level11_SirenProjectile", ActorPrototype_Level11_SirenProjectile },
        { "ActorPrototype_Level11_TridentProjectile", ActorPrototype_Level11_TridentProjectile },

        // Level 12
        { "ActorPrototype_Level12_Mercat", ActorPrototype_Level12_Mercat },
        { "ActorPrototype_Level12_CrumblingPeg", ActorPrototype_Level12_CrumblingPeg },
        { "ActorPrototype_Level12_Siren", ActorPrototype_Level12_Siren },
        { "ActorPrototype_Level12_Fish", ActorPrototype_Level12_Fish },
        { "ActorPrototype_Level12_TogglePeg", ActorPrototype_Level12_TogglePeg },
        { "ActorPrototype_Level12_Aquatis", ActorPrototype_Level12_Aquatis },
        { "ActorPrototype_Level12_RockSpring", ActorPrototype_Level12_RockSpring },
        { "ActorPrototype_Level12_PathElevator", ActorPrototype_Level12_PathElevator },
        { "ActorPrototype_Level12_BossStager", ActorPrototype_Level12_BossStager },
        { "ActorPrototype_Level12_BossGem", ActorPrototype_Level12_BossGem },

        // Level 13
        { "ActorPrototype_Level13_BearSailor", ActorPrototype_Level13_BearSailor },
        { "ActorPrototype_Level13_RedTailPirate", ActorPrototype_Level13_RedTailPirate },
        { "ActorPrototype_Level13_WaterRock", ActorPrototype_Level13_WaterRock },
        { "ActorPrototype_Level13_SpringBoard", ActorPrototype_Level13_SpringBoard },
        { "ActorPrototype_Level13_PathElevator", ActorPrototype_Level13_PathElevator },
        { "ActorPrototype_Level13_TogglePeg", ActorPrototype_Level13_TogglePeg },
        { "ActorPrototype_Level13_CrumblingPeg", ActorPrototype_Level13_CrumblingPeg },
        { "ActorPrototype_Level13_SteppingGround", ActorPrototype_Level13_SteppingGround },
        { "ActorPrototype_Level13_PuffDartSpawner", ActorPrototype_Level13_PuffDartSpawner },
        { "ActorPrototype_Level13_RedTail", ActorPrototype_Level13_RedTail },
        { "ActorPrototype_Level13_BossGem", ActorPrototype_Level13_BossGem },
        { "ActorPrototype_Level13_BossStager", ActorPrototype_Level13_BossStager },

        //=======================================
        // Menu
        //=======================================

        { "ActorPrototype_StaticImage", ActorPrototype_StaticImage },
        { "ActorPrototype_StaticAnimatedImage", ActorPrototype_StaticAnimatedImage },
    };

    auto findIt = actorPrototypeStringToEnumMap.find(actorProtoStr);
    if (findIt == actorPrototypeStringToEnumMap.end())
    {
        LOG_ERROR("Could not find actor enum: " + actorProtoStr);
        assert(false && "Could not convert ActorPrototype enum to string");
    }

    return findIt->second;
}

//=============================================================================
// FixtureType
//=============================================================================

FixtureType FixtureTypeStringToEnum(const std::string& fixtureTypeStr)
{
    FixtureType fixtureType = FixtureType_None;

    if (fixtureTypeStr == "Solid") { fixtureType = FixtureType_Solid; }
    else if (fixtureTypeStr == "Ground") { fixtureType = FixtureType_Ground; }
    else if (fixtureTypeStr == "Climb") { fixtureType = FixtureType_Climb; }
    else if (fixtureTypeStr == "Death") { fixtureType = FixtureType_Death; }
    else if (fixtureTypeStr == "Trigger") { fixtureType = FixtureType_Trigger; }
    else if (fixtureTypeStr == "Projectile") { fixtureType = FixtureType_Projectile; }
    else if (fixtureTypeStr == "Crate") { fixtureType = FixtureType_Crate; }
    else if (fixtureTypeStr == "Pickup") { fixtureType = FixtureType_Pickup; }
    else if (fixtureTypeStr == "Trigger") { fixtureType = FixtureType_Trigger; }
    else if (fixtureTypeStr == "Controller") { fixtureType = FixtureType_Controller; }
    else if (fixtureTypeStr == "PowderKeg") { fixtureType = FixtureType_PowderKeg; }
    else if (fixtureTypeStr == "Explosion") { fixtureType = FixtureType_Explosion; }
    else if (fixtureTypeStr == "EnemyAI") { fixtureType = FixtureType_EnemyAI; }
    else if (fixtureTypeStr == "EnemyAIMeleeSensor") { fixtureType = FixtureType_EnemyAIMeleeSensor; }
    else if (fixtureTypeStr == "EnemyAIDuckMeleeSensor") { fixtureType = FixtureType_EnemyAIDuckMeleeSensor; }
    else if (fixtureTypeStr == "EnemyAIRangedSensor") { fixtureType = FixtureType_EnemyAIRangedSensor; }
    else if (fixtureTypeStr == "EnemyAIDuckRangedSensor") { fixtureType = FixtureType_EnemyAIDuckRangedSensor; }
    else if (fixtureTypeStr == "EnemyAIDiveAreaSensor") { fixtureType = FixtureType_EnemyAIDiveAreaSensor; }
    else if (fixtureTypeStr == "DamageAura") { fixtureType = FixtureType_DamageAura; }
    else if (fixtureTypeStr == "Trigger_SpawnArea") { fixtureType = FixtureType_Trigger_SpawnArea; }
    else if (fixtureTypeStr == "Trigger_GabrielButton") { fixtureType = FixtureType_Trigger_GabrielButton; }
    else if (fixtureTypeStr == "Trigger_ChaseEnemyAreaSensor") { fixtureType = FixtureType_Trigger_ChaseEnemyAreaSensor; }
    else if (fixtureTypeStr == "Trigger_RollAreaSensor") { fixtureType = FixtureType_Trigger_RollAreaSensor; }
    else
    {
        assert(false && "Unknown body type");
    }

    return fixtureType;
}

//=============================================================================
// DamageType
//=============================================================================

DamageType StringToDamageTypeEnum(const std::string& str)
{
    static const std::map<std::string, DamageType> s_StringToDamageTypeEnumMap =
    {
        { "DamageType_None", DamageType_None },
        { "DamageType_MeleeAttack", DamageType_MeleeAttack },
        { "DamageType_Bullet", DamageType_Bullet },
        { "DamageType_Magic", DamageType_Magic },
        { "DamageType_Explosion", DamageType_Explosion },
        { "DamageType_FireSword", DamageType_FireSword },
        { "DamageType_LightningSword", DamageType_LightningSword },
        { "DamageType_FrostSword", DamageType_FrostSword },
        { "DamageType_GabrielCannonBall", DamageType_GabrielCannonBall },
        { "DamageType_Trident", DamageType_Trident },
        { "DamageType_SirenProjectile", DamageType_SirenProjectile }
    };

    auto findIt = s_StringToDamageTypeEnumMap.find(str);
    if (findIt == s_StringToDamageTypeEnumMap.end())
    {
        LOG_ERROR("Could not find DamageType: " + str);
        assert(false);
    }

    return findIt->second;
}

//=============================================================================
// b2BodyType
//=============================================================================

b2BodyType BodyTypeStringToEnum(const std::string& bodyTypeStr)
{
    b2BodyType bodyType = b2_staticBody;

    if (bodyTypeStr == "Static") { bodyType = b2_staticBody; }
    else if (bodyTypeStr == "Kinematic") { bodyType = b2_kinematicBody; }
    else if (bodyTypeStr == "Dynamic") { bodyType = b2_dynamicBody; }
    else
    {
        assert(false && "Unknown body type");
    }

    return bodyType;
}

//=============================================================================
// Direction
//=============================================================================

Direction StringToEnum_Direction(const std::string& dirStr)
{
    static const std::map<std::string, Direction> directionEnumToStringMap =
    {
        { "Direction_None", Direction_None },
        { "Direction_Up", Direction_Up },
        { "Direction_Down", Direction_Down },
        { "Direction_Left", Direction_Left },
        { "Direction_Right", Direction_Right },
        { "Direction_Up_Left", Direction_Up_Left },
        { "Direction_Down_Left", Direction_Down_Left },
        { "Direction_Up_Right", Direction_Up_Right },
        { "Direction_Down_Right", Direction_Down_Right },
    };

    auto findIt = directionEnumToStringMap.find(dirStr);
    if (findIt == directionEnumToStringMap.end())
    {
        LOG_ERROR("Could not find Direction: " + dirStr);
        assert(false);
    }

    return findIt->second;
}

std::string EnumToString_Direction(Direction dir)
{
    static std::map<Direction, std::string> stringToDirectionEnumMap =
    {
        // Actor enemies
        { Direction_None, "Direction_None" },
        { Direction_Up, "Direction_Up" },
        { Direction_Down, "Direction_Down" },
        { Direction_Left, "Direction_Left" },
        { Direction_Right, "Direction_Right" },
        { Direction_Up_Left, "Direction_Up_Left" },
        { Direction_Down_Left, "Direction_Down_Left" },
        { Direction_Up_Right, "Direction_Up_Right" },
        { Direction_Down_Right, "Direction_Down_Right" },
    };

    auto findIt = stringToDirectionEnumMap.find(dir);
    if (findIt == stringToDirectionEnumMap.end())
    {
        LOG_ERROR("Could not find direction enum: " + ToStr((int)dir));
        assert(false && "Could not convert Direction enum to string");
    }

    return findIt->second;
}
