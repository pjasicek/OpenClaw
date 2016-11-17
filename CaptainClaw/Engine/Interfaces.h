#ifndef __INTERFACES_H__
#define __INTERFACES_H__

#include <SDL.h>
#include <stdint.h>
#include <memory>
#include <list>
#include <map>
#include <Tinyxml/tinyxml.h>

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

class CameraNode;
class Point;
class IGamePhysics
{
public:
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

    // Debugging
    virtual void VRenderDiagnostics(SDL_Renderer* pRenderer, std::shared_ptr<CameraNode> pCamera) = 0;

    // Physics world modifiers
    virtual void VCreateTrigger(WeakActorPtr pActor, const Point& pos, Point& size) = 0;
    virtual void VApplyForce(uint32_t actorId, const Point& impulse) = 0;
    virtual void VApplyLinearImpulse(uint32_t actorId, const Point& impulse) = 0;
    virtual bool VKinematicMove(const Point& pos, uint32_t actorId) = 0;

    // Physics actor states
    virtual void VStopActor(uint32_t actorId) = 0;
    virtual Point VGetVelocity(uint32_t actorId) = 0;
    virtual void SetVelocity(uint32_t actorId, const Point& velocity) = 0;
    virtual void VTranslate(uint32_t actorId, const Point& dir) = 0;
    virtual void VSetLinearSpeed(uint32_t actorId, const Point& speed) = 0;
    virtual void VSetGravityScale(uint32_t actorId, const float gravityScale) = 0;
    virtual void VSetLinearSpeedEx(uint32_t actorId, const Point& speed) = 0;

    virtual void VActivate(uint32_t actorId) = 0;
    virtual void VDeactivate(uint32_t actorId) = 0;

    virtual void VSetPosition(uint32_t actorId, const Point& position) = 0;
    virtual Point VGetPosition(uint32_t actorId) = 0;
};

enum GameViewType
{
    GameView_Human,
    GameView_Remote,
    GameView_AI,
    GameView_Recorder,
    GameView_Other
};

class IGameLogic
{
public:
    virtual WeakActorPtr VGetActor(const uint32_t actorId) = 0;
    virtual StrongActorPtr VCreateActor(const std::string& actorResource, TiXmlElement* overrides) = 0;
    virtual StrongActorPtr VCreateActor(TiXmlElement* pActorRoot, TiXmlElement* overrides) = 0;
    virtual void VDestroyActor(const uint32_t actorId) = 0;
    virtual bool VLoadGame(const char* xmlLevelResource) = 0;
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