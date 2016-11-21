#include <Tinyxml\tinyxml.h>
#include "PhysicsComponent.h"
#include "../../Physics/ClawPhysics.h"
#include "../../GameApp/BaseGameLogic.h"
#include "../../GameApp/BaseGameApp.h"

#include "PositionComponent.h"
#include "ControllableComponent.h"

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
    m_pPhysics(nullptr)
{ }

PhysicsComponent::~PhysicsComponent()
{
    
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
        m_GravityScale = std::stof(pElem->GetText());
    }
    if (TiXmlElement* pElem = data->FirstChildElement("Friction"))
    {
        m_Friction = std::stof(pElem->GetText());
    }
    if (TiXmlElement* pElem = data->FirstChildElement("Density"))
    {
        m_Density = std::stof(pElem->GetText());
    }

    TiXmlElement* pBodySizeElem = data->FirstChildElement("CollisionSize");
    if (pBodySizeElem)
    {
        pBodySizeElem->Attribute("width", &m_BodySize.x);
        pBodySizeElem->Attribute("height", &m_BodySize.y);
    }

    return true;
}

void PhysicsComponent::VPostInit()
{
    m_pPhysics->VAddDynamicActor(_owner);
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
//      - Controlled actor
//      - Kinematic actors (moving platforms)
//      - AI actors (enemies)
//
// TODO: HACK: (so I can easily search hacks and stuff): Note that this piece of code is absolutely terrible
// since I dont know how to properly implement character control system. I really am aware that this
// horrible state machine is pretty far from being acceptable. Given time this should be refactored.
//
void PhysicsComponent::VUpdate(uint32 msDiff)
{
    assert(m_NumFootContacts >= 0);
    //assert(m_IsJumping && m_IsFalling && "Cannot be jumping and falling at the same time");

    //LOG("Jumping: " + ToStr((m_IsJumping)) + ", Falling: " + ToStr(m_IsFalling) + ", JumpHeight: " + ToStr(m_HeightInAir) + ", NumFootContacts: " + ToStr(m_NumFootContacts));
    //LOG(ToStr(m_OverlappingLaddersList.size()));
    //LOG("Vel X: " + ToStr(GetVelocity().x) + ", Vel Y: " + ToStr(GetVelocity().y));
    //LOG(ToStr(m_OverlappingKinematicBodiesList.size()));
    
    if (m_pControllableComponent && !m_pControllableComponent->CanMove())
    {
        SetVelocity(Point(0, 0));
        m_CurrentSpeed = Point(0, 0);
        return;
    }

    //LOG("Climbing: " + ToStr(m_IsClimbing));
    if (m_IsClimbing)
    {
        
        if (m_IgnoreJump && (fabs(m_CurrentSpeed.y) < DBL_EPSILON))
        {
            m_IgnoreJump = false;
        }
        else if (!m_IgnoreJump && (fabs(m_CurrentSpeed.y) > DBL_EPSILON))
        {
            m_IsClimbing = false;
            m_IsJumping = true;
            m_IsFalling = false;
            m_HeightInAir = 0;
            m_pPhysics->VSetGravityScale(_owner->GetGUID(), m_GravityScale);
        }
        if (m_OverlappingLaddersList.empty())
        {
            m_IsClimbing = false;
            m_pPhysics->VSetGravityScale(_owner->GetGUID(), m_GravityScale);
        }
        else
        {
            //LOG("ClimbingSpeed: " + ToStr(m_ClimbingSpeed.y));
            m_pPhysics->VSetLinearSpeedEx(_owner->GetGUID(), Point(0, m_ClimbingSpeed.y));
            if (m_pControllableComponent && fabs(m_ClimbingSpeed.y) < DBL_EPSILON)
            {
                m_pControllableComponent->VOnStopClimbing();
            }
            else
            {
                m_pControllableComponent->VOnClimb();
            }
            m_ClimbingSpeed = Point(0, 0);
            return;
        }
    }

    /*if (m_OverlappingLaddersList.size() > 1)
        LOG(ToStr(m_OverlappingLaddersList.size()));*/
    // This should be available only to controlled actors
    if (m_CanJump)
    {
        //LOG(ToStr(GetVelocity().y));
        // This is to ensure one jump per one space press
        
        bool wasFalling = GetVelocity().y > FLT_EPSILON;
        bool wasJumping = m_IsJumping;

        // "20" lets us skip uneven ground and therefore skip spamming transition between falling/jumping
        if (m_HeightInAir > 20 && fabs(GetVelocity().y) < FLT_EPSILON)
        {
            m_IgnoreJump = true;
            m_CurrentSpeed.y = 0;
        }
        else if (fabs(m_CurrentSpeed.y) < DBL_EPSILON && (GetVelocity().y < FLT_EPSILON) && IsInAir())
        {
            m_IgnoreJump = true;
            m_CurrentSpeed.y = 0;
        }
        else if (m_IgnoreJump && (fabs(m_CurrentSpeed.y) < DBL_EPSILON) && m_NumFootContacts > 0)
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
        if (m_HeightInAir > m_MaxJumpHeight)
        {
            m_IgnoreJump = true;
            m_CurrentSpeed.y = 0;
        }

        // Set velocity here
        //=====================================================================
        Point velocity = GetVelocity();

        double ySpeed = m_CurrentSpeed.y;
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
            SetVelocity(Point(m_CurrentSpeed.x < 0 ? -5 : 5, velocity.y));
        }
        else
        {
            SetVelocity(Point(0, velocity.y));
        }

        ApplyForce(m_pPhysics->GetGravity());

        velocity = GetVelocity();
        if (velocity.y < -8.8)
        {
            SetVelocity(Point(velocity.x, -8.8));
        }
        if (velocity.y > 14)
        {
            SetVelocity(Point(velocity.x, 14));
        }

        //=====================================================================

        if (m_IsJumping || m_IsFalling)
        {
            m_IsRunning = false;
            m_IsStopped = false;
        }
        
        bool wasRunning = m_IsRunning;
        bool wasStopped = m_IsStopped;
        Direction prevDirection = m_Direction;

        velocity = GetVelocity();
        if (fabs(velocity.x) > DBL_EPSILON)
        {
            m_Direction = velocity.x < 0 ? Direction_Left : Direction_Right;
            if (!IsInAir())
            {
                m_IsRunning = true;
                m_IsStopped = false;
            }
        }
        else
        {
            if (!IsInAir())
            {
                m_IsStopped = true;
                m_IsRunning = false;
            }
        }
         
        //LOG("Running: " + ToStr(m_IsRunning) + ", Stopped: " + ToStr(m_IsStopped));
        if (m_pControllableComponent)
        {
            if (prevDirection != m_Direction)
            {
                m_pControllableComponent->VOnDirectionChange(m_Direction);
            }
            if (m_IsRunning) // TODO: Dont poll here. State changing didnt work.
            {
                m_pControllableComponent->VOnRun();
            }
            else if (m_IsStopped && ((fabs(GetVelocity().y) < DBL_EPSILON) || !m_OverlappingKinematicBodiesList.empty()))
            {
                m_pControllableComponent->VOnStopMoving();
            }
        }
        
    }

    /*if (m_HasConstantSpeed)
    {
        m_pPhysics->VSetLinearSpeed(_owner->GetGUID(), m_ConstantSpeed);
    }*/

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
            m_pControllableComponent->VOnLandOnGround();
        }
    }

    m_NumFootContacts++; 
    //LOG(ToStr(m_HeightInAir));
    m_HeightInAir = 0;
}

void PhysicsComponent::OnEndFootContact()
{ 
    m_NumFootContacts--; 
}

void PhysicsComponent::OnStartFalling()
{ 
    if (m_IsClimbing)
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
    if (m_IsClimbing)
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
    if (falling)
    {
        if (m_HeightInAir < 2 && (GetVelocity().y < 3))
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

    shared_ptr<PositionComponent> pPositionComponent =
        MakeStrongPtr(_owner->GetComponent<PositionComponent>(PositionComponent::g_Name));
    assert(pPositionComponent);

    Point actorPosition = pPositionComponent->GetPosition();
    for (const b2Fixture* pLadderFixture : m_OverlappingLaddersList)
    {
        // Assume ladder is rectangular which means I can aswell use its aabb for exact position
        b2AABB aabb = pLadderFixture->GetAABB(0);
        Point bottomright = b2Vec2ToPoint(MetersToPixels(aabb.upperBound));
        Point topleft = b2Vec2ToPoint(MetersToPixels(aabb.lowerBound));
        Point center = b2Vec2ToPoint(MetersToPixels(aabb.GetCenter()));

        if (actorPosition.x > topleft.x && actorPosition.x < bottomright.x)
        {
            m_pPhysics->VSetPosition(_owner->GetGUID(), Point(center.x, actorPosition.y));
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
            m_IsClimbing = true;
            m_pControllableComponent->VOnClimb();
            m_IgnoreJump = true;
            m_pPhysics->VSetGravityScale(_owner->GetGUID(), 0);
        }
    }
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