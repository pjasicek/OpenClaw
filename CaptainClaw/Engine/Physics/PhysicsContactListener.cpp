#include "PhysicsContactListener.h"
#include "ClawPhysics.h"
#include "../SharedDefines.h"
#include "../Actor/Actor.h"
#include "../Actor/Components/PhysicsComponent.h"
#include "../Actor/Components/KinematicComponent.h"
#include "../Actor/Components/AIComponents/CrumblingPegAIComponent.h"

int numFootContacts = 0;

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
            if (pFixtureB->GetUserData() == (void*)FixtureType_Solid)
            {
                shared_ptr<PhysicsComponent> pPhysicsComponent = GetPhysicsComponentFromB2Body(pFixtureA->GetBody());
                pPhysicsComponent->OnBeginFootContact();
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
                pPhysicsComponent->AddOverlappingLadder(pFixtureA);
            }
        }
    }
    // Collision with "One-Way Ground" tile - mostly platforms, elevators and such
    {
        if (pFixtureB->GetUserData() == (void*)FixtureType_Ground)
        {
            std::swap(pFixtureA, pFixtureB);
        }

        if (pFixtureA->GetUserData() == (void*)FixtureType_Ground)
        {
            if (pFixtureB->GetBody()->GetType() == b2_dynamicBody)
            {
                shared_ptr<PhysicsComponent> pPhysicsComponent = GetPhysicsComponentFromB2Body(pFixtureB->GetBody());
                //LOG("bodyAABB y: " + ToStr(MetersToPixels(bodyAABB.upperBound.y)) + ", Fixture lower: " + ToStr(MetersToPixels(pFixtureA->GetAABB(0).lowerBound.y)));
                b2AABB bodyAABB = GetBodyAABB(pFixtureB->GetBody());
                if (/*pFixtureB->GetBody()->GetLinearVelocity().y >= 0 &&*/
                    (bodyAABB.upperBound.y - PixelsToMeters(5)) < pFixtureA->GetAABB(0).lowerBound.y)
                {
                    //LOG("ENTERING");
                    pFixtureA->SetSensor(false);
                    pPhysicsComponent->AddOverlappingGround(pFixtureA);
                }
                // Moving platform (elevator)
                if (!pFixtureA->IsSensor() && pFixtureA->GetBody()->GetType() == b2_kinematicBody && !pFixtureB->IsSensor())
                {
                    //LOG("ADDED");
                    shared_ptr<KinematicComponent> pKinematicComponent = GetKinematicComponentFromB2Body(pFixtureA->GetBody());
                    pKinematicComponent->AddCarriedBody(pFixtureB->GetBody());
                    pPhysicsComponent->AddOverlappingKinematicBody(pFixtureA->GetBody());
                }

                // TODO: HACK: Crumbling peg, hackerino but who cares
                if (!pFixtureA->IsSensor() && !pFixtureB->IsSensor() && 
                    pFixtureA->GetBody()->GetType() == b2_staticBody && pFixtureA->GetBody()->GetUserData())
                {
                    Actor* pActor = static_cast<Actor*>(pFixtureA->GetBody()->GetUserData());
                    assert(pActor);

                    shared_ptr<CrumblingPegAIComponent> pCrumblingPegComponent =
                        MakeStrongPtr(pActor->GetComponent<CrumblingPegAIComponent>(CrumblingPegAIComponent::g_Name));
                    if (pCrumblingPegComponent)
                    {
                        pCrumblingPegComponent->OnContact(pFixtureB->GetBody());
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
            if (pFixtureB->GetUserData() == (void*)FixtureType_Solid)
            {
                shared_ptr<PhysicsComponent> pPhysicsComponent = GetPhysicsComponentFromB2Body(pFixtureA->GetBody());
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
                pPhysicsComponent->RemoveOverlappingLadder(pFixtureA);
            }
        }
    }
    // Collision with "One-Way Ground" tile - mostly platforms, elevators and such
    {
        if (pFixtureB->GetUserData() == (void*)FixtureType_Ground)
        {
            std::swap(pFixtureA, pFixtureB);
        }

        if (pFixtureA->GetUserData() == (void*)FixtureType_Ground)
        {
            if (pFixtureB->GetBody()->GetType() == b2_dynamicBody)
            {
                shared_ptr<PhysicsComponent> pPhysicsComponent = GetPhysicsComponentFromB2Body(pFixtureB->GetBody());

                // Moving platform (elevator)
                if (!pFixtureA->IsSensor() && pFixtureA->GetBody()->GetType() == b2_kinematicBody && !pFixtureB->IsSensor())
                {
                    //LOG("REMOVED");
                    shared_ptr<KinematicComponent> pKinematicComponent = GetKinematicComponentFromB2Body(pFixtureA->GetBody());
                    pKinematicComponent->RemoveCarriedBody(pFixtureB->GetBody());
                    pPhysicsComponent->RemoveOverlappingKinematicBody(pFixtureA->GetBody());
                }

                if (!pFixtureA->IsSensor())
                {
                    //LOG("LEAVING");
                    pPhysicsComponent->RemoveOverlappingGround(pFixtureA);
                }
                pFixtureA->SetSensor(true);
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