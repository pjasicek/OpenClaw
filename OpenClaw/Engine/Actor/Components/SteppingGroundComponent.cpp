#include "SteppingGroundComponent.h"
#include "PositionComponent.h"

#include "../../GameApp/BaseGameApp.h"
#include "../../GameApp/BaseGameLogic.h"

#include "../../Events/Events.h"
#include "../../Events/EventMgr.h"

const char* SteppingGroundComponent::g_Name = "SteppingGroundComponent";

SteppingGroundComponent::SteppingGroundComponent()
    :
    m_bIsSteppedOn(false),
    m_TimeBeforeToggleLeft(0),
    m_TimeOffLeft(0),
    m_pAnimationComponent(NULL)
{

}

bool SteppingGroundComponent::VInit(TiXmlElement* pData)
{
    assert(pData != NULL);

    m_pPhysics = g_pApp->GetGameLogic()->VGetGamePhysics();
    assert(m_pPhysics != nullptr);

    m_Properties.LoadFromXml(pData, true);
    assert(m_Properties.toggleSound.length() > 0);

    return true;
}

void SteppingGroundComponent::VPostInit()
{
    m_pAnimationComponent = MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>()).get();
    assert(m_pAnimationComponent != NULL);

    m_pAnimationComponent->SetReverseAnimation(true);
    m_pAnimationComponent->PauseAnimation();
    m_pAnimationComponent->AddObserver(this);
}

void SteppingGroundComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    if (pNewFrame->idx > pLastFrame->idx)
    {
        if (pNewFrame->idx == m_Properties.toggleFrameIdx)
        {
            m_pPhysics->VDeactivate(m_pOwner->GetGUID());
        }
        if (pAnimation->IsAtLastAnimFrame())
        {
            m_pAnimationComponent->SetDelay(m_Properties.timeOff);
        }
    }
    else
    {
        if (pNewFrame->idx == pAnimation->GetAnimFramesSize() - 2)
        {
            SoundInfo sound(m_Properties.toggleSound);
            sound.soundSourcePosition = m_pOwner->GetPositionComponent()->GetPosition();
            sound.setDistanceEffect = true;
            sound.soundVolume = 50;
            IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Request_Play_Sound(sound)));
        }

        if (pAnimation->IsAtFirstAnimFrame())
        {
            m_pPhysics->VActivate(m_pOwner->GetGUID());
            m_pAnimationComponent->PauseAnimation();
            m_bIsSteppedOn = false;
        }
    }
}

void SteppingGroundComponent::VOnAnimationAtLastFrame(Animation* pAnimation)
{

}

void SteppingGroundComponent::OnActorContact(Actor* pActor)
{
    if (m_bIsSteppedOn)
    {
        return;
    }

    m_pAnimationComponent->ResumeAnimation();
    m_pAnimationComponent->SetDelay(m_Properties.timeOn);
    m_bIsSteppedOn = true;
}