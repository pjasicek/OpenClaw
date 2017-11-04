#include "ProjectileSpawnerComponent.h"
#include "../../GameApp/BaseGameApp.h"
#include "../../GameApp/BaseGameLogic.h"
#include "PositionComponent.h"
#include "../../UserInterface/HumanView.h"
#include "RenderComponent.h"

const char* ProjectileSpawnerComponent::g_Name = "ProjectileSpawnerComponent";

ProjectileSpawnerComponent::ProjectileSpawnerComponent()
    :
    m_ActorsInTriggerArea(0),
    m_bReady(false),
    m_StartDelayLeft(0)
{

}

ProjectileSpawnerComponent::~ProjectileSpawnerComponent()
{

}

bool ProjectileSpawnerComponent::VInit(TiXmlElement* pData)
{
    assert(pData != NULL);

    m_Properties.LoadFromXml(pData, true);

    m_StartDelayLeft = m_Properties.startSpawnDelay;

    return true;
}

TiXmlElement* ProjectileSpawnerComponent::VGenerateXml()
{
    return m_Properties.ToXml();
}

void ProjectileSpawnerComponent::VPostInit()
{
    m_pAnimationComponent = 
        MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>()).get();
    m_pARC = m_pOwner->GetRawComponent<ActorRenderComponent>(true);
    assert(m_pAnimationComponent != NULL);

    if (m_Properties.idleAnim == "INVISIBLE")
    {
        m_pARC->SetVisible(false);
        m_pAnimationComponent->PauseAnimation();
    }
    else
    {
        m_pAnimationComponent->SetAnimation(m_Properties.idleAnim);
    }

    shared_ptr<TriggerComponent> pTriggerComponent = 
        MakeStrongPtr(m_pOwner->GetComponent<TriggerComponent>());
    assert(pTriggerComponent != nullptr);

    m_pAnimationComponent->AddObserver(this);
    pTriggerComponent->AddObserver(this);
}

void ProjectileSpawnerComponent::VPostPostInit()
{
    if (!m_Properties.triggerAreaSize.IsZeroXY() && !m_Properties.isAlwaysOn)
    {
        ActorFixtureDef fixtureDef;
        fixtureDef.collisionShape = "Rectangle";
        fixtureDef.fixtureType = FixtureType_Trigger;
        fixtureDef.size = m_Properties.triggerAreaSize;
        fixtureDef.offset = m_Properties.triggerAreaOffset;
        fixtureDef.collisionMask = m_Properties.triggerCollisionMask;
        fixtureDef.collisionFlag = CollisionFlag_Trigger;
        fixtureDef.isSensor = true;

        g_pApp->GetGameLogic()->VGetGamePhysics()->VAddActorFixtureToBody(m_pOwner->GetGUID(), &fixtureDef);
    }
}

void ProjectileSpawnerComponent::VUpdate(uint32 msDiff)
{
    if (!m_bReady)
    {
        m_StartDelayLeft -= msDiff;
        if (m_StartDelayLeft <= 0)
        {
            m_bReady = true;
        }
    }

    TryToFire();
}

void ProjectileSpawnerComponent::VOnActorEnteredTrigger(Actor* pActorWhoEntered, FixtureType triggerType)
{
    m_ActorsInTriggerArea++;
    m_pARC->SetVisible(true);
    TryToFire();
    assert(m_ActorsInTriggerArea >= 0);
}

void ProjectileSpawnerComponent::VOnActorLeftTrigger(Actor* pActorWhoLeft, FixtureType triggerType)
{
    m_ActorsInTriggerArea--;
    assert(m_ActorsInTriggerArea >= 0);
}

void ProjectileSpawnerComponent::VOnAnimationLooped(Animation* pAnimation)
{
    if (pAnimation->GetName() == m_Properties.fireAnim && m_Properties.projectileSpawnAnimFrameIdx == 0)
    {
        SpawnProjectile();
    }

    if (m_Properties.idleAnim == "INVISIBLE")
    {
        m_pARC->SetVisible(false);
        m_pAnimationComponent->PauseAnimation();
    }
    else
    {
        m_pAnimationComponent->SetAnimation(m_Properties.idleAnim);
    }

    TryToFire();
}

void ProjectileSpawnerComponent::SpawnProjectile()
{
    assert(g_pApp->GetHumanView() && g_pApp->GetHumanView()->GetCamera());

    // Check if the spawned projectile is within some bounds of Claw's Human View display
    // If not, then we can't either hear it nor do we care that there is some projectile
    Point projectilePos = m_pOwner->GetPositionComponent()->GetPosition();
    if (g_pApp->GetHumanView()->GetCamera()->IntersectsWithPoint(projectilePos, 1.25f))
    {
        // Spawn the projectile
        Point projectilePosition = m_pOwner->GetPositionComponent()->GetPosition() + m_Properties.projectileSpawnOffset;
        ActorTemplates::CreateActor_Projectile(
            m_Properties.projectileProto,
            projectilePosition,
            m_Properties.projectileDirection,
            m_pOwner->GetGUID());
    }
}

void ProjectileSpawnerComponent::VOnAnimationFrameChanged(
    Animation* pAnimation, 
    AnimationFrame* pLastFrame, 
    AnimationFrame* pNewFrame)
{
    if (m_Properties.idleAnim == "INVISIBLE" && !m_pARC->IsVisible())
    {
        return;
    }

    if (m_Properties.projectileSpawnAnimFrameIdx == pNewFrame->idx)
    {
        //assert(m_Properties.projectileSpawnAnimFrameIdx != 0);
        if (m_Properties.projectileSpawnAnimFrameIdx == 0)
        {
            return;
        }
        
        SpawnProjectile();
    }
}

void ProjectileSpawnerComponent::VOnAnimationEndedDelay(Animation* pAnimation)
{
    if (m_Properties.idleAnim == "INVISIBLE")
    {
        m_pARC->SetVisible(true);
    }
}

bool ProjectileSpawnerComponent::TryToFire()
{
    if (!m_bReady)
    {
        return false;
    }

    /*if (m_Properties.idleAnim == "INVISIBLE" && (m_Properties.isAlwaysOn || m_ActorsInTriggerArea > 0))
    {
        m_pARC->SetVisible(true);
    }*/

    // If it is already firing, dont do anything
    if (IsFiring())
    {
        return false;
    }

    // If we can fire, do just that
    if ((m_Properties.isAlwaysOn || m_ActorsInTriggerArea > 0))
    {
        if (m_Properties.idleAnim == "INVISIBLE")
        {
            m_pARC->SetVisible(false);
        }

        m_pAnimationComponent->SetAnimation(m_Properties.fireAnim);
        m_pAnimationComponent->ResumeAnimation();
        int randDelay = Util::GetRandomNumber(m_Properties.minSpawnDelay, m_Properties.maxSpawnDelay);
        m_pAnimationComponent->SetDelay(randDelay);
        return true;
    }
    else
    {
        if (m_Properties.idleAnim == "INVISIBLE")
        {
            m_pARC->SetVisible(false);
            m_pAnimationComponent->PauseAnimation();
        }
        else
        {
            m_pAnimationComponent->SetAnimation(m_Properties.idleAnim);
        }
    }

    return false;
}

bool ProjectileSpawnerComponent::IsFiring()
{ 
    return (m_pAnimationComponent->GetCurrentAnimationName() == m_Properties.fireAnim) &&
        !m_pAnimationComponent->GetCurrentAnimation()->IsPaused();
}