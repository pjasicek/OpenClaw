#include "SoundTriggerComponent.h"
#include "../ControllableComponent.h"
#include "../FollowableComponent.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"

//=====================================================================================================================
//
// SoundTriggerComponent Implementation
//
//=====================================================================================================================

const char* SoundTriggerComponent::g_Name = "SoundTriggerComponent";

SoundTriggerComponent::SoundTriggerComponent() :
    m_bActivateDialog(false),
    m_EnterCount(1),
    m_bIsInfinite(false)
{ }

bool SoundTriggerComponent::VInit(TiXmlElement* pData)
{
    assert(pData);

    ParseValueFromXmlElem(&m_bActivateDialog, pData->FirstChildElement("ActivateDialog"));
    ParseValueFromXmlElem(&m_EnterCount, pData->FirstChildElement("EnterCount"));
    ParseValueFromXmlElem(&m_TriggerSound, pData->FirstChildElement("Sound"));

    assert(!m_TriggerSound.empty());

    m_bIsInfinite = m_EnterCount == -1;

    return true;
}

void SoundTriggerComponent::VPostInit()
{
    shared_ptr<TriggerComponent> pTriggerComponent =
        MakeStrongPtr(m_pOwner->GetComponent<TriggerComponent>(TriggerComponent::g_Name));
    assert(pTriggerComponent);

    pTriggerComponent->AddObserver(this);
}

TiXmlElement* SoundTriggerComponent::VGenerateXml()
{
    // TODO: Implement
    return NULL;
}

void SoundTriggerComponent::VOnActorEnteredTrigger(Actor* pActorWhoPickedThis, FixtureType triggerType)
{
    shared_ptr<ClawControllableComponent> pClaw =
        MakeStrongPtr(pActorWhoPickedThis->GetComponent<ClawControllableComponent>(ClawControllableComponent::g_Name));
    assert(pClaw != nullptr);

    SoundInfo soundInfo(m_TriggerSound);
    IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
        new EventData_Request_Play_Sound(soundInfo)));

    if (!m_bIsInfinite)
    {
        m_EnterCount--;
        if (m_EnterCount == 0)
        {
            shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(m_pOwner->GetGUID()));
            IEventMgr::Get()->VQueueEvent(pEvent);
        }
    }

    if (m_bActivateDialog)
    {
        shared_ptr<FollowableComponent> pExclamationMark =
            MakeStrongPtr(pActorWhoPickedThis->GetComponent<FollowableComponent>(FollowableComponent::g_Name));
        if (pExclamationMark)
        {
            pExclamationMark->Activate(2000);
        }
    }
}