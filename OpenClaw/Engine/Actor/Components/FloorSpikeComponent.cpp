#include "FloorSpikeComponent.h"
#include "AuraComponents/AuraComponent.h"
#include "PositionComponent.h"

#include "../../GameApp/BaseGameApp.h"
#include "../../GameApp/BaseGameLogic.h"

#include "../../Events/EventMgr.h"
#include "../../Events/Events.h"

const char* FloorSpikeComponent::g_Name = "FloorSpikeComponent";

FloorSpikeComponent::FloorSpikeComponent()
    :
    m_ActiveFrameIdx(0),
    m_StartDelay(0),
    m_TimeOn(0),
    m_TimeOff(0)
{

}

bool FloorSpikeComponent::VInit(TiXmlElement* pData)
{
    assert(pData != NULL);

    DO_AND_CHECK(ParseValueFromXmlElem(&m_ActiveFrameIdx, pData->FirstChildElement("ActiveFrameIdx")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_StartDelay, pData->FirstChildElement("StartDelay")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_TimeOn, pData->FirstChildElement("TimeOn")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_TimeOff, pData->FirstChildElement("TimeOff")));
    ParseValueFromXmlElem(&m_ActivateSound, pData->FirstChildElement("ActivateSound"));
    ParseValueFromXmlElem(&m_DeactivateSound, pData->FirstChildElement("DeactivateSound"));

    return true;
}

void FloorSpikeComponent::VPostInit()
{
    m_pDamageAuraComponent = MakeStrongPtr(m_pOwner->GetComponent<DamageAuraComponent>()).get();
    shared_ptr<AnimationComponent> pAnimComponent = MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>());

    assert(m_pDamageAuraComponent != NULL);
    assert(pAnimComponent != nullptr);

    pAnimComponent->AddObserver(this);
    pAnimComponent->GetCurrentAnimation()->SetDelay(m_StartDelay);
    pAnimComponent->GetCurrentAnimation()->SetReverseAnim(true);

    // Correct m_TimeOn and m_TimeOff time to sync spike delays
    // m_TimeOn + m_TimeOff is a full animation cycle
    for (const auto &frame : pAnimComponent->GetCurrentAnimation()->GetAnimFrames())
    {
        m_TimeOn -= frame.duration;
        m_TimeOff -= frame.duration;
    }
    m_TimeOn = m_TimeOn >= 0 ? m_TimeOn : 0;
    m_TimeOff = m_TimeOff >= 0 ? m_TimeOff : 0;
}

void FloorSpikeComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    const bool isUpAnimation = pNewFrame->idx > pLastFrame->idx;
    if (isUpAnimation)
    {
        if (pNewFrame->idx == m_ActiveFrameIdx)
        {
            m_pDamageAuraComponent->SetEnabled(true);
        }
    }
    else
    {
        if (pNewFrame->idx == m_ActiveFrameIdx - 1)
        {
            m_pDamageAuraComponent->SetEnabled(false);
        }
    }

    const bool isDownAction = !isUpAnimation && pAnimation->IsAtFirstAnimFrame();
    const bool isUpAction = isUpAnimation && pAnimation->IsAtLastAnimFrame();

    if (isDownAction || isUpAction)
    {
        pAnimation->SetDelay(isUpAction ? m_TimeOn : m_TimeOff);

        SoundInfo sound;
        sound.soundVolume = 40;
        sound.setPositionEffect = true;
        sound.setDistanceEffect = true;
        sound.soundSourcePosition = m_pOwner->GetPositionComponent()->GetPosition();
        if (isUpAction && !m_ActivateSound.empty())
        {
            sound.soundToPlay = m_ActivateSound;

            IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Request_Play_Sound(sound)));
        }
        else if (isDownAction && !m_DeactivateSound.empty())
        {
            sound.soundToPlay = m_DeactivateSound;

            IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Request_Play_Sound(sound)));
        }
    }
}