#include "GlitterComponent.h"
#include "PositionComponent.h"
#include "PhysicsComponent.h"
#include "../../Events/EventMgr.h"
#include "../../Events/Events.h"

#include "../ActorTemplates.h"

const char* GlitterComponent::g_Name = "GlitterComponent";

// This whole thing feels like a HACK
// This component (e.g. actor) is created in PowerupComponent component

GlitterComponent::GlitterComponent()
    :
    m_SpawnImmediate(false),
    m_FollowOwner(false),
    m_GlitterType("Glitter_Yellow"),
    m_pGlitter(nullptr),
    m_Active(true)
{ }

GlitterComponent::~GlitterComponent()
{
    if (m_pGlitter)
    {
        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(m_pGlitter->GetGUID()));
        IEventMgr::Get()->VTriggerEvent(pEvent);
    }
}

bool GlitterComponent::VInit(TiXmlElement* pData)
{
    assert(pData);

    if (TiXmlElement* pElem = pData->FirstChildElement("SpawnImmediate"))
    {
        m_SpawnImmediate = std::string(pElem->GetText()) == "true";
    }
    if (TiXmlElement* pElem = pData->FirstChildElement("FollowOwner"))
    {
        m_FollowOwner = std::string(pElem->GetText()) == "true";
    }
    // 3 Types: "Glitter_Yellow", "Glitter_Red", "Glitter_Green"
    if (TiXmlElement* pElem = pData->FirstChildElement("GlitterType"))
    {
        m_GlitterType = pElem->GetText();
    }

    return true;
}

void GlitterComponent::VPostInit()
{
    m_pPositonComponent = m_pOwner->GetPositionComponent().get();
    assert(m_pPositonComponent);

    if (m_SpawnImmediate)
    {
        m_pGlitter = ActorTemplates::CreateGlitter(m_GlitterType, m_pPositonComponent->GetPosition());
    }
}

TiXmlElement* GlitterComponent::VGenerateXml()
{
    return NULL;

    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    return baseElement;
}

void GlitterComponent::VUpdate(uint32 msDiff)
{
    // Update position if necessary
    if (m_pGlitter && m_FollowOwner)
    {
        shared_ptr<EventData_Move_Actor> pEvent(new EventData_Move_Actor(m_pGlitter->GetGUID(), m_pPositonComponent->GetPosition()));
        IEventMgr::Get()->VTriggerEvent(pEvent);
    }
    // Spawn glitter
    else if (!m_pGlitter && m_Active)
    {
        shared_ptr<PhysicsComponent> pPhysicsComponent = m_pOwner->GetPhysicsComponent();
        assert(pPhysicsComponent && "Sparkle not spawned in post init and sparkle cannot be spawned in update");

        // Spawn sparkle if actor is still
        if (pPhysicsComponent && !pPhysicsComponent->IsAwake())
        {
            m_pGlitter = ActorTemplates::CreateGlitter(m_GlitterType, m_pPositonComponent->GetPosition());
        }
    }
}

void GlitterComponent::Deactivate()
{
    m_Active = false;

    if (m_pGlitter)
    {
        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(m_pGlitter->GetGUID()));
        IEventMgr::Get()->VTriggerEvent(pEvent);

        m_pGlitter.reset();
        m_pGlitter = nullptr;
    }
}