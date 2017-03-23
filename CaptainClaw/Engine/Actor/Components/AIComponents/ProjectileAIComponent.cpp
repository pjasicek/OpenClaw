#include "ProjectileAIComponent.h"
#include "../../../GameApp/BaseGameApp.h"
#include "../../../GameApp/BaseGameLogic.h"

#include "../../../Graphics2D/Image.h"
#include "../../../Physics/ClawPhysics.h"

#include "../PositionComponent.h"
#include "../RenderComponent.h"
#include "../PowerupSparkleAIComponent.h"
#include "../PhysicsComponent.h"
#include "../../ActorTemplates.h"
#include "../ControllerComponents/HealthComponent.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"

const char* ProjectileAIComponent::g_Name = "ProjectileAIComponent";

ProjectileAIComponent::ProjectileAIComponent()
    :
    m_Damage(0),
    m_Type("Unknown"),
    m_pPhysics(nullptr),
    m_IsActive(true)
{ }

ProjectileAIComponent::~ProjectileAIComponent()
{

}

bool ProjectileAIComponent::VInit(TiXmlElement* data)
{
    assert(data != NULL);

    m_pPhysics = g_pApp->GetGameLogic()->VGetGamePhysics();
    if (!m_pPhysics)
    {
        LOG_WARNING("Attemtping to create projectile component without valid physics");
        return false;
    }

    if (TiXmlElement* pElem = data->FirstChildElement("Damage"))
    {
        m_Damage = std::stoi(pElem->GetText());
    }
    if (TiXmlElement* pElem = data->FirstChildElement("ProjectileType"))
    {
        // Possible types: "Bullet", "Magic", "Dynamite"
        m_Type = pElem->GetText();
    }

    assert(m_Damage > 0);

    return true;
}

void ProjectileAIComponent::VPostInit()
{

}

TiXmlElement* ProjectileAIComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    //

    return baseElement;
}

void ProjectileAIComponent::OnCollidedWithSolidTile()
{
    if (m_IsActive)
    {
        m_pPhysics->VRemoveActor(_owner->GetGUID());

        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(_owner->GetGUID()));
        IEventMgr::Get()->VQueueEvent(pEvent);

        m_IsActive = false;
    }
}

void ProjectileAIComponent::OnCollidedWithActor(Actor* pActorWhoWasShot)
{
    shared_ptr<HealthComponent> pHealthComponent =
        MakeStrongPtr(pActorWhoWasShot->GetComponent<HealthComponent>(HealthComponent::g_Name));
    if (pHealthComponent)
    {
        pHealthComponent->AddHealth((-1) * m_Damage);
    }

    if (m_Type == "Bullet")
    {
        OnCollidedWithSolidTile();
    }
}