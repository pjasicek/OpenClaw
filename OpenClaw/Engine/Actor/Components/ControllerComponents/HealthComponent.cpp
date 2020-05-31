#include "HealthComponent.h"
#include "../ControllableComponent.h"
#include "../../Actor.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"

#include "../../../GameApp/BaseGameApp.h"

const char* HealthComponent::g_Name = "HealthComponent";

HealthComponent::HealthComponent()
    :
    m_CurrentHealth(0),
    m_MaxHealth(10),
    m_IsController(false),
    m_bInvulnerable(false)
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

    return true;
}

void HealthComponent::VPostInit()
{
    if (MakeStrongPtr(m_pOwner->GetComponent<ClawControllableComponent>(ClawControllableComponent::g_Name)))
    {
        m_IsController = true;
    }

    BroadcastHealthChanged(0, m_CurrentHealth, DamageType_None, Point(0, 0), true);
}

TiXmlElement* HealthComponent::VGenerateXml()
{
    // TODO:
    return NULL;
}

void HealthComponent::AddHealth(int32 health, DamageType damageType, Point impactPoint, int sourceActorId)
{
    if ((m_bInvulnerable && health < 0) && (damageType != DamageType_DeathTile))
    {
        return;
    }

    if (g_pApp->GetGameCheats()->clawInvincible && health < 0)
    {
        if (MakeStrongPtr(m_pOwner->GetComponent<ClawControllableComponent>(ClawControllableComponent::g_Name)))
        {
            return;
        }
    }

    // TODO: Is this against subject-observer pattern ? Seems odd.
    if (AskCanResistDamage(damageType, impactPoint))
    {
        NotifyResistDamage(damageType, impactPoint);
        return;
    }

    int32 oldHealth = m_CurrentHealth;
    m_CurrentHealth += health;
    if (m_CurrentHealth > m_MaxHealth)
    {
        m_CurrentHealth = m_MaxHealth;
    }

    if (oldHealth != m_CurrentHealth || 
        damageType == DamageType_SirenProjectile) // TODO: hacky solution and needs to be fixed somehow: Siren Projectile has 0 damage
    {
        BroadcastHealthChanged(oldHealth, m_CurrentHealth, damageType, impactPoint, sourceActorId);
    }
}

void HealthComponent::SetCurrentHealth(int32 health)
{
    int32 oldHealth = m_CurrentHealth;
    m_CurrentHealth = health;
    if (m_CurrentHealth > m_MaxHealth)
    {
        m_CurrentHealth = m_MaxHealth;
    }

    if (oldHealth != m_CurrentHealth)
    {
        BroadcastHealthChanged(oldHealth, m_CurrentHealth, DamageType_None, Point(0, 0), INVALID_ACTOR_ID);
    }
}

void HealthComponent::BroadcastHealthChanged(int32 oldHealth, int32 newHealth, DamageType damageType, Point impactPoint, int sourceActorId, bool isInitial)
{
    NotifyHealthChanged(oldHealth, newHealth, damageType, impactPoint, sourceActorId);
    if (newHealth <= 0)
    {
        NotifyHealthBelowZero(damageType, sourceActorId);
    }

    // Only broadcast controllers health. this is abit hacky
    if (m_IsController)
    {
        shared_ptr<EventData_Updated_Health> pEvent(new EventData_Updated_Health(oldHealth, newHealth, isInitial));
        IEventMgr::Get()->VQueueEvent(pEvent);
    }
}

//=====================================================================================================================
// HealthSubject implementation
//=====================================================================================================================

void HealthSubject::NotifyHealthChanged(int32 oldHealth, int32 newHealth, DamageType damageType, Point impactPoint, int sourceActorId)
{
    for (HealthObserver* pObserver : m_Observers)
    {
        pObserver->VOnHealthChanged(oldHealth, newHealth, damageType, impactPoint, sourceActorId);
    }
}

void HealthSubject::NotifyHealthBelowZero(DamageType damageType, int sourceActorId)
{
    for (HealthObserver* pObserver : m_Observers)
    {
        pObserver->VOnHealthBelowZero(damageType, sourceActorId);
    }
}

void HealthSubject::NotifyResistDamage(DamageType damageType, Point impactPoint)
{
    for (HealthObserver* pObserver : m_Observers)
    {
        pObserver->VOnResistDamage(damageType, impactPoint);
    }
}

bool HealthSubject::AskCanResistDamage(DamageType damageType, Point impactPoint)
{
    for (HealthObserver* pObserver : m_Observers)
    {
        if (pObserver->VCanResistDamage(damageType, impactPoint))
        {
            return true;
        }
    }

    return false;
}