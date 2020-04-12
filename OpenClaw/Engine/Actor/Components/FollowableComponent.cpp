#include "FollowableComponent.h"
#include "PositionComponent.h"
#include "RenderComponent.h"
#include "../ActorTemplates.h"
#include "../Actor.h"

#include "../../Events/EventMgr.h"
#include "../../Events/Events.h"

//=====================================================================================================================
//
// FollowableComponent Implementation
//
//=====================================================================================================================

const char* FollowableComponent::g_Name = "FollowableComponent";

FollowableComponent::FollowableComponent() :
    m_pPositionComponent(NULL),
    m_pTargetPositionComponent(NULL),
    m_pTargetRenderComponent(NULL),
    m_MsDuration(0),
    m_CurrentMsDuration(0)
{ }

FollowableComponent::~FollowableComponent()
{
    shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(m_pFollowingActor->GetGUID()));
    IEventMgr::Get()->VQueueEvent(pEvent);
}

bool FollowableComponent::VInit(TiXmlElement* pData)
{
    assert(pData);

    if (TiXmlElement* pElem = pData->FirstChildElement("Offset"))
    {
        pElem->Attribute("x", &m_Offset.x);
        pElem->Attribute("y", &m_Offset.y);
    }

    SetStringIfDefined(&m_ImageSet, pData->FirstChildElement("ImageSet"));
    SetStringIfDefined(&m_AnimationPath, pData->FirstChildElement("AnimationPath"));

    return true;
}

void FollowableComponent::VPostInit()
{
    m_pPositionComponent = m_pOwner->GetPositionComponent().get();

    Point ownerPos = m_pPositionComponent->GetPosition();
    m_pFollowingActor = ActorTemplates::CreateRenderedActor(
        Point(ownerPos.x + m_Offset.x, ownerPos.y + m_Offset.y), m_ImageSet, m_AnimationPath, zIndexes::FollowableComponent).get();
    assert(m_pFollowingActor != NULL);

    m_pTargetPositionComponent = m_pFollowingActor->GetPositionComponent().get();
    m_pTargetRenderComponent =
        MakeStrongPtr(m_pFollowingActor->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name)).get();

    assert(m_pPositionComponent != NULL);
    assert(m_pTargetPositionComponent != NULL);
    assert(m_pTargetRenderComponent != NULL);

    m_pTargetRenderComponent->SetVisible(false);
}

TiXmlElement* FollowableComponent::VGenerateXml()
{
    // TODO: Implement
    return NULL;
}

void FollowableComponent::VUpdate(uint32 msDiff)
{
    if (m_CurrentMsDuration < m_MsDuration)
    {
        m_CurrentMsDuration += msDiff;

        Point ownerPos = m_pPositionComponent->GetPosition();
        m_pTargetPositionComponent->SetPosition(ownerPos.x + m_Offset.x, ownerPos.y + m_Offset.y);

        shared_ptr<EventData_Move_Actor> pEvent(
            new EventData_Move_Actor(m_pFollowingActor->GetGUID(), m_pTargetPositionComponent->GetPosition()));
        IEventMgr::Get()->VTriggerEvent(pEvent);
    }
    else if (m_MsDuration > 0)
    {
        Deactivate();
    }
}

void FollowableComponent::Activate(int msDuration)
{
    if (msDuration > 0)
    {
        m_pTargetRenderComponent->SetVisible(true);

        Point ownerPos = m_pPositionComponent->GetPosition();
        m_pTargetPositionComponent->SetPosition(ownerPos.x + m_Offset.x, ownerPos.y + m_Offset.y);

        m_MsDuration = msDuration;
        m_CurrentMsDuration = 0;
    }
}

void FollowableComponent::Deactivate()
{
    m_MsDuration = -1;
    m_CurrentMsDuration = 0;
    m_pTargetRenderComponent->SetVisible(false);
}