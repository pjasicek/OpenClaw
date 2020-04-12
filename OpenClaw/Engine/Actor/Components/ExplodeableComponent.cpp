#include "ExplodeableComponent.h"
#include "../../GameApp/BaseGameApp.h"
#include "../../GameApp/BaseGameLogic.h"
#include "PositionComponent.h"

#include "../../Events/EventMgr.h"
#include "../../Events/Events.h"

#include "../ActorTemplates.h"

const char* ExplodeableComponent::g_Name = "ExplodeableComponent";

ExplodeableComponent::ExplodeableComponent()
    :
    m_ExplosionSize(Point(0, 0)),
    m_ExplodingTime(50),
    m_Damage(50)
{ }

ExplodeableComponent::~ExplodeableComponent()
{

}

bool ExplodeableComponent::VInit(TiXmlElement* pData)
{
    assert(pData != NULL);

    if (TiXmlElement* pElem = pData->FirstChildElement("ExplosionSize"))
    {
        pElem->Attribute("width", &m_ExplosionSize.x);
        pElem->Attribute("height", &m_ExplosionSize.y);
    }
    if (TiXmlElement* pElem = pData->FirstChildElement("ExplodingTime"))
    {
        m_ExplodingTime = std::stoi(pElem->GetText());
    }
    if (TiXmlElement* pElem = pData->FirstChildElement("Damage"))
    {
        m_Damage = std::stoi(pElem->GetText());
    }

    assert(m_ExplodingTime > 0);
    assert(!m_ExplosionSize.IsZero());

    return true;
}

void ExplodeableComponent::VPostInit()
{
    shared_ptr<HealthComponent> pHealthComponent =
        MakeStrongPtr(m_pOwner->GetComponent<HealthComponent>(HealthComponent::g_Name));
    // If there is no health component then the object cannot explode
    assert(pHealthComponent);
    if (pHealthComponent)
    {
        pHealthComponent->AddObserver(this);
    }
}

TiXmlElement* ExplodeableComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    return baseElement;
}

void ExplodeableComponent::VOnHealthBelowZero(DamageType damageType, int sourceActorId)
{
    shared_ptr<PositionComponent> pPositionComponent = m_pOwner->GetPositionComponent();
    assert(pPositionComponent);

    ActorTemplates::CreateAreaDamage(
        pPositionComponent->GetPosition(), 
        m_ExplosionSize, 
        m_Damage, 
        CollisionFlag_Explosion, 
        "Circle", 
        DamageType_Explosion, 
        Direction_None, 
        m_pOwner->GetGUID(),
        Point(0, 40));
}