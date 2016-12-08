#include "ClawPhysics.h"
#include "../Scene/SceneNodes.h"
#include "../Events/EventMgr.h"
#include "../Events/Events.h"
#include "../GameApp/BaseGameApp.h"
#include "../GameApp/BaseGameLogic.h"

#include "../Actor/Components/PositionComponent.h"
#include "../Actor/Components/CollisionComponent.h"
#include "../Actor/Components/PhysicsComponent.h"
#include "../Actor/Components/KinematicComponent.h"
#include "../Actor/Components/TriggerComponents/TriggerComponent.h"
#include "../Actor/Components/AIComponents/ProjectileAIComponent.h"

#include "PhysicsDebugDrawer.h"
#include "PhysicsContactListener.h"
#include "../UserInterface/HumanView.h"

//=====================================================================================================================
// ClawPhysics implementation
//=====================================================================================================================

const float METERS_TO_PIXELS = 75.0f;

b2Vec2 PixelsToMeters(b2Vec2& pixels)
{
    return b2Vec2(pixels.x / METERS_TO_PIXELS, pixels.y / METERS_TO_PIXELS);
}

float PixelsToMeters(float pixels)
{
    return pixels / METERS_TO_PIXELS;
}

b2Vec2 MetersToPixels(b2Vec2 meters)
{
    return b2Vec2(meters.x * METERS_TO_PIXELS, meters.y * METERS_TO_PIXELS);
}

float MetersToPixels(float pixels)
{
    return pixels * METERS_TO_PIXELS;
}

Point b2Vec2ToPoint(b2Vec2& vec)
{
    return Point(vec.x, vec.y);
}

b2Vec2 PointToB2Vec2(const Point& point)
{
    return b2Vec2((float)point.x, (float)point.y);
}

shared_ptr<PhysicsComponent> GetPhysicsComponentFromB2Body(const b2Body* pBody)
{
    assert(pBody);

    Actor* pActor = static_cast<Actor*>(pBody->GetUserData());
    assert(pActor);

    shared_ptr<PhysicsComponent> pPhysicsComponent =
        MakeStrongPtr(pActor->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
    assert(pPhysicsComponent);

    return pPhysicsComponent;
}

shared_ptr<KinematicComponent> GetKinematicComponentFromB2Body(const b2Body* pBody)
{
    assert(pBody);

    Actor* pActor = static_cast<Actor*>(pBody->GetUserData());
    assert(pActor);

    shared_ptr<KinematicComponent> pKinematicComponent =
        MakeStrongPtr(pActor->GetComponent<KinematicComponent>(KinematicComponent::g_Name));
    assert(pKinematicComponent);

    return pKinematicComponent;
}

shared_ptr<TriggerComponent> GetTriggerComponentFromB2Body(const b2Body* pBody)
{
    assert(pBody);

    Actor* pActor = static_cast<Actor*>(pBody->GetUserData());
    assert(pActor);

    // May no longer be valid, calling methods have to check it
    shared_ptr<TriggerComponent> pTriggerComponent =
        MakeStrongPtr(pActor->GetComponent<TriggerComponent>(TriggerComponent::g_Name));

    return pTriggerComponent;
}

shared_ptr<ProjectileAIComponent> GetProjectileAIComponentFromB2Body(const b2Body* pBody)
{
    assert(pBody);

    Actor* pActor = static_cast<Actor*>(pBody->GetUserData());
    assert(pActor);

    // May no longer be valid, calling methods have to check it
    shared_ptr<ProjectileAIComponent> pComponent =
        MakeStrongPtr(pActor->GetComponent<ProjectileAIComponent>(ProjectileAIComponent::g_Name));

    return pComponent;
}

b2AABB GetBodyAABB(b2Body* pBody)
{
    b2AABB aabb;
    aabb.lowerBound = b2Vec2(FLT_MAX, FLT_MAX);
    aabb.upperBound = b2Vec2(-FLT_MAX, -FLT_MAX);
    b2Fixture* fixture = pBody->GetFixtureList();
    while (fixture != NULL)
    {
        if (fixture->IsSensor())
        {
            fixture = fixture->GetNext();
            continue;
        }
        aabb.Combine(aabb, fixture->GetAABB(0));
        fixture = fixture->GetNext();
    }

    return aabb;
}

//-----------------------------------------------------------------------------
// ClawPhysics::ClawPhysics
//
//    Constructor. This only sets default values.
//
ClawPhysics::ClawPhysics()
{

}

//-----------------------------------------------------------------------------
// ClawPhysics::~ClawPhysics
//
//    Destructor. Releases all allocated resources
//
ClawPhysics::~ClawPhysics()
{

}

//-----------------------------------------------------------------------------
// ClawPhysics::VInitialize
//
//    Initializes ClawPhysics subsystem with respect to loaded config options.
//
bool ClawPhysics::VInitialize()
{
    b2Vec2 gravity(0, 9.8f);
    m_pWorld.reset(new b2World(gravity));
    m_pDebugDrawer.reset(new PhysicsDebugDrawer());

    // Create debug drawer
    m_pWorld->SetDebugDraw(m_pDebugDrawer.get());

    m_pPhysicsContactListener.reset(new PhysicsContactListener);
    m_pWorld->SetContactListener(m_pPhysicsContactListener.get());

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    m_pTiles = m_pWorld->CreateBody(&bodyDef);

    return true;
}

//-----------------------------------------------------------------------------
// ClawPhysics::VSyncVisibleScene
//
//    Keeps graphics and physics in sync
//
void ClawPhysics::VSyncVisibleScene()
{
    // check all the existing actor's bodies for changes. 
    //  If there is a change, send the appropriate event for the game system.
    for (ActorIDToBox2DBodyMap::const_iterator it = m_ActorToBodyMap.begin();
        it != m_ActorToBodyMap.end();
        ++it)
    {
        uint32 actorId = it->first;
        b2Body* pActorBody = it->second;
        assert(pActorBody);

        StrongActorPtr pGameActor = MakeStrongPtr(g_pApp->GetGameLogic()->VGetActor(actorId));
        if (pGameActor && pActorBody)
        {
            shared_ptr<PositionComponent> pPositionComponent = MakeStrongPtr(pGameActor->GetComponent<PositionComponent>(PositionComponent::g_Name));

            Point bodyPixelPosition = b2Vec2ToPoint(MetersToPixels(pActorBody->GetPosition()));
            Point actorPixelPosition = pPositionComponent->GetPosition();
            if (pPositionComponent)
            {
                if (pActorBody->GetType() == b2_staticBody)
                {
                    continue;
                }

                if (pActorBody->GetType() == b2_dynamicBody)
                {
                    shared_ptr<PhysicsComponent> pPhysicsComponent = GetPhysicsComponentFromB2Body(pActorBody);
                    bool wasFalling = pPhysicsComponent->IsFalling();
                    bool wasJumping = pPhysicsComponent->IsJumping();
                    // Set jumping / falling properties
                    //if ((uint32)bodyPixelPosition.y != (uint32)actorPixelPosition.y)
                    if (fabs(bodyPixelPosition.y - actorPixelPosition.y) > DBL_EPSILON)
                    {
                        // He might be on platform
                        if (pPhysicsComponent->IsOnGround())
                        {
                            //LOG("GROUNDED");
                            pPhysicsComponent->SetFalling(false);
                            pPhysicsComponent->SetJumping(false);
                        }
                        // Falling
                        else if ((bodyPixelPosition.y - actorPixelPosition.y) > DBL_EPSILON)
                        {
                            pPhysicsComponent->SetFalling(true);
                            pPhysicsComponent->SetJumping(false);
                        }
                        else // Jumping
                        {
                            pPhysicsComponent->SetFalling(false);
                            pPhysicsComponent->SetJumping(true);
                            pPhysicsComponent->AddJumpHeight(fabs(bodyPixelPosition.y - actorPixelPosition.y));
                        }
                    }
                    else if (fabs(bodyPixelPosition.y - actorPixelPosition.y) < DBL_EPSILON)
                    {
                        // TODO: Check this. This causes animation glitches on connected ground platforms
                        // Should be fixed when all consecutive tiles are joined, but anyway, keep this in mind
                        if (!pPhysicsComponent->IsOnGround() && (bodyPixelPosition.y - actorPixelPosition.y) > DBL_EPSILON)
                        {
                            //LOG("!pPhysicsComponent->IsOnGround()");
                            pPhysicsComponent->SetFalling(true);
                        }
                        else
                        {
                            pPhysicsComponent->SetFalling(false);
                        }
                        pPhysicsComponent->SetJumping(false);
                    }

                    // Notify change of states
                    if (!wasFalling && pPhysicsComponent->IsFalling())
                    {
                        //LOG("----" + ToStr(bodyPixelPosition.y - actorPixelPosition.y));
                        pPhysicsComponent->OnStartFalling();
                    }
                    if (!wasJumping && pPhysicsComponent->IsJumping())
                    {
                        //LOG(ToStr(bodyPixelPosition.y - actorPixelPosition.y));
                        pPhysicsComponent->OnStartJumping();
                    }
                }

                /*if (((uint32)bodyPixelPosition.x != (uint32)actorPixelPosition.x) ||
                ((uint32)bodyPixelPosition.y != (uint32)actorPixelPosition.y))*/
                if ((fabs(bodyPixelPosition.x - actorPixelPosition.x)) > DBL_EPSILON ||
                    (fabs(bodyPixelPosition.y - actorPixelPosition.y)) > DBL_EPSILON)
                {
                    // Box2D has moved the physics object. Update actor's position and notify subsystems which care
                    pPositionComponent->SetPosition(bodyPixelPosition);
                    shared_ptr<EventData_Move_Actor> pEvent(new EventData_Move_Actor(actorId, bodyPixelPosition));
                    IEventMgr::Get()->VTriggerEvent(pEvent);

                    // If it is kinematic body (moving platform, elevator), notify it
                    if (pActorBody->GetType() == b2_kinematicBody)
                    {
                        shared_ptr<KinematicComponent> pKinematicComponent = GetKinematicComponentFromB2Body(pActorBody);
                        pKinematicComponent->RemoveCarriedBody(pActorBody);
                        pKinematicComponent->OnMoved(bodyPixelPosition);
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// ClawPhysics::VOnUpdate
//
//    Updates physics.
//
void ClawPhysics::VOnUpdate(const uint32 msDiff)
{
    //PROFILE_CPU("ClawPhysics::VOnUpdate");

    m_pWorld->Step(msDiff / 1000.0f, 8, 3);

    // Remove actors form physics simulation which are scheduled to be destroyed
    for (uint32 actorId : m_ActorsToBeDestroyed)
    {
        if (b2Body* pBody = FindBox2DBody(actorId))
        {
            pBody->SetActive(false);
            pBody->SetUserData(NULL);
            m_pWorld->DestroyBody(pBody);
            m_ActorToBodyMap.erase(actorId);
            m_BodyToActorMap.erase(pBody);
        }
    }
    m_ActorsToBeDestroyed.clear();

    // Create any pending actors
    for (const ActorBodyDef* pActorBodyDef : m_ActorBodiesToBeCreated)
    {
        VAddActorBody(pActorBodyDef);
    }
    m_ActorBodiesToBeCreated.clear();
}

//-----------------------------------------------------------------------------
// ClawPhysics::VAddCircle
//
//    NOT IMPLEMENTED
//
void ClawPhysics::VAddCircle(float radius, uint32_t thickness, WeakActorPtr pTargetActor)
{
    assert(false && "Not implemented.");
}

//-----------------------------------------------------------------------------
// ClawPhysics::VAddRect
//
//    NOT IMPLEMENTED
//
void ClawPhysics::VAddRect(uint32_t thickness, WeakActorPtr pTargetActor)
{
    assert(false && "Not implemented.");
}

//-----------------------------------------------------------------------------
// ClawPhysics::VAddLine
//
//    NOT IMPLEMENTED
//
void ClawPhysics::VAddLine(Point from, Point to, uint32_t thickness)
{
    assert(false && "Not implemented.");
}

//-----------------------------------------------------------------------------
// ClawPhysics::VAddStaticGeometry
//
//    Adds static geometry to physics world, like ladder or solid wall.
//
void ClawPhysics::VAddStaticGeometry(Point position, Point size, CollisionType collisionType)
{
    if (collisionType == CollisionType_None)
    {
        return;
    }

    // Convert pixel position and size to Box2D meters
    b2Vec2 b2Position = PixelsToMeters(PointToB2Vec2(position));
    b2Vec2 b2Size = PixelsToMeters(PointToB2Vec2(size));

    b2PolygonShape bodyShape;
    bodyShape.SetAsBox(b2Size.x / 2, b2Size.y / 2, 
        b2Vec2(b2Position.x + b2Size.x / 2, b2Position.y + b2Size.y / 2), 0);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &bodyShape;
    fixtureDef.friction = 0.0f;

    // Assign static geometry type (= tile type)
    if (collisionType == CollisionType_Solid) 
    { 
        fixtureDef.filter.categoryBits = CollisionFlag_Solid;
        fixtureDef.userData = (void*)FixtureType_Solid; 
        fixtureDef.friction = 0.18f;
    }
    else if (collisionType == CollisionType_Ground) 
    { 
        fixtureDef.filter.categoryBits = CollisionFlag_Ground;
        fixtureDef.userData = (void*)FixtureType_Ground; 
        fixtureDef.friction = 0.18f;
    }
    else if (collisionType == CollisionType_Climb) 
    { 
        fixtureDef.filter.categoryBits = CollisionFlag_Ladder;
        fixtureDef.userData = (void*)FixtureType_Climb; 
    }
    else if (collisionType == CollisionType_Death) 
    { 
        fixtureDef.filter.categoryBits = CollisionFlag_Death;
        fixtureDef.userData = (void*)FixtureType_Death; 
    }
    else 
    { 
        fixtureDef.userData = (void*)FixtureType_None; 
    }

    if ((int)fixtureDef.userData != FixtureType_Solid) { fixtureDef.isSensor = true; }

    m_pTiles->CreateFixture(&fixtureDef);
}

void ClawPhysics::VAddDynamicActor(WeakActorPtr pActor)
{
    //LOG("Creating dynamic actor");

    StrongActorPtr pStrongActor = MakeStrongPtr(pActor);
    if (!pStrongActor)
    {
        return;
    }

    shared_ptr<PositionComponent> pPositionComponent =
        MakeStrongPtr(pStrongActor->GetComponent<PositionComponent>(PositionComponent::g_Name));
    if (!pPositionComponent)
    {
        return;
    }
    Point position = pPositionComponent->GetPosition();

    shared_ptr<PhysicsComponent> pPhysicsComponent =
        MakeStrongPtr(pStrongActor->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
    if (!pPhysicsComponent)
    {
        return;
    }
    Point bodySize = pPhysicsComponent->GetBodySize();

    // Convert pixel position and size to Box2D meters
    b2Vec2 b2Position = PixelsToMeters(PointToB2Vec2(position));
    b2Vec2 b2BodySize = PixelsToMeters(PointToB2Vec2(bodySize));

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(b2Position.x, b2Position.y);
    bodyDef.fixedRotation = true;
    b2Body* pBody = m_pWorld->CreateBody(&bodyDef);
    pBody->SetUserData(pStrongActor.get());
    pBody->SetGravityScale(pPhysicsComponent->GetGravityScale());

    b2CircleShape bodyShape;
    bodyShape.m_p.Set(0, b2BodySize.x / 2 - b2BodySize.y / 2);
    bodyShape.m_radius = b2BodySize.x / 2;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &bodyShape;
    fixtureDef.density = pPhysicsComponent->GetDensity();
    fixtureDef.friction = pPhysicsComponent->GetFriction();
    pBody->CreateFixture(&fixtureDef);

    bodyShape.m_p.Set(0, b2BodySize.y / 2 - b2BodySize.x / 2);
    fixtureDef.shape = &bodyShape;
    //fixtureDef.friction = 100.0;
    pBody->CreateFixture(&fixtureDef);

    b2PolygonShape polygonShape;
    polygonShape.SetAsBox((b2BodySize.x / 2) - PixelsToMeters(2), (b2BodySize.y - b2BodySize.x) / 2);
    fixtureDef.shape = &polygonShape;
    pBody->CreateFixture(&fixtureDef);

    // Add foot sensor
    float sensorHeight = PixelsToMeters(24);
    polygonShape.SetAsBox(b2BodySize.x / 2 - PixelsToMeters(2), sensorHeight / 2, b2Vec2(0, b2BodySize.y / 2), 0);
    fixtureDef.shape = &polygonShape;
    fixtureDef.isSensor = true;
    fixtureDef.userData = (void*)FixtureType_FootSensor;
    pBody->CreateFixture(&fixtureDef);

    m_ActorToBodyMap.insert(std::make_pair(pStrongActor->GetGUID(), pBody));
    m_BodyToActorMap.insert(std::make_pair(pBody, pStrongActor->GetGUID()));
}

//-----------------------------------------------------------------------------
// ClawPhysics::VAddKinematicBody
//
//    Adds kinematic body to world.
//
void ClawPhysics::VAddKinematicBody(WeakActorPtr pActor)
{
    //LOG("Creating kinematic actor");

    StrongActorPtr pStrongActor = MakeStrongPtr(pActor);
    if (!pStrongActor)
    {
        return;
    }

    shared_ptr<PositionComponent> pPositionComponent =
        MakeStrongPtr(pStrongActor->GetComponent<PositionComponent>(PositionComponent::g_Name));
    if (!pPositionComponent)
    {
        LOG_ERROR("Attempting to create kinematic body without position component");
        return;
    }
    Point position = pPositionComponent->GetPosition();

    shared_ptr<KinematicComponent> pKinematicComponent =
        MakeStrongPtr(pStrongActor->GetComponent<KinematicComponent>(KinematicComponent::g_Name));
    if (!pKinematicComponent)
    {
        LOG_ERROR("Attempting to create kinematic body without kinematic component");
        return;
    }
    Point bodySize = pKinematicComponent->GetSize();

    // Convert pixel position and size to Box2D meters
    b2Vec2 b2Position = PixelsToMeters(PointToB2Vec2(position));
    b2Vec2 b2BodySize = PixelsToMeters(PointToB2Vec2(bodySize));

    b2BodyDef bodyDef;
    bodyDef.type = b2_kinematicBody;
    bodyDef.position.Set(b2Position.x, b2Position.y);
    bodyDef.fixedRotation = true;
    b2Body* pBody = m_pWorld->CreateBody(&bodyDef);
    pBody->SetUserData(pStrongActor.get());

    b2PolygonShape bodyShape;
    bodyShape.SetAsBox(b2BodySize.x / 2, b2BodySize.y / 2);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &bodyShape;
    fixtureDef.friction = 0.0f;
    fixtureDef.userData = (void*)FixtureType_Ground;
    fixtureDef.isSensor = true;
    pBody->CreateFixture(&fixtureDef);

    m_ActorToBodyMap.insert(std::make_pair(pStrongActor->GetGUID(), pBody));
    m_BodyToActorMap.insert(std::make_pair(pBody, pStrongActor->GetGUID()));
}

void ClawPhysics::VAddStaticBody(WeakActorPtr pActor, Point bodySize, CollisionType collisionType)
{
    //LOG("Creating static actor");

    StrongActorPtr pStrongActor = MakeStrongPtr(pActor);
    if (!pStrongActor)
    {
        return;
    }

    shared_ptr<PositionComponent> pPositionComponent =
        MakeStrongPtr(pStrongActor->GetComponent<PositionComponent>(PositionComponent::g_Name));
    if (!pPositionComponent)
    {
        LOG_ERROR("Attempting to create static body without position component");
        return;
    }
    Point position = pPositionComponent->GetPosition();

    // Convert pixel position and size to Box2D meters
    b2Vec2 b2Position = PixelsToMeters(PointToB2Vec2(position));
    b2Vec2 b2BodySize = PixelsToMeters(PointToB2Vec2(bodySize));

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(b2Position.x, b2Position.y);
    bodyDef.fixedRotation = true;
    b2Body* pBody = m_pWorld->CreateBody(&bodyDef);
    pBody->SetUserData(pStrongActor.get());

    b2PolygonShape bodyShape;
    bodyShape.SetAsBox(b2BodySize.x / 2, b2BodySize.y / 2);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &bodyShape;
    fixtureDef.friction = 0.0f;
    fixtureDef.userData = (void*)collisionType;
    fixtureDef.isSensor = true;
    pBody->CreateFixture(&fixtureDef);

    m_ActorToBodyMap.insert(std::make_pair(pStrongActor->GetGUID(), pBody));
    m_BodyToActorMap.insert(std::make_pair(pBody, pStrongActor->GetGUID()));
}

void ClawPhysics::VAddActorBody(const ActorBodyDef* actorBodyDef)
{
    assert(actorBodyDef->collisionMask != 0x0);
    assert(actorBodyDef->collisionFlag != 0x0);
    assert(actorBodyDef->fixtureType != FixtureType_None);

    //LOG("Creating actor body..");

    StrongActorPtr pStrongActor = MakeStrongPtr(actorBodyDef->pActor);
    if (!pStrongActor)
    {
        assert(false && "Supplied invalid actor to create physics body from");
        return;
    }

    // Cannot add bodies while the world is stepping, we need to schedule it
    if (m_pWorld->IsLocked())
    {
        m_ActorBodiesToBeCreated.push_back(actorBodyDef);
        return;
    }

    // Convert pixel position and size to Box2D meters
    b2Vec2 b2Position = PixelsToMeters(PointToB2Vec2(actorBodyDef->position));
    b2Vec2 b2BodySize = PixelsToMeters(PointToB2Vec2(actorBodyDef->size));

    b2BodyDef bodyDef;
    bodyDef.type = actorBodyDef->bodyType;
    bodyDef.position.Set(b2Position.x, b2Position.y);
    bodyDef.fixedRotation = true;
    b2Body* pBody = m_pWorld->CreateBody(&bodyDef);
    assert(pBody != NULL && "Failed to create Box2D body.  Is the world currently stepping ?");
    pBody->SetUserData(pStrongActor.get());
    pBody->SetBullet(actorBodyDef->makeBullet);
    pBody->SetGravityScale(actorBodyDef->gravityScale);

    if (actorBodyDef->makeCapsule)
    {
        assert((b2BodySize.x < b2BodySize.y) && "Making capsule shape for body with width bigger than height not yet supported.");

        b2CircleShape bodyShape;
        bodyShape.m_p.Set(0, b2BodySize.x / 2 - b2BodySize.y / 2);
        bodyShape.m_radius = b2BodySize.x / 2;

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &bodyShape;
        fixtureDef.density = actorBodyDef->density;
        fixtureDef.friction = actorBodyDef->friction;
        fixtureDef.restitution = actorBodyDef->restitution;
        fixtureDef.isSensor = actorBodyDef->makeSensor;
        pBody->CreateFixture(&fixtureDef);

        bodyShape.m_p.Set(0, b2BodySize.y / 2 - b2BodySize.x / 2);
        fixtureDef.shape = &bodyShape;
        pBody->CreateFixture(&fixtureDef);

        b2PolygonShape polygonShape;
        polygonShape.SetAsBox((b2BodySize.x / 2) - PixelsToMeters(2), (b2BodySize.y - b2BodySize.x) / 2);
        fixtureDef.shape = &polygonShape;
        pBody->CreateFixture(&fixtureDef);
    }
    else
    {
        b2PolygonShape bodyShape;
        bodyShape.SetAsBox(b2BodySize.x / 2, b2BodySize.y / 2);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &bodyShape;
        fixtureDef.friction = actorBodyDef->friction;
        fixtureDef.density = actorBodyDef->density;
        fixtureDef.restitution = actorBodyDef->restitution;
        fixtureDef.userData = (void*)actorBodyDef->fixtureType;
        fixtureDef.isSensor = actorBodyDef->makeSensor;
        fixtureDef.filter.categoryBits = actorBodyDef->collisionFlag;
        fixtureDef.filter.maskBits = actorBodyDef->collisionMask;
        pBody->CreateFixture(&fixtureDef);
    }

    if (actorBodyDef->addFootSensor)
    {
        // Add foot sensor
        b2PolygonShape polygonShape;
        b2FixtureDef fixtureDef;

        float sensorHeight = PixelsToMeters(24);
        polygonShape.SetAsBox(b2BodySize.x / 2 - PixelsToMeters(2), sensorHeight / 2, b2Vec2(0, b2BodySize.y / 2), 0);
        fixtureDef.shape = &polygonShape;
        fixtureDef.isSensor = true;
        fixtureDef.userData = (void*)FixtureType_FootSensor;
        pBody->CreateFixture(&fixtureDef);
    }

    m_ActorToBodyMap.insert(std::make_pair(pStrongActor->GetGUID(), pBody));
    m_BodyToActorMap.insert(std::make_pair(pBody, pStrongActor->GetGUID()));

    if (actorBodyDef->setInitialSpeed)
    {
        VSetLinearSpeed(pStrongActor->GetGUID(), actorBodyDef->initialSpeed);
    }
    else if (actorBodyDef->setInitialImpulse)
    {
        VApplyLinearImpulse(pStrongActor->GetGUID(), actorBodyDef->initialSpeed);
    }
}

//-----------------------------------------------------------------------------
// ClawPhysics::VRemoveActor
//
//    Removes actor from physics world.
//
void ClawPhysics::VRemoveActor(uint32_t actorId)
{
    ScheduleActorForRemoval(actorId);
}

//-----------------------------------------------------------------------------
// ClawPhysics::VRenderDiagnostics
//
//    Renders contact points, lines etc. for debugging purposes.
//        This (should be) configurable.
//
void ClawPhysics::VRenderDiagnostics(SDL_Renderer* pRenderer, shared_ptr<CameraNode> pCamera)
{
    //return;
    //PROFILE_CPU("Render diagnostics");
    m_pDebugDrawer->PrepareForDraw(pRenderer, pCamera);

    // Set camera bounds, dont render everything, only relevant stuff on visible scene by camera
    // 15ms -> 0.3ms performance boost per frame on level 1
    b2AABB aabb;
    Point cameraPos = pCamera->GetPosition();

    // Convert pixel coordinates to meters
    b2Vec2 b2LowerBound = PixelsToMeters(PointToB2Vec2(cameraPos));
    b2Vec2 b2UpperBound = PixelsToMeters(PointToB2Vec2(Point(cameraPos.x + pCamera->GetWidth(), cameraPos.y + pCamera->GetHeight())));

    aabb.lowerBound = b2LowerBound; //b2Vec2(cameraPos.x, cameraPos.y);
    aabb.upperBound = b2UpperBound; //b2Vec2(cameraPos.x + pCamera->GetWidth(), cameraPos.y + pCamera->GetHeight());
    m_pWorld->DrawDebugDataInAABB(aabb);
}

//-----------------------------------------------------------------------------
// ClawPhysics::VCreateTrigger
//
//    Creates a trigger from given actor.
//
void ClawPhysics::VCreateTrigger(WeakActorPtr pActor, const Point& pos, Point& size, bool isStatic)
{
    StrongActorPtr pStrongActor = MakeStrongPtr(pActor);
    if (!pStrongActor)
    {
        return;
    }

    // Convert pixel position and size to Box2D meters
    b2Vec2 b2Position = PixelsToMeters(PointToB2Vec2(pos));
    b2Vec2 b2BodySize = PixelsToMeters(PointToB2Vec2(size));

    b2BodyDef bodyDef;
    if (isStatic)
    {
        bodyDef.type = b2_staticBody;
    }
    else
    {
        bodyDef.type = b2_dynamicBody;
    }
    bodyDef.position.Set(b2Position.x, b2Position.y);
    bodyDef.fixedRotation = true;
    b2Body* pBody = m_pWorld->CreateBody(&bodyDef);
    pBody->SetUserData(pStrongActor.get());

    b2PolygonShape bodyShape;
    bodyShape.SetAsBox(b2BodySize.x / 2, b2BodySize.y / 2);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &bodyShape;
    fixtureDef.userData = (void*)FixtureType_Trigger;
    fixtureDef.isSensor = true;
    pBody->CreateFixture(&fixtureDef);

    m_ActorToBodyMap.insert(std::make_pair(pStrongActor->GetGUID(), pBody));
    m_BodyToActorMap.insert(std::make_pair(pBody, pStrongActor->GetGUID()));
}

//-----------------------------------------------------------------------------
// ClawPhysics::VApplyForce
//
//    Applies force to given actor.
//
void ClawPhysics::VApplyForce(uint32_t actorId, const Point& force)
{
    if (b2Body* pBody = FindBox2DBody(actorId))
    {
        pBody->ApplyForceToCenter(PointToB2Vec2(force), true);
    }
}

//-----------------------------------------------------------------------------
// ClawPhysics::VApplyLinearImpulse
//
//    Applies linear impulse to given actor.
//
void ClawPhysics::VApplyLinearImpulse(uint32_t actorId, const Point& impulse)
{
    if (b2Body* pBody = FindBox2DBody(actorId))
    {
        pBody->ApplyLinearImpulseToCenter(PointToB2Vec2(impulse), true);
    }
}

//-----------------------------------------------------------------------------
// ClawPhysics::VKinematicMove
//
//    Forces position change to given actor.
//
bool ClawPhysics::VKinematicMove(const Point& pos, uint32_t actorId)
{
    return true;
}

//-----------------------------------------------------------------------------
// ClawPhysics::VStopActor
//
//    Stops given actor's movement.
//
void ClawPhysics::VStopActor(uint32_t actorId)
{

}

//-----------------------------------------------------------------------------
// ClawPhysics::VGetVelocity
//
//    Gets velocity of given actor.
//
Point ClawPhysics::VGetVelocity(uint32_t actorId)
{
    if (b2Body* pActorBody = FindBox2DBody(actorId))
    {
        b2Vec2 velocity = pActorBody->GetLinearVelocity();
        return b2Vec2ToPoint(velocity);
    }

    return Point();
}

//-----------------------------------------------------------------------------
// ClawPhysics::SetVelocity
//
//    Sets velocity to given actor
//
void ClawPhysics::SetVelocity(uint32_t actorId, const Point& velocity)
{

}

//-----------------------------------------------------------------------------
// ClawPhysics::VTranslate
//
//    Attempts to move actor in given direction.
//
void ClawPhysics::VTranslate(uint32_t actorId, const Point& dir)
{

}

//-----------------------------------------------------------------------------
// ClawPhysics::VSetLinearSpeed
//
//    Assigns linear speed to actor
//
void ClawPhysics::VSetLinearSpeed(uint32_t actorId, const Point& speed)
{
    if (b2Body* pBody = FindBox2DBody(actorId))
    {
        pBody->SetLinearVelocity(b2Vec2(speed.x, speed.y));
        /*shared_ptr<PhysicsComponent> pPhysicsComponent = GetPhysicsComponentFromB2Body(pBody);
        if (pPhysicsComponent->HasConstantSpeed())
        {
            pBody->SetLinearVelocity(PointToB2Vec2(speed));
        }
        else
        {
            b2Vec2 grav = pBody->GetLinearVelocity();
            double ySpeed = speed.y;
            if (speed.y < 0)
            {
                pBody->SetLinearVelocity(b2Vec2(grav.x, -8.8));
            }
            else if (grav.y < -2 && speed.y >= 0)
            {
                pBody->SetLinearVelocity(b2Vec2(grav.x, -2));
            }
            grav = pBody->GetLinearVelocity();
            //pBody->SetLinearVelocity(b2Vec2(speed.x, speed.y));
            //pBody->SetLinearVelocity(b2Vec2(50, grav.y));
            //pBody->SetGravityScale(0.0f);
            if (fabs(speed.x) > DBL_EPSILON)
            {
                pBody->SetLinearVelocity(b2Vec2(speed.x < 0 ? -5 : 5, grav.y));
            }
            else
            {
                pBody->SetLinearVelocity(b2Vec2(0, grav.y));
            }
            pBody->ApplyForce(m_pWorld->GetGravity(), pBody->GetWorldCenter(), true);

            grav = pBody->GetLinearVelocity();
            if (grav.y < -8.8)
            {
                pBody->SetLinearVelocity(b2Vec2(grav.x, -8.8));
            }
            if (grav.y > 14)
            {
                pBody->SetLinearVelocity(b2Vec2(grav.x, 14));
            }
        }*/
    }
}

//-----------------------------------------------------------------------------
// ClawPhysics::VSetPosition
//
//    Sets position to given actor.
//
void ClawPhysics::VSetPosition(uint32_t actorId, const Point& position)
{
    b2Vec2 b2Position = PixelsToMeters(PointToB2Vec2(position));
    if (b2Body* pBody = FindBox2DBody(actorId))
    {
        pBody->SetTransform(b2Position, 0);
    }
}

//-----------------------------------------------------------------------------
// ClawPhysics::VGetPosition
//
//    Gets position of given actor.
//
Point ClawPhysics::VGetPosition(uint32_t actorId)
{
    return Point();
}

//-----------------------------------------------------------------------------
// ClawPhysics::VActivate
//
//    Activates processing of the body in Box2D world.
//
void ClawPhysics::VActivate(uint32_t actorId)
{
    if (b2Body* pBody = FindBox2DBody(actorId))
    {
        pBody->SetActive(true);
    }
}

//-----------------------------------------------------------------------------
// ClawPhysics::VDeactivate
//
//    Deactivates processing of the body in Box2D world.
//
void ClawPhysics::VDeactivate(uint32_t actorId)
{
    if (b2Body* pBody = FindBox2DBody(actorId))
    {
        pBody->SetActive(false);
    }
}

//=====================================================================================================================
// Private implementations
//=====================================================================================================================

b2Body* ClawPhysics::FindBox2DBody(uint32 actorId)
{
    ActorIDToBox2DBodyMap::const_iterator found = m_ActorToBodyMap.find(actorId);
    if (found != m_ActorToBodyMap.end())
        return found->second;

    return NULL;
}

uint32 ClawPhysics::FindActorId(b2Body* pBody)
{
    Box2DBodyToActorIDMap::const_iterator found = m_BodyToActorMap.find(pBody);
    if (found != m_BodyToActorMap.end())
        return found->second;

    return 0;
}

//=====================================================================================================================

IGamePhysics* CreateClawPhysics()
{
    std::auto_ptr<IGamePhysics> pClawPhysics;
    pClawPhysics.reset(new ClawPhysics);

    if (pClawPhysics.get() && !pClawPhysics->VInitialize())
    {
        pClawPhysics.reset();
    }

    return pClawPhysics.release();
}


void ClawPhysics::VSetLinearSpeedEx(uint32_t actorId, const Point& speed)
{
    if (b2Body* pBody = FindBox2DBody(actorId))
    {
        pBody->SetLinearVelocity(b2Vec2(speed.x, speed.y));
    }
}

void ClawPhysics::VSetGravityScale(uint32_t actorId, const float gravityScale)
{
    if (b2Body* pBody = FindBox2DBody(actorId))
    {
        pBody->SetGravityScale(gravityScale);
    }
}

Point ClawPhysics::GetGravity() const
{
    return b2Vec2ToPoint(m_pWorld->GetGravity());
}