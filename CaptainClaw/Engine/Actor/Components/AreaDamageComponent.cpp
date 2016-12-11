#include "AreaDamageComponent.h"
#include "../../GameApp/BaseGameApp.h"
#include "../../GameApp/BaseGameLogic.h"

#include "../../Events/EventMgr.h"
#include "../../Events/Events.h"

const char* AreaDamageComponent::g_Name = "AreaDamageComponent";

AreaDamageComponent::AreaDamageComponent()
    :
    m_Damage(50),
    m_Duration(50),
    m_ActiveTime(0)
{ }

bool AreaDamageComponent::VDelegateInit(TiXmlElement* pData)
{
    assert(pData);

    if (TiXmlElement* pElem = pData->FirstChildElement("Duration"))
    {
        m_Duration = std::stoi(pElem->GetText());
    }
    if (TiXmlElement* pElem = pData->FirstChildElement("Damage"))
    {
        m_Damage = std::stoi(pElem->GetText());
    }

    assert(m_Duration > 0);

    return true;
}

void AreaDamageComponent::VCreateInheritedXmlElements(TiXmlElement* pBaseElement)
{

}

bool AreaDamageComponent::VOnApply(Actor* pActorWhoPickedThis)
{
    shared_ptr<HealthComponent> pHealthComponent =
        MakeStrongPtr(pActorWhoPickedThis->GetComponent<HealthComponent>(HealthComponent::g_Name));
    if (pHealthComponent)
    {
        pHealthComponent->AddHealth(-m_Damage);
    }

    return false;
}

void AreaDamageComponent::VUpdate(uint32 msDiff)
{
    m_ActiveTime += msDiff;

    if (m_ActiveTime >= m_Duration)
    {
        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(_owner->GetGUID()));
        IEventMgr::Get()->VQueueEvent(pEvent);
    }
}