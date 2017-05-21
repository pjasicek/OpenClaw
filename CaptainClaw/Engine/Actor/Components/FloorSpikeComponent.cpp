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
    m_TimeOn(0)
{

}

bool FloorSpikeComponent::VInit(TiXmlElement* pData)
{
    assert(pData != NULL);

    assert(ParseValueFromXmlElem(&m_ActiveFrameIdx, pData->FirstChildElement("ActiveFrameIdx")));
    assert(ParseValueFromXmlElem(&m_StartDelay, pData->FirstChildElement("StartDelay")));
    assert(ParseValueFromXmlElem(&m_TimeOn, pData->FirstChildElement("TimeOn")));

    return true;
}

void FloorSpikeComponent::VPostInit()
{
    m_pDamageAuraComponent = MakeStrongPtr(_owner->GetComponent<DamageAuraComponent>()).get();
    shared_ptr<AnimationComponent> pAnimComponent = MakeStrongPtr(_owner->GetComponent<AnimationComponent>());

    assert(m_pDamageAuraComponent != NULL);
    assert(pAnimComponent != nullptr);

    pAnimComponent->AddObserver(this);
    pAnimComponent->GetCurrentAnimation()->SetDelay(m_StartDelay);
    pAnimComponent->GetCurrentAnimation()->SetReverseAnim(true);
}

void FloorSpikeComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    if (pNewFrame->idx > pLastFrame->idx)
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

    if ((pLastFrame->idx == 1 && pNewFrame->idx == 0) ||
        (pNewFrame->idx == pAnimation->GetAnimFramesSize() - 1 && pLastFrame->idx == pAnimation->GetAnimFramesSize() - 2))
    {

        pAnimation->SetDelay(m_TimeOn);
    }

    SoundInfo sound;
    sound.soundVolume = 40;
    sound.setPositionEffect = true;
    sound.soundSourcePosition = _owner->GetPositionComponent()->GetPosition();
    if (pLastFrame->idx == 0 && pNewFrame->idx == 1)
    {
        sound.soundToPlay = "/LEVEL3/SOUNDS/FLOORSPIKEUP.WAV";

        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Request_Play_Sound(sound)));
    }
    else if (pLastFrame->idx == pAnimation->GetAnimFramesSize() - 1 && pNewFrame->idx == pAnimation->GetAnimFramesSize() - 2)
    {
        sound.soundToPlay = "/LEVEL3/SOUNDS/FLOORSPIKEDOWN.WAV";

        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Request_Play_Sound(sound)));
    }
}