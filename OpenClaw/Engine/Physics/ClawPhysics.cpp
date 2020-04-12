#include "ClawPhysics.h"
#include "../Scene/SceneNodes.h"
#include "../Events/EventMgr.h"
#include "../Events/Events.h"
#include "../GameApp/BaseGameApp.h"
#include "../GameApp/BaseGameLogic.h"

#include "../Graphics2D/Image.h"

#include "../Actor/Components/PositionComponent.h"
#include "../Actor/Components/CollisionComponent.h"
#include "../Actor/Components/PhysicsComponent.h"
#include "../Actor/Components/KinematicComponent.h"
#include "../Actor/Components/PathElevatorComponent.h"
#include "../Actor/Components/RenderComponent.h"
#include "../Actor/Components/TriggerComponents/TriggerComponent.h"
#include "../Actor/Components/AIComponents/ProjectileAIComponent.h"

#include "PhysicsDebugDrawer.h"
#include "PhysicsContactListener.h"
#include "../UserInterface/HumanView.h"

//=====================================================================================================================
// ClawPhysics implementation
//=====================================================================================================================


shared_ptr<PhysicsComponent> GetPhysicsComponentFromB2Body(const b2Body* pBody)
{
    assert(pBody);

    Actor* pActor = static_cast<Actor*>(pBody->GetUserData());
    if (!pActor)
    {
        return nullptr;
    }

    shared_ptr<PhysicsComponent> pPhysicsComponent = pActor->GetPhysicsComponent();
    assert(pPhysicsComponent);

    return pPhysicsComponent;
}

shared_ptr<KinematicComponent> GetKinematicComponentFromB2Body(const b2Body* pBody)
{
    assert(pBody);

    Actor* pActor = static_cast<Actor*>(pBody->GetUserData());
    if (!pActor)
    {
        return nullptr;
    }

    shared_ptr<KinematicComponent> pKinematicComponent =
        MakeStrongPtr(pActor->GetComponent<KinematicComponent>(KinematicComponent::g_Name));

    return pKinematicComponent;
}

shared_ptr<PathElevatorComponent> GetPathElevatorComponentFromB2Body(const b2Body* pBody)
{
    assert(pBody);

    Actor* pActor = static_cast<Actor*>(pBody->GetUserData());
    if (!pActor)
    {
        return nullptr;
    }

    shared_ptr<PathElevatorComponent> pPathElevatorComponent =
        MakeStrongPtr(pActor->GetComponent<PathElevatorComponent>());

    return pPathElevatorComponent;
}

shared_ptr<TriggerComponent> GetTriggerComponentFromB2Body(const b2Body* pBody)
{
    assert(pBody);

    // Actor might have been destroyed
    Actor* pActor = static_cast<Actor*>(pBody->GetUserData());
    if (!pActor)
    {
        return nullptr;
    }

    auto pWeakTrigComp = pActor->GetComponent<TriggerComponent>(TriggerComponent::g_Name);
    if (!pWeakTrigComp.expired())
    {
        shared_ptr<TriggerComponent> pTriggerComponent = MakeStrongPtr(pWeakTrigComp);
        return pTriggerComponent;
    }

    return nullptr;
}

shared_ptr<ProjectileAIComponent> GetProjectileAIComponentFromB2Body(const b2Body* pBody)
{
    assert(pBody);

    Actor* pActor = static_cast<Actor*>(pBody->GetUserData());
    if (!pActor)
    {
        return nullptr;
    }

    // May no longer be valid, calling methods have to check it
    shared_ptr<ProjectileAIComponent> pComponent =
        MakeStrongPtr(pActor->GetComponent<ProjectileAIComponent>(ProjectileAIComponent::g_Name));

    return pComponent;
}

b2AABB GetBodyAABB(b2Body* pBody, bool discardSensors)
{
    b2AABB aabb;
    aabb.lowerBound = b2Vec2(FLT_MAX, FLT_MAX);
    aabb.upperBound = b2Vec2(-FLT_MAX, -FLT_MAX);
    b2Fixture* fixture = pBody->GetFixtureList();
    while (fixture != NULL)
    {
        if (discardSensors && fixture->IsSensor())
        {
            fixture = fixture->GetNext();
            continue;
        }
        aabb.Combine(aabb, fixture->GetAABB(0));
        fixture = fixture->GetNext();
    }

    return aabb;
}

b2Fixture* GetLowermostFixture(b2Body* pBody, bool discardSensors)
{
    b2Fixture* pLowermostFixture = NULL;
//    b2AABB aabb;
//    aabb.lowerBound = b2Vec2(FLT_MAX, FLT_MAX);
//    aabb.upperBound = b2Vec2(-FLT_MAX, -FLT_MAX);
    b2Fixture* fixture = pBody->GetFixtureList();
    while (fixture != NULL)
    {
        if (fixture->IsSensor())
        {
            fixture = fixture->GetNext();
            continue;
        }

        if (pLowermostFixture == NULL)
        {
            pLowermostFixture = fixture;
        }
        else if (fixture->GetAABB(0).upperBound.y > pLowermostFixture->GetAABB(0).upperBound.y)
        {
            pLowermostFixture = fixture;
        }

        fixture = fixture->GetNext();
    }


    assert(pLowermostFixture != NULL);
    return pLowermostFixture;
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
    LOG("Destroying current ClawPhysics");
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

    // Iterating vector or list has lower overhead
    /*for (ActorIDToBox2DBodyMap::const_iterator it = m_ActorToBodyMap.begin();
        it != m_ActorToBodyMap.end();
        ++it)*/
    for (const std::pair<uint32, b2Body*>& it : m_ActorIdAndBodyList)
    {
        b2Body* pActorBody = it.second;
        assert(pActorBody);

        if (pActorBody->GetType() == b2_staticBody)
        {
            continue;
        }

        uint32 actorId = it.first;

        //StrongActorPtr pGameActor = MakeStrongPtr(g_pApp->GetGameLogic()->VGetActor(actorId));
        //assert(pGameActor);
        auto pGameActor = static_cast<Actor*>(pActorBody->GetUserData());
        assert(pGameActor && pGameActor->GetGUID() == actorId);

        if (pGameActor && pActorBody)
        {
            /*shared_ptr<PositionComponent> pPositionComponent = MakeStrongPtr(pGameActor->GetComponent<PositionComponent>(PositionComponent::g_Name));*/

            shared_ptr<PositionComponent> pPositionComponent = pGameActor->GetPositionComponent();
            assert(pPositionComponent);

            Point bodyPixelPosition = b2Vec2ToPoint(MetersToPixels(pActorBody->GetPosition()));
            Point actorPixelPosition = pPositionComponent->GetPosition();

            // TODO: Now only Claw cares about falling / jumping states, maybe refactor in future
            //if (pGameActor->GetName() == "Claw")

            // This causes slight CPU (1.5%) overhead
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
                        pPhysicsComponent->SetFallHeight(0);
                        pPhysicsComponent->SetFalling(false);
                        pPhysicsComponent->SetJumping(false);
                        if (pActorBody->GetLinearVelocity().y < -5)
                        {
                            pPhysicsComponent->AddJumpHeight(fabs(bodyPixelPosition.y - actorPixelPosition.y));
                        }
                    }
                    // Falling
                    else if ((bodyPixelPosition.y - actorPixelPosition.y) > DBL_EPSILON)
                    {
                        // TODO: Some unknown stuff is making claw cancel jump
                        /*if (pGameActor->GetName() == "Claw")
                        {
                        if ((bodyPixelPosition.y - actorPixelPosition.y) > 2 || true)
                        {
                        pPhysicsComponent->SetFalling(true);
                        pPhysicsComponent->SetJumping(false);
                        }
                        }
                        else
                        {
                        pPhysicsComponent->SetFalling(true);
                        pPhysicsComponent->SetJumping(false);
                        }*/

                        pPhysicsComponent->AddFallHeight(fabs(bodyPixelPosition.y - actorPixelPosition.y));
                        pPhysicsComponent->SetFalling(true);
                        pPhysicsComponent->SetJumping(false);
                    }
                    else // Jumping
                    {
                        if (pPhysicsComponent->CanJump())
                        {
                            pPhysicsComponent->SetFallHeight(0);
                            pPhysicsComponent->SetFalling(false);
                            pPhysicsComponent->SetJumping(true);
                            pPhysicsComponent->AddJumpHeight(fabs(bodyPixelPosition.y - actorPixelPosition.y));

                            float jumpPixelsLeft = pPhysicsComponent->GetMaxJumpHeight() - pPhysicsComponent->GetHeightInAir();

                            //if (pGameActor->GetName() == "Claw")
                            //{
                                //LOG("LEFT: " + ToStr(jumpPixelsLeft));
                            //}

                            // Jumped past limit
                            if ((!g_pApp->GetGameCheats()->clawInfiniteJump || pPhysicsComponent->GetIsForcedUp()) && jumpPixelsLeft < 0.0f)
                            {
                                // Set b2Body to its max height
                                bodyPixelPosition = Point(bodyPixelPosition.x, bodyPixelPosition.y + fabs(jumpPixelsLeft));
                                VSetPosition(actorId, bodyPixelPosition);
                                pPhysicsComponent->SetForceFall();
                            }
                        }
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
                    /*if (pGameActor->GetName() == "Claw")
                    {
                    LOG("----" + ToStr(bodyPixelPosition.y - actorPixelPosition.y));

                    int count;
                    auto keys = SDL_GetKeyboardState(&count);

                    LOG("space: " + ToStr(keys[SDL_SCANCODE_SPACE]));
                    }*/
                    pPhysicsComponent->OnStartFalling();
                }
                if (!wasJumping && pPhysicsComponent->IsJumping())
                {
                    //LOG(ToStr(bodyPixelPosition.y - actorPixelPosition.y));
                    pPhysicsComponent->OnStartJumping();
                }
            }

            // Body moved by some portion
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
                    // TODO: Refactor
                    if (shared_ptr<KinematicComponent> pKinematicComponent = GetKinematicComponentFromB2Body(pActorBody))
                    {
                        pKinematicComponent->OnMoved(bodyPixelPosition);
                    }
                    else if (shared_ptr<PathElevatorComponent> pPathElevatorComponent = GetPathElevatorComponentFromB2Body(pActorBody))
                    {
                        pPathElevatorComponent->OnMoved(bodyPixelPosition);
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

    m_pWorld->Step(msDiff / 1000.0f, 10, 8);

    // Remove actors form physics simulation which are scheduled to be destroyed
    for (uint32 actorId : m_ActorsToBeDestroyed)
    {
        if (b2Body* pBody = FindBox2DBody(actorId))
        {
            assert(m_pWorld->IsLocked() == false);

            pBody->SetActive(false);
            pBody->SetUserData(NULL);
            m_pWorld->DestroyBody(pBody);
            m_ActorToBodyMap.erase(actorId);
            m_BodyToActorMap.erase(pBody);
            for (auto iter = m_ActorIdAndBodyList.begin(); iter != m_ActorIdAndBodyList.end(); iter++)
            {
                if (iter->first == actorId)
                {
                    m_ActorIdAndBodyList.erase(iter);
                    break;
                }
            }
        }
    }
    m_ActorsToBeDestroyed.clear();

    // Create any pending actors
    for (const ActorBodyDef* pActorBodyDef : m_ActorBodiesToBeCreated)
    {
        VAddActorBody(pActorBodyDef);
    }
    m_ActorBodiesToBeCreated.clear();

    // Create any pending fixtures
    for (const auto& fixturePair : m_FixturesToBeCreated)
    {
        VAddActorFixtureToBody(fixturePair.first, fixturePair.second);
    }
    m_FixturesToBeCreated.clear();

    // Create any pending fixtures
    for (const auto& forcePair : m_DeferredAppliedForce)
    {
        VApplyForce(forcePair.first, forcePair.second);
    }
    m_DeferredAppliedForce.clear();
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
void ClawPhysics::VAddStaticGeometry(const Point& position, const Point& size, CollisionType collisionType, FixtureType fixtureType)
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
    fixtureDef.userData = (void*)fixtureType;

    // Assign static geometry type (= tile type)
    if (collisionType == CollisionType_Solid) 
    { 
        fixtureDef.filter.categoryBits = CollisionFlag_Solid;
        fixtureDef.friction = 0.18f;
    }
    else if (collisionType == CollisionType_Ground) 
    { 
        fixtureDef.filter.categoryBits = CollisionFlag_Ground;
        fixtureDef.friction = 0.18f;
    }
    else if (collisionType == CollisionType_Climb) 
    { 
        fixtureDef.filter.categoryBits = CollisionFlag_Ladder;
        fixtureDef.isSensor = true;
    }
    else if (collisionType == CollisionType_Death) 
    { 
        fixtureDef.filter.categoryBits = CollisionFlag_Death;
    }

    /*if ((int)fixtureDef.userData != FixtureType_Solid &&
        (int)fixtureDef.userData != FixtureType_Death && 
        (int)fixtureDef.userData != FixtureType_Ground)
    { 
        fixtureDef.isSensor = true; 
    }*/

    // Have to treat ground as independent body
    // TODO: This is really ugly, redo to something better
    if (collisionType == CollisionType_Ground)
    {
        b2BodyDef bodyDef;
        bodyDef.type = b2_staticBody;
        bodyDef.position.Set(b2Position.x + b2Size.x / 2, b2Position.y + b2Size.y / 2);
        bodyDef.fixedRotation = true;
        b2Body* pBody = m_pWorld->CreateBody(&bodyDef);

        b2PolygonShape bodyShape;
        bodyShape.SetAsBox(b2Size.x / 2, b2Size.y / 2);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &bodyShape;
        fixtureDef.friction = 0.18f;
        fixtureDef.filter.categoryBits = CollisionFlag_Ground;
        fixtureDef.userData = (void*)fixtureType;
        fixtureDef.isSensor = false;
        pBody->CreateFixture(&fixtureDef);
    }
    else
    {
        m_pTiles->CreateFixture(&fixtureDef);
    }
}

// TODO: This is only applicable to Claw... make him use ActorBodyDef
void ClawPhysics::VAddDynamicActor(WeakActorPtr pActor)
{
    //LOG("Creating dynamic actor");

    StrongActorPtr pStrongActor = MakeStrongPtr(pActor);
    if (!pStrongActor)
    {
        return;
    }
    
    assert(pStrongActor->GetName() == "Claw");

    shared_ptr<PositionComponent> pPositionComponent = pStrongActor->GetPositionComponent();
    if (!pPositionComponent)
    {
        return;
    }
    Point position = pPositionComponent->GetPosition();

    shared_ptr<PhysicsComponent> pPhysicsComponent = pStrongActor->GetPhysicsComponent();
    if (!pPhysicsComponent)
    {
        return;
    }
    Point bodySize = pPhysicsComponent->GetBodySize();

    // Convert pixel position and size to Box2D meters
    b2Vec2 b2Position = PixelsToMeters(PointToB2Vec2(position));
    b2Vec2 b2BodySize = PixelsToMeters(PointToB2Vec2(bodySize));

    b2BodyDef bodyDef;
    bodyDef.allowSleep = false; // TODO: Move this into ActoBodyDef aswell
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(b2Position.x, b2Position.y);
    bodyDef.fixedRotation = true;
    b2Body* pBody = m_pWorld->CreateBody(&bodyDef);
    pBody->SetUserData(pStrongActor.get());
    pBody->SetGravityScale(pPhysicsComponent->GetGravityScale());

    b2CircleShape bodyShape;
    bodyShape.m_p.Set(0, (b2BodySize.x / 2 - b2BodySize.y / 2) + PixelsToMeters(5));
    bodyShape.m_radius = b2BodySize.x / 2;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &bodyShape;
    fixtureDef.density = pPhysicsComponent->GetDensity();
    fixtureDef.friction = pPhysicsComponent->GetFriction();
    fixtureDef.filter.categoryBits = CollisionFlag_Controller;
    pBody->CreateFixture(&fixtureDef);

    bodyShape.m_p.Set(0, (b2BodySize.y / 2 - b2BodySize.x / 2) + PixelsToMeters(10));
    fixtureDef.shape = &bodyShape;
    //fixtureDef.friction = 100.0;
    pBody->CreateFixture(&fixtureDef);

    b2PolygonShape polygonShape;
    polygonShape.SetAsBox((b2BodySize.x / 2) - PixelsToMeters(2), (b2BodySize.y - b2BodySize.x) / 2);
    fixtureDef.shape = &polygonShape;
    pBody->CreateFixture(&fixtureDef);

    // Add foot sensor
    float sensorHeight = PixelsToMeters(24);
    polygonShape.SetAsBox(b2BodySize.x / 2 - PixelsToMeters(2), sensorHeight / 2, b2Vec2(0, (b2BodySize.y / 2) + PixelsToMeters(10)), 0);
    fixtureDef.shape = &polygonShape;
    fixtureDef.isSensor = true;
    fixtureDef.userData = (void*)FixtureType_FootSensor;
    pBody->CreateFixture(&fixtureDef);

    m_ActorToBodyMap.insert(std::make_pair(pStrongActor->GetGUID(), pBody));
    m_BodyToActorMap.insert(std::make_pair(pBody, pStrongActor->GetGUID()));
    m_ActorIdAndBodyList.push_back(std::make_pair(pStrongActor->GetGUID(), pBody));
}

//-----------------------------------------------------------------------------
// ClawPhysics::VAddKinematicBody
//
//    Adds kinematic body to world.
//
void ClawPhysics::VAddKinematicBody(WeakActorPtr pActor)
{
    assert(false && "Deprecated and not used");
}

void ClawPhysics::VAddStaticBody(WeakActorPtr pActor, const Point& bodySize, CollisionType collisionType)
{
    //LOG("Creating static actor");

    StrongActorPtr pStrongActor = MakeStrongPtr(pActor);
    if (!pStrongActor)
    {
        return;
    }

    shared_ptr<PositionComponent> pPositionComponent = pStrongActor->GetPositionComponent();
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
    fixtureDef.isSensor = false;
    pBody->CreateFixture(&fixtureDef);

    m_ActorToBodyMap.insert(std::make_pair(pStrongActor->GetGUID(), pBody));
    m_BodyToActorMap.insert(std::make_pair(pBody, pStrongActor->GetGUID()));
    m_ActorIdAndBodyList.push_back(std::make_pair(pStrongActor->GetGUID(), pBody));
}

void ClawPhysics::VAddActorBody(const ActorBodyDef* actorBodyDef)
{
    //assert(actorBodyDef->collisionMask != 0x0);
    if (actorBodyDef->collisionMask == 0x0)
    {
        //LOG_WARNING("Creating actor body with CollisionMask == 0x0 !");
    }
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
        fixtureDef.filter.categoryBits = actorBodyDef->collisionFlag;
        fixtureDef.filter.maskBits = actorBodyDef->collisionMask;
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
        b2FixtureDef fixtureDef;

        b2PolygonShape rectangleShape;
        b2CircleShape circleShape;

        if (actorBodyDef->collisionShape == "Rectangle")
        {
            b2Vec2 b2Offset = PixelsToMeters(PointToB2Vec2(actorBodyDef->positionOffset));
            rectangleShape.SetAsBox(b2BodySize.x / 2, b2BodySize.y / 2, b2Offset, 0);
            fixtureDef.shape = &rectangleShape;
        }
        else if (actorBodyDef->collisionShape == "Circle")
        {
            circleShape.m_p.Set(0, 0);
            circleShape.m_radius = b2BodySize.x / 2;
            fixtureDef.shape = &circleShape;
        }
        else
        {
            LOG_ERROR("Conflicting shape: " + actorBodyDef->collisionShape);
            assert(false && "Unknown collision shape.");
        }

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

    // TODO: Remove reduntant code up there... 
    for (ActorFixtureDef actorFixtureDef : actorBodyDef->fixtureList)
    {
        // If it has 0 size, set it to body size
        if (actorFixtureDef.size.IsZeroXY())
        {
            actorFixtureDef.size = actorBodyDef->size;
        }

        AddActorFixtureToBody(pBody, &actorFixtureDef);
    }

    m_ActorToBodyMap.insert(std::make_pair(pStrongActor->GetGUID(), pBody));
    m_BodyToActorMap.insert(std::make_pair(pBody, pStrongActor->GetGUID()));
    m_ActorIdAndBodyList.push_back(std::make_pair(pStrongActor->GetGUID(), pBody));

    if (actorBodyDef->setInitialSpeed)
    {
        VSetLinearSpeed(pStrongActor->GetGUID(), actorBodyDef->initialSpeed);
    }
    else if (actorBodyDef->setInitialImpulse)
    {
        VApplyLinearImpulse(pStrongActor->GetGUID(), actorBodyDef->initialSpeed);
    }
}

void ClawPhysics::AddActorFixtureToBody(b2Body* pBody, const ActorFixtureDef* pFixtureDef)
{
    assert(pBody);
    assert(pFixtureDef);

    b2Vec2 b2FixtureSize = PixelsToMeters(PointToB2Vec2(pFixtureDef->size));
    b2Vec2 b2Offset = PixelsToMeters(PointToB2Vec2(pFixtureDef->offset));

    b2FixtureDef fixture;

    b2PolygonShape rectangleShape;
    b2CircleShape circleShape;

    if (pFixtureDef->collisionShape == "Rectangle")
    {
        rectangleShape.SetAsBox(b2FixtureSize.x / 2, b2FixtureSize.y / 2, b2Offset, 0);
        fixture.shape = &rectangleShape;
    }
    else if (pFixtureDef->collisionShape == "Circle")
    {
        circleShape.m_p.Set(b2Offset.x, b2Offset.y);
        circleShape.m_radius = b2FixtureSize.x / 2;
        fixture.shape = &circleShape;
    }
    else
    {
        LOG_ERROR("Conflicting shape: " + pFixtureDef->collisionShape);
        assert(false && "Unknown collision shape.");
    }

    fixture.friction = pFixtureDef->friction;
    fixture.density = pFixtureDef->density;
    fixture.restitution = pFixtureDef->restitution;
    fixture.userData = (void*)pFixtureDef->fixtureType;
    fixture.isSensor = pFixtureDef->isSensor;
    fixture.filter.categoryBits = pFixtureDef->collisionFlag;
    fixture.filter.maskBits = pFixtureDef->collisionMask;
    DO_AND_CHECK(pBody->CreateFixture(&fixture));
}

void ClawPhysics::VAddActorFixtureToBody(uint32_t actorId, const ActorFixtureDef* pFixtureDef)
{
    if (m_pWorld->IsLocked())
    {
        m_FixturesToBeCreated.push_back(std::make_pair(actorId, pFixtureDef));
        return;
    }

    if (b2Body* pBody = FindBox2DBody(actorId))
    {
        AddActorFixtureToBody(pBody, pFixtureDef);
    }
    else
    {
        LOG_WARNING("Failed to add fixture to body. Is Physics world updating: " + ToStr(m_pWorld->IsLocked()));
    }
}

//-----------------------------------------------------------------------------
// ClawPhysics::VRemoveActor
//
//    Removes actor from physics world.
//
void ClawPhysics::VRemoveActor(uint32_t actorId)
{
    // Clear any user data
    if (b2Body* pBody = FindBox2DBody(actorId))
    {
        pBody->SetUserData(NULL);
    }

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
    if (!g_pApp->GetGameCheats()->showPhysicsDebug)
    {
        return;
    }

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
    m_ActorIdAndBodyList.push_back(std::make_pair(pStrongActor->GetGUID(), pBody));
}

//-----------------------------------------------------------------------------
// ClawPhysics::VApplyForce
//
//    Applies force to given actor.
//
void ClawPhysics::VApplyForce(uint32_t actorId, const Point& force)
{
    if (m_pWorld->IsLocked())
    {
        m_DeferredAppliedForce.push_back(std::make_pair(actorId, force));
        return;
    }

    if (b2Body* pBody = FindBox2DBody(actorId))
    {
        pBody->ApplyLinearImpulseToCenter(PointToB2Vec2(force), true);
    }
    else
    {
        LOG_WARNING("Failed to find actor's body");
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
    assert(false && "VKinematicMove: implement me");
    return true;
}

//-----------------------------------------------------------------------------
// ClawPhysics::VStopActor
//
//    Stops given actor's movement.
//
void ClawPhysics::VStopActor(uint32_t actorId)
{
    assert(false && "VStopActor: implement me");
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
    assert(false && "SetVelocity: implement me");
}

//-----------------------------------------------------------------------------
// ClawPhysics::VTranslate
//
//    Attempts to move actor in given direction.
//
void ClawPhysics::VTranslate(uint32_t actorId, const Point& dir)
{
    assert(false && "VTranslate: implement me");
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

void ClawPhysics::VAddLinearSpeed(uint32_t actorId, const Point& speedIncrement)
{
    if (b2Body* pBody = FindBox2DBody(actorId))
    {
        b2Vec2 currSpeed = pBody->GetLinearVelocity();
        b2Vec2 modifiedSpeed = currSpeed + PointToB2Vec2(speedIncrement);
        pBody->SetLinearVelocity(modifiedSpeed);
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
    assert(false && "VGetPosition: implement me");
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

bool ClawPhysics::VIsAwake(uint32_t actorId)
{
    if (b2Body* pBody = FindBox2DBody(actorId))
    {
        return pBody->IsAwake();
    }

    return false;
}

void ClawPhysics::VChangeCollisionFlag(uint32_t actorId, uint32 fromFlag, uint32 toFlag)
{
    if (b2Body* pBody = FindBox2DBody(actorId))
    {
        b2Fixture* pFixture = pBody->GetFixtureList();
        while (pFixture != NULL)
        {
            b2Filter filter = pFixture->GetFilterData();
            if (filter.categoryBits & fromFlag)
            {
                filter.categoryBits &= ~fromFlag;
                filter.categoryBits |= toFlag;
                pFixture->SetFilterData(filter);
            }
            pFixture = pFixture->GetNext();
        }
    }
}

SDL_Rect ClawPhysics::VGetAABB(uint32_t actorId, bool discardSensors)
{
    SDL_Rect aabbRect = { 0, 0, 0, 0 };
    if (b2Body* pBody = FindBox2DBody(actorId))
    {
        b2AABB aabb = GetBodyAABB(pBody, discardSensors);
        /*aabb.lowerBound = b2Vec2(FLT_MAX, FLT_MAX);
        aabb.upperBound = b2Vec2(-FLT_MAX, -FLT_MAX);
        b2Fixture* pFixture = pBody->GetFixtureList();
        while (pFixture != NULL)
        {
            aabb.Combine(aabb, pFixture->GetAABB(0));
            pFixture = pFixture->GetNext();
        }*/

        Point pointLowerBound = b2Vec2ToPoint(MetersToPixels(aabb.lowerBound));
        Point pointUpperBound = b2Vec2ToPoint(MetersToPixels(aabb.upperBound));

        aabbRect.x = (int)pointLowerBound.x;
        aabbRect.y = (int)pointLowerBound.y;
        aabbRect.w = (int)(pointUpperBound.x - pointLowerBound.x);
        aabbRect.h = (int)(pointUpperBound.y - pointLowerBound.y);
        
        return aabbRect;
    }

    assert(false && "Could not find actor to retrieve AABB");
    return aabbRect;
}

/// Callback to check for overlap of given body.
struct CheckOverlapCallback : b2QueryCallback
{
    CheckOverlapCallback(const b2Body* body, FixtureType overlapType) :
    m_body(body), m_isOverlap(false), m_TestOverlapType(overlapType) {}

    // override
    bool ReportFixture(b2Fixture* fixture)
    {
        // Skip self.
        if (fixture->GetBody() == m_body)
            return true;

        for (const b2Fixture* bodyFixture = m_body->GetFixtureList(); bodyFixture;
            bodyFixture = bodyFixture->GetNext())
        {
            if (b2TestOverlap(fixture->GetShape(), 0, bodyFixture->GetShape(), 0,
                fixture->GetBody()->GetTransform(), m_body->GetTransform()))
            {
                if (fixture->GetUserData() == (void*)m_TestOverlapType)
                {
                    m_isOverlap = true;
                    return false;
                }
            }
        }

        return true;
    }

    FixtureType m_TestOverlapType;
    const b2Body* m_body;
    bool m_isOverlap;
};

bool ClawPhysics::VIsActorOverlap(uint32_t actorId, FixtureType overlapType)
{
    if (b2Body* pBody = FindBox2DBody(actorId))
    {
        CheckOverlapCallback callback(pBody, overlapType);
        b2AABB aabb = GetBodyAABB(pBody, true);
        m_pWorld->QueryAABB(&callback, aabb);
        return callback.m_isOverlap;
    }

    return false;
}

class RayCastCallback_Filtered : public b2RayCastCallback
{
public:
    RayCastCallback_Filtered(const Point& p1, const Point& p2, uint32 filter)
    {
        m_Filter = filter;
        m_Fraction = 1.0f;
        
        float diffX = fabs(p1.x - p2.x);
        float diffY = fabs(p1.y - p2.y);
        m_MaxDistance = sqrt((diffX * diffX) + (diffY * diffY));

        m_RaycastResult.deltaX = p2.x - p1.x;
        m_RaycastResult.deltaY = p2.y - p1.y;
    }

    virtual float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) override
    {
        if (!(fixture->GetFilterData().categoryBits & m_Filter))
        {
            return 1.0f;
        }

        if (fabs(fraction) < m_Fraction)
        {
            m_Fraction = fraction;
            m_RaycastResult.foundIntersection = true;
        }

        return 1.0f;
    }

    RaycastResult GetRaycastResult()
    {
        m_RaycastResult.closestPixelDistance = m_Fraction * m_MaxDistance;
        m_RaycastResult.deltaX *= m_Fraction;
        m_RaycastResult.deltaY *= m_Fraction;

        return m_RaycastResult;
    }

private:
    uint32 m_Filter;
    float32 m_Fraction;
    float m_MaxDistance;
    RaycastResult m_RaycastResult;
};

RaycastResult ClawPhysics::VRayCast(const Point& fromPoint, const Point& toPoint, uint32 filterMask)
{
    RayCastCallback_Filtered callback(fromPoint, toPoint, filterMask);

    b2Vec2 b2fromPoint = PixelsToMeters(PointToB2Vec2(fromPoint));
    b2Vec2 b2toPoint = PixelsToMeters(PointToB2Vec2(toPoint));

    m_pWorld->RayCast(&callback, b2fromPoint, b2toPoint);

    return callback.GetRaycastResult();
}

// HACK: THIS WHOLE METHOD IS A HACK AND IT DOES NOT DO WHAT IT SHOULD DO
// THIS IS TIGHTLY COUPLED TO CLAW'S CROUCHING
void ClawPhysics::VScaleActor(uint32_t actorId, double scale)
{
    if (b2Body* pBody = FindBox2DBody(actorId))
    {
        if (scale < 1.0)
        {
            b2Fixture* pBodyRectangleFixture = NULL;
            b2Fixture* pHeadCircleFixture = NULL;

            b2Fixture* fixture = pBody->GetFixtureList();
            while (fixture != NULL)
            {
                if (fixture->IsSensor())
                {
                    fixture = fixture->GetNext();
                    continue;
                }
                if (fixture->GetShape()->GetType() == b2Shape::e_circle)
                {
                    if (pHeadCircleFixture == NULL)
                    {
                        pHeadCircleFixture = fixture;
                    }
                    else
                    {
                        if (fixture->GetAABB(0).lowerBound.y < pHeadCircleFixture->GetAABB(0).lowerBound.y)
                        {
                            pHeadCircleFixture = fixture;
                        }
                    }
                }
                else if (fixture->GetShape()->GetType() == b2Shape::e_polygon)
                {
                    if (pBodyRectangleFixture == NULL)
                    {
                        pBodyRectangleFixture = fixture;
                    }
                    else
                    {
                        if (fixture->GetAABB(0).lowerBound.y < pBodyRectangleFixture->GetAABB(0).lowerBound.y)
                        {
                            pBodyRectangleFixture = fixture;
                        }
                    }
                }
                fixture = fixture->GetNext();
            }

            assert(pBodyRectangleFixture && pHeadCircleFixture);
            auto a = pBodyRectangleFixture->GetFilterData();
            auto b = pHeadCircleFixture->GetFilterData();
            a.maskBits = 0x0;
            b.maskBits = 0x0;

            pBodyRectangleFixture->SetFilterData(a);
            pHeadCircleFixture->SetFilterData(b);
        }
        else
        {
            b2Fixture* fixture = pBody->GetFixtureList();
            while (fixture != NULL)
            {
                if (fixture->IsSensor())
                {
                    fixture = fixture->GetNext();
                    continue;
                }
                
                auto a = fixture->GetFilterData();
                a.maskBits = 0xFFFFFFFF;
                fixture->SetFilterData(a);

                fixture = fixture->GetNext();
            }
        }
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
    std::unique_ptr<IGamePhysics> pClawPhysics;
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
