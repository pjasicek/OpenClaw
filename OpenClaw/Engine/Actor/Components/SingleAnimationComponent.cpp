#include "SingleAnimationComponent.h"
#include "../Actor.h"

#include "../../Events/EventMgr.h"
#include "../../Events/Events.h"

const char* SingleAnimationComponent::g_Name = "SingleAnimationComponent";

bool SingleAnimationComponent::VInit(TiXmlElement* data)
{
    return true;
}

TiXmlElement* SingleAnimationComponent::VGenerateXml()
{
    return NULL;
}

void SingleAnimationComponent::VPostInit()
{

}

void SingleAnimationComponent::VPostPostInit()
{
    shared_ptr<AnimationComponent> pAnimationComponent =
        MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>(AnimationComponent::g_Name));
    assert(pAnimationComponent != nullptr);
    assert(pAnimationComponent->GetCurrentAnimation() != NULL);
    pAnimationComponent->AddObserver(this);
}

void SingleAnimationComponent::VOnAnimationAtLastFrame(Animation* pAnimation)
{
    shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(m_pOwner->GetGUID()));
    IEventMgr::Get()->VQueueEvent(pEvent);
}