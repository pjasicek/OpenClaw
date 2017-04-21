#ifndef __INTERFACES_H__
#define __INTERFACES_H__

#include <SDL2/SDL.h>
#include <stdint.h>
#include <memory>
#include <list>
#include <map>
#include <tinyxml.h>
#include <stdlib.h>
#include <float.h>

class Actor;
typedef std::shared_ptr<Actor> StrongActorPtr;
typedef std::weak_ptr<Actor> WeakActorPtr;

class ActorComponent;
typedef std::shared_ptr<ActorComponent> StrongActorComponentPtr;
typedef std::weak_ptr<ActorComponent> WeakActorComponentPtr;

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
    CollisionFlag_None              = 0x0,
    CollisionFlag_Controller        = 0x2,
    CollisionFlag_DynamicActor      = 0x4,
    CollisionFlag_Bullet            = 0x8,
    CollisionFlag_Explosion         = 0x10,
    CollisionFlag_Magic             = 0x20,
    CollisionFlag_Crate             = 0x40,
    CollisionFlag_Rope              = 0x80,
    CollisionFlag_Solid             = 0x100,
    CollisionFlag_Ground            = 0x200,
    CollisionFlag_Death             = 0x400,
    CollisionFlag_Ladder            = 0x800,
    CollisionFlag_PowderKeg         = 0x1000,
    CollisionFlag_Trigger           = 0x2000,
    CollisionFlag_Pickup            = 0x4000,
    CollisionFlag_Checkpoint        = 0x8000,
    CollisionFlag_ClawAttack        = 0x10000,
    CollisionFlag_EnemyAIAttack     = 0x20000,
    CollisionFlag_EnemyAIProjectile = 0x40000,
    CollisionFlag_DamageAura        = 0x40000,
};

enum FixtureType
{
    FixtureType_None = 0,
    // Tiles
    FixtureType_Solid,
    FixtureType_Ground,
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
    FixtureType_EnemyAIRangedSensor,
    FixtureType_DamageAura
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
    PowerupType_IceSword,
    PowerupType_LightningSword,
    PowerupType_Max
};

enum PickupType
{
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
    PickupType_Powerup_IceSword,
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
    DamageType_IceSword,
    DamageType_DeathTile,
    DamageType_EnemyAura,
    DamageType_Max
};

enum AnimationType
{
    AnimationType_Explosion,
    AnimationType_RedHitPoint,
    AnimationType_BlueHitPoint,
    AnimationType_TarSplash
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

struct ActorBodyDef;
struct ActorFixtureDef;
class CameraNode;
class Point;
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
    virtual void VAddStaticGeometry(Point position, Point size, CollisionType collisionType) = 0;
    virtual void VAddDynamicActor(WeakActorPtr pActor) = 0;
    virtual void VAddKinematicBody(WeakActorPtr pActor) = 0;
    virtual void VAddStaticBody(WeakActorPtr pActor, Point bodySize, CollisionType collisionType) = 0;
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

    virtual void VActivate(uint32_t actorId) = 0;
    virtual void VDeactivate(uint32_t actorId) = 0;

    virtual void VSetPosition(uint32_t actorId, const Point& position) = 0;
    virtual Point VGetPosition(uint32_t actorId) = 0;

    virtual SDL_Rect VGetAABB(uint32_t actorId, bool discardSensors) = 0;

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
    GameState_Menu,
    GameState_LoadingLevel,
    GameState_IngameRunning,
    GameState_IngamePaused,
    GameState_FinishedLevel,
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

#endif