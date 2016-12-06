#include "HealthComponent.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"

const char* HealthComponent::g_Name = "HealthComponent";

HealthComponent::HealthComponent()
    :
    m_CurrentHealth(0),
    m_MaxHealth(10)
{ }

bool HealthComponent::VInit(TiXmlElement* pData)
{
    if (TiXmlElement* pElem = pData->FirstChildElement("Health"))
    {
        m_CurrentHealth = std::stoi(pElem->GetText());
    }
    if (TiXmlElement* pElem = pData->FirstChildElement("MaxHealth"))
    {
        m_MaxHealth = std::stoi(pElem->GetText());
    }
    LOG(ToStr(m_MaxHealth));

    return true;
}

void HealthComponent::VPostInit()
{
    BroadcastHealthChanged(0, m_CurrentHealth, true);
}

TiXmlElement* HealthComponent::VGenerateXml()
{
    // TODO:
    return NULL;
}

void HealthComponent::AddHealth(uint32 health)
{
    uint32 oldHealth = m_CurrentHealth;
    m_CurrentHealth += health;
    if (m_CurrentHealth > m_MaxHealth)
    {
        m_CurrentHealth = m_MaxHealth;
    }

    if (oldHealth != m_CurrentHealth)
    {
        BroadcastHealthChanged(oldHealth, m_CurrentHealth);
    }
}

void HealthComponent::SetCurrentHealth(uint32 health)
{
    uint32 oldHealth = m_CurrentHealth;
    m_CurrentHealth = health;
    if (m_CurrentHealth > m_MaxHealth)
    {
        m_CurrentHealth = m_MaxHealth;
    }

    if (oldHealth != m_CurrentHealth)
    {
        BroadcastHealthChanged(oldHealth, m_CurrentHealth);
    }
}

void HealthComponent::BroadcastHealthChanged(uint32 oldHealth, uint32 newHealth, bool isInitial)
{
    NotifyHealthChanged(oldHealth, newHealth);
    if (newHealth <= 0)
    {
        NotifyHealthBelowZero();
    }

    shared_ptr<EventData_Updated_Health> pEvent(new EventData_Updated_Health(oldHealth, newHealth, isInitial));
    IEventMgr::Get()->VQueueEvent(pEvent);
}

//=====================================================================================================================
// HealthSubject implementation
//=====================================================================================================================

void HealthSubject::NotifyHealthChanged(int32 oldHealth, int32 newHealth)
{
    for (HealthObserver* pObserver : m_Observers)
    {
        pObserver->VOnHealthChanged(oldHealth, newHealth);
    }
}

void HealthSubject::NotifyHealthBelowZero()
{
    for (HealthObserver* pObserver : m_Observers)
    {
        pObserver->VOnHealthBelowZero();
    }
}