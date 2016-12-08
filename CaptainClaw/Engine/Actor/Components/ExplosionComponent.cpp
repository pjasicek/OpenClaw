#include "ExplosionComponent.h"
#include "../../GameApp/BaseGameApp.h"
#include "../../GameApp/BaseGameLogic.h"

#include "../../Events/EventMgr.h"
#include "../../Events/Events.h"

const char* ExplosionComponent::g_Name = "ExplosionComponent";

ExplosionComponent::ExplosionComponent()
    :
    m_Damage(50),
    m_ExplodingTime(50),
    m_ActiveTime(0)
{ }

bool ExplosionComponent::VDelegateInit(TiXmlElement* pData)
{
    assert(pData);

    if (TiXmlElement* pElem = pData->FirstChildElement("ExplodingTime"))
    {
        m_ExplodingTime = std::stoi(pElem->GetText());
    }
    if (TiXmlElement* pElem = pData->FirstChildElement("Damage"))
    {
        m_Damage = std::stoi(pElem->GetText());
    }

    assert(m_ExplodingTime > 0);

    return true;
}

void ExplosionComponent::VCreateInheritedXmlElements(TiXmlElement* pBaseElement)
{

}

bool ExplosionComponent::VOnApply(Actor* pActorWhoPickedThis)
{
    shared_ptr<HealthComponent> pHealthComponent =
        MakeStrongPtr(pActorWhoPickedThis->GetComponent<HealthComponent>(HealthComponent::g_Name));
    if (pHealthComponent)
    {
        pHealthComponent->AddHealth(-m_Damage);
    }

    return false;
}

void ExplosionComponent::VUpdate(uint32 msDiff)
{
    m_ActiveTime += msDiff;

    if (m_ActiveTime >= m_ExplodingTime)
    {
        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(_owner->GetGUID()));
        IEventMgr::Get()->VQueueEvent(pEvent);
    }
}