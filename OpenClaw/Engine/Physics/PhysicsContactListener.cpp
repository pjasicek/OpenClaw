#include <cstdint>
#include "PhysicsContactListener.h"
#include "ClawPhysics.h"
#include "../SharedDefines.h"
#include "../Actor/Actor.h"
#include "../Actor/Components/PhysicsComponent.h"
#include "../Actor/Components/KinematicComponent.h"
#include "../Actor/Components/AIComponents/CrumblingPegAIComponent.h"
#include "../Actor/Components/TriggerComponents/TriggerComponent.h"
#include "../Actor/Components/AIComponents/ProjectileAIComponent.h"
#include "../Actor/Components/ControllerComponents/HealthComponent.h"
#include "../Actor/Components/EnemyAI/EnemyAIStateComponent.h"
#include "../Actor/Components/ControllableComponent.h"
#include "../Actor/Components/PositionComponent.h"
#include "../Actor/Components/PathElevatorComponent.h"
#include "../Actor/Components/SteppingGroundComponent.h"
#include "../Actor/Components/SpringBoardComponent.h"
#include "../Actor/Components/ConveyorBeltComponent.h"
#include "../Actor/Components/AuraComponents/AuraComponent.h"

int numFootContacts = 0;

#define SWAP_IF_FIXTURE_B_EQUALS(FixtureA, FixtureB, FixtureType) \
    if (FixtureB->GetUserData() == (void*)FixtureType) \
    { \
        std::swap(FixtureA, FixtureB); \
        } \


template<typename T>
void TryCallActorEnteredOrLeftAgroRange(
    b2Fixture* pFixtureA,
    b2Fixture* pFixtureB,
    FixtureType agroFixtureType,
    bool didActorEnter)
{
    SWAP_IF_FIXTURE_B_EQUALS(pFixtureA, pFixtureB, agroFixtureType);
    if (pFixtureA->GetUserData() == (void*)agroFixtureType)
    {
        if (pFixtureB->GetBody()->GetUserData() != NULL)
        {
            Actor* pActorwhoEntered = static_cast<Actor*>(pFixtureB->GetBody()->GetUserData());
            Actor* pActorWithMeleeSensor = static_cast<Actor*>(pFixtureA->GetBody()->GetUserData());

            if (pActorwhoEntered && pActorWithMeleeSensor)
            {
                shared_ptr<T> pStateComponent =
                    MakeStrongPtr(pActorWithMeleeSensor->GetComponent<T>(T::g_Name));
                assert(pStateComponent != nullptr);
                if (pStateComponent)
                {
                    if (didActorEnter)
                    {
                        pStateComponent->OnEnemyEnterAgroRange(pActorwhoEntered);
                    }
                    else
                    {
                        pStateComponent->OnEnemyLeftAgroRange(pActorwhoEntered);
                    }
                }
            }
        }
    }
}

// lol
template<typename T>
void TryCallActorEnteredAgroRange(
    b2Fixture* pFixtureA,
    b2Fixture* pFixtureB,
    FixtureType agroFixtureType)
{
    TryCallActorEnteredOrLeftAgroRange<T>(
        pFixtureA,
        pFixtureB,
        agroFixtureType,
        true);
}

// lol
template<typename T>
void TryCallActorLeftAgroRange(
    b2Fixture* pFixtureA,
    b2Fixture* pFixtureB,
    FixtureType agroFixtureType)
{
    TryCallActorEnteredOrLeftAgroRange<T>(
        pFixtureA,
        pFixtureB,
        agroFixtureType,
        false);
}

static bool IsTriggerFixture(FixtureType fixtureType)
{
    return (
        fixtureType == FixtureType_Trigger ||
        fixtureType == FixtureType_Trigger_SpawnArea ||
        fixtureType == FixtureType_Trigger_GabrielButton ||
        fixtureType == FixtureType_Trigger_ChaseEnemyAreaSensor ||
        fixtureType == FixtureType_Trigger_RollAreaSensor);
}

//=====================================================================================================================
//
// PhysicsContactListener::BeginContact
//

void PhysicsContactListener::BeginContact(b2Contact* pContact)
{
    b2Fixture* pFixtureA = pContact->GetFixtureA();
    b2Fixture* pFixtureB = pContact->GetFixtureB();

    // Foot contact
    {
        // Make it in predictable order
        if (pFixtureB->GetUserData() == (void*)FixtureType_FootSensor)
        {
            std::swap(pFixtureA, pFixtureB);
        }

        if (pFixtureA->GetUserData() == (void*)FixtureType_FootSensor)
        {
            if (pFixtureB->GetUserData() == (void*)FixtureType_Solid ||
                pFixtureB->GetUserData() == (void*)FixtureType_Death)
            {
                shared_ptr<PhysicsComponent> pPhysicsComponent = GetPhysicsComponentFromB2Body(pFixtureA->GetBody());
                assert(pPhysicsComponent != nullptr);

                pPhysicsComponent->OnBeginFootContact();
            }
        }
    }
    // Ladder contact
    {
        SWAP_IF_FIXTURE_B_EQUALS(pFixtureA, pFixtureB, FixtureType_Climb);
        if (pFixtureA->GetUserData() == (void*)FixtureType_Climb)
        {
            if (pFixtureB->GetBody()->GetType() == b2_dynamicBody)
            {
                shared_ptr<PhysicsComponent> pPhysicsComponent = GetPhysicsComponentFromB2Body(pFixtureB->GetBody());
                assert(pPhysicsComponent != nullptr);

                pPhysicsComponent->AddOverlappingLadder(pFixtureA);
            }
        }
    }
    // Collision with "One-Way Ground" tile - mostly platforms, elevators and such
    {
        SWAP_IF_FIXTURE_B_EQUALS(pFixtureA, pFixtureB, FixtureType_Ground);
        SWAP_IF_FIXTURE_B_EQUALS(pFixtureA, pFixtureB, FixtureType_TopLadderGround);
        if ((pFixtureA->GetUserData() == (void*)FixtureType_Ground) ||
            (pFixtureA->GetUserData() == (void*)FixtureType_TopLadderGround))
        {
            if (pFixtureB->GetBody()->GetType() == b2_dynamicBody/* && pFixtureB->GetUserData() != (void*)FixtureType_Trigger*/)
            {
                shared_ptr<PhysicsComponent> pPhysicsComponent = GetPhysicsComponentFromB2Body(pFixtureB->GetBody());
                if (pPhysicsComponent == nullptr)
                {
                    LOG_ERROR("Ground fixture: Box2D step with already deleted physics component !");
                    return;
                }
                //LOG("bodyAABB y: " + ToStr(MetersToPixels(bodyAABB.upperBound.y)) + ", Fixture lower: " + ToStr(MetersToPixels(pFixtureA->GetAABB(0).lowerBound.y)));

                int numPoints = pContact->GetManifold()->pointCount;
                b2WorldManifold worldManifold;
                pContact->GetWorldManifold(&worldManifold);

                if (GetLowermostFixture(pFixtureB->GetBody()) != pFixtureB)
                {
                    pContact->SetEnabled(false);
                    return;
                }

                bool checkFurther = false;
                Actor* pActor = static_cast<Actor*>(pFixtureB->GetBody()->GetUserData());
                Actor* pGroundActor = static_cast<Actor*>(pFixtureA->GetBody()->GetUserData());
                /*if (pActor->GetName() == "Claw" && pGroundActor && pGroundActor->GetName() == "Level7_SpringBoard")
                {
                    LOG("Contact");
                    checkFurther = true;
                }*/

                pContact->SetEnabled(false);
                for (int pointIdx = 0; pointIdx < numPoints; pointIdx++)
                {
                    b2Vec2 pointVelocity = pFixtureB->GetBody()->GetLinearVelocityFromWorldPoint(worldManifold.points[pointIdx]);

                    if (pointVelocity.y > -2)
                    {
                        b2AABB bodyAABB = GetBodyAABB(pFixtureB->GetBody(), true);
                        /*LOG("Actor upper AABB.y: " + ToStr(bodyAABB.upperBound.y));
                        LOG("Fixture lower AABB.y: " + ToStr(pFixtureA->GetAABB(0).lowerBound.y));
                        LOG("Lowermost fixture y: " + ToStr(GetLowermostFixture(pFixtureB->GetBody())->GetAABB(0).upperBound.y));*/
                        /*if ((bodyAABB.upperBound.y - PixelsToMeters(5)) < pFixtureA->GetAABB(0).lowerBound.y)
                        {
                            pContact->SetEnabled(true);
                            pPhysicsComponent->AddOverlappingGround(pFixtureA);
                        }*/
                        //pFixtureA->GetAABB()

                        b2Vec2 relativePointA = pFixtureA->GetBody()->GetLocalPoint(worldManifold.points[pointIdx]);
                        b2Vec2 relativePointB = pFixtureB->GetBody()->GetLocalPoint(worldManifold.points[pointIdx]);
                        //LOG("Relative point Y: " + ToStr(relativePoint.y));
                        float platformFaceY = 0.5f;//front of platform, from fixture definition :(

                        /*if (pActor->GetName() == "Claw" && pGroundActor && pGroundActor->GetName() == "Level7_SpringBoard")
                        {
                            LOG("RelativePointA.y: " + ToStr(relativePointA.y));
                        }*/

                        //platformFaceY *= (width / height);

                        const b2AABB& groundAABB = pFixtureA->GetAABB(0);
                        /*float width = aabb.upperBound.x - aabb.lowerBound.x;
                        float height = aabb.upperBound.y - aabb.lowerBound.y;
                        float ratio = width / height;

                        // Extra wide and thin stuff.. First appearance on level 7 - Aircart elevators
                        if (ratio > 3.0)
                        {
                            //relativePointA.y /= 2 * ratio;
                        }*/

                        /*Point bodyPos = b2Vec2ToPoint(MetersToPixels(pFixtureA->GetBody()->GetPosition()));
                        Point fixturePos = b2Vec2ToPoint(MetersToPixels(aabb.GetCenter()));

                        LOG("BodyPos: " + bodyPos.ToString() + ", FixturePos: " + fixturePos.ToString());*/

                        // In case the origin of the fixture is not in the body's center
                        relativePointA += pFixtureA->GetBody()->GetPosition() - groundAABB.GetCenter();

                        if (relativePointA.y < (platformFaceY - 0.05))
                        {
                            
                            /*if (pActor->GetName() == "Claw" && pFixtureA->GetBody()->GetType() == b2_kinematicBody)
                            {
                                LOG("y: " + ToStr(relativePointA.y));
                                LOG("x: " + ToStr(relativePointA.x));
                                LOG("PointVelocity.y: " + ToStr(pointVelocity.y));
                            }*/
                            
                            // Only allow to actually land from ABOVE not from the side when still below
                            // Still hacked though... Causes some items to fall through when level load maybe ? (crates level 2)
                            if (fabs(relativePointA.y) < 0.1f && fabs(relativePointA.x) > 0.1f && fabs(relativePointB.x) > 0.01)
                            {
                                /*if (pActor->GetName() == "Claw")
                                {
                                    LOG("-------------");
                                    LOG("NOT DE");
                                    LOG("relativePointA.x: " + ToStr(relativePointA.x) + ", relativePointA.y: " + ToStr(relativePointA.y));
                                    LOG("RelativePointB.x: " + ToStr(relativePointB.x) + ", RelativePointB.y: " + ToStr(relativePointB.y));
                                    
                                }
                                return;*/
                            }
                            /*else
                            {
                                if (pActor->GetName() == "Claw")
                                {
                                    LOG("-------------");
                                    LOG("DESTR");
                                    LOG("relativePointA.x: " + ToStr(relativePointA.x) + ", relativePointA.y: " + ToStr(relativePointA.y));
                                    LOG("RelativePointB.x: " + ToStr(relativePointB.x) + ", RelativePointB.y: " + ToStr(relativePointB.y));
                                }
                            }*/

                            // If bellow the platform the contact should be disabled
                            if (relativePointA.y > 0.1f)
                            {
                                /*if (pActor->GetName() == "Claw" && pGroundActor && pGroundActor->GetName() == "Level7_SpringBoard")
                                {
                                    LOG("Nope 2: RelativePointA.y: " + ToStr(relativePointA.y));
                                }*/
                                return;
                            }

                            /*if (pActor->GetName() == "Claw" && pGroundActor && pGroundActor->GetName() == "Level7_SpringBoard")
                            {
                                LOG("Yep: RelativePointA.y: " + ToStr(relativePointA.y));
                            }*/

                            // TODO: Think about better solution and rename this to something better
                            if (pFixtureA->GetUserData() == (void*)FixtureType_TopLadderGround)
                            {
                                pPhysicsComponent->SetTopLadderContact(pContact);
                            }
                            
                            //pFixtureB->GetBody()->SetLinearVelocity(pFixtureA->GetBody()->GetLinearVelocity());

                            /*if (pActor->GetName() == "Claw")
                            {
                                LOG("Enabling");
                            }*/
                            
                            pContact->SetEnabled(true);
                            pPhysicsComponent->AddOverlappingGround(pFixtureA);
                            break;
                        }
                    }
                    else
                    {
                        //LOG("Velocity = " + ToStr(pointVelocity.y));
                    }

                    /*if (pActor->GetName() == "Claw" && pFixtureA->GetBody()->GetType() == b2_kinematicBody)
                    {
                        LOG("TEST 2");
                    }*/
                }

                /*if (checkFurther)
                {
                    LOG("Is enabled in the end: " + ToStr(pContact->IsEnabled()));
                }*/
#if 0
                b2AABB bodyAABB = GetBodyAABB(pFixtureB->GetBody());
                if (/*pFixtureB->GetBody()->GetLinearVelocity().y >= 0 &&*/
                    (bodyAABB.upperBound.y - PixelsToMeters(20)) < pFixtureA->GetAABB(0).lowerBound.y)
                {
                    
                    //pFixtureA->SetSensor(false);
                    pContact->SetEnabled(true);
                    pPhysicsComponent->AddOverlappingGround(pFixtureA);
                }
                else
                {
                    pContact->SetEnabled(false);
                }
#endif
                // Moving platform (elevator)
                if (pContact->IsEnabled() /*!pFixtureA->IsSensor()*/ && pFixtureA->GetBody()->GetType() == b2_kinematicBody && !pFixtureB->IsSensor())
                {
                    if (shared_ptr<KinematicComponent> pKinematicComponent = GetKinematicComponentFromB2Body(pFixtureA->GetBody()))
                    {
                        pKinematicComponent->AddCarriedBody(pFixtureB->GetBody());
                    }
                    else if (shared_ptr<PathElevatorComponent> pPathElevatorComponent = GetPathElevatorComponentFromB2Body(pFixtureA->GetBody()))
                    {
                        pPathElevatorComponent->AddCarriedBody(pFixtureB->GetBody());
                    }
                    /*shared_ptr<KinematicComponent> pKinematicComponent = GetKinematicComponentFromB2Body(pFixtureA->GetBody());
                    pKinematicComponent->AddCarriedBody(pFixtureB->GetBody());*/
                    pPhysicsComponent->AddOverlappingKinematicBody(pFixtureA->GetBody());
                    pContact->SetFriction(100.0f);
                    pPhysicsComponent->SetMovingPlatformContact(pContact);
                }

                // TODO: HACK: Crumbling peg, hackerino but who cares
                if (pContact->IsEnabled() /*!pFixtureA->IsSensor()*/ && !pFixtureB->IsSensor() && 
                    pFixtureA->GetBody()->GetType() == b2_staticBody && pFixtureA->GetBody()->GetUserData())
                {
                    Actor* pActor = static_cast<Actor*>(pFixtureA->GetBody()->GetUserData());
                    assert(pActor);

                    bool bIsClaw = MakeStrongPtr(static_cast<Actor*>(pFixtureB->GetBody()->GetUserData())->GetComponent<ClawControllableComponent>()) != nullptr;

                    shared_ptr<CrumblingPegAIComponent> pCrumblingPegComponent =
                        MakeStrongPtr(pActor->GetComponent<CrumblingPegAIComponent>(CrumblingPegAIComponent::g_Name));
                    if (pCrumblingPegComponent && bIsClaw)
                    {
                        pCrumblingPegComponent->OnContact(pFixtureB->GetBody());
                    }

                    shared_ptr<SteppingGroundComponent> pSteppingGroundComponent =
                        MakeStrongPtr(pActor->GetComponent<SteppingGroundComponent>());
                    if (pSteppingGroundComponent && bIsClaw)
                    {
                        Actor* pOtherActor = static_cast<Actor*>(pFixtureB->GetBody()->GetUserData());
                        pSteppingGroundComponent->OnActorContact(pOtherActor);
                    }

                    shared_ptr<SpringBoardComponent> pSpringBoardComponent =
                        MakeStrongPtr(pActor->GetComponent<SpringBoardComponent>());
                    if (pSpringBoardComponent && bIsClaw)
                    {
                        Actor* pOtherActor = static_cast<Actor*>(pFixtureB->GetBody()->GetUserData());
                        pSpringBoardComponent->OnActorBeginContact(pOtherActor);
                    }

                    shared_ptr<ConveyorBeltComponent> pConveyorBeltComponent =
                        MakeStrongPtr(pActor->GetComponent<ConveyorBeltComponent>());
                    if (pConveyorBeltComponent && bIsClaw)
                    {
                        Actor* pOtherActor = static_cast<Actor*>(pFixtureB->GetBody()->GetUserData());
                        pConveyorBeltComponent->OnActorBeginContact(pOtherActor);
                    }
                }

                /*if (pActor->GetName() == "Claw")
                {
                    //LOG("Contact was enabled: " + ToStr(pContact->IsEnabled()));
                }*/
            }
        }
    }
    // , contact
    {
        SWAP_IF_FIXTURE_B_EQUALS(pFixtureA, pFixtureB, FixtureType_Trigger);
        SWAP_IF_FIXTURE_B_EQUALS(pFixtureA, pFixtureB, FixtureType_Trigger_SpawnArea);
        SWAP_IF_FIXTURE_B_EQUALS(pFixtureA, pFixtureB, FixtureType_Trigger_GabrielButton);
        SWAP_IF_FIXTURE_B_EQUALS(pFixtureA, pFixtureB, FixtureType_Trigger_ChaseEnemyAreaSensor);
        SWAP_IF_FIXTURE_B_EQUALS(pFixtureA, pFixtureB, FixtureType_Trigger_RollAreaSensor);
        FixtureType fixtureType = FixtureType(reinterpret_cast<std::intptr_t>(pFixtureA->GetUserData()));
        if (IsTriggerFixture(fixtureType))
        {
            if (pFixtureB->GetBody()->GetUserData() != NULL)
            {
                Actor* pActor = static_cast<Actor*>(pFixtureB->GetBody()->GetUserData());
                assert(pActor);

                shared_ptr<TriggerComponent> pTriggerComponent = GetTriggerComponentFromB2Body(pFixtureA->GetBody());
                if (pTriggerComponent)
                {
                    pTriggerComponent->OnActorEntered(pActor, fixtureType);
                }
            }
        }
    }
    // Projectile contact
    {
        if (pFixtureB->GetUserData() == (void*)FixtureType_Projectile)
        {
            std::swap(pFixtureA, pFixtureB);
        }

        if (pFixtureA->GetUserData() == (void*)FixtureType_Projectile)
        {
            // Collided with some actor
            if (pFixtureB->GetBody()->GetUserData() != (void*)NULL)
            {
                Actor* pActor = static_cast<Actor*>(pFixtureB->GetBody()->GetUserData());
                assert(pActor);

                shared_ptr<ProjectileAIComponent> pProjectileComponent = GetProjectileAIComponentFromB2Body(pFixtureA->GetBody());

                if (pProjectileComponent)
                {
                    // HACK:
                    /*if (shared_ptr<ClawControllableComponent> pClaw =
                        MakeStrongPtr(pActor->GetComponent<ClawControllableComponent>(ClawControllableComponent::g_Name)))
                    {
                        Actor* pProjectileActor = static_cast<Actor*>(pFixtureA->GetBody()->GetUserData());
                        shared_ptr<PositionComponent> pProjectilePositionComponent =
                            MakeStrongPtr(pProjectileActor->GetComponent<PositionComponent>(PositionComponent::g_Name));

                        shared_ptr<PositionComponent> pClawPositionComponent =
                            MakeStrongPtr(pActor->GetComponent<PositionComponent>(PositionComponent::g_Name));

                        assert(pProjectilePositionComponent);
                        assert(pClawPositionComponent);
                        if (pProjectilePositionComponent->GetX() < pClawPositionComponent->GetX())
                        {
                            pClaw->m_LastHitDirection = Direction_Left;
                        }
                        else
                        {
                            pClaw->m_LastHitDirection = Direction_Right;
                        }
                    }*/

                    pProjectileComponent->OnCollidedWithActor(pActor);
                }
            }
            // Projectile collided with solid tile
            else if (pFixtureB->GetBody()->GetType() == b2_staticBody/* &&
                (pFixtureB->GetUserData() == (void*)FixtureType_Solid ||
                 pFixtureB->GetUserData() == (void*)FixtureType_TopLadderGround ||
                 pFixtureB->GetUserData() == (void*)FixtureType_Ground)*/)
            {
                if (pFixtureB->GetUserData() == (void*)FixtureType_TopLadderGround)
                {
                    pContact->SetEnabled(false);
                    return;
                }

                shared_ptr<ProjectileAIComponent> pProjectileComponent = GetProjectileAIComponentFromB2Body(pFixtureA->GetBody());
                if (pProjectileComponent)
                {
                    pProjectileComponent->OnCollidedWithSolidTile();
                }
            }
        }
    }
    // Death contact
    {
        if (pFixtureB->GetUserData() == (void*)FixtureType_Death)
        {
            std::swap(pFixtureA, pFixtureB);
        }

        if (pFixtureA->GetUserData() == (void*)FixtureType_Death)
        {
            if (pFixtureB->GetBody()->GetUserData() != NULL)
            {
                Actor* pActor = static_cast<Actor*>(pFixtureB->GetBody()->GetUserData());
                assert(pActor);

                shared_ptr<HealthComponent> pHealthComponent =
                    MakeStrongPtr(pActor->GetComponent<HealthComponent>(HealthComponent::g_Name));
                if (pHealthComponent)
                {
                    pHealthComponent->AddHealth(-1 * (pHealthComponent->GetHealth() + 1), DamageType_DeathTile, Point(0, 0), INVALID_ACTOR_ID);
                }
            }
        }
    }
    // Enemy agro melee contact
    {
        TryCallActorEnteredAgroRange<MeleeAttackAIStateComponent>(pFixtureA, pFixtureB, FixtureType_EnemyAIMeleeSensor);
    }
    // Enemy duck agro melee contact
    {
        TryCallActorEnteredAgroRange<DuckMeleeAttackAIStateComponent>(pFixtureA, pFixtureB, FixtureType_EnemyAIDuckMeleeSensor);
    }
    // Enemy ranged agro range contact
    {
        TryCallActorEnteredAgroRange<RangedAttackAIStateComponent>(pFixtureA, pFixtureB, FixtureType_EnemyAIRangedSensor);
    }
    // Enemy duck ranged agro range contact
    {
        TryCallActorEnteredAgroRange<DuckRangedAttackAIStateComponent>(pFixtureA, pFixtureB, FixtureType_EnemyAIDuckRangedSensor);
    }
    // Enemy dive area agro range contact
    {
        TryCallActorEnteredAgroRange<DiveAttackAIStateComponent>(pFixtureA, pFixtureB, FixtureType_EnemyAIDiveAreaSensor);
    }
    // Damage aura
    {
        if (pFixtureB->GetUserData() == (void*)FixtureType_DamageAura)
        {
            std::swap(pFixtureA, pFixtureB);
        }

        if (pFixtureA->GetUserData() == (void*)FixtureType_DamageAura)
        {
            if (pFixtureB->GetBody()->GetUserData() != NULL)
            {
                Actor* pActorwhoEntered = static_cast<Actor*>(pFixtureB->GetBody()->GetUserData());
                Actor* pActorWithDamageAura = static_cast<Actor*>(pFixtureA->GetBody()->GetUserData());

                if (pActorwhoEntered && pActorWithDamageAura)
                {
                    shared_ptr<DamageAuraComponent> pDamageAuraComponent =
                        MakeStrongPtr(pActorWithDamageAura->GetComponent<DamageAuraComponent>(DamageAuraComponent::g_Name));
                    if (pDamageAuraComponent)
                    {
                        pDamageAuraComponent->OnActorEntered(pActorwhoEntered);
                    }
                }
            }
        }
    }
}

//=====================================================================================================================
//
// PhysicsContactListener::EndContact
//

void PhysicsContactListener::EndContact(b2Contact* pContact)
{
    b2Fixture* pFixtureA = pContact->GetFixtureA();
    b2Fixture* pFixtureB = pContact->GetFixtureB();

    {
        // Make it in predictable order
        if (pFixtureB->GetUserData() == (void*)FixtureType_FootSensor)
        {
            std::swap(pFixtureA, pFixtureB);
        }

        if (pFixtureA->GetUserData() == (void*)FixtureType_FootSensor)
        {
            if (pFixtureB->GetUserData() == (void*)FixtureType_Solid || 
                pFixtureB->GetUserData() == (void*)FixtureType_Death)
            {
                shared_ptr<PhysicsComponent> pPhysicsComponent = GetPhysicsComponentFromB2Body(pFixtureA->GetBody());
                assert(pPhysicsComponent != nullptr);

                pPhysicsComponent->OnEndFootContact();
            }
        }
    }
    // Ladder contact
    {
        if (pFixtureB->GetUserData() == (void*)FixtureType_Climb)
        {
            std::swap(pFixtureA, pFixtureB);
        }

        if (pFixtureA->GetUserData() == (void*)FixtureType_Climb)
        {
            if (pFixtureB->GetBody()->GetType() == b2_dynamicBody)
            {
                shared_ptr<PhysicsComponent> pPhysicsComponent = GetPhysicsComponentFromB2Body(pFixtureB->GetBody());
                assert(pPhysicsComponent != nullptr);

                pPhysicsComponent->RemoveOverlappingLadder(pFixtureA);
            }
        }
    }
    // Collision with "One-Way Ground" tile - mostly platforms, elevators and such
    {
        SWAP_IF_FIXTURE_B_EQUALS(pFixtureA, pFixtureB, FixtureType_Ground);
        SWAP_IF_FIXTURE_B_EQUALS(pFixtureA, pFixtureB, FixtureType_TopLadderGround);
        if ((pFixtureA->GetUserData() == (void*)FixtureType_Ground) ||
            (pFixtureA->GetUserData() == (void*)FixtureType_TopLadderGround))
        {
            if (pFixtureB->GetBody()->GetType() == b2_dynamicBody && pFixtureB->GetUserData() != (void*)FixtureType_Trigger)
            {
                shared_ptr<PhysicsComponent> pPhysicsComponent = GetPhysicsComponentFromB2Body(pFixtureB->GetBody());
                if (pPhysicsComponent)
                {
                    // Moving platform (elevator)
                    if (pContact->IsEnabled()/*!pFixtureA->IsSensor()*/ && pFixtureA->GetBody()->GetType() == b2_kinematicBody && !pFixtureB->IsSensor())
                    {
                        //LOG("REMOVED");
                        if (shared_ptr<KinematicComponent> pKinematicComponent = GetKinematicComponentFromB2Body(pFixtureA->GetBody()))
                        {
                            pKinematicComponent->RemoveCarriedBody(pFixtureB->GetBody());
                        }
                        else if (shared_ptr<PathElevatorComponent> pPathElevatorComponent = GetPathElevatorComponentFromB2Body(pFixtureA->GetBody()))
                        {
                            pPathElevatorComponent->RemoveCarriedBody(pFixtureB->GetBody());
                        }
                        /*shared_ptr<KinematicComponent> pKinematicComponent = GetKinematicComponentFromB2Body(pFixtureA->GetBody());
                        pKinematicComponent->RemoveCarriedBody(pFixtureB->GetBody());*/
                        pPhysicsComponent->RemoveOverlappingKinematicBody(pFixtureA->GetBody());
                        pPhysicsComponent->SetMovingPlatformContact(NULL);
                    }

                    /*if (!pFixtureA->IsSensor())
                    {
                        pPhysicsComponent->RemoveOverlappingGround(pFixtureA);
                    }
                    pFixtureA->SetSensor(true);*/

                    if (pContact->IsEnabled() || pPhysicsComponent->GetTopLadderContact() == pContact)
                    {
                        pPhysicsComponent->RemoveOverlappingGround(pFixtureA);
                    }

                    Actor* pGroundActor = static_cast<Actor*>(pFixtureA->GetBody()->GetUserData());
                    if (pGroundActor)
                    {
                        shared_ptr<SpringBoardComponent> pSpringBoardComponent =
                            MakeStrongPtr(pGroundActor->GetComponent<SpringBoardComponent>());
                        if (pSpringBoardComponent)
                        {
                            Actor* pOtherActor = static_cast<Actor*>(pFixtureB->GetBody()->GetUserData());
                            pSpringBoardComponent->OnActorEndContact(pOtherActor);
                        }

                        shared_ptr<ConveyorBeltComponent> pConveyorBeltComponent =
                            MakeStrongPtr(pGroundActor->GetComponent<ConveyorBeltComponent>());
                        if (pConveyorBeltComponent)
                        {
                            Actor* pOtherActor = static_cast<Actor*>(pFixtureB->GetBody()->GetUserData());
                            pConveyorBeltComponent->OnActorEndContact(pOtherActor);
                        }
                    }

                    pContact->SetEnabled(false);

                    /*if (pFixtureB->GetBody()->GetLinearVelocity().y >= 0)
                    {
                    LOG("HERE");
                    pFixtureA->SetSensor(false);
                    shared_ptr<PhysicsComponent> pPhysicsComponent = GetPhysicsComponentFromB2Body(pFixtureB->GetBody());
                    pPhysicsComponent->OnBeginFootContact();
                    }
                    else
                    {
                    pFixtureA->SetSensor(true);
                    }*/
                }
            }
        }
    }
    // Trigger contact
    {
        SWAP_IF_FIXTURE_B_EQUALS(pFixtureA, pFixtureB, FixtureType_Trigger);
        SWAP_IF_FIXTURE_B_EQUALS(pFixtureA, pFixtureB, FixtureType_Trigger_SpawnArea);
        SWAP_IF_FIXTURE_B_EQUALS(pFixtureA, pFixtureB, FixtureType_Trigger_GabrielButton);
        SWAP_IF_FIXTURE_B_EQUALS(pFixtureA, pFixtureB, FixtureType_Trigger_ChaseEnemyAreaSensor);
        SWAP_IF_FIXTURE_B_EQUALS(pFixtureA, pFixtureB, FixtureType_Trigger_RollAreaSensor);
        FixtureType fixtureType = FixtureType(reinterpret_cast<std::intptr_t>(pFixtureA->GetUserData()));
        if (IsTriggerFixture(fixtureType))
        {
            if (pFixtureB->GetBody()->GetUserData() != NULL)
            {
                Actor* pActor = static_cast<Actor*>(pFixtureB->GetBody()->GetUserData());
                assert(pActor);

                shared_ptr<TriggerComponent> pTriggerComponent = GetTriggerComponentFromB2Body(pFixtureA->GetBody());
                if (pTriggerComponent)
                {
                    pTriggerComponent->OnActorLeft(pActor, fixtureType);
                }
            }
        }
    }
    // Enemy agro melee contact
    {
        TryCallActorLeftAgroRange<MeleeAttackAIStateComponent>(pFixtureA, pFixtureB, FixtureType_EnemyAIMeleeSensor);
    }
    // Enemy duck agro melee contact
    {
        TryCallActorLeftAgroRange<DuckMeleeAttackAIStateComponent>(pFixtureA, pFixtureB, FixtureType_EnemyAIDuckMeleeSensor);
    }
    // Enemy ranged agro range contact
    {
        TryCallActorLeftAgroRange<RangedAttackAIStateComponent>(pFixtureA, pFixtureB, FixtureType_EnemyAIRangedSensor);
    }
    // Enemy duck ranged agro range contact
    {
        TryCallActorLeftAgroRange<DuckRangedAttackAIStateComponent>(pFixtureA, pFixtureB, FixtureType_EnemyAIDuckRangedSensor);
    }
    // Enemy dive area agro range contact
    {
        TryCallActorLeftAgroRange<DiveAttackAIStateComponent>(pFixtureA, pFixtureB, FixtureType_EnemyAIDiveAreaSensor);
    }
    // Damage aura
    {
        if (pFixtureB->GetUserData() == (void*)FixtureType_DamageAura)
        {
            std::swap(pFixtureA, pFixtureB);
        }

        if (pFixtureA->GetUserData() == (void*)FixtureType_DamageAura)
        {
            if (pFixtureB->GetBody()->GetUserData() != NULL)
            {
                Actor* pActorwhoEntered = static_cast<Actor*>(pFixtureB->GetBody()->GetUserData());
                Actor* pActorWithDamageAura = static_cast<Actor*>(pFixtureA->GetBody()->GetUserData());

                if (pActorwhoEntered && pActorWithDamageAura)
                {
                    shared_ptr<DamageAuraComponent> pDamageAuraComponent =
                        MakeStrongPtr(pActorWithDamageAura->GetComponent<DamageAuraComponent>(DamageAuraComponent::g_Name));
                    if (pDamageAuraComponent)
                    {
                        pDamageAuraComponent->OnActorLeft(pActorwhoEntered);
                    }
                }
            }
        }
    }
}

void PhysicsContactListener::PreSolve(b2Contact* pContact, const b2Manifold* pOldManifold)
{
    
}

void PhysicsContactListener::PostSolve(b2Contact* pContact, const b2ContactImpulse* pImpulse)
{
    
}