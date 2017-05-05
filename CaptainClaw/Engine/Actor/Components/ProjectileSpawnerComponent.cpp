#include "ProjectileSpawnerComponent.h"
#include "../../GameApp/BaseGameApp.h"
#include "../../GameApp/BaseGameLogic.h"
#include "PositionComponent.h"

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
        MakeStrongPtr(_owner->GetComponent<AnimationComponent>()).get();
    assert(m_pAnimationComponent != NULL);

    m_pAnimationComponent->SetAnimation(m_Properties.idleAnim);

    shared_ptr<TriggerComponent> pTriggerComponent = 
        MakeStrongPtr(_owner->GetComponent<TriggerComponent>());
    assert(pTriggerComponent != nullptr);

    m_pAnimationComponent->AddObserver(this);
    pTriggerComponent->AddObserver(this);
}

void ProjectileSpawnerComponent::VPostPostInit()
{
    if (!m_Properties.triggerAreaSize.IsZeroXY())
    {
        ActorFixtureDef fixtureDef;
        fixtureDef.collisionShape = "Rectangle";
        fixtureDef.fixtureType = FixtureType_Trigger;
        fixtureDef.size = m_Properties.triggerAreaSize;
        fixtureDef.offset = m_Properties.triggerAreaOffset;
        fixtureDef.collisionMask = m_Properties.triggerCollisionMask;
        fixtureDef.collisionFlag = CollisionFlag_Trigger;
        fixtureDef.isSensor = true;

        LOG("Size: " + fixtureDef.size.ToString());

        g_pApp->GetGameLogic()->VGetGamePhysics()->VAddActorFixtureToBody(_owner->GetGUID(), &fixtureDef);
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

void ProjectileSpawnerComponent::VOnActorEnteredTrigger(Actor* pActorWhoEntered)
{
    TryToFire();
    m_ActorsInTriggerArea++;
    assert(m_ActorsInTriggerArea >= 0);
}

void ProjectileSpawnerComponent::VOnActorLeftTrigger(Actor* pActorWhoLeft)
{
    m_ActorsInTriggerArea--;
    assert(m_ActorsInTriggerArea >= 0);
}

void ProjectileSpawnerComponent::VOnAnimationLooped(Animation* pAnimation)
{
    m_pAnimationComponent->SetAnimation(m_Properties.idleAnim);
    TryToFire();
}

void ProjectileSpawnerComponent::VOnAnimationFrameChanged(
    Animation* pAnimation, 
    AnimationFrame* pLastFrame, 
    AnimationFrame* pNewFrame)
{
    if (m_Properties.projectileSpawnAnimFrameIdx == pNewFrame->idx)
    {
        assert(m_Properties.projectileSpawnAnimFrameIdx != 0);

        // Spawn the projectile
        Point projectilePosition = _owner->GetPositionComponent()->GetPosition() + m_Properties.projectileSpawnOffset;
        ActorTemplates::CreateActor_Projectile(
            m_Properties.projectileProto,
            projectilePosition,
            m_Properties.projectileDirection);
    }
}

bool ProjectileSpawnerComponent::TryToFire()
{
    if (!m_bReady)
    {
        return false;
    }

    // If it is already firing, dont do anything
    if (IsFiring())
    {
        return false;
    }

    // If we can fire, do just that
    if ((m_Properties.isAlwaysOn || m_ActorsInTriggerArea > 0))
    {
        m_pAnimationComponent->SetAnimation(m_Properties.fireAnim);
        int randDelay = Util::GetRandomNumber(m_Properties.minSpawnDelay, m_Properties.maxSpawnDelay);
        m_pAnimationComponent->SetDelay(randDelay);
        return true;
    }
    else
    {
        m_pAnimationComponent->SetAnimation(m_Properties.idleAnim);
    }

    return false;
}