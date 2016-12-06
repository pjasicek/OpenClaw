#ifndef __CLAWPHYSICS_H__
#define __CLAWPHYSICS_H__

#include "../Interfaces.h"
#include "../SharedDefines.h"

#include <Box2D/Box2D.h>

typedef std::map<uint32, b2Body*> ActorIDToBox2DBodyMap;
typedef std::map<b2Body*, uint32> Box2DBodyToActorIDMap;

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

    virtual void VAddStaticGeometry(Point position, Point size, CollisionType collisionType) override;
    virtual void VAddDynamicActor(WeakActorPtr pActor) override;
    virtual void VAddKinematicBody(WeakActorPtr pActor) override;
    virtual void VAddStaticBody(WeakActorPtr pActor, Point bodySize, CollisionType collisionType) override;
    virtual void VRemoveActor(uint32_t actorId) override;

    virtual void VAddActorBody(const ActorBodyDef* actorBodyDef) override;

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
    virtual void VSetLinearSpeed(uint32_t actorId, const Point& speed) override;
    virtual void VSetGravityScale(uint32_t actorId, const float gravityScale) override;
    virtual void VSetLinearSpeedEx(uint32_t actorId, const Point& speed) override;

    virtual void VActivate(uint32_t actorId) override;
    virtual void VDeactivate(uint32_t actorId) override;

    virtual void VSetPosition(uint32_t actorId, const Point& position) override;
    virtual Point VGetPosition(uint32_t actorId) override;

private:
    TiXmlDocument LoadPhysicsXmlConfig();
    b2Body* FindBox2DBody(uint32 actorId);
    uint32 FindActorId(b2Body* pBody);
    void ScheduleActorForRemoval(uint32 actorId) { m_ActorsToBeDestroyed.push_back(actorId); }
    
    unique_ptr<b2World> m_pWorld;
    unique_ptr<PhysicsDebugDrawer> m_pDebugDrawer;
    unique_ptr<PhysicsContactListener> m_pPhysicsContactListener;

    b2Body* m_pTiles;

    std::vector<uint32> m_ActorsToBeDestroyed;
    
    ActorIDToBox2DBodyMap m_ActorToBodyMap;
    Box2DBodyToActorIDMap m_BodyToActorMap;
};

class KinematicComponent;
class PhysicsComponent;
class TriggerComponent;
class ProjectileAIComponent;

extern b2Vec2 PixelsToMeters(b2Vec2& pixels);
extern float PixelsToMeters(float pixels);
extern b2Vec2 MetersToPixels(b2Vec2 meters);
extern float MetersToPixels(float pixels);
extern Point b2Vec2ToPoint(b2Vec2& vec);
extern b2Vec2 PointToB2Vec2(const Point& point);
extern shared_ptr<PhysicsComponent> GetPhysicsComponentFromB2Body(const b2Body* pBody);
extern shared_ptr<KinematicComponent> GetKinematicComponentFromB2Body(const b2Body* pBody);
extern shared_ptr<TriggerComponent> GetTriggerComponentFromB2Body(const b2Body* pBody);
extern shared_ptr<ProjectileAIComponent> GetProjectileAIComponentFromB2Body(const b2Body* pBody);
extern b2AABB GetBodyAABB(b2Body* pBody);

extern IGamePhysics* CreateClawPhysics();

#endif