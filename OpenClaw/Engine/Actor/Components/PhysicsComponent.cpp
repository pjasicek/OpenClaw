#include "PhysicsComponent.h"
#include "../../Physics/ClawPhysics.h"
#include "../../GameApp/BaseGameLogic.h"
#include "../../GameApp/BaseGameApp.h"
#include "RenderComponent.h"
#include "../../Graphics2D/Image.h"

#include "PositionComponent.h"
#include "ControllableComponent.h"
#include "ControllerComponents/PowerupComponent.h"

#include "../../Events/EventMgr.h"
#include "../../Events/Events.h"

const char* PhysicsComponent::g_Name = "PhysicsComponent";

PhysicsComponent::PhysicsComponent() :
    m_CanClimb(false),
    m_CanBounce(false),
    m_CanJump(false),
    m_MaxJumpHeight(0),
    m_GravityScale(0),
    m_Friction(0),
    m_Density(0),
    m_BodySize(Point(0, 0)),
    m_CurrentSpeed(Point(0, 0)),
    m_ConstantSpeed(Point(0, 0)),
    m_ClimbingSpeed(Point(0, 0)),
    m_HasConstantSpeed(false),
    m_NumFootContacts(0),
    m_IsClimbing(false),
    m_IsStopped(false),
    m_IsRunning(false),
    m_Direction(Direction_Right),
    m_IsFalling(false),
    m_IsJumping(false),
    m_IgnoreJump(false),
    m_HeightInAir(20),
    m_pControllableComponent(nullptr),
    m_pPhysics(nullptr),
    m_pTopLadderContact(NULL),
    m_pMovingPlatformContact(NULL),
    m_bClampToGround(false),
    m_DoNothingTimeout(0),
    m_bIsForcedUp(false),
    m_ForcedUpHeight(0),
    m_FallHeight(0.0f)
{ }

PhysicsComponent::~PhysicsComponent()
{
    m_pPhysics->VRemoveActor(m_pOwner->GetGUID());
}

bool PhysicsComponent::VInit(TiXmlElement* data)
{
    assert(data != NULL);

    m_pPhysics = g_pApp->GetGameLogic()->VGetGamePhysics();
    if (!m_pPhysics)
    {
        LOG_WARNING("Attemtping to create physics component without valid physics");
        return false;
    }

    if (TiXmlElement* pElem = data->FirstChildElement("CanClimb"))
    {
        m_CanClimb = std::string(pElem->GetText()) == "true";
    }
    if (TiXmlElement* pElem = data->FirstChildElement("CanBounce"))
    {
        m_CanBounce = std::string(pElem->GetText()) == "true";
    }
    if (TiXmlElement* pElem = data->FirstChildElement("CanJump"))
    {
        m_CanJump = std::string(pElem->GetText()) == "true";
    }
    if (TiXmlElement* pElem = data->FirstChildElement("JumpHeight"))
    {
        m_MaxJumpHeight = std::stoi(pElem->GetText());
    }
    if (TiXmlElement* pElem = data->FirstChildElement("GravityScale"))
    {
        m_ActorBodyDef.gravityScale = std::stof(pElem->GetText());

        // Backwards compatibility, can be removed in future
        m_GravityScale = std::stof(pElem->GetText());
    }
    

    TiXmlElement* pBodySizeElem = data->FirstChildElement("CollisionSize");
    if (pBodySizeElem)
    {
        pBodySizeElem->Attribute("width", &m_ActorBodyDef.size.x);
        pBodySizeElem->Attribute("height", &m_ActorBodyDef.size.y);

        // Backwards compatibility, can be removed in future
        pBodySizeElem->Attribute("width", &m_BodySize.x);
        pBodySizeElem->Attribute("height", &m_BodySize.y);
    }

    // ActorBodyDef addition

    // Allowed types are: "Static", "Kinematic" and "Dynamic"
    if (TiXmlElement* pElem = data->FirstChildElement("BodyType"))
    {
        m_ActorBodyDef.bodyType = BodyTypeStringToEnum(pElem->GetText());
    }
    if (TiXmlElement* pElem = data->FirstChildElement("HasFootSensor"))
    {
        m_ActorBodyDef.addFootSensor = std::string(pElem->GetText()) == "true";
    }
    if (TiXmlElement* pElem = data->FirstChildElement("HasCapsuleShape"))
    {
        m_ActorBodyDef.makeCapsule = std::string(pElem->GetText()) == "true";
    }
    if (TiXmlElement* pElem = data->FirstChildElement("HasBulletBehaviour"))
    {
        m_ActorBodyDef.makeBullet = std::string(pElem->GetText()) == "true";
    }
    if (TiXmlElement* pElem = data->FirstChildElement("HasSensorBehaviour"))
    {
        m_ActorBodyDef.makeSensor = std::string(pElem->GetText()) == "true";
    }
    // Allowed types are: "Solid", "Ground", "Climb", "Death", "Trigger", "Projectile"
    if (TiXmlElement* pElem = data->FirstChildElement("FixtureType"))
    {
        m_ActorBodyDef.fixtureType = FixtureTypeStringToEnum(pElem->GetText());
    }
    if (TiXmlElement* pElem = data->FirstChildElement("PositionOffset"))
    {
        pElem->Attribute("x", &m_ActorBodyDef.positionOffset.x);
        pElem->Attribute("y", &m_ActorBodyDef.positionOffset.y);
    }
    if (TiXmlElement* pElem = data->FirstChildElement("CollisionShape"))
    {
        m_ActorBodyDef.collisionShape = pElem->GetText();
    }
    if (TiXmlElement* pElem = data->FirstChildElement("HasInitialSpeed"))
    {
        m_ActorBodyDef.setInitialSpeed = std::string(pElem->GetText()) == "true";
    }
    if (TiXmlElement* pElem = data->FirstChildElement("HasInitialImpulse"))
    {
        m_ActorBodyDef.setInitialImpulse = std::string(pElem->GetText()) == "true";
    }
    if (TiXmlElement* pElem = data->FirstChildElement("InitialSpeed"))
    {
        pElem->Attribute("x", &m_ActorBodyDef.initialSpeed.x);
        pElem->Attribute("y", &m_ActorBodyDef.initialSpeed.y);
    }
    if (TiXmlElement* pElem = data->FirstChildElement("CollisionFlag"))
    {
        m_ActorBodyDef.collisionFlag = CollisionFlag(std::stoi(pElem->GetText()));
    }
    if (TiXmlElement* pElem = data->FirstChildElement("CollisionMask"))
    {
        m_ActorBodyDef.collisionMask = std::stoul(pElem->GetText());
    }
    if (TiXmlElement* pElem = data->FirstChildElement("Friction"))
    {
        m_ActorBodyDef.friction = std::stof(pElem->GetText());
    }
    if (TiXmlElement* pElem = data->FirstChildElement("Density"))
    {
        m_ActorBodyDef.density = std::stof(pElem->GetText());
    }
    if (TiXmlElement* pElem = data->FirstChildElement("Restitution"))
    {
        m_ActorBodyDef.restitution = std::stof(pElem->GetText());
    }
    if (TiXmlElement* pElem = data->FirstChildElement("PrefabType"))
    {
        //m_ActorBodyDef.prefabType = pElem->GetText();
    }

    for (TiXmlElement* pFixtureElem = data->FirstChildElement("ActorFixture");
        pFixtureElem != NULL; pFixtureElem = pFixtureElem->NextSiblingElement("ActorFixture"))
    {
        ActorFixtureDef fixtureDef = ActorTemplates::XmlToActorFixtureDef(pFixtureElem);

        m_ActorBodyDef.fixtureList.push_back(fixtureDef);
    }

    ParseValueFromXmlElem(&m_bClampToGround, data->FirstChildElement("ClampToGround"));

    return true;
}

void PhysicsComponent::VPostInit()
{
    shared_ptr<PositionComponent> pPositionComponent = m_pOwner->GetPositionComponent();
    assert(pPositionComponent);

    if (m_ActorBodyDef.collisionFlag != CollisionFlag_None)
    {
        m_ActorBodyDef.position = pPositionComponent->GetPosition();

        if (fabs(m_ActorBodyDef.size.x) < DBL_EPSILON || fabs(m_ActorBodyDef.size.y) < DBL_EPSILON)
        {
            shared_ptr<ActorRenderComponent> pRenderComponent =
                MakeStrongPtr(m_pOwner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name));
            assert(pRenderComponent);

            shared_ptr<Image> pImage = MakeStrongPtr(pRenderComponent->GetCurrentImage());
            assert(pImage != nullptr);

            // Also offset position
            m_ActorBodyDef.position = Point(m_ActorBodyDef.position.x + pImage->GetOffsetX(), m_ActorBodyDef.position.y + pImage->GetOffsetY());

            m_ActorBodyDef.size.x = pImage->GetWidth();
            m_ActorBodyDef.size.y = pImage->GetHeight();

            //LOG("-------- X: " + ToStr(pImage->GetOffsetX()) + " Y: " + ToStr(pImage->GetOffsetY()));

            for (ActorFixtureDef fixture : m_ActorBodyDef.fixtureList)
            {
                if (fixture.size.IsZero())
                {
                    fixture.size = Point(pImage->GetWidth(), pImage->GetHeight());
                }
            }

            // HACK:
//            if (m_pOwner->GetName() == "/LEVEL1/IMAGES/RATBOMB/*")
//            {
//                pImage->SetOffset(0, 0);
//            }
        }

        m_ActorBodyDef.position += m_ActorBodyDef.positionOffset;
        m_ActorBodyDef.pActor = m_pOwner;

        m_pPhysics->VAddActorBody(&m_ActorBodyDef);
    }
    else
    {
        m_pPhysics->VAddDynamicActor(m_pOwner);
    }
}

void PhysicsComponent::VPostPostInit()
{
    if (m_bClampToGround)
    {
        auto pPositionComponent = m_pOwner->GetPositionComponent();
        // Position enemy to the floor
        Point fromPoint = pPositionComponent->GetPosition();
        Point toPoint = pPositionComponent->GetPosition() + Point(0, 100);
        RaycastResult raycastDown = m_pPhysics->VRayCast(fromPoint, toPoint, (CollisionFlag_Solid | CollisionFlag_Ground | CollisionFlag_All));
        if (!raycastDown.foundIntersection)
        {
            LOG_ERROR("Failed to get raycast result down from position: " + pPositionComponent->GetPosition().ToString());
            return;
            //assert(raycastDown.foundIntersection && "Did not find intersection. Enemy is too far in the air with no ground below him");
        }

        double deltaY = raycastDown.deltaY - m_pPhysics->VGetAABB(m_pOwner->GetGUID(), true).h / 2;

        pPositionComponent->SetY(pPositionComponent->GetY() + deltaY - 1);
        const Point newPos = pPositionComponent->GetPosition();
        m_pPhysics->VSetPosition(m_pOwner->GetGUID(), newPos);
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
                new EventData_Move_Actor(m_pOwner->GetGUID(), newPos)));
    }
}

TiXmlElement* PhysicsComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    //

    return baseElement;
};

//-----------------------------------------------------------------------------
// PhysicsComponent::VUpdate
//
//    Used to control movement of:
//      - Controlled actor (Claw)
//
// TODO: HACK: (so I can easily search hacks and stuff): Note that this piece of code is absolutely terrible
// since I dont know how to properly implement character control system. I really am aware that this
// horrible state machine is pretty far from being acceptable. Given time this should be refactored.
//
void PhysicsComponent::VUpdate(uint32 msDiff)
{
    // Just move this to ClawControllerComponent update......................
    if (!m_pControllableComponent)
    {
        return;
    }

    assert(m_NumFootContacts >= 0);
    //assert(m_IsJumping && m_IsFalling && "Cannot be jumping and falling at the same time");

    /*LOG("Jumping: " + ToStr((m_IsJumping)) + ", Falling: " + ToStr(m_IsFalling) + ", JumpHeight: " + ToStr(m_HeightInAir) + ", NumFootContacts: " + ToStr(m_NumFootContacts));
    LOG("Movement: " + m_CurrentSpeed.ToString());
    LOG(ToStr(m_OverlappingLaddersList.size()));
    LOG("Vel X: " + ToStr(GetVelocity().x) + ", Vel Y: " + ToStr(GetVelocity().y));
    LOG(ToStr(m_OverlappingKinematicBodiesList.size()));
    
    LOG("CLIMBING Y: " + ToStr(m_ClimbingSpeed.y));*/
    
    //LOG("ROPE_1: " + ToStr(m_pControllableComponent->VIsAttachedToRope()));


    if (!m_pControllableComponent->CanMove() && m_pControllableComponent->IsActorFrozen())
    {
        m_pControllableComponent->AddFrozenTime(msDiff);
    }
    else
    {
        m_pControllableComponent->SetFrozenTime(0);
    }

    m_DoNothingTimeout -= msDiff;
    if (m_DoNothingTimeout > 0)
    {
        SetVelocity(Point(0, 0));
        m_pPhysics->VSetGravityScale(m_pOwner->GetGUID(), 0.0f);
        m_CurrentSpeed.Set(0, 0);
        return;
    }

    if (m_pControllableComponent->VIsAttachedToRope())
    {
        m_CurrentSpeed.SetX(0);
        if (m_ClimbingSpeed.y > 0)
        {
            m_ClimbingSpeed.y = 0;
        }
    }

    if (!m_IsJumping)
    {
        m_MaxJumpHeight = m_pControllableComponent->GetMaxJumpHeight();
    }

    if (m_bIsForcedUp)
    {
        if (m_IsFalling || m_pControllableComponent->VIsAttachedToRope() || m_pControllableComponent->IsClimbing())
        {
            m_bIsForcedUp = false;
            m_ForcedUpHeight = 0;
            m_MaxJumpHeight = (int)g_pApp->GetGlobalOptions()->maxJumpHeight;
        }
        else
        {
            m_MaxJumpHeight = m_ForcedUpHeight;
            m_CurrentSpeed.y = -10;
        }
    }


    if (m_OverlappingKinematicBodiesList.empty())
    {
        m_ExternalSourceSpeed.Set(0, 0);
    }

    if (m_pControllableComponent && !m_pControllableComponent->InPhysicsCapableState())
    {
        SetVelocity(Point(0, 0));
        m_CurrentSpeed = Point(0, 0);
        m_ClimbingSpeed = Point(0, 0);
        m_IsClimbing = false;
        // When Claw takes damage while ducking he stands up.. TODO: Think of better solution
        m_pPhysics->VScaleActor(m_pOwner->GetGUID(), 2.0);

        m_pControllableComponent->SetDuckingTime(0);
        m_pControllableComponent->SetLookingUpTime(0);

        return;
    }

    if (m_IsClimbing)
    {
        m_HeightInAir = 0;
    }

    if (m_pControllableComponent && m_pControllableComponent->CanMove() 
        && (m_pControllableComponent->IsDucking() && fabs(m_ClimbingSpeed.y) < DBL_EPSILON)
        && IsOnGround())
    {
        m_pControllableComponent->OnStand();
        // TODO: HACK: one of the biggest hacks so far
        m_pPhysics->VScaleActor(m_pOwner->GetGUID(), 2.0);
    }

    if (m_pControllableComponent && 
        (!m_pControllableComponent->CanMove() || 
        (m_pControllableComponent->IsDucking() && m_ClimbingSpeed.y > DBL_EPSILON)))
    {
        if (fabs(m_CurrentSpeed.x) > DBL_EPSILON)
        {
            Direction direction = m_CurrentSpeed.x < 0 ? Direction_Left : Direction_Right;
            //m_pControllableComponent->VOnDirectionChange(m_Direction);

            SetDirection(direction);
        }

        //LOG("RETURN 1");
        Point currSpeed = GetVelocity();
        SetVelocity(Point(0, currSpeed.y));
        if (m_pMovingPlatformContact)
        {
            m_pMovingPlatformContact->SetFriction(100.0f);
        }

        if (m_ClimbingSpeed.y > DBL_EPSILON)
        {
            m_pControllableComponent->AddDuckingTime(msDiff);
            m_pControllableComponent->SetLookingUpTime(0);
        }
        else if (m_ClimbingSpeed.y < (-1.0 * DBL_EPSILON))
        {
            m_pControllableComponent->AddLookingUpTime(msDiff);
            m_pControllableComponent->SetDuckingTime(0);
        }
        else
        {
            m_pControllableComponent->SetLookingUpTime(0);
            m_pControllableComponent->SetDuckingTime(0);
        }

        m_CurrentSpeed = Point(0, 0);
        m_ClimbingSpeed = Point(0, 0);

        return;
    }

    // I will give 100$ to someone who will refactor this method so that it is extendable
    // and does not contain uncomprehandable state machine

    //LOG("Climbing: " + ToStr(m_IsClimbing));
    if (m_IsClimbing)
    {
        // TODO: Climbing up when on top of the ladder should be disabled
        /*if (m_ClimbingSpeed.y < (-1.0 * DBL_EPSILON) && m_pTopLadderContact && m_OverlappingLaddersList.size() > 0)
        {
            LOG(".");
            m_ClimbingSpeed.Set(0, 0);
            m_CurrentSpeed.Set(0, 0);
            m_IgnoreJump = false;
            m_IsClimbing = false;
            m_IsJumping = false;
            m_IsFalling = false;
            m_HeightInAir = 0;
            m_pPhysics->VSetGravityScale(m_pOwner->GetGUID(), m_GravityScale);
            //m_pControllableComponent->VOnStopMoving();
            m_pPhysics->VSetLinearSpeedEx(m_pOwner->GetGUID(), Point(0,0));
            return;
        }*/

        if (fabs(m_ClimbingSpeed.y) > DBL_EPSILON)
        {
            m_CurrentSpeed = Point(0, 0);
            m_IgnoreJump = true;
        }
        else
        {
            m_IgnoreJump = false;
        }

        //LOG("IgnoreJump: " + ToStr(m_IgnoreJump));
        //if (m_IgnoreJump && m_CanJump)
        if (!m_IgnoreJump && (fabs(m_CurrentSpeed.y) > DBL_EPSILON) &&
            fabs(m_ClimbingSpeed.y) < DBL_EPSILON)
        {
            //LOG("GOTO");
            //LOG("1");
            m_pControllableComponent->VOnStartJumping();
            m_IgnoreJump = false;
            m_IsClimbing = false;
            m_IsJumping = true;
            m_IsFalling = false;
            m_HeightInAir = 0;
            m_pPhysics->VSetGravityScale(m_pOwner->GetGUID(), m_GravityScale);
            goto set_velocity;
        }
        else if (!m_IgnoreJump && (fabs(m_CurrentSpeed.y) > DBL_EPSILON))
        {
            //LOG(".");
            //LOG("2");
            m_IsClimbing = false;
            m_IsJumping = true;
            m_IsFalling = false;
            m_HeightInAir = 0;
            m_pPhysics->VSetGravityScale(m_pOwner->GetGUID(), m_GravityScale);
        }
        if (m_OverlappingLaddersList.empty())
        {
            m_IsClimbing = false;
            m_pPhysics->VSetGravityScale(m_pOwner->GetGUID(), m_GravityScale);
        }
        else
        {
            //LOG("ClimbingSpeed: " + ToStr(m_ClimbingSpeed.y));
            m_pPhysics->VSetLinearSpeedEx(m_pOwner->GetGUID(), Point(0, m_ClimbingSpeed.y));
            if (m_pControllableComponent && fabs(m_ClimbingSpeed.y) < DBL_EPSILON)
            {
                m_pControllableComponent->VOnStopClimbing();
            }
            else
            {
                // If on top of the ladder climb through the artificial ground patch
                if (m_pTopLadderContact != NULL)
                {
                    m_pTopLadderContact->SetEnabled(false);
                }

                bool bIsOnTopLadder = CheckOverlap(FixtureType_TopLadderGround);
                bool bIsClimbingUp = m_ClimbingSpeed.y < (-1.0 * DBL_EPSILON);
                m_pControllableComponent->VOnClimb(bIsClimbingUp, bIsOnTopLadder);
            }
            m_ClimbingSpeed = Point(0, 0);

            m_pControllableComponent->SetDuckingTime(0);
            m_pControllableComponent->SetLookingUpTime(0);

            return;
        }
    }
    
    if (m_pControllableComponent)
    {
        if (!m_IsClimbing && !IsInAir() && m_ClimbingSpeed.y > DBL_EPSILON &&
            (GetVelocity().y < 0.1 || IsOnGround()))
        {
            m_pControllableComponent->OnDuck();
            // TODO: HACK: one of the biggest hacks so far
            m_pPhysics->VScaleActor(m_pOwner->GetGUID(), 0.5);

            if (fabs(m_CurrentSpeed.x) > DBL_EPSILON)
            {
                Direction direction = m_CurrentSpeed.x < 0 ? Direction_Left : Direction_Right;
                //m_pControllableComponent->VOnDirectionChange(m_Direction);

                SetDirection(direction);
            }

            SetVelocity(Point(0, 0));
            m_CurrentSpeed = Point(0, 0);
            m_ClimbingSpeed = Point(0, 0);
            return;
        }
        else if (m_pControllableComponent->IsDucking())
        {
            m_pControllableComponent->OnStand();
        }

        if (!m_pControllableComponent->IsDucking())
        {
            // TODO: HACK: one of the biggest hacks so far
            m_pPhysics->VScaleActor(m_pOwner->GetGUID(), 2.0);
        }
    }

    if (!m_IsClimbing && !IsInAir() && m_ClimbingSpeed.y > 0)
    {
        
    }

    /*if (m_OverlappingLaddersList.size() > 1)
        LOG(ToStr(m_OverlappingLaddersList.size()));*/
    // This should be available only to controlled actors
    if (m_CanJump)
    {
        //LOG("Pre CurrentSpeed: " + m_CurrentSpeed.ToString());

        //LOG(ToStr(GetVelocity().y));
        // This is to ensure one jump per one space press
        if (m_IsClimbing)
        {
        
        }
        // "20" lets us skip uneven ground and therefore skip spamming transition between falling/jumping
        else if (m_HeightInAir > 20 && fabs(GetVelocity().y) < FLT_EPSILON && !m_pControllableComponent->VIsAttachedToRope())
        {
            m_IgnoreJump = true;
            m_CurrentSpeed.y = 0;
        }
        else if (fabs(m_CurrentSpeed.y) < DBL_EPSILON && (GetVelocity().y < FLT_EPSILON) && IsInAir())
        {
            m_IgnoreJump = true;
            m_CurrentSpeed.y = 0;
        }
        else if (m_IgnoreJump && 
                (fabs(m_CurrentSpeed.y) < DBL_EPSILON) && 
                (m_NumFootContacts > 0 || m_pControllableComponent->VIsAttachedToRope()))
        {
            m_IgnoreJump = false;
        }
        else if (m_IgnoreJump)
        {
            m_CurrentSpeed.y = 0;
        }
        else if (m_IsFalling && m_NumFootContacts == 0)
        {
            m_IgnoreJump = true;
            m_CurrentSpeed.y = 0;
        }
        if (!g_pApp->GetGameCheats()->clawInfiniteJump && (m_HeightInAir > m_MaxJumpHeight))
        {
            m_IgnoreJump = true;
            m_CurrentSpeed.y = 0;
        }

        //LOG("Post CurrentSpeed: " + m_CurrentSpeed.ToString());

set_velocity:

        //=====================================================================
        // Set velocity here
        //=====================================================================
        Point velocity = GetVelocity();

        double ySpeed = m_CurrentSpeed.y;
        if (m_pOwner->GetName() == "Claw")
        {
            //LOG("CurrentSpeed.y: " + ToStr(m_CurrentSpeed.y));
        }
        if (ySpeed < 0)
        {
            SetVelocity(Point(velocity.x, -8.8));

        }
        else if (velocity.y < -2 && ySpeed >= 0)
        {
            SetVelocity(Point(velocity.x, -2));
        }
        velocity = GetVelocity();

        if (fabs(m_CurrentSpeed.x) > DBL_EPSILON)
        {
            double runSpeed = g_pApp->GetGlobalOptions()->runSpeed;
            if (auto pPowerupComp = MakeStrongPtr(m_pOwner->GetComponent<PowerupComponent>()))
            {
                if (pPowerupComp->HasPowerup(PowerupType_Catnip))
                {
                    runSpeed = g_pApp->GetGlobalOptions()->powerupRunSpeed;
                }
            }
            SetVelocity(Point(m_CurrentSpeed.x < 0 ? -1.0 * runSpeed : runSpeed, velocity.y));
        }
        else
        {
            SetVelocity(Point(0, velocity.y));
        }

        if (m_pMovingPlatformContact && fabs(m_CurrentSpeed.x) < DBL_EPSILON)
        {
            // Not moving while getting carried, set high friction
            m_pMovingPlatformContact->SetFriction(100.0f);
        }
        else if (m_pMovingPlatformContact && (fabs(m_CurrentSpeed.x) > DBL_EPSILON))
        {
            // Moving while on platform
            m_pMovingPlatformContact->SetFriction(0.0f);
            Point externalSourceSpeedX(m_ExternalSourceSpeed.x, 0);
            m_pPhysics->VAddLinearSpeed(m_pOwner->GetGUID(), externalSourceSpeedX);

            // If moving on platform, be slower
            velocity = GetVelocity();
            velocity.Set((velocity.x * 2) / 3, velocity.y);
            SetVelocity(velocity);
        }
        else if (fabs(m_ExternalConveyorBeltSpeed.x) > DBL_EPSILON)
        {
            m_pPhysics->VAddLinearSpeed(m_pOwner->GetGUID(), m_ExternalConveyorBeltSpeed);
            m_ExternalConveyorBeltSpeed.Set(0.0, 0.0);
        }

        bool applyForce = true;
        // TODO: Add config to choose between fixed physics timestep and variable
        /*if (true)
        {
            if (m_CurrentSpeed.y > -2)
            {
                ApplyForce(m_pPhysics->GetGravity());
            }
            //ApplyForce(m_pPhysics->GetGravity());
        }
        else
        {
            static uint32 timeSinceLastUpdate = 0;
            const uint32 updateInterval = 1000 / 120;

            timeSinceLastUpdate += msDiff;
            if (timeSinceLastUpdate >= updateInterval)
            {
                ApplyForce(m_pPhysics->GetGravity());

                timeSinceLastUpdate = 0;
            }
        }*/

        if (m_pControllableComponent->VIsAttachedToRope())
        {
            applyForce = false;
        }

        bool disableGravity = false;
        Point gravity = m_pPhysics->GetGravity();
        velocity = GetVelocity();

        double maxJumpSpeed = -1.0 * fabs(g_pApp->GetGlobalOptions()->maxJumpSpeed);
        double maxFallSpeed = fabs(g_pApp->GetGlobalOptions()->maxFallSpeed);

        if (velocity.y < maxJumpSpeed)
        {
            SetVelocity(Point(velocity.x, maxJumpSpeed));
            applyForce = false;
        }
        if (velocity.y > maxFallSpeed)
        {
            //LOG("Velocity: " + ToStr(velocity.y));
            SetVelocity(Point(velocity.x, maxFallSpeed));
            applyForce = false;
        }
        if (applyForce)
        {
            m_pPhysics->VSetGravityScale(m_pOwner->GetGUID(), m_GravityScale);
            //ApplyForce(m_pPhysics->GetGravity());
        }
        else
        {
            m_pPhysics->VSetGravityScale(m_pOwner->GetGUID(), 0.0f);
        }

        if (m_bIsForcedUp)
        {
            velocity = GetVelocity();

            double springSpeed = -1.0 * fabs(g_pApp->GetGlobalOptions()->springBoardSpringSpeed);
            SetVelocity(Point(velocity.x, springSpeed));
        }

        /*if (true && applyForce)
        {
            if (m_CurrentSpeed.y > -2)
            {
                ApplyForce(m_pPhysics->GetGravity());
            }
            //ApplyForce(m_pPhysics->GetGravity());
        }*/
        /*else
        {
            static uint32 timeSinceLastUpdate = 0;
            const uint32 updateInterval = 1000 / 120;

            timeSinceLastUpdate += msDiff;
            if (timeSinceLastUpdate >= updateInterval)
            {
                ApplyForce(m_pPhysics->GetGravity());

                timeSinceLastUpdate = 0;
            }
        }*/

        //=====================================================================

        if (m_IsJumping || m_IsFalling)
        {
            m_IsRunning = false;
            m_IsStopped = false;
        }
        
        bool wasRunning = m_IsRunning;
        bool wasStopped = m_IsStopped;
        Direction prevDirection = m_Direction;
        Direction currDirection = m_Direction;

        velocity = GetVelocity();
        if (fabs(velocity.x) > DBL_EPSILON)
        {
            currDirection = velocity.x < 0 ? Direction_Left : Direction_Right;
            if (!IsInAir() && IsOnGround())
            {
                m_IsRunning = true;
                m_IsStopped = false;
            }
        }
        else
        {
            if (!IsInAir() && IsOnGround())
            {
                m_IsStopped = true;
                m_IsRunning = false;
            }
        }
         
        //LOG("Running: " + ToStr(m_IsRunning) + ", Stopped: " + ToStr(m_IsStopped));
        if (m_pControllableComponent)
        {
            if (prevDirection != currDirection)
            {
                SetDirection(currDirection);
            }
            if (m_IsRunning && IsOnGround()) // TODO: Dont poll here. State changing didnt work.
            {
                m_pControllableComponent->VOnRun();
            }
            else if (m_IsStopped && (((fabs(GetVelocity().y) < DBL_EPSILON) && (fabs(GetVelocity().x) < DBL_EPSILON)) || !m_OverlappingKinematicBodiesList.empty()) && IsOnGround())
            {
                m_pControllableComponent->VOnStopMoving();
            }

            //LOG(ToStr(m_pPhysics->VGetVelocity(m_pOwner->GetGUID()).x) + " - " + ToStr(m_pPhysics->VGetVelocity(m_pOwner->GetGUID()).y));
        }
        
    }

    /*if (m_HasConstantSpeed)
    {
        m_pPhysics->VSetLinearSpeed(m_pOwner->GetGUID(), m_ConstantSpeed);
    }*/

    if (m_ClimbingSpeed.y < (-1.0 * DBL_EPSILON))
    {
        m_pControllableComponent->AddLookingUpTime(msDiff);
    }
    else
    {
        m_pControllableComponent->SetLookingUpTime(0);
        m_pControllableComponent->SetDuckingTime(0);
    }

    if (m_pControllableComponent->VIsAttachedToRope())
    {
        if (!m_IgnoreJump && m_CurrentSpeed.y <= (-1.0 * DBL_EPSILON))
        {
            m_pControllableComponent->VDetachFromRope();
        }

        if (m_CurrentSpeed.y >= (-1.0 * DBL_EPSILON))
        {
            SetVelocity(Point(0, 0));
            m_pPhysics->VSetGravityScale(m_pOwner->GetGUID(), 0.0f);
        }

        //LOG("CurrentSpeed: " + m_CurrentSpeed.ToString());
    }

    //LOG("ROPE_2: " + ToStr(m_pControllableComponent->VIsAttachedToRope()));

    m_CurrentSpeed.Set(0, 0);
    m_ClimbingSpeed = Point(0, 0);
}

// Events
void PhysicsComponent::OnBeginFootContact()
{
    if (m_NumFootContacts == 0)
    {
        if (m_pControllableComponent && (m_HeightInAir > 2 /*|| !m_OverlappingKinematicBodiesList.empty()*/))
        {
            m_pControllableComponent->VOnLandOnGround(m_FallHeight);
        }
    }

    m_NumFootContacts++; 
    //LOG(ToStr(m_HeightInAir));
    if (!m_bIsForcedUp)
    {
        m_HeightInAir = 0;
    }
}

void PhysicsComponent::OnEndFootContact()
{ 
    m_NumFootContacts--; 
}

void PhysicsComponent::OnStartFalling()
{ 
    if (m_DoNothingTimeout > 0 || (m_pControllableComponent && m_pControllableComponent->VIsAttachedToRope()))
    {
        return;
    }

    if (m_IsClimbing)
    {
        return;
    }

    if (m_pControllableComponent && !m_pControllableComponent->InPhysicsCapableState())
    {
        return;
    }

    //LOG("FALL");
    m_IsFalling = true;
    if (m_pControllableComponent)
    {
        m_pControllableComponent->VOnStartFalling();
    }
}

void PhysicsComponent::OnStartJumping()
{  
    if (m_DoNothingTimeout > 0)
    {
        return;
    }

    if (m_IsClimbing)
    {
        return;
    }

    if (m_pControllableComponent && !m_pControllableComponent->InPhysicsCapableState())
    {
        return;
    }

    //LOG("JUMP");
    m_IsJumping = true;
    if (m_pControllableComponent)
    {
        m_pControllableComponent->VOnStartJumping();
    }
}

// HACK: TODO:
// Forcing falling / jumping events to fire only when it is really happening
// Also ignoring invisible bumps on tiles (2 pixel tolerancy, probably too much)
void PhysicsComponent::SetFalling(bool falling)
{
    if (m_DoNothingTimeout > 0 || (m_pControllableComponent && m_pControllableComponent->VIsAttachedToRope()))
    {
        return;
    }

    if (falling)
    {
        if (m_HeightInAir < 5 && (GetVelocity().y < 5))
        {
            return;
        }
        else
        {
            //LOG(ToStr(m_HeightInAir) + " - " + ToStr(GetVelocity().y));
            m_IsFalling = true;
        }
    }
    else
    {
        m_IsFalling = false;
    }
}

void PhysicsComponent::SetJumping(bool jumping)
{
    if (m_DoNothingTimeout > 0 || (m_pControllableComponent && m_pControllableComponent->VIsAttachedToRope() && !m_IgnoreJump))
    {
        return;
    }

    if (jumping)
    {
        if (m_HeightInAir < 2)
        {
            //LOG(ToStr(m_HeightInAir));
            //LOG("RETURNING");
            return;
        }
        else
        {
            m_IsJumping = true;
        }
    }
    else
    {
        m_IsJumping = false;
    }
}

bool PhysicsComponent::AttachToLadder()
{
    if (!m_CanClimb)
    {
        return false;
    }

    if (!m_pControllableComponent->InPhysicsCapableState())
    {
        return false;
    }

    // Unit can attach to ladder but check if it is standing on ground and that the ground
    // is not the top-side ladder ground patch
    if (m_ClimbingSpeed.y > DBL_EPSILON)
    {
        if (m_pTopLadderContact == NULL && m_NumFootContacts > 0)
        {
            return false;
        }
    }

    shared_ptr<PositionComponent> pPositionComponent = m_pOwner->GetPositionComponent();
    assert(pPositionComponent);

    Point actorPosition = pPositionComponent->GetPosition();
    for (const b2Fixture* pLadderFixture : m_OverlappingLaddersList)
    {
        // Assume ladder is rectangular which means I can aswell use its aabb for exact position
        b2AABB aabb = pLadderFixture->GetAABB(0);

        b2Vec2 b2bottomRight = MetersToPixels(aabb.upperBound);
        Point bottomright = b2Vec2ToPoint(b2bottomRight);

        b2Vec2 b2topLeft = MetersToPixels(aabb.lowerBound);
        Point topleft = b2Vec2ToPoint(b2topLeft);

        b2Vec2 b2center = MetersToPixels(aabb.GetCenter());
        Point center = b2Vec2ToPoint(b2center);

        if (actorPosition.x > topleft.x && actorPosition.x < bottomright.x)
        {
            m_pPhysics->VSetPosition(m_pOwner->GetGUID(), Point(center.x, actorPosition.y));
            //LOG("CAN CLIMB");
            return true;
        }
    }

    return false;
}

void PhysicsComponent::RequestClimb(Point climbMovement)
{
    m_ClimbingSpeed = climbMovement;

    if (!m_IsClimbing)
    {
        if (AttachToLadder())
        {
            bool bIsOnTopLadder = CheckOverlap(FixtureType_TopLadderGround);
            bool bIsClimbingUp = m_ClimbingSpeed.y < (-1.0 * DBL_EPSILON);
            m_pControllableComponent->VOnClimb(bIsClimbingUp, bIsOnTopLadder);

            m_IsClimbing = true;
            m_IgnoreJump = true;
            m_pPhysics->VSetGravityScale(m_pOwner->GetGUID(), 0);
        }
    }
}

bool PhysicsComponent::CheckOverlap(FixtureType withWhatFixture)
{
    return m_pPhysics->VIsActorOverlap(m_pOwner->GetGUID(), withWhatFixture);
}

void PhysicsComponent::RemoveOverlappingLadder(const b2Fixture* ladder)
{
    for (auto iter = m_OverlappingLaddersList.begin(); iter != m_OverlappingLaddersList.end(); ++iter)
    {
        if ((*iter) == ladder)
        {
            m_OverlappingLaddersList.erase(iter);
            return;
        }
    }
}

void PhysicsComponent::AddOverlappingGround(const b2Fixture* pGround)
{
    //LOG_ERROR("owner: " + m_pOwner->GetName());
    if (std::find(m_OverlappingGroundsList.begin(), m_OverlappingGroundsList.end(), pGround) == m_OverlappingGroundsList.end())
    {
        m_OverlappingGroundsList.push_back(pGround); 
        OnBeginFootContact();
    }
}

void PhysicsComponent::RemoveOverlappingGround(const b2Fixture* pGround)
{
    for (auto iter = m_OverlappingGroundsList.begin(); iter != m_OverlappingGroundsList.end(); ++iter)
    {
        if ((*iter) == pGround)
        {
            m_OverlappingGroundsList.erase(iter);
            OnEndFootContact();
            return;
        }
    }
    LOG_WARNING("Could not remove overlapping ground - no such fixture found")
}

void PhysicsComponent::AddOverlappingKinematicBody(const b2Body* pBody)
{
    if (std::find(m_OverlappingKinematicBodiesList.begin(), m_OverlappingKinematicBodiesList.end(), pBody) == m_OverlappingKinematicBodiesList.end())
    {
        m_OverlappingKinematicBodiesList.push_back(pBody);
    }
}

void PhysicsComponent::RemoveOverlappingKinematicBody(const b2Body* pBody)
{
    for (auto iter = m_OverlappingKinematicBodiesList.begin(); iter != m_OverlappingKinematicBodiesList.end(); ++iter)
    {
        if ((*iter) == pBody)
        {
            m_OverlappingKinematicBodiesList.erase(iter);
            return;
        }
    }
    LOG_WARNING("Could not remove overlapping kinematic body - no such body found")
}

void PhysicsComponent::SetForceFall()
{
    m_IsRunning = false;
    m_bIsForcedUp = false;
    m_IsStopped = false;
    m_IsFalling = true;
    m_IsJumping = false;
    m_HeightInAir = 0;
    m_IgnoreJump = true;
    SetVelocity(Point(GetVelocity().x, 0));
    if (m_pControllableComponent)
    {
        m_pControllableComponent->VOnStartFalling();
    }
}

void PhysicsComponent::OnAttachedToRope()
{
    m_IgnoreJump = true;
    m_HeightInAir = 0;
    m_IsJumping = false;
    m_IsFalling = false;
    m_DoNothingTimeout = 250;
}

void PhysicsComponent::OnDetachedFromRope()
{
    m_HeightInAir = 0;
}

void PhysicsComponent::SetIsForcedUp(bool isForcedUp, int forcedUpHeight)
{
    m_bIsForcedUp = isForcedUp;
    if (isForcedUp)
    {
        m_IsJumping = true;
        m_IsFalling = false;
        m_ForcedUpHeight = forcedUpHeight;
        m_MaxJumpHeight = m_ForcedUpHeight;
    }
    else
    {
        m_ForcedUpHeight = 0;
    }
}

void PhysicsComponent::SetMaxJumpHeight(int32 maxJumpHeight)
{
    //m_MaxJumpHeight = maxJumpHeight; 
    // HACK:
    if (m_pControllableComponent)
    {
        m_pControllableComponent->SetMaxJumpHeight(maxJumpHeight);
    }
}

void PhysicsComponent::SetControllableComponent(ControllableComponent* pComp)
{
    m_pControllableComponent = pComp;
    // HACK:
    if (m_pControllableComponent)
    {
        m_pControllableComponent->SetMaxJumpHeight(m_MaxJumpHeight);
    }
}

void PhysicsComponent::SetDirection(Direction newDirection)
{
    if (m_Direction == newDirection)
    {
        return;
    }

    if (m_pControllableComponent && m_pControllableComponent->VOnDirectionChange(newDirection))
    {
        m_Direction = newDirection;
    }
}