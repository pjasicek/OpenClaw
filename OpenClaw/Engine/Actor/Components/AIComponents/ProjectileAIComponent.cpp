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
#include "../EnemyAI/EnemyAIComponent.h"
#include "../ExplodeableComponent.h"
#include "../ControllableComponent.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"
#include "../../../UserInterface/HumanView.h"

const char* ProjectileAIComponent::g_Name = "ProjectileAIComponent";

ProjectileAIComponent::ProjectileAIComponent()
    :
    m_Damage(0),
    m_DamageType(DamageType_None),
    m_pPhysics(nullptr),
    m_IsActive(true),
    m_SourceActorId(INVALID_ACTOR_ID),
    m_DetonationTime(0),
    m_bHasDetonationTime(false),
    m_NumSparkles(0)
{ }

ProjectileAIComponent::~ProjectileAIComponent()
{
    for (const auto &pSparkle : m_PowerupSparkles)
    {
        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(pSparkle->GetGUID()));
        IEventMgr::Get()->VQueueEvent(pEvent);
    }

    m_PowerupSparkles.clear();
}

bool ProjectileAIComponent::VInit(TiXmlElement* pData)
{
    assert(pData != NULL);

    m_pPhysics = g_pApp->GetGameLogic()->VGetGamePhysics();
    if (!m_pPhysics)
    {
        LOG_WARNING("Attemtping to create projectile component without valid physics");
        return false;
    }

    std::string damageTypeStr;

    ParseValueFromXmlElem(&m_Damage, pData->FirstChildElement("Damage"));
    ParseValueFromXmlElem(&damageTypeStr, pData->FirstChildElement("ProjectileType"));
    ParseValueFromXmlElem(&m_ProjectileSpeed, pData->FirstChildElement("ProjectileSpeed"), "x", "y");
    ParseValueFromXmlElem(&m_SourceActorId, pData->FirstChildElement("SourceActorId"));
    ParseValueFromXmlElem(&m_DetonationTime, pData->FirstChildElement("DetonationTime"));
    ParseValueFromXmlElem(&m_NumSparkles, pData->FirstChildElement("NumSparkles"));
    ParseValueFromXmlElem(&m_bDestroyAfterAnimLoop, pData->FirstChildElement("DestroyAfterAnimLoop"));

    m_bHasDetonationTime = m_DetonationTime > 0;
    m_DamageType = StringToDamageTypeEnum(damageTypeStr);

    assert(m_DamageType != DamageType_None);
    assert(m_Damage >= 0);

    return true;
}

void ProjectileAIComponent::VPostInit()
{
    if (!m_ProjectileSpeed.IsZeroXY())
    {
        m_pPhysics->VSetLinearSpeed(m_pOwner->GetGUID(), m_ProjectileSpeed);
    }

    m_PowerupSparkles.reserve(m_NumSparkles);
    for (int sparkleIdx = 0; sparkleIdx < m_NumSparkles; sparkleIdx++)
    {
        StrongActorPtr pPowerupSparkle = ActorTemplates::CreatePowerupSparkleActor(50);
        assert(pPowerupSparkle);

        shared_ptr<PositionComponent> pPositionComponent = m_pOwner->GetPositionComponent();
        assert(pPositionComponent);

        shared_ptr<PhysicsComponent> pPhysicsComponent = m_pOwner->GetPhysicsComponent();
        assert(pPhysicsComponent);

        shared_ptr<PowerupSparkleAIComponent> pPowerupSparkleAIComponent =
            MakeStrongPtr(pPowerupSparkle->GetComponent<PowerupSparkleAIComponent>(PowerupSparkleAIComponent::g_Name));
        assert(pPowerupSparkleAIComponent);

        pPowerupSparkleAIComponent->SetTargetSize(pPhysicsComponent->GetBodySize());
        pPowerupSparkleAIComponent->SetTargetPositionComponent(pPositionComponent.get());
        
        shared_ptr<ActorRenderComponent> pSparkleRenderComponent = MakeStrongPtr(pPowerupSparkle->GetComponent<ActorRenderComponent>());
        assert(pSparkleRenderComponent != nullptr);

        pSparkleRenderComponent->SetVisible(true);

        m_PowerupSparkles.push_back(pPowerupSparkle);
    }

    if (m_bDestroyAfterAnimLoop)
    {
        AnimationComponent* pAC = MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>()).get();
        assert(pAC != nullptr);

        pAC->AddObserver(this);
    }
}

TiXmlElement* ProjectileAIComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    //

    return baseElement;
}

void ProjectileAIComponent::VUpdate(uint32 msDiff)
{
    assert(g_pApp->GetHumanView() && g_pApp->GetHumanView()->GetCamera());

    Point projectilePos = m_pOwner->GetPositionComponent()->GetPosition();
    if (!g_pApp->GetHumanView()->GetCamera()->IntersectsWithPoint(projectilePos, 1.25f))
    {
        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(m_pOwner->GetGUID()));
        IEventMgr::Get()->VQueueEvent(pEvent);
    }

    if (!m_IsActive && m_bHasDetonationTime)
    {
        m_DetonationTime -= msDiff;
        if (m_DetonationTime <= 0)
        {
            Detonate();
            m_bHasDetonationTime = false;
        }
    }
}

void ProjectileAIComponent::Detonate()
{
    m_pPhysics->VRemoveActor(m_pOwner->GetGUID());

    shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(m_pOwner->GetGUID()));
    IEventMgr::Get()->VQueueEvent(pEvent);

    m_IsActive = false;

    if (m_DamageType == DamageType_Explosion)
    {
        ActorTemplates::CreateSingleAnimation(m_pOwner->GetPositionComponent()->GetPosition(), AnimationType_Explosion);

        SoundInfo soundInfo(SOUND_LEVEL1_KEG_EXPLODE);
        //soundInfo.soundSourcePosition = m_pOwner->GetPositionComponent()->GetPosition();
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));
        ActorTemplates::CreateAreaDamage(
            m_pOwner->GetPositionComponent()->GetPosition(),
            Point(150, 150),
            50,
            CollisionFlag_Explosion,
            "Circle",
            DamageType_Explosion,
            Direction_None,
            m_SourceActorId);
    }
}

void ProjectileAIComponent::OnCollidedWithSolidTile()
{
    if (m_IsActive && !m_bHasDetonationTime)
    {
        Detonate();
    }

    m_IsActive = false;
}

void ProjectileAIComponent::OnCollidedWithActor(Actor* pActorWhoWasShot)
{
    shared_ptr<HealthComponent> pHealthComponent =
        MakeStrongPtr(pActorWhoWasShot->GetComponent<HealthComponent>(HealthComponent::g_Name));
    if (pHealthComponent)
    {
        SDL_Rect areaDamageAABB = g_pApp->GetGameLogic()->VGetGamePhysics()->VGetAABB(m_pOwner->GetGUID(), false);
        Point projectileSpeed = m_pPhysics->VGetVelocity(m_pOwner->GetGUID());

        Direction dir = Direction_Right;
        if (projectileSpeed.x < 0)
        {
            dir = Direction_Left;
        }

        Point contactPoint = Point(areaDamageAABB.x + areaDamageAABB.w / 2, areaDamageAABB.y);
        if (dir == Direction_Left)
        {
            contactPoint.x = areaDamageAABB.x - areaDamageAABB.w / 2;
        }

        pHealthComponent->AddHealth((-1) * m_Damage, m_DamageType, contactPoint, m_SourceActorId);
    }

    if (m_DamageType == DamageType_Bullet)
    {
        OnCollidedWithSolidTile();
    }
    else if (m_DamageType == DamageType_Explosion &&
             (MakeStrongPtr(pActorWhoWasShot->GetComponent<EnemyAIComponent>()) != nullptr ||
              MakeStrongPtr(pActorWhoWasShot->GetComponent<ExplodeableComponent>()) != nullptr) ||
              MakeStrongPtr(pActorWhoWasShot->GetComponent<ClawControllableComponent>()) != nullptr)
    {
        OnCollidedWithSolidTile();
    }
}

void ProjectileAIComponent::VOnAnimationLooped(Animation* pAnimation)
{
    if (m_bDestroyAfterAnimLoop)
    {
        m_pPhysics->VRemoveActor(m_pOwner->GetGUID());

        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(m_pOwner->GetGUID()));
        IEventMgr::Get()->VQueueEvent(pEvent);

        m_IsActive = false;
    }
}
