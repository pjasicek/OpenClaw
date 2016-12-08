#include <Tinyxml/tinyxml.h>
#include "TriggerComponent.h"
#include "../PositionComponent.h"
#include "../../../GameApp/BaseGameApp.h"
#include "../../../GameApp/BaseGameLogic.h"
#include "../RenderComponent.h"
#include "../../../Graphics2D/Image.h"
#include "../../../Physics/ClawPhysics.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"

const char* TriggerComponent::g_Name = "TriggerComponent";

TriggerComponent::TriggerComponent()
    :
    /*m_IsTriggerOnce(false),
    m_IsTriggerUnlimited(false),
    m_TriggerRemaining(0),*/
    m_Size(Point(0, 0)),
    m_IsStatic(false)
{ }

TriggerComponent::~TriggerComponent()
{
    m_pPhysics->VRemoveActor(_owner->GetGUID());
}

bool TriggerComponent::VInit(TiXmlElement* data)
{
    assert(data != NULL);

    m_pPhysics = g_pApp->GetGameLogic()->VGetGamePhysics();
    if (!m_pPhysics)
    {
        LOG_ERROR("Invalid physics");
        return false;
    }
    
    if (TiXmlElement* pElem = data->FirstChildElement("Size"))
    {
        pElem->Attribute("width", &m_Size.x);
        pElem->Attribute("height", &m_Size.y);
    }
    if (TiXmlElement* pElem = data->FirstChildElement("IsStatic"))
    {
        m_IsStatic = std::string(pElem->GetText()) == "true";
    }
    /*if (TiXmlElement* pElem = data->FirstChildElement("TriggerUnlimited"))
    {
        m_IsTriggerUnlimited = std::string(pElem->GetText()) == "true";
    }
    if (TiXmlElement* pElem = data->FirstChildElement("TriggerOnce"))
    {
        m_IsTriggerOnce = std::string(pElem->GetText()) == "true";
    }
    if (TiXmlElement* pElem = data->FirstChildElement("TriggerFinitedTimes"))
    {
        m_TriggerRemaining = std::stoi(std::string(pElem->GetText()));
    }*/

    return true;
}

void TriggerComponent::VPostInit()
{
    if (m_IsStatic)
    {
        int offsetX = 0;
        int offsetY = 0;

        // Set size from current image if necessary
        if (fabs(m_Size.x) < DBL_EPSILON || fabs(m_Size.y) < DBL_EPSILON)
        {
            shared_ptr<ActorRenderComponent> pRenderComponent =
                MakeStrongPtr(_owner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name));
            assert(pRenderComponent);

            shared_ptr<Image> pImage = MakeStrongPtr(pRenderComponent->GetCurrentImage());

            m_Size.x = pImage->GetWidth();
            m_Size.y = pImage->GetHeight();

            offsetX = pImage->GetOffsetX();
            offsetY = pImage->GetOffsetY();
        }

        shared_ptr<PositionComponent> pPositionComponent =
            MakeStrongPtr(_owner->GetComponent<PositionComponent>(PositionComponent::g_Name));
        assert(pPositionComponent);

        Point physPos = Point(pPositionComponent->GetX() + offsetX, pPositionComponent->GetY() + offsetY);

        m_pPhysics->VCreateTrigger(_owner, physPos , m_Size, m_IsStatic);
    }
}

TiXmlElement* TriggerComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    //

    return baseElement;
}

void TriggerComponent::OnActorEntered(Actor* pActor)
{
    // Part of the actor may already be inside the trigger. If it is, ignore it
    if (HasOverlappingActor(pActor))
    {
        AddOverlappingActor(pActor);
        return;
    }
    AddOverlappingActor(pActor);

    NotifyEnterTrigger(pActor);

    /*m_TriggerRemaining--;
    if (!m_IsTriggerUnlimited && (m_IsTriggerOnce || (m_TriggerRemaining <= 0)))
    {
        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(_owner->GetGUID()));
        IEventMgr::Get()->VQueueEvent(pEvent);
    }*/
}

void TriggerComponent::OnActorLeft(Actor* pActor)
{
    RemoveOverlappingActor(pActor);
    if (HasOverlappingActor(pActor))
    {
        return;
    }

    NotifyLeaveTrigger(pActor);
}

SDL_Rect TriggerComponent::GetTriggerArea()
{
    SDL_Rect triggerArea = { 0 };

    shared_ptr<PositionComponent> pPositionComponent =
        MakeStrongPtr(_owner->GetComponent<PositionComponent>(PositionComponent::g_Name));
    if (!pPositionComponent)
    {
        return triggerArea;
    }

    Point triggerPosition = pPositionComponent->GetPosition();
    triggerArea = { triggerPosition.x, triggerPosition.y, m_Size.x, m_Size.y };

    return triggerArea;
}

void TriggerComponent::AddOverlappingActor(Actor* pActor)
{
    m_ActorsInsideList.push_back(pActor);
}

void TriggerComponent::RemoveOverlappingActor(Actor* pActor)
{
    for (auto iter = m_ActorsInsideList.begin(); iter != m_ActorsInsideList.end(); ++iter)
    {
        if ((*iter) == pActor)
        {
            m_ActorsInsideList.erase(iter);
            return;
        }
    }
    LOG_WARNING("Could not remove overlapping actor - no such actor found")
}

bool TriggerComponent::HasOverlappingActor(Actor* pActor)
{
    if (std::find(m_ActorsInsideList.begin(), m_ActorsInsideList.end(), pActor) != m_ActorsInsideList.end())
    {
        return true;
    }

    return false;
}

//=====================================================================================================================
// TriggerSubject implementation
//=====================================================================================================================

void TriggerSubject::NotifyEnterTrigger(Actor* pActorWhoEntered)
{
    for (TriggerObserver* pObserver : m_Observers)
    {
        pObserver->VOnActorEnteredTrigger(pActorWhoEntered);
    }
}

void TriggerSubject::NotifyLeaveTrigger(Actor* pActorWhoLeft)
{
    for (TriggerObserver* pObserver : m_Observers)
    {
        pObserver->VOnActorLeftTrigger(pActorWhoLeft);
    }
}