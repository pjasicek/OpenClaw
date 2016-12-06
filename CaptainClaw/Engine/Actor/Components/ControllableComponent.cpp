#include <Tinyxml\tinyxml.h>
#include "ControllableComponent.h"
#include "../../Events/Events.h"
#include "../../Events/EventMgr.h"
#include "AnimationComponent.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "PositionComponent.h"
#include "../ActorTemplates.h"
#include "ControllerComponents/AmmoComponent.h"
#include "ControllerComponents/PowerupComponent.h"

const char* ControllableComponent::g_Name = "ControllableComponent";
const char* ClawControllableComponent::g_Name = "ClawControllableComponent";

ControllableComponent::ControllableComponent()
    : m_Active(false)
{ }

bool ControllableComponent::VInit(TiXmlElement* data)
{
    assert(data != NULL);

    if (TiXmlElement* isActiveElement = data->FirstChildElement("IsActive"))
    {
        m_Active = std::string(isActiveElement->GetText()) == "true";
    }
    else
    {
        m_Active = false;
    }

    return VInitDelegate(data);
}

void ControllableComponent::VPostInit()
{
    shared_ptr<PhysicsComponent> pPhysicsComponent =
        MakeStrongPtr(_owner->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
    if (pPhysicsComponent)
    {
        pPhysicsComponent->SetControllableComponent(this);
    }

    if (m_Active)
    {
        shared_ptr<EventData_Attach_Actor> pEvent(new EventData_Attach_Actor(_owner->GetGUID()));
        IEventMgr::Get()->VTriggerEvent(pEvent);
    }
}

TiXmlElement* ControllableComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    //

    return baseElement;
}

//=====================================================================================================================

ClawControllableComponent::ClawControllableComponent()
{
    m_pClawAnimationComponent = NULL;
    m_pRenderComponent = NULL;
    m_State = ClawState_None;
    m_Direction = Direction_Right;
}

ClawControllableComponent::~ClawControllableComponent()
{

}

bool ClawControllableComponent::VInitDelegate(TiXmlElement* data)
{
    return true;
}

void ClawControllableComponent::VPostInit()
{
    ControllableComponent::VPostInit();

    m_pRenderComponent = MakeStrongPtr(_owner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name)).get();
    m_pClawAnimationComponent = MakeStrongPtr(_owner->GetComponent<AnimationComponent>(AnimationComponent::g_Name)).get();
    m_pPositionComponent = MakeStrongPtr(_owner->GetComponent<PositionComponent>(PositionComponent::g_Name)).get();
    m_pAmmoComponent = MakeStrongPtr(_owner->GetComponent<AmmoComponent>(AmmoComponent::g_Name)).get();
    m_pPowerupComponent = MakeStrongPtr(_owner->GetComponent<PowerupComponent>(PowerupComponent::g_Name)).get();
    assert(m_pClawAnimationComponent);
    assert(m_pRenderComponent);
    assert(m_pPositionComponent);
    assert(m_pAmmoComponent);
    assert(m_pPowerupComponent);
    m_pClawAnimationComponent->AddObserver(this);

    m_pPhysicsComponent = MakeStrongPtr(_owner->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name)).get();
}

void ClawControllableComponent::VUpdate(uint32 msDiff)
{

}

// Interface for subclasses
void ClawControllableComponent::VOnStartFalling()
{
    if (m_State == ClawState_JumpShooting ||
        m_State == ClawState_JumpAttacking)
    {
        return;
    }
    m_pClawAnimationComponent->SetAnimation("fall");
    m_State = ClawState_Falling;
}

void ClawControllableComponent::VOnLandOnGround()
{
    m_pClawAnimationComponent->SetAnimation("stand");
    m_State = ClawState_Standing;
}

void ClawControllableComponent::VOnStartJumping()
{
    if (m_State == ClawState_JumpShooting ||
        m_State == ClawState_JumpAttacking)
    {
        return;
    }
    m_pClawAnimationComponent->SetAnimation("jump");
    m_State = ClawState_Jumping;
}

void ClawControllableComponent::VOnDirectionChange(Direction direction)
{
    m_pRenderComponent->SetMirrored(direction == Direction_Left);
    m_Direction = direction;
}

void ClawControllableComponent::VOnStopMoving()
{
    if (m_State == ClawState_Shooting)
    {
        return;
    }
    m_pClawAnimationComponent->SetAnimation("stand");
    m_State = ClawState_Standing;
}

void ClawControllableComponent::VOnRun()
{
    if (m_State == ClawState_Shooting)
    {
        return;
    }
    m_pClawAnimationComponent->SetAnimation("walk");
    m_State = ClawState_Walking;
}

void ClawControllableComponent::VOnClimb()
{
    m_pClawAnimationComponent->ResumeAnimation();
    m_pClawAnimationComponent->SetAnimation("climb");
    m_State = ClawState_Climbing;
}

void ClawControllableComponent::VOnStopClimbing()
{
    m_pClawAnimationComponent->PauseAnimation();
}

void ClawControllableComponent::OnAttack()
{
    if (IsAttackingOrShooting() ||
        m_State == ClawState_Climbing)
    {
        return;
    }

    if (m_State == ClawState_Falling ||
        m_State == ClawState_Jumping)
    {
        m_pClawAnimationComponent->SetAnimation("jumpswipe");
        m_State = ClawState_JumpAttacking;
    }
    else
    {
        int attackType = rand() % 5;
        if (attackType == 0)
        {
            m_pClawAnimationComponent->SetAnimation("kick");
        }
        else if (attackType == 1)
        {
            m_pClawAnimationComponent->SetAnimation("uppercut");
        }
        else
        {
            m_pClawAnimationComponent->SetAnimation("swipe");
        }
        
        m_State = ClawState_Attacking;
    }
}

void ClawControllableComponent::OnFire(bool outOfAmmo)
{
    if (IsAttackingOrShooting() ||
        m_State == ClawState_Climbing)
    {
        return;
    }

    AmmoType activeAmmoType = m_pAmmoComponent->GetActiveAmmoType();
    if (m_State == ClawState_Falling ||
        m_State == ClawState_Jumping)
    {
        if (activeAmmoType == AmmoType_Pistol)
        {
            m_pClawAnimationComponent->SetAnimation("jumppistol");
        }
        else if (activeAmmoType == AmmoType_Magic)
        {
            m_pClawAnimationComponent->SetAnimation("jumpmagic");
        }
        else if (activeAmmoType == AmmoType_Dynamite)
        {
            m_pClawAnimationComponent->SetAnimation("jumpdynamite");
        }
        m_State = ClawState_JumpShooting;
    }
    else
    {
        if (activeAmmoType == AmmoType_Pistol)
        {
            m_pClawAnimationComponent->SetAnimation("pistol");
        }
        else if (activeAmmoType == AmmoType_Magic)
        {
            m_pClawAnimationComponent->SetAnimation("magic");
        }
        else if (activeAmmoType == AmmoType_Dynamite)
        {
            m_pClawAnimationComponent->SetAnimation("postdynamite");
        }
        m_State = ClawState_Shooting;
    }
}

void ClawControllableComponent::OnDuck()
{

}

bool ClawControllableComponent::CanMove()
{
    if (m_State == ClawState_Shooting ||
        m_State == ClawState_Attacking ||
        m_State == ClawState_Ducking)
    {
        return false;
    }

    return true;
}

void ClawControllableComponent::SetCurrentPhysicsState()
{
    if (m_pPhysicsComponent->IsFalling())
    {
        m_pClawAnimationComponent->SetAnimation("fall");
        m_State = ClawState_Falling;
    }
    else if (m_pPhysicsComponent->IsJumping())
    {
        m_pClawAnimationComponent->SetAnimation("jump");
        m_State = ClawState_Jumping;
    }
    else if (m_pPhysicsComponent->IsOnGround())
    {
        m_pClawAnimationComponent->SetAnimation("stand");
        m_State = ClawState_Standing;
    }
    else
    {
        m_pClawAnimationComponent->SetAnimation("stand");
        m_State = ClawState_Standing;
        LOG_ERROR("Unknown physics state. Assume standing");
    }
}

void ClawControllableComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    std::string animName = pAnimation->GetName();

    // Shooting, only magic and pistol supported at the moment
    if (((animName.find("pistol") != std::string::npos) && pNewFrame->hasEvent) ||
        ((animName.find("magic") != std::string::npos) && pNewFrame->hasEvent) ||
        ((animName.find("dynamite") != std::string::npos) && pNewFrame->hasEvent))
    {
        if (m_pAmmoComponent->CanFire())
        {
            AmmoType projectileType = m_pAmmoComponent->GetActiveAmmoType();
            int32 offsetX = 50;
            if (m_Direction == Direction_Left) { offsetX *= -1; }
            ActorTemplates::CreateClawProjectile(projectileType, m_Direction, Point(m_pPositionComponent->GetX() + offsetX, m_pPositionComponent->GetY() - 20));
        }
    }

    if (((animName.find("pistol") != std::string::npos) || 
        (animName.find("magic") != std::string::npos) || 
        (animName.find("dynamite") != std::string::npos))
        && pAnimation->IsAtLastAnimFrame())
    {
        SetCurrentPhysicsState();
    }
    else if ((animName == "swipe" ||
            animName == "kick" ||
            animName == "uppercut" ||
            animName == "jumpswipe" ||
            animName == "duckswipe")
            && pAnimation->IsAtLastAnimFrame())
    {
        SetCurrentPhysicsState();
    }
}

bool ClawControllableComponent::IsAttackingOrShooting()
{
    if (m_State == ClawState_Shooting ||
        m_State == ClawState_JumpShooting ||
        m_State == ClawState_DuckShooting ||
        m_State == ClawState_Attacking ||
        m_State == ClawState_DuckAttacking ||
        m_State == ClawState_JumpAttacking)
    {
        return true;
    }

    return false;
}