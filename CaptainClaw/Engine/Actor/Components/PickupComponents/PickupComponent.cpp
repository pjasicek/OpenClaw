#include "PickupComponent.h"
#include "../TriggerComponents/TriggerComponent.h"
#include "../ControllerComponents/ScoreComponent.h"
#include "../RenderComponent.h"

#include "../../../GameApp/BaseGameApp.h"
#include "../../../UserInterface/HumanView.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"

const char* PickupComponent::g_Name = "PickupComponent";
const char* TreasurePickupComponent::g_Name = "TreasurePickupComponent";

//=====================================================================================================================
//
// PickupComponent Implementation
//
//=====================================================================================================================

bool PickupComponent::VInit(TiXmlElement* data)
{
    

    if (!VDelegateInit(data))
    {
        return false;
    }

    return true;
}

void PickupComponent::VPostInit()
{
    shared_ptr<TriggerComponent> pTriggerComponent =
        MakeStrongPtr(_owner->GetComponent<TriggerComponent>(TriggerComponent::g_Name));
    assert(pTriggerComponent);

    pTriggerComponent->AddObserver(this);
}

TiXmlElement* PickupComponent::VGenerateXml()
{
    // TODO: Implement
    return NULL;
}

void PickupComponent::VOnActorEnteredTrigger(Actor* pActorWhoEntered)
{
    // Check if pickup was really "picked up"
    // Note: Actor is not destroyed here. subclasses need to handle it.
    if (VOnApply(pActorWhoEntered))
    {
        // TODO: Is this necessary ?
        shared_ptr<TriggerComponent> pTriggerComponent =
            MakeStrongPtr(_owner->GetComponent<TriggerComponent>(TriggerComponent::g_Name));
        if (pTriggerComponent)
        {
            //pTriggerComponent->Deactivate();
            pTriggerComponent->Destroy();
        }

        //LOG("Pickup up");
    }
    else
    {
        //LOG("Not picked up");
    }
}

//=====================================================================================================================
//
// TreasurePickupComponent Implementation
//
//=====================================================================================================================

TreasurePickupComponent::TreasurePickupComponent()
    :
    m_ScorePoints(0),
    m_IsPickedUp(false)
{ }

bool TreasurePickupComponent::VDelegateInit(TiXmlElement* data)
{
    assert(data);

    if (TiXmlElement* pElem = data->FirstChildElement("ScorePoints"))
    {
        m_ScorePoints = std::stoi(pElem->GetText());
    }

    return true;
}

void TreasurePickupComponent::VCreateInheritedXmlElements(TiXmlElement* pBaseElement)
{

}

bool TreasurePickupComponent::VOnApply(Actor* pActorWhoPickedThis)
{
    shared_ptr<ScoreComponent> pScoreComponent =
        MakeStrongPtr(pActorWhoPickedThis->GetComponent<ScoreComponent>(ScoreComponent::g_Name));
    if (pScoreComponent)
    {
        pScoreComponent->AddScorePoints(m_ScorePoints);

        // TODO: Make it fly towards the left corner
        m_IsPickedUp = true;

        return true;
    }

    return false;
}

void TreasurePickupComponent::VUpdate(uint32 msDiff)
{
    if (m_IsPickedUp)
    {
        // TODO: Make it fly towards the left corner
        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(_owner->GetGUID()));
        IEventMgr::Get()->VQueueEvent(pEvent);

        // This feels like a hack
        /*if (shared_ptr<CameraNode> pCamera = g_pApp->GetHumanView()->GetCamera())
        {

        }*/
    }
}