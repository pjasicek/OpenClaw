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

SoundTriggerComponent::SoundTriggerComponent()
{ }

bool SoundTriggerComponent::VDelegateInit(TiXmlElement* pData)
{
    assert(pData);

    SetBoolIfDefined(&m_bActivateDialog, pData->FirstChildElement("ActivateDialog"));
    SetIntIfDefined(&m_EnterCount, pData->FirstChildElement("EnterCount"));

    return true;
}

void SoundTriggerComponent::VCreateInheritedXmlElements(TiXmlElement* pBaseElement)
{

}

bool SoundTriggerComponent::VOnApply(Actor* pActorWhoPickedThis)
{
    shared_ptr<ClawControllableComponent> pClaw =
        MakeStrongPtr(pActorWhoPickedThis->GetComponent<ClawControllableComponent>(ClawControllableComponent::g_Name));
    assert(pClaw != nullptr);

    if (m_EnterCount != -1)
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

    return true;
}