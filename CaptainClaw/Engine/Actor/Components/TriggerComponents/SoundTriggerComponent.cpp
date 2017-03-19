#include "SoundTriggerComponent.h"
#include "../ControllableComponent.h"

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

    SetBoolIfDefined(&m_bActivateDialog, pData->FirstChildElement("ActivateDialog"));
    SetIntIfDefined(&m_EnterCount, pData->FirstChildElement("EnterCount"));
    SetStringIfDefined(&m_TriggerSound, pData->FirstChildElement("Sound"));

    assert(!m_TriggerSound.empty());

    m_bIsInfinite = m_EnterCount == -1;

    return true;
}

void SoundTriggerComponent::VPostInit()
{
    shared_ptr<TriggerComponent> pTriggerComponent =
        MakeStrongPtr(_owner->GetComponent<TriggerComponent>(TriggerComponent::g_Name));
    assert(pTriggerComponent);

    pTriggerComponent->AddObserver(this);
}

TiXmlElement* SoundTriggerComponent::VGenerateXml()
{
    // TODO: Implement
    return NULL;
}

void SoundTriggerComponent::VOnActorEnteredTrigger(Actor* pActorWhoPickedThis)
{
    shared_ptr<ClawControllableComponent> pClaw =
        MakeStrongPtr(pActorWhoPickedThis->GetComponent<ClawControllableComponent>(ClawControllableComponent::g_Name));
    assert(pClaw != nullptr);

    IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Request_Play_Sound(m_TriggerSound.c_str(), 100, false)));

    if (!m_bIsInfinite)
    {
        m_EnterCount--;
        if (m_EnterCount == 0)
        {
            shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(_owner->GetGUID()));
            IEventMgr::Get()->VQueueEvent(pEvent);
        }
    }

    if (m_bActivateDialog)
    {
        LOG("Activate claw dialog");
    }
}