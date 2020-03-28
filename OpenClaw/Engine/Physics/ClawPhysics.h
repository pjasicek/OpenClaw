#ifndef __CLAWPHYSICS_H__
#define __CLAWPHYSICS_H__

#include "../Interfaces.h"
#include "../SharedDefines.h"

#include <Box2D/Box2D.h>

typedef std::map<uint32, b2Body*> ActorIDToBox2DBodyMap;
typedef std::map<b2Body*, uint32> Box2DBodyToActorIDMap;
typedef std::vector<std::pair<uint32, b2Body*>> ActorIdAndBodyList;

class PhysicsContactListener;
class PhysicsDebugDrawer;
class ClawPhysics : public IGamePhysics
{
public:
    ClawPhysics();
    virtual ~ClawPhysics();

    // Initialization and maintanance of the Physics World
    virtual bool VInitialize() override;
    virtual void VSyncVisibleScene() override;
    virtual void VOnUpdate(const uint32_t msDiff) override;

    // Initialization of Physics Objects
    virtual void VAddCircle(float radius, uint32_t thickness, WeakActorPtr pTargetActor) override;
    virtual void VAddRect(uint32_t thickness, WeakActorPtr pTargetActor) override;
    virtual void VAddLine(Point from, Point to, uint32_t thickness) override;

    virtual void VAddStaticGeometry(const Point& position, const Point& size, CollisionType collisionType, FixtureType fixtureType) override;
    virtual void VAddDynamicActor(WeakActorPtr pActor) override;
    virtual void VAddKinematicBody(WeakActorPtr pActor) override;
    virtual void VAddStaticBody(WeakActorPtr pActor, const Point& bodySize, CollisionType collisionType) override;
    virtual void VRemoveActor(uint32_t actorId) override;

    virtual void VAddActorBody(const ActorBodyDef* actorBodyDef) override;
    virtual void VAddActorFixtureToBody(uint32_t actorId, const ActorFixtureDef* pFixtureDef) override;

    // Debugging
    virtual void VRenderDiagnostics(SDL_Renderer* pRenderer, shared_ptr<CameraNode> pCamera) override;

    // Physics world modifiers
    virtual void VCreateTrigger(WeakActorPtr pActor, const Point& pos, Point& size, bool isStatic) override;
    virtual void VApplyForce(uint32_t actorId, const Point& force) override;
    virtual void VApplyLinearImpulse(uint32_t actorId, const Point& impulse) override;
    virtual bool VKinematicMove(const Point& pos, uint32_t actorId) override;

    virtual Point GetGravity() const override;

    // Physics actor states
    virtual void VStopActor(uint32_t actorId) override;
    virtual Point VGetVelocity(uint32_t actorId) override;
    virtual void SetVelocity(uint32_t actorId, const Point& velocity) override;
    virtual void VTranslate(uint32_t actorId, const Point& dir) override;
    virtual void VAddLinearSpeed(uint32_t actorId, const Point& speedIncrement) override;
    virtual void VSetLinearSpeed(uint32_t actorId, const Point& speed) override;
    virtual void VSetGravityScale(uint32_t actorId, const float gravityScale) override;
    virtual void VSetLinearSpeedEx(uint32_t actorId, const Point& speed) override;
    virtual bool VIsAwake(uint32_t actorId) override;

    virtual void VChangeCollisionFlag(uint32_t actorId, uint32 fromFlag, uint32 toFlag) override;

    virtual void VActivate(uint32_t actorId) override;
    virtual void VDeactivate(uint32_t actorId) override;

    virtual void VSetPosition(uint32_t actorId, const Point& position) override;
    virtual Point VGetPosition(uint32_t actorId) override;

    virtual SDL_Rect VGetAABB(uint32_t actorId, bool discardSensors) override;
    virtual bool VIsActorOverlap(uint32_t actorId, FixtureType overlapType) override;

    virtual RaycastResult VRayCast(const Point& fromPoint, const Point& toPoint, uint32 filterMask) override;

    virtual void VScaleActor(uint32_t actorId, double scale) override;

private:
    b2Body* FindBox2DBody(uint32 actorId);
    uint32 FindActorId(b2Body* pBody);
    void ScheduleActorForRemoval(uint32 actorId) { m_ActorsToBeDestroyed.push_back(actorId); }
    void AddActorFixtureToBody(b2Body* pBody, const ActorFixtureDef* pFixtureDef);
    
    unique_ptr<b2World> m_pWorld;
    unique_ptr<PhysicsDebugDrawer> m_pDebugDrawer;
    unique_ptr<PhysicsContactListener> m_pPhysicsContactListener;

    b2Body* m_pTiles;

    std::vector<uint32> m_ActorsToBeDestroyed;
    std::vector<const ActorBodyDef*> m_ActorBodiesToBeCreated;
    std::vector<std::pair<uint32, const ActorFixtureDef*>> m_FixturesToBeCreated;
    std::vector<std::pair<uint32, const Point>> m_DeferredAppliedForce;
    
    ActorIDToBox2DBodyMap m_ActorToBodyMap;
    Box2DBodyToActorIDMap m_BodyToActorMap;
    ActorIdAndBodyList m_ActorIdAndBodyList;
};

class KinematicComponent;
class PhysicsComponent;
class TriggerComponent;
class ProjectileAIComponent;
class PathElevatorComponent;

extern shared_ptr<PhysicsComponent> GetPhysicsComponentFromB2Body(const b2Body* pBody);
extern shared_ptr<KinematicComponent> GetKinematicComponentFromB2Body(const b2Body* pBody);
extern shared_ptr<PathElevatorComponent> GetPathElevatorComponentFromB2Body(const b2Body* pBody);
extern shared_ptr<TriggerComponent> GetTriggerComponentFromB2Body(const b2Body* pBody);
extern shared_ptr<ProjectileAIComponent> GetProjectileAIComponentFromB2Body(const b2Body* pBody);
extern b2AABB GetBodyAABB(b2Body* pBody, bool discardSensors);
extern b2Fixture* GetLowermostFixture(b2Body* pBody, bool discardSensors = true);

extern IGamePhysics* CreateClawPhysics();

const float METERS_TO_PIXELS = 75.0f;

inline b2Vec2 PixelsToMeters(const b2Vec2& pixels)
{
    return b2Vec2(pixels.x / METERS_TO_PIXELS, pixels.y / METERS_TO_PIXELS);
}

inline float PixelsToMeters(float pixels)
{
    return pixels / METERS_TO_PIXELS;
}

inline b2Vec2 MetersToPixels(b2Vec2 meters)
{
    return b2Vec2(meters.x * METERS_TO_PIXELS, meters.y * METERS_TO_PIXELS);
}

inline float MetersToPixels(float pixels)
{
    return pixels * METERS_TO_PIXELS;
}

inline Point b2Vec2ToPoint(const b2Vec2& vec)
{
    return Point(vec.x, vec.y);
}

inline b2Vec2 PointToB2Vec2(const Point& point)
{
    return b2Vec2((float)point.x, (float)point.y);
}

//-------------------------------------------------------------------------------------------------
// NullPhysics - everything is stubbed out
//-------------------------------------------------------------------------------------------------

class NullPhysics : public IGamePhysics
{
public:
    NullPhysics() { }
    virtual ~NullPhysics() { }

    // Initialization and maintanance of the Physics World
    virtual bool VInitialize() override { return true; }
    virtual void VSyncVisibleScene() override { }
    virtual void VOnUpdate(const uint32_t msDiff) override { }

    // Initialization of Physics Objects
    virtual void VAddCircle(float radius, uint32_t thickness, WeakActorPtr pTargetActor) override { }
    virtual void VAddRect(uint32_t thickness, WeakActorPtr pTargetActor) override { }
    virtual void VAddLine(Point from, Point to, uint32_t thickness) override { }

    virtual void VAddStaticGeometry(const Point& position, const Point& size, CollisionType collisionType, FixtureType fixtureType) override { }
    virtual void VAddDynamicActor(WeakActorPtr pActor) override { }
    virtual void VAddKinematicBody(WeakActorPtr pActor) override { }
    virtual void VAddStaticBody(WeakActorPtr pActor, const Point& bodySize, CollisionType collisionType) override { }
    virtual void VRemoveActor(uint32_t actorId) override { }

    virtual void VAddActorBody(const ActorBodyDef* actorBodyDef) override { }
    virtual void VAddActorFixtureToBody(uint32_t actorId, const ActorFixtureDef* pFixtureDef) override { }

    // Debugging
    virtual void VRenderDiagnostics(SDL_Renderer* pRenderer, shared_ptr<CameraNode> pCamera) override { }

    // Physics world modifiers
    virtual void VCreateTrigger(WeakActorPtr pActor, const Point& pos, Point& size, bool isStatic) override { }
    virtual void VApplyForce(uint32_t actorId, const Point& force) override { }
    virtual void VApplyLinearImpulse(uint32_t actorId, const Point& impulse) override { }
    virtual bool VKinematicMove(const Point& pos, uint32_t actorId) override { return false; }

    virtual Point GetGravity() const override { return Point(); }

    // Physics actor states
    virtual void VStopActor(uint32_t actorId) override { }
    virtual Point VGetVelocity(uint32_t actorId) override { return Point(); }
    virtual void SetVelocity(uint32_t actorId, const Point& velocity) override { }
    virtual void VTranslate(uint32_t actorId, const Point& dir) override { }
    virtual void VAddLinearSpeed(uint32_t actorId, const Point& speedIncrement) override { }
    virtual void VSetLinearSpeed(uint32_t actorId, const Point& speed) override { }
    virtual void VSetGravityScale(uint32_t actorId, const float gravityScale) override { }
    virtual void VSetLinearSpeedEx(uint32_t actorId, const Point& speed) override { }
    virtual bool VIsAwake(uint32_t actorId) override { return false; }

    virtual void VChangeCollisionFlag(uint32_t actorId, uint32 fromFlag, uint32 toFlag) override { }

    virtual void VActivate(uint32_t actorId) override { }
    virtual void VDeactivate(uint32_t actorId) override { }

    virtual void VSetPosition(uint32_t actorId, const Point& position) override { }
    virtual Point VGetPosition(uint32_t actorId) override { return Point(); }

    virtual SDL_Rect VGetAABB(uint32_t actorId, bool discardSensors) override  { return{ 0, 0, 0, 0 }; }
    virtual bool VIsActorOverlap(uint32_t actorId, FixtureType overlapType) { return false; }

    virtual RaycastResult VRayCast(const Point& fromPoint, const Point& toPoint, uint32 filterMask) override { return RaycastResult(); }

    virtual void VScaleActor(uint32_t actorId, double scale) override { }
};

inline IGamePhysics* CreateNullPhysics()
{
    return new NullPhysics;
}

#endif
