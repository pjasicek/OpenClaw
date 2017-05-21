#include "RopeComponent.h"

const char* RopeComponent::g_Name = "RopeComponent";

RopeComponent::RopeComponent()
{

}

bool RopeComponent::VInit(TiXmlElement* data)
{
    assert(data != NULL);

    return true;
}

void RopeComponent::VPostInit()
{
    shared_ptr<AnimationComponent>pAnimationComponent =
        MakeStrongPtr(_owner->GetComponent<AnimationComponent>());
    shared_ptr<TriggerComponent> pTriggerComponent = 
        MakeStrongPtr(_owner->GetComponent<TriggerComponent>());

    assert(pAnimationComponent);
    assert(pTriggerComponent);

    pAnimationComponent->AddObserver(this);
    pTriggerComponent->AddObserver(this);
}

void RopeComponent::VPostPostInit()
{

}

void RopeComponent::VUpdate(uint32 msDiff)
{

}

void RopeComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{

}

void RopeComponent::VOnActorEnteredTrigger(Actor* pActorWhoEntered)
{
    assert(m_pAttachedActor == NULL);
    m_pAttachedActor = pActorWhoEntered;
}

void RopeComponent::VOnActorLeftTrigger(Actor* pActorWhoLeft)
{
    assert(m_pAttachedActor != NULL);
    m_pAttachedActor = NULL;
}