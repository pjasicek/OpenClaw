#include "AuraComponent.h"
#include "../ControllerComponents/HealthComponent.h"
#include "../../Actor.h"

const char* BaseAuraComponent::g_Name = "BaseAuraComponent";
const char* DamageAuraComponent::g_Name = "DamageAuraComponent";

//=====================================================================================================================
//
// BaseAuraComponent Implementation
//
//=====================================================================================================================

BaseAuraComponent::BaseAuraComponent()
    :
    m_bIsPulsating(false),
    m_bIsGroupPulse(false),
    m_bApplyAuraOnEnter(true),
    m_bRemoveActorAfterPulse(false),
    m_PulseInterval(0),
    m_TimeSinceLastPulse(0),
    m_bIsEnabled(true)
{

}

bool BaseAuraComponent::VInit(TiXmlElement* data)
{
    SetBoolIfDefined(&m_bIsPulsating, data->FirstChildElement("IsPulsating"));
    SetBoolIfDefined(&m_bIsGroupPulse, data->FirstChildElement("IsGroupPulse"));
    SetBoolIfDefined(&m_bApplyAuraOnEnter, data->FirstChildElement("ApplyAuraOnEnter"));
    SetBoolIfDefined(&m_bRemoveActorAfterPulse, data->FirstChildElement("RemoveAfterPulse"));
    SetIntIfDefined(&m_PulseInterval, data->FirstChildElement("PulseInterval"));

    if (!VDelegateInit(data))
    {
        return false;
    }

    return true;
}

void BaseAuraComponent::VPostInit()
{

}

void BaseAuraComponent::VPostPostInit()
{

}

TiXmlElement* BaseAuraComponent::VGenerateXml()
{
    // TODO: Implement
    return NULL;
}

void BaseAuraComponent::VUpdate(uint32 msDiff)
{
    if (!m_bIsPulsating || !m_bIsEnabled)
    {
        return;
    }

    if (m_bIsGroupPulse)
    {
        m_TimeSinceLastPulse += msDiff;
        if (m_TimeSinceLastPulse >= m_PulseInterval)
        {
            for (PulseInfo actorPulse : m_ActivePulseList)
            {
                if (actorPulse.pActor == NULL)
                {
                    continue;
                }

                VOnAuraApply(actorPulse.pActor);
            }

            m_TimeSinceLastPulse = 0;
        }
    }
    else
    {
        for (PulseInfo actorPulse : m_ActivePulseList)
        {
            actorPulse.timeSinceLastPulseMs += msDiff;
            if (actorPulse.timeSinceLastPulseMs >= m_PulseInterval)
            {
                if (actorPulse.pActor == NULL)
                {
                    continue;
                }

                VOnAuraApply(actorPulse.pActor);
                actorPulse.timeSinceLastPulseMs = 0;

                if (m_bRemoveActorAfterPulse)
                {
                    OnActorLeft(actorPulse.pActor);
                }
            }
        }
    }
}

void BaseAuraComponent::OnActorEntered(Actor* pActor)
{
    for (ActivePulseList::iterator iter = m_ActivePulseList.begin();
        iter != m_ActivePulseList.end();
        ++iter)
    {
        if (iter->pActor == pActor)
        {
            return;
        }
    }

    // Actor who entered was not actually in this aura, add him

    PulseInfo newActorPulse;
    newActorPulse.pActor = pActor;
    m_ActivePulseList.push_back(newActorPulse);

    if (m_bApplyAuraOnEnter)
    {
        VOnAuraApply(pActor);
    }
}

void BaseAuraComponent::OnActorLeft(Actor* pActor)
{
    for (ActivePulseList::iterator iter = m_ActivePulseList.begin();
        iter != m_ActivePulseList.end();
        ++iter)
    {
        if (iter->pActor == pActor)
        {
            m_ActivePulseList.erase(iter);
            return;
        }
    }
}

//=====================================================================================================================
//
// DamageAuraComponent Implementation
//
//=====================================================================================================================

DamageAuraComponent::DamageAuraComponent()
    :
    m_Damage(0)
{
}

bool DamageAuraComponent::VDelegateInit(TiXmlElement* data)
{
    assert(data);

    SetIntIfDefined(&m_Damage, data->FirstChildElement("Damage"));

    return true;
}

void DamageAuraComponent::VPostInit()
{
    
}

void DamageAuraComponent::VCreateInheritedXmlElements(TiXmlElement* pBaseElement)
{

}

void DamageAuraComponent::VOnAuraApply(Actor* pActorInAura)
{
    shared_ptr<HealthComponent> pHealthComponent =
        MakeStrongPtr(pActorInAura->GetComponent<HealthComponent>(HealthComponent::g_Name));
    if (pHealthComponent)
    {
        pHealthComponent->AddHealth((-1) * m_Damage);
    }
}