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
#include "ControllerComponents/HealthComponent.h"

#include "../../GameApp/BaseGameApp.h"

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
    m_pHealthComponent = MakeStrongPtr(_owner->GetComponent<HealthComponent>(HealthComponent::g_Name)).get();
    assert(m_pClawAnimationComponent);
    assert(m_pRenderComponent);
    assert(m_pPositionComponent);
    assert(m_pAmmoComponent);
    assert(m_pPowerupComponent);
    assert(m_pHealthComponent);
    m_pClawAnimationComponent->AddObserver(this);

    auto pHealthComponent = MakeStrongPtr(_owner->GetComponent<HealthComponent>(HealthComponent::g_Name));
    pHealthComponent->AddObserver(this);

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
    if (m_State == ClawState_Shooting ||
        IsDucking())
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
        m_State == ClawState_Climbing ||
        m_State == ClawState_TakingDamage)
    {
        return;
    }

    if (m_State == ClawState_Falling ||
        m_State == ClawState_Jumping)
    {
        m_pClawAnimationComponent->SetAnimation("jumpswipe");
        m_State = ClawState_JumpAttacking;
    }
    else if (IsDucking())
    {
        m_pClawAnimationComponent->SetAnimation("duckswipe");
        m_State = ClawState_DuckAttacking;
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
        m_State == ClawState_Climbing ||
        m_State == ClawState_Dying || 
        m_State == ClawState_TakingDamage)
    {
        return;
    }

    AmmoType activeAmmoType = m_pAmmoComponent->GetActiveAmmoType();
    if (m_State == ClawState_Falling ||
        m_State == ClawState_Jumping)
    {
        if (activeAmmoType == AmmoType_Pistol)
        {
            if (m_pAmmoComponent->CanFire())
            {
                m_pClawAnimationComponent->SetAnimation("jumppistol");
            }
            else
            {
                m_pClawAnimationComponent->SetAnimation("emptyjumppistol");
            }
        }
        else if (activeAmmoType == AmmoType_Magic)
        {
            if (m_pAmmoComponent->CanFire())
            {
                m_pClawAnimationComponent->SetAnimation("jumpmagic");
            }
            else
            {
                m_pClawAnimationComponent->SetAnimation("emptyjumpmagic");
            }
        }
        else if (activeAmmoType == AmmoType_Dynamite)
        {
            if (m_pAmmoComponent->CanFire())
            {
                m_pClawAnimationComponent->SetAnimation("jumpdynamite");
            }
            else
            {
                m_pClawAnimationComponent->SetAnimation("emptyjumpdynamite");
            }
        }
        m_State = ClawState_JumpShooting;
    }
    else if (IsDucking())
    {
        if (activeAmmoType == AmmoType_Pistol)
        {
            if (m_pAmmoComponent->CanFire())
            {
                m_pClawAnimationComponent->SetAnimation("duckpistol");
            }
            else
            {
                m_pClawAnimationComponent->SetAnimation("duckemptypistol");
            }
        }
        else if (activeAmmoType == AmmoType_Magic)
        {
            if (m_pAmmoComponent->CanFire())
            {
                m_pClawAnimationComponent->SetAnimation("duckmagic");
            }
            else
            {
                m_pClawAnimationComponent->SetAnimation("duckemptymagic");
            }
        }
        else if (activeAmmoType == AmmoType_Dynamite)
        {
            if (m_pAmmoComponent->CanFire())
            {
                m_pClawAnimationComponent->SetAnimation("duckpostdynamite");
            }
            else
            {
                m_pClawAnimationComponent->SetAnimation("duckemptydynamite");
            }
        }
        m_State = ClawState_DuckAttacking;
    }
    else
    {
        if (activeAmmoType == AmmoType_Pistol)
        {
            if (m_pAmmoComponent->CanFire())
            {
                m_pClawAnimationComponent->SetAnimation("pistol");
            }
            else
            {
                m_pClawAnimationComponent->SetAnimation("emptypistol");
            }
        }
        else if (activeAmmoType == AmmoType_Magic)
        {
            if (m_pAmmoComponent->CanFire())
            {
                m_pClawAnimationComponent->SetAnimation("magic");
            }
            else
            {
                m_pClawAnimationComponent->SetAnimation("emptymagic");
            }
        }
        else if (activeAmmoType == AmmoType_Dynamite)
        {
            if (m_pAmmoComponent->CanFire())
            {
                m_pClawAnimationComponent->SetAnimation("postdynamite");
            }
            else
            {
                m_pClawAnimationComponent->SetAnimation("emptydynamite");
            }
        }
        m_State = ClawState_Shooting;
    }
}

bool ClawControllableComponent::CanMove()
{
    if (m_State == ClawState_Shooting ||
        m_State == ClawState_Attacking ||
        m_State == ClawState_Dying ||
        m_State == ClawState_DuckAttacking ||
        m_State == ClawState_DuckShooting ||
        m_State == ClawState_TakingDamage)
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
    else if (IsDucking())
    {
        m_pClawAnimationComponent->SetAnimation("duck");
        m_State = ClawState_Ducking;
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
            int32 offsetY = -20;
            if (IsDucking()) { offsetY += 40; }
            ActorTemplates::CreateClawProjectile(projectileType, m_Direction, Point(m_pPositionComponent->GetX() + offsetX, m_pPositionComponent->GetY() + offsetY));

            if (IsDucking())
            {
                SetCurrentPhysicsState();
            }

            if (!g_pApp->GetGameCheats()->clawInfiniteAmmo)
            {
                // TODO: Better name of this method ? 
                m_pAmmoComponent->OnFired();
            }
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
            animName == "duckswipe"))
    {
        if (pAnimation->GetCurrentAnimationFrame()->hasEvent ||
            (animName == "swipe" && pNewFrame->idx == 3))
        {
            Point position = m_pPositionComponent->GetPosition();
            Point positionOffset(60, 20);
            if (animName == "jumpswipe")
            {
                positionOffset.y -= 10;
                positionOffset.x += 5;
            }
            if (m_Direction == Direction_Left)
            {
                positionOffset = Point(-1.0 * positionOffset.x, positionOffset.y);
            }
            position += positionOffset;

            ActorTemplates::CreateAreaDamage(position, Point(50, 25), 10, CollisionFlag_ClawAttack, "Rectangle");
        }
        if (pAnimation->IsAtLastAnimFrame())
        {
            SetCurrentPhysicsState();
        }
    }
}

void ClawControllableComponent::VOnAnimationLooped(Animation* pAnimation)
{
    std::string animName = pAnimation->GetName();
    if (pAnimation->GetName().find("death") != std::string::npos)
    {
        shared_ptr<EventData_Claw_Died> pEvent(new EventData_Claw_Died(_owner->GetGUID()));
        IEventMgr::Get()->VTriggerEvent(pEvent);

        SetCurrentPhysicsState();
        m_pPhysicsComponent->RestoreGravityScale();
    }
    else if (animName == "damage1" ||
             animName == "damage2")
    {
        SetCurrentPhysicsState();
    }

    m_pHealthComponent->SetInvulnerable(false);
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

void ClawControllableComponent::VOnHealthBelowZero()
{
    if (m_State == ClawState_Dying)
    {
        return;
    }

    // TODO: Track how exactly claw died
    if (m_pClawAnimationComponent->GetCurrentAnimationName() != "spikedeath")
    {
        m_pClawAnimationComponent->SetAnimation("spikedeath");
        m_pPhysicsComponent->SetGravityScale(0.0f);
        m_State = ClawState_Dying;
    }
}

void ClawControllableComponent::VOnHealthChanged(int32 oldHealth, int32 newHealth)
{
    // When claw takes damage but does not actually die
    if (newHealth > 0 && oldHealth > newHealth)
    {
        if (Util::GetRandomNumber(0, 1) == 0)
        {
            m_pClawAnimationComponent->SetAnimation("damage1");
        }
        else
        {
            m_pClawAnimationComponent->SetAnimation("damage2");
        }

        Point knockback(-10, 0);
        if (m_pRenderComponent->IsMirrored())
        {
            knockback = Point(knockback.x * -1.0, knockback.y);
        }
        m_pPositionComponent->SetPosition(m_pPositionComponent->GetX() + knockback.x, m_pPositionComponent->GetY() + knockback.y);

        shared_ptr<EventData_Teleport_Actor> pEvent(new EventData_Teleport_Actor
            (_owner->GetGUID(), m_pPositionComponent->GetPosition()));
        IEventMgr::Get()->VQueueEvent(pEvent);

        m_pHealthComponent->SetInvulnerable(true);
        
        m_State = ClawState_TakingDamage;
    }
}

bool ClawControllableComponent::IsDucking()
{
    return (m_State == ClawState_Ducking ||
        m_State == ClawState_DuckAttacking ||
        m_State == ClawState_DuckShooting);
}

void ClawControllableComponent::OnDuck()
{
    if (!IsDucking())
    {
        m_pClawAnimationComponent->SetAnimation("duck");
        m_State = ClawState_Ducking;
    }
}

void ClawControllableComponent::OnStand()
{
    m_State = ClawState_Standing;
    SetCurrentPhysicsState();
}