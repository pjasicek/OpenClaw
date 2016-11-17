#include <Tinyxml\tinyxml.h>
#include "ControllableComponent.h"
#include "../../Events/Events.h"
#include "../../Events/EventMgr.h"
#include "AnimationComponent.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"

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
    assert(m_pClawAnimationComponent);
    assert(m_pRenderComponent);
}

// Interface for subclasses
void ClawControllableComponent::VOnStartFalling()
{
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
    m_pClawAnimationComponent->SetAnimation("jump");
}

void ClawControllableComponent::VOnDirectionChange(Direction direction)
{
    m_pRenderComponent->SetMirrored(direction == Direction_Left);
}

void ClawControllableComponent::VOnStopMoving()
{
    m_pClawAnimationComponent->SetAnimation("stand");
}

void ClawControllableComponent::VOnRun()
{
    m_pClawAnimationComponent->SetAnimation("walk");
}

void ClawControllableComponent::VOnClimb()
{
    m_pClawAnimationComponent->ResumeAnimation();
    m_pClawAnimationComponent->SetAnimation("climb");
}

void ClawControllableComponent::VOnStopClimbing()
{
    m_pClawAnimationComponent->PauseAnimation();
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