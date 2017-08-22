#include "LifeComponent.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"

const char* LifeComponent::g_Name = "LifeComponent";

LifeComponent::LifeComponent()
    :
    m_CurrentLives(0)
{ }

bool LifeComponent::VInit(TiXmlElement* pData)
{
    if (TiXmlElement* pElem = pData->FirstChildElement("Lives"))
    {
        m_CurrentLives = std::stoi(pElem->GetText());
    }

    return true;
}

void LifeComponent::VPostInit()
{
    BroadcastLivesChanged(0, m_CurrentLives, true);
}

TiXmlElement* LifeComponent::VGenerateXml()
{
    // TODO:
    return NULL;
}

void LifeComponent::AddLives(uint32 numLives)
{
    uint32 oldLives = m_CurrentLives;
    m_CurrentLives += numLives;
    if (m_CurrentLives > 9)
    {
        m_CurrentLives = 9;
    }

    if (oldLives != m_CurrentLives)
    {
        BroadcastLivesChanged(oldLives, m_CurrentLives);
    }
}

void LifeComponent::SetCurrentLives(uint32 numNewLives)
{
    uint32 oldLives = m_CurrentLives;
    m_CurrentLives = numNewLives;
    if (m_CurrentLives > 9)
    {
        m_CurrentLives = 9;
    }

    if (oldLives != m_CurrentLives)
    {
        BroadcastLivesChanged(oldLives, m_CurrentLives);
    }
}

void LifeComponent::BroadcastLivesChanged(uint32 oldLives, uint32 newLives, bool isInitial)
{
    shared_ptr<EventData_Updated_Lives> pEvent(new EventData_Updated_Lives(oldLives, newLives, isInitial));
    IEventMgr::Get()->VQueueEvent(pEvent);
}