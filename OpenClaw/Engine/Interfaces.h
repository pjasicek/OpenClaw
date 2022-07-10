#ifndef __INTERFACES_H__
#define __INTERFACES_H__

#include <SDL2/SDL.h>
#include <Box2D/Box2D.h>
#include <stdint.h>
#include <memory>
#include <list>
#include <map>
#include <tinyxml.h>
#include <stdlib.h>
#include <float.h>
#include <vector>
#include <assert.h>
#include "Util/EnumString.h"
#include "UserInterface/Touch/TouchEvents.h"
#include "UserInterface/Touch/TouchRecognizers/AbstractRecognizer.h"

class Actor;
typedef std::shared_ptr<Actor> StrongActorPtr;
typedef std::weak_ptr<Actor> WeakActorPtr;

typedef std::vector<Actor*> ActorList;

class ActorComponent;
typedef std::shared_ptr<ActorComponent> StrongActorComponentPtr;
typedef std::weak_ptr<ActorComponent> WeakActorComponentPtr;

typedef std::vector<std::string> SoundList;

enum ActionType
{
    ActionType_Fire,
    ActionType_Attack,
    ActionType_Change_Ammo_Type
};

enum Direction
{
    Direction_None,
    Direction_Up,
    Direction_Down,
    Direction_Left,
    Direction_Right,
    Direction_Up_Left,
    Direction_Up_Right,
    Direction_Down_Right,
    Direction_Down_Left
};

enum MovementInfoEvent
{
    MovementEvent_None,
    MovementEvent_SidehitSolid,
    MovementEvent_FloorHitSolid,
    MovementInfo_CeilingHitSolid,
    MovementInfo_LandedLadder,
    MovementInfo_HitDeath
};

enum CollisionType
{
    CollisionType_None,
    CollisionType_Solid,
    CollisionType_Ground,
    CollisionType_Climb,
    CollisionType_Death
};

enum CollisionFlag
{
    CollisionFlag_None                  = 0x0,
    CollisionFlag_All                   = 0x1,
    CollisionFlag_Controller            = 0x2,
    CollisionFlag_DynamicActor          = 0x4,
    CollisionFlag_Bullet                = 0x8,
    CollisionFlag_Explosion             = 0x10,
    CollisionFlag_Magic                 = 0x20,
    CollisionFlag_Crate                 = 0x40,
    CollisionFlag_Rope                  = 0x80,
    CollisionFlag_Solid                 = 0x100,
    CollisionFlag_Ground                = 0x200,
    CollisionFlag_Death                 = 0x400,
    CollisionFlag_Ladder                = 0x800,
    CollisionFlag_PowderKeg             = 0x1000,
    CollisionFlag_Trigger               = 0x2000,
    CollisionFlag_Pickup                = 0x4000,
    CollisionFlag_Checkpoint            = 0x8000,
    CollisionFlag_ClawAttack            = 0x10000,
    CollisionFlag_EnemyAIAttack         = 0x20000,
    CollisionFlag_EnemyAIProjectile     = 0x40000,
    CollisionFlag_DamageAura            = 0x80000,
    CollisionFlag_InvisibleController   = 0x100000,
    CollisionFlag_RopeSensor            = 0x400000,
};

enum FixtureType
{
    FixtureType_Min = -1,
    FixtureType_None = 0,
    // Tiles
    FixtureType_Solid,
    FixtureType_Ground,
    FixtureType_TopLadderGround,
    FixtureType_Climb,
    FixtureType_Death,
    // Sensors
    FixtureType_FootSensor,
    FixtureType_Controller,
    FixtureType_AmbientTrigger,
    FixtureType_Trigger,
    FixtureType_Projectile,
    FixtureType_Crate,
    FixtureType_PowderKeg,
    FixtureType_Pickup,
    FixtureType_Explosion,
    FixtureType_EnemyAI,
    FixtureType_EnemyAIMeleeSensor,
    FixtureType_EnemyAIDuckMeleeSensor,
    FixtureType_EnemyAIRangedSensor,
    FixtureType_EnemyAIDuckRangedSensor,
    FixtureType_EnemyAIDiveAreaSensor,
    FixtureType_DamageAura,
    FixtureType_RopeSensor,
    FixtureType_Trigger_SpawnArea,
    FixtureType_Trigger_GabrielButton,
    FixtureType_Trigger_ChaseEnemyAreaSensor,
    FixtureType_Trigger_RollAreaSensor,
    FixtureType_Max,
};

enum PlayerStat
{
    PlayerStat_Score,
    PlayerStat_Health,
    PlayerStat_Lives,
    PlayerStat_Bullets,
    PlayerStat_Magic,
    PlayerStat_Dynamite
};

enum AmmoType
{
    AmmoType_None = -1,
    AmmoType_Pistol,
    AmmoType_Magic,
    AmmoType_Dynamite,
    //-----
    AmmoType_Max
};

enum PowerupType
{
    PowerupType_None = -1,
    PowerupType_Catnip,
    PowerupType_Invulnerability,
    PowerupType_Invisibility,
    PowerupType_FireSword,
    PowerupType_FrostSword,
    PowerupType_LightningSword,
    PowerupType_Max
};

enum PickupType
{
    PickupType_None = -1,
    PickupType_Default = 0,
    PickupType_Treasure_Goldbars,
    PickupType_Treasure_Rings_Red,
    PickupType_Treasure_Rings_Green,
    PickupType_Treasure_Rings_Blue,
    PickupType_Treasure_Rings_Purple,
    PickupType_Treasure_Necklace,
    PickupType_Treasure_Crosses_Red,
    PickupType_Treasure_Crosses_Green,
    PickupType_Treasure_Crosses_Blue,
    PickupType_Treasure_Crosses_Purple,
    PickupType_Treasure_Scepters_Red,
    PickupType_Treasure_Scepters_Green,
    PickupType_Treasure_Scepters_Blue,
    PickupType_Treasure_Scepters_Purple,
    PickupType_Treasure_Geckos_Red,
    PickupType_Treasure_Geckos_Green,
    PickupType_Treasure_Geckos_Blue,
    PickupType_Treasure_Geckos_Purple,
    PickupType_Ammo_Deathbag,
    PickupType_Ammo_Shot,
    PickupType_Ammo_Shotbag,
    PickupType_Powerup_Catnip_1,
    PickupType_Powerup_Catnip_2,
    PickupType_Health_Breadwater,
    PickupType_Health_25,
    PickupType_Health_10,
    PickupType_Health_15,
    PickupType_Ammo_Magic_5,
    PickupType_Ammo_Magic_10,
    PickupType_Ammo_Magic_25,
    PickupType_Mappiece,
    PickupType_Warp,
    PickupType_Treasure_Coins,
    PickupType_Ammo_Dynamite,
    PickupType_Curse_Ammo,
    PickupType_Curse_Magic,
    PickupType_Curse_Health,
    PickupType_Curse_Death,
    PickupType_Curse_Treasure,
    PickupType_Curse_Freeze,
    PickupType_Treasure_Chalices_Red,
    PickupType_Treasure_Chalices_Green,
    PickupType_Treasure_Chalices_Blue,
    PickupType_Treasure_Chalices_Purple,
    PickupType_Treasure_Crowns_Red,
    PickupType_Treasure_Crowns_Green,
    PickupType_Treasure_Crowns_Blue,
    PickupType_Treasure_Crowns_Purple,
    PickupType_Treasure_Skull_Red,
    PickupType_Treasure_Skull_Green,
    PickupType_Treasure_Skull_Blue,
    PickupType_Treasure_Skull_Purple,
    PickupType_Powerup_Invisibility,
    PickupType_Powerup_Invincibility,
    PickupType_Powerup_Life,
    PickupType_Powerup_FireSword,
    PickupType_Powerup_LightningSword,
    PickupType_Powerup_FrostSword,
    PickupType_BossWarp,
    PickupType_Level2_Gem,
    PickupType_Max
};

enum DamageType
{
    DamageType_None = -1,
    DamageType_MeleeAttack,
    DamageType_Bullet,
    DamageType_Magic,
    DamageType_Explosion,
    DamageType_FireSword,
    DamageType_LightningSword,
    DamageType_FrostSword,
    DamageType_DeathTile,
    DamageType_EnemyAura,
    DamageType_GabrielCannonBall,
    DamageType_Trident,
    DamageType_SirenProjectile,
    DamageType_Max
};

enum AnimationType
{
    AnimationType_Explosion,
    AnimationType_TridentExplosion,
    AnimationType_RedHitPoint,
    AnimationType_BlueHitPoint,
    AnimationType_TarSplash
};

/*enum TriggerType
{
    TriggerType_None = -1,

    TriggerType_Undefined = 0,
    TriggerType_SpawnArea,

    TriggerType_Max,
};

Begin_Enum_String(TriggerType)
{
    Enum_String(TriggerType_None);
    Enum_String(TriggerType_Undefined);
    Enum_String(TriggerType_SpawnArea);
    Enum_String(TriggerType_Max);
}
End_Enum_String;*/

enum ActorPrototype
{
    ActorPrototype_None = -1,

    ActorPrototype_Start,

    ActorPrototype_Null,

    //=======================================
    // Level specific
    //=======================================

    // Enemy actors
    ActorPrototype_BaseEnemy,

    ActorPrototype_Level1_Soldier,
    ActorPrototype_Level1_Officer,
    ActorPrototype_Level1_Rat,

    ActorPrototype_Level2_Soldier,
    ActorPrototype_Level2_Officer,
    ActorPrototype_Level2_PunkRat,

    ActorPrototype_Level3_Rat,
    ActorPrototype_Level3_CutThroat,
    ActorPrototype_Level3_RobberThief,

    ActorPrototype_Level4_Rat,
    ActorPrototype_Level4_CutThroat,
    ActorPrototype_Level4_RobberThief,

    ActorPrototype_Level5_TownGuard1,
    ActorPrototype_Level5_TownGuard2,
    ActorPrototype_Level5_Seagull,

    ActorPrototype_Level6_TownGuard1,
    ActorPrototype_Level6_TownGuard2,
    ActorPrototype_Level6_Rat,
    ActorPrototype_Level6_Seagull,

    // Elevators
    ActorPrototype_BaseElevator,

    ActorPrototype_BasePathElevator,

    // Toggle pegs
    ActorPrototype_BaseTogglePeg,

    ActorPrototype_Level1_TogglePeg,
    ActorPrototype_Level2_TogglePeg,
    ActorPrototype_Level5_TogglePeg,

    // Crumbling pegs
    ActorPrototype_BaseCrumblingPeg,

    ActorPrototype_Level1_CrumblingPeg,
    ActorPrototype_Level3_CrumblingPeg,
    ActorPrototype_Level4_CrumblingPeg,
    ActorPrototype_Level5_CrumblingPeg,
    ActorPrototype_Level6_CrumblingPeg,

    // Projectile spawners
    ActorPrototype_Level2_TowerCannonLeft,
    ActorPrototype_Level2_TowerCannonRight,

    // Projectiles
    ActorPrototype_Level1_RatBomb,
    ActorPrototype_Level2_CannonBall,
    ActorPrototype_Level2_RatProjectile,
    ActorPrototype_Level3_RatBomb,
    ActorPrototype_Level3_ArrowProjectile,
    ActorPrototype_Level3_KnifeProjectile,
    ActorPrototype_Level4_RatBomb,
    ActorPrototype_Level4_ArrowProjectile,
    ActorPrototype_Level4_KnifeProjectile,
    ActorPrototype_Level6_WolvingtonMagic,

    // Boss stagers
    ActorPrototype_Level2_BossStager,
    ActorPrototype_Level4_BossStager,
    ActorPrototype_Level6_BossStager,

    // Bosses
    ActorPrototype_Level2_LaRaux,
    ActorPrototype_Level4_Katherine,
    ActorPrototype_Level6_Wolvington,

    // Floor spike
    ActorPrototype_Level3_FloorSpike,
    ActorPrototype_Level4_FloorSpike,
    ActorPrototype_Level12_FloorSpike,
    ActorPrototype_Level13_FloorSpike,

    // Stepping stones
    ActorPrototype_Level4_SteppingGround,
    ActorPrototype_Level6_SteppingGround,

    // Spring board
    ActorPrototype_Level4_SpringBoard,
    ActorPrototype_Level6_GroundBlower,

    // Loot containers
    ActorPrototype_BaseLootContainer,
    ActorPrototype_Level5_LootStatue,

    // Floor Spikes / Floor goovents etc
    ActorPrototype_Level6_GooVent,

    //=======================================
    // General
    //=======================================

    ActorPrototype_BaseProjectile,
    ActorPrototype_FireSwordProjectile,
    ActorPrototype_FrostSwordProjectile,
    ActorPrototype_LightningSwordProjectile,

    ActorPrototype_BaseProjectileSpawner,

    // Sound
    ActorPrototype_LocalAmbientSound,

    // Stepping Stone
    ActorPrototype_BaseSteppingGround,

    // Spring board
    ActorPrototype_BaseSpringBoard,

    // Menu
    ActorPrototype_StaticImage,
    ActorPrototype_StaticAnimatedImage,

    ActorPrototype_BaseBossStager,

    ActorPrototype_BaseFloorSpike,

    ActorPrototype_BaseRope,
    
    ActorPrototype_BaseActorSpawner,

    // Level 2
    ActorPrototype_Level2_PunkRatCannon,

    // Level 7
    ActorPrototype_Level7_CrumblingPeg,
    ActorPrototype_Level7_PathElevator,
    ActorPrototype_Level7_Seagull,
    ActorPrototype_Level7_SpringBoard,
    ActorPrototype_Level7_SteppingGround,
    ActorPrototype_Level7_BearSailor,
    ActorPrototype_Level7_RedTailPirate,
    ActorPrototype_Level7_HermitCrab,
    ActorPrototype_Level7_CrabNest,

    // Level 8
    ActorPrototype_Level8_Seagull,
    ActorPrototype_Level8_BossStager,
    ActorPrototype_Level8_SteppingGround,
    ActorPrototype_Level8_BearSailor,
    ActorPrototype_Level8_RedTailPirate,
    ActorPrototype_Level8_PunkRat,
    ActorPrototype_Level8_TogglePeg,
    ActorPrototype_Level8_TogglePeg_2,
    ActorPrototype_Level8_CannonBall,
    ActorPrototype_Level8_PunkRatCannon,
    ActorPrototype_Level8_GabrielButton,
    ActorPrototype_Level8_GabrielCannon,
    ActorPrototype_Level8_Gabriel,
    ActorPrototype_Level8_GabrielPirate,
    ActorPrototype_Level8_GabrielBomb,
    ActorPrototype_Level8_BossGem,

    // Level 9
    ActorPrototype_Level9_Seagull,
    ActorPrototype_Level9_SpringBoard,
    ActorPrototype_Level9_PuffDartSpawner,
    ActorPrototype_Level9_DartProjectile_Up,
    ActorPrototype_Level9_DartProjectile_Down,
    ActorPrototype_Level9_DartProjectile_Right,
    ActorPrototype_Level9_DartProjectile_Left,
    ActorPrototype_Level9_SawBlade,
    ActorPrototype_Level9_SkullCannon,
    ActorPrototype_Level9_CannonBall,
    ActorPrototype_Level9_CrazyHook,
    ActorPrototype_Level9_PegLeg,
    ActorPrototype_Level9_BulletProjectile,
    ActorPrototype_Level9_ConveyorBelt,

    // Level 10
    ActorPrototype_Level10_Seagull,
    ActorPrototype_Level10_PuffDartSpawner,
    ActorPrototype_Level10_DartProjectile_Up,
    ActorPrototype_Level10_DartProjectile_Down,
    ActorPrototype_Level10_DartProjectile_Right,
    ActorPrototype_Level10_DartProjectile_Left,
    ActorPrototype_Level10_CrazyHook,
    ActorPrototype_Level10_PegLeg,
    ActorPrototype_Level10_CrumblingPeg,
    ActorPrototype_Level10_TogglePeg,
    ActorPrototype_Level10_BossStager,
    ActorPrototype_Level10_Marrow,
    ActorPrototype_Level10_MarrowFloor,
    ActorPrototype_Level10_MarrowParrot,
    ActorPrototype_Level10_BossGem,

    // Level 11
    ActorPrototype_Level11_Mercat,
    ActorPrototype_Level11_Siren,
    ActorPrototype_Level11_Fish,
    ActorPrototype_Level11_TogglePeg,
    ActorPrototype_Level11_BreakPlank,
    ActorPrototype_Level11_ConveyorBelt,
    ActorPrototype_Level11_Laser,
    ActorPrototype_Level11_SirenProjectile,
    ActorPrototype_Level11_TridentProjectile,

    // Level 12
    ActorPrototype_Level12_Mercat,
    ActorPrototype_Level12_CrumblingPeg,
    ActorPrototype_Level12_Siren,
    ActorPrototype_Level12_Fish,
    ActorPrototype_Level12_TogglePeg,
    ActorPrototype_Level12_Aquatis,
    ActorPrototype_Level12_RockSpring,
    ActorPrototype_Level12_PathElevator,
    ActorPrototype_Level12_BossStager,
    ActorPrototype_Level12_BossGem,

    // Level 13
    ActorPrototype_Level13_BearSailor,
    ActorPrototype_Level13_RedTailPirate,
    ActorPrototype_Level13_WaterRock,
    ActorPrototype_Level13_SpringBoard,
    ActorPrototype_Level13_PathElevator,
    ActorPrototype_Level13_TogglePeg,
    ActorPrototype_Level13_CrumblingPeg,
    ActorPrototype_Level13_SteppingGround,
    ActorPrototype_Level13_PuffDartSpawner,
    ActorPrototype_Level13_RedTail,
    ActorPrototype_Level13_BossGem,
    ActorPrototype_Level13_BossStager,

    ActorPrototype_Max
};

struct RaycastResult
{
    RaycastResult()
    {
        foundIntersection = false;
        closestPixelDistance = FLT_MAX;
        deltaX = 0;
        deltaY = 0;
    }

    bool foundIntersection;
    float closestPixelDistance;
    float deltaX;
    float deltaY;
};

class Point;
struct ActorBodyDef;
struct ActorFixtureDef;
class CameraNode;
class IGamePhysics
{
public:
    virtual ~IGamePhysics() { }

    // Initialization and maintanance of the Physics World
    virtual bool VInitialize() = 0;
    virtual void VSyncVisibleScene() = 0;
    virtual void VOnUpdate(const uint32_t msDiff) = 0;

    // Initialization of Physics Objects
    virtual void VAddCircle(float radius, uint32_t thickness, WeakActorPtr pTargetActor) = 0;
    virtual void VAddRect(uint32_t thickness, WeakActorPtr pTargetActor) = 0;
    virtual void VAddLine(Point from, Point to, uint32_t thickness) = 0;
    virtual void VAddStaticGeometry(const Point& position, const Point& size, CollisionType collisionType, FixtureType fixtureType) = 0;
    virtual void VAddDynamicActor(WeakActorPtr pActor) = 0;
    virtual void VAddKinematicBody(WeakActorPtr pActor) = 0;
    virtual void VAddStaticBody(WeakActorPtr pActor, const Point& bodySize, CollisionType collisionType) = 0;
    virtual void VRemoveActor(uint32_t actorId) = 0;

    virtual void VAddActorBody(const ActorBodyDef* actorBodyDef) = 0;
    virtual void VAddActorFixtureToBody(uint32_t actorId, const ActorFixtureDef* pFixtureDef) = 0;

    // Debugging
    virtual void VRenderDiagnostics(SDL_Renderer* pRenderer, std::shared_ptr<CameraNode> pCamera) = 0;

    // Physics world modifiers
    virtual void VCreateTrigger(WeakActorPtr pActor, const Point& pos, Point& size, bool isStatic) = 0;
    virtual void VApplyForce(uint32_t actorId, const Point& impulse) = 0;
    virtual void VApplyLinearImpulse(uint32_t actorId, const Point& impulse) = 0;
    virtual bool VKinematicMove(const Point& pos, uint32_t actorId) = 0;

    virtual Point GetGravity() const = 0;

    // Physics actor states
    virtual void VStopActor(uint32_t actorId) = 0;
    virtual Point VGetVelocity(uint32_t actorId) = 0;
    virtual void SetVelocity(uint32_t actorId, const Point& velocity) = 0;
    virtual void VTranslate(uint32_t actorId, const Point& dir) = 0;
    virtual void VSetLinearSpeed(uint32_t actorId, const Point& speed) = 0;
    virtual void VAddLinearSpeed(uint32_t actorId, const Point& speedIncrement) = 0;
    virtual void VSetGravityScale(uint32_t actorId, const float gravityScale) = 0;
    virtual void VSetLinearSpeedEx(uint32_t actorId, const Point& speed) = 0;
    virtual bool VIsAwake(uint32_t actorId) = 0;

    virtual void VChangeCollisionFlag(uint32_t actorId, uint32_t fromFlag, uint32_t toFlag) = 0;

    virtual void VActivate(uint32_t actorId) = 0;
    virtual void VDeactivate(uint32_t actorId) = 0;

    virtual void VSetPosition(uint32_t actorId, const Point& position) = 0;
    virtual Point VGetPosition(uint32_t actorId) = 0;

    virtual SDL_Rect VGetAABB(uint32_t actorId, bool discardSensors) = 0;
    virtual bool VIsActorOverlap(uint32_t actorId, FixtureType overlapType) = 0;

    virtual RaycastResult VRayCast(const Point& fromPoint, const Point& toPoint, uint32_t filterMask) = 0;

    virtual void VScaleActor(uint32_t actorId, double scale) = 0;
};

enum GameViewType
{
    GameView_Human,
    GameView_Remote,
    GameView_AI,
    GameView_Recorder,
    GameView_Other
};

enum GameState
{
    GameState_Invalid,
    GameState_Initializing,
    GameState_LoadingMenu,
    GameState_Menu,
    GameState_LoadingLevel,
    GameState_IngameRunning,
    GameState_IngamePaused,
    GameState_LoadingScoreScreen,
    GameState_ScoreScreen,
    GameState_Cutscene
};

class IGameLogic
{
public:
    virtual WeakActorPtr VGetActor(const uint32_t actorId) = 0;
    virtual StrongActorPtr VCreateActor(const std::string& actorResource, TiXmlElement* overrides) = 0;
    virtual StrongActorPtr VCreateActor(TiXmlElement* pActorRoot, TiXmlElement* overrides) = 0;
    virtual void VDestroyActor(const uint32_t actorId) = 0;
    virtual bool VLoadGame(const char* xmlLevelResource) = 0;
    virtual bool VEnterMenu(const char* xmlMenuResource) = 0;
    virtual void VSetProxy() = 0;
    virtual void VOnUpdate(uint32_t msDiff) = 0;
    virtual void VChangeState(enum GameState newState) = 0;
    virtual void VMoveActor(const uint32_t actorId, Point newPosition) = 0;
    virtual std::shared_ptr<IGamePhysics> VGetGamePhysics() = 0;
};

class IGameView
{
public:
    virtual ~IGameView() { }

    virtual void VOnRender(uint32_t msDiff) = 0;
    virtual void VOnLostDevice() = 0;
    virtual GameViewType VGetType() = 0;
    virtual uint32_t VGetId() const = 0;
    virtual void VOnAttach(uint32_t viewId, uint32_t actorId) = 0;

    virtual bool VOnEvent(SDL_Event& evt) = 0;
    virtual void VOnUpdate(uint32_t msDiff) = 0;
};

class IScreenElement
{
public:
    virtual ~IScreenElement() { }

    virtual void VOnLostDevice() = 0;
    virtual void VOnRender(uint32_t msDiff) = 0;
    virtual void VOnUpdate(uint32_t msDiff) = 0;

    virtual int32_t VGetZOrder() const = 0;
    virtual void VSetZOrder(int32_t const zOrder) = 0;
    virtual bool VIsVisible() = 0;
    virtual void VSetVisible(bool visible) = 0;

    virtual bool VOnEvent(SDL_Event& evt) = 0;

    virtual bool const operator < (IScreenElement const &other) { return VGetZOrder() < other.VGetZOrder(); }
};

typedef std::list<std::shared_ptr<IScreenElement>> ScreenElementList;
typedef std::list<std::shared_ptr<IGameView>> GameViewList;

class IKeyboardHandler
{
public:
    virtual bool VOnKeyDown(SDL_Keycode key) = 0;
    virtual bool VOnKeyUp(SDL_Keycode key) = 0;
};

class IPointerHandler
{
public:
    virtual bool VOnPointerMove(SDL_MouseMotionEvent& mouseEvent) = 0;
    virtual bool VOnPointerButtonDown(SDL_MouseButtonEvent& mouseEvent) = 0;
    virtual bool VOnPointerButtonUp(SDL_MouseButtonEvent& mouseEvent) = 0;
};

class ITouchHandler {
public:
    virtual std::vector<std::shared_ptr<AbstractRecognizer>> VRegisterRecognizers() = 0;
    virtual bool VOnTouch(const Touch_Event &evt) = 0;
};

//-------------------------------------------------------------------------------------------------
// Abstract generic factory
//-------------------------------------------------------------------------------------------------

template <class BaseType, class SubType>
BaseType* GenericObjectCreationFunction(void) { return new SubType; }

template <class BaseClass, class IdType>
class GenericObjectFactory
{
public:
    template <class SubClass>
    bool Register(IdType id)
    {
        auto findIter = _creationFunctions.find(id);
        if (findIter == _creationFunctions.end())
        {
            _creationFunctions[id] = &GenericObjectCreationFunction < BaseClass, SubClass >;
            return true;
        }

        return false;
    }

    template <class SubClass>
    bool Register()
    {
        IdType id = SubClass::GetIdFromName(SubClass::g_Name);

        return Register<SubClass>(id);
    }

    BaseClass* Create(IdType id)
    {
        auto findIter = _creationFunctions.find(id);
        if (findIter != _creationFunctions.end())
        {
            ObjectCreationFunction func = findIter->second;
            return func();
        }

        return NULL;
    }

private:
    typedef BaseClass* (*ObjectCreationFunction)();
    std::map<IdType, ObjectCreationFunction> _creationFunctions;
};

template <typename T> class Singleton
{
public:
    static T* Instance()
    {
        static T _singleton;
        return &_singleton;
    }

protected:
    Singleton() { }
    ~Singleton() { }
};

//-------------------------------------------------------------------------------------------------
// String-To-Enum
// Enum-To-String
//-------------------------------------------------------------------------------------------------

FixtureType FixtureTypeStringToEnum(const std::string& fixtureTypeStr);

DamageType StringToDamageTypeEnum(const std::string& str);

b2BodyType BodyTypeStringToEnum(const std::string& bodyTypeStr);

Direction StringToEnum_Direction(const std::string& dirStr);
std::string EnumToString_Direction(Direction dir);

std::string EnumToString_ActorPrototype(ActorPrototype actorProto);
ActorPrototype StringToEnum_ActorPrototype(const std::string& actorProtoStr);

#endif
