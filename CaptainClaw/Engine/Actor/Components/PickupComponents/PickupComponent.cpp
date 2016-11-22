#include "PickupComponent.h"
#include "../TriggerComponents/TriggerComponent.h"
#include "../ControllerComponents/ScoreComponent.h"
#include "../RenderComponent.h"
#include "../PositionComponent.h"

#include "../../../GameApp/BaseGameApp.h"
#include "../../../UserInterface/HumanView.h"
#include "../../../Scene/SceneNodes.h"

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

void TreasurePickupComponent::VPostInit()
{
    PickupComponent::VPostInit();

    m_pRenderComponent = MakeStrongPtr(_owner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name));
    m_pPositionComponent = MakeStrongPtr(_owner->GetComponent<PositionComponent>(PositionComponent::g_Name));
    assert(m_pRenderComponent);
    assert(m_pPositionComponent);
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
        // I want it to disappear after ~900ms
        Point moveDelta(-(800 / 900.0 * msDiff), -(800 / 900.0f * msDiff));
        m_pPositionComponent->SetPosition(m_pPositionComponent->GetX() + moveDelta.x, m_pPositionComponent->GetY() + moveDelta.y);

        // This feels like a hack
        if (HumanView* pHumanView = g_pApp->GetHumanView())
        {
            shared_ptr<CameraNode> pCamera = pHumanView->GetCamera();
            if (pCamera)
            {
                shared_ptr<EventData_Move_Actor> pEvent(new EventData_Move_Actor(_owner->GetGUID(), m_pPositionComponent->GetPosition()));
                IEventMgr::Get()->VTriggerEvent(pEvent);

                SDL_Rect dummy;
                SDL_Rect renderRect = m_pRenderComponent->VGetPositionRect();
                SDL_Rect cameraRect = pCamera->GetCameraRect();
                if (!SDL_IntersectRect(&renderRect, &cameraRect, &dummy))
                {
                    shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(_owner->GetGUID()));
                    IEventMgr::Get()->VQueueEvent(pEvent);
                }
            }
            else
            {
                LOG_ERROR("Could not retrieve camera");
            }
        }
    }
}