#include "EnemyAIComponent.h"
#include "EnemyAIStateComponent.h"
#include "../RenderComponent.h"
#include "../PositionComponent.h"
#include "../ControllerComponents/HealthComponent.h"
#include "../PhysicsComponent.h"
#include "../AnimationComponent.h"

#include "../../../GameApp/BaseGameApp.h"
#include "../../../UserInterface/HumanView.h"
#include "../../../Scene/SceneNodes.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"

const char* EnemyAIComponent::g_Name = "EnemyAIComponent";

EnemyAIComponent::EnemyAIComponent()
    :
    m_bInitialized(false),
    m_bDead(false),
    m_bHasStateLock(true)
{

}

EnemyAIComponent::~EnemyAIComponent()
{

}

bool EnemyAIComponent::VInit(TiXmlElement* pData)
{
    assert(pData);

    if (TiXmlElement* pElem = pData->FirstChildElement("DeathAnimation"))
    {
        m_DeathAnimation = pElem->GetText();
    }

    for (TiXmlElement* pElem = pData->FirstChildElement("Sound");
        pElem != NULL;
        pElem = pElem->NextSiblingElement())
    {
        std::string soundType = pElem->Attribute("SoundType");
        std::string soundName = pElem->Attribute("SoundName");

        if (soundType == "TakeDamage")
        {
            m_TakeDamageSounds.push_back(soundName);
        }
        else if (soundType == "MeleeAttack")
        {
            m_MeleeAttackSounds.push_back(soundName);
        }
        else if (soundType == "RangedAttack")
        {
            m_RangedAttackSounds.push_back(soundName);
        }
        else if (soundType == "Death")
        {
            m_DeathSounds.push_back(soundName);
        }
        else if (soundType == "Quote")
        {
            m_QuoteToHostileUnitSounds.push_back(soundName);
        }
        else
        {
            LOG_ERROR("Conflicting sound type: " + soundType + ", SoundName: " + soundName);
            assert(false && "Unknown sound type");
        }
    }

    return true;
}

void EnemyAIComponent::VPostInit()
{
    m_pRenderComponent = MakeStrongPtr(_owner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name));
    m_pPositionComponent = MakeStrongPtr(_owner->GetComponent<PositionComponent>(PositionComponent::g_Name));
    assert(m_pRenderComponent);
    assert(m_pPositionComponent);

    shared_ptr<HealthComponent> pHealthComp = 
        MakeStrongPtr(_owner->GetComponent<HealthComponent>(HealthComponent::g_Name));
    assert(pHealthComp);

    pHealthComp->AddObserver(this);
}

void EnemyAIComponent::VUpdate(uint32 msDiff)
{
    if (!m_bInitialized)
    {
        assert(!m_StateMap.empty());
        EnterState("PatrolState");

        m_bInitialized = true;
    }

    if (m_bDead)
    {
        // I want it to disappear after ~900ms
        Point moveDelta((600 / 900.0 * msDiff), (600 / 900.0f * msDiff));
        m_pPositionComponent->SetPosition(m_pPositionComponent->GetX() + moveDelta.x, m_pPositionComponent->GetY() + moveDelta.y);

        // This feels like a hack
        if (HumanView* pHumanView = g_pApp->GetHumanView())
        {
            shared_ptr<CameraNode> pCamera = pHumanView->GetCamera();
            if (pCamera)
            {
                shared_ptr<EventData_Move_Actor> pEvent(new EventData_Move_Actor(_owner->GetGUID(), m_pPositionComponent->GetPosition()));
                IEventMgr::Get()->VTriggerEvent(pEvent);

                SDL_Rect dummy;
                SDL_Rect renderRect = m_pRenderComponent->VGetPositionRect();
                SDL_Rect cameraRect = pCamera->GetCameraRect();
                if (!SDL_IntersectRect(&renderRect, &cameraRect, &dummy))
                {
                    shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(_owner->GetGUID()));
                    IEventMgr::Get()->VQueueEvent(pEvent);
                }
            }
            else
            {
                LOG_ERROR("Could not retrieve camera");
            }
        }

        if (!m_DeathAnimation.empty())
        {
            auto pAnimComp = MakeStrongPtr(_owner->GetComponent<AnimationComponent>(AnimationComponent::g_Name));
            assert(pAnimComp);

            pAnimComp->SetAnimation(m_DeathAnimation);
        }
    }
}

void EnemyAIComponent::VOnHealthBelowZero(DamageType damageType)
{
    m_bDead = true;
    for (auto stateComponentIter : m_StateMap)
    {
        stateComponentIter.second->VOnStateLeave();
    }

    // Play deaht sound
    Util::PlayRandomSoundFromList(m_DeathSounds);

    shared_ptr<PhysicsComponent> pPhysicsComponent =
        MakeStrongPtr(_owner->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
    assert(pPhysicsComponent);

    pPhysicsComponent->Destroy();
}

void EnemyAIComponent::VOnHealthChanged(int32 oldHealth, int32 newHealth, DamageType damageType, Point impactPoint)
{
    // If he died we do not care
    if (newHealth >= 0)
    {
        if (HasState("TakeDamageState"))
        {
            EnterState("TakeDamageState");
        }

        // Spawn graphics in the hit point
        if (damageType != DamageType_Magic)
        {
            ActorTemplates::CreateSingleAnimation(impactPoint, AnimationType_RedHitPoint);
            Util::PlayRandomHitSound();
        }
    }
}

void EnemyAIComponent::OnEnemyEnteredMeleeZone(Actor* pEnemy)
{
    m_EnemiesInMeleeZone.push_back(pEnemy);

    if (m_EnemiesInMeleeZone.size() == 1)
    {
        if (m_bHasStateLock)
        {
            m_bHasStateLock = false;
            EnterState("MeleeAttackState");
        }
    }
}

void EnemyAIComponent::OnEnemyLeftMeleeZone(Actor* pEnemy)
{
    for (auto iter = m_EnemiesInMeleeZone.begin(); iter != m_EnemiesInMeleeZone.end(); ++iter)
    {
        if ((*iter) == pEnemy)
        {
            m_EnemiesInMeleeZone.erase(iter);
            if (m_EnemiesInMeleeZone.empty())
            {
                if (m_bHasStateLock)
                {
                    if (m_EnemiesInRangedZone.size() > 0 && HasState("RangedAttackState"))
                    {
                        m_bHasStateLock = false;
                        EnterState("RangedAttackState");
                    }
                    else
                    {
                        EnterState("PatrolState");
                    }
                }
            }
            return;
        }
    }

    LOG_WARNING("Could not remove enemy - no such actor found");
}

void EnemyAIComponent::OnEnemyEnteredRangedZone(Actor* pEnemy)
{
    m_EnemiesInRangedZone.push_back(pEnemy);

    if (m_EnemiesInRangedZone.size() == 1)
    {
        if (m_bHasStateLock && HasState("RangedAttackState"))
        {
            m_bHasStateLock = false;
            EnterState("RangedAttackState");
        }
    }
}

void EnemyAIComponent::OnEnemyLeftRangedZone(Actor* pEnemy)
{
    for (auto iter = m_EnemiesInRangedZone.begin(); iter != m_EnemiesInRangedZone.end(); ++iter)
    {
        if ((*iter) == pEnemy)
        {
            m_EnemiesInRangedZone.erase(iter);
            if (m_EnemiesInRangedZone.empty())
            {
                if (m_bHasStateLock)
                {
                    if (m_EnemiesInMeleeZone.size() > 0 && HasState("MeleeAttackState"))
                    {
                        m_bHasStateLock = false;
                        EnterState("MeleeAttackState");
                    }
                    else
                    {
                        EnterState("PatrolState");
                    }
                }
            }
            return;
        }
    }

    LOG_WARNING("Could not remove enemy - no such actor found");
}

Point EnemyAIComponent::FindClosestHostileActorOffset()
{
    Point closest(0, 0);

    if (m_EnemiesInMeleeZone.empty() && m_EnemiesInRangedZone.empty())
    {
        return closest;
    }

    if (!m_EnemiesInMeleeZone.empty())
    {
        for (Actor* pHostileActor : m_EnemiesInMeleeZone)
        {
            shared_ptr<PositionComponent> pHostileActorPositionComponent =
                MakeStrongPtr(pHostileActor->GetComponent<PositionComponent>(PositionComponent::g_Name));
            assert(pHostileActorPositionComponent);

            Point positionDiff = pHostileActorPositionComponent->GetPosition() - m_pPositionComponent->GetPosition();
            if (positionDiff.Length() < (m_pPositionComponent->GetPosition() - closest).Length())
            {
                closest = positionDiff;
            }
        }
    }
    else
    {
        for (Actor* pHostileActor : m_EnemiesInRangedZone)
        {
            shared_ptr<PositionComponent> pHostileActorPositionComponent =
                MakeStrongPtr(pHostileActor->GetComponent<PositionComponent>(PositionComponent::g_Name));
            assert(pHostileActorPositionComponent);

            Point positionDiff = pHostileActorPositionComponent->GetPosition() - m_pPositionComponent->GetPosition();
            if (positionDiff.Length() < (m_pPositionComponent->GetPosition() - closest).Length())
            {
                closest = positionDiff;
            }
        }
    }

    assert(std::fabs(closest.x) > DBL_EPSILON || std::fabs(closest.y) > DBL_EPSILON);

    return closest;
}

void EnemyAIComponent::LeaveAllStates()
{
    for (auto stateIter : m_StateMap)
    {
        stateIter.second->VOnStateLeave();
    }
}

void EnemyAIComponent::EnterState(std::string stateName)
{
    LeaveAllStates();

    auto findIt = m_StateMap.find(stateName);
    assert(findIt != m_StateMap.end());

    findIt->second->VOnStateEnter();
}

void EnemyAIComponent::AcquireStateLock()
{
    LeaveAllStates();
    m_bHasStateLock = true;
}

void EnemyAIComponent::OnStateCanFinish()
{
    EnemyAIState currentState = GetCurrentState();

    // Force acquire state lock - there is ALWAYS a better state than this
    if (currentState == EnemyAIState_TakingDamage)
    {
        AcquireStateLock();
        if (m_EnemiesInMeleeZone.size() > 0)
        {
            EnterState("MeleeAttackState");
            m_bHasStateLock = false;
        }
        else if (m_EnemiesInRangedZone.size() > 0)
        {
            EnterState("RangedAttackState");
            m_bHasStateLock = false;
        }
    }

    if (currentState == EnemyAIState_MeleeAttacking)
    {
        if (m_EnemiesInMeleeZone.empty())
        {
            AcquireStateLock();
            if (m_EnemiesInRangedZone.size() > 0)
            {
                EnterState("RangedAttackState");
                m_bHasStateLock = false;
            }
        }
    }
    else if (currentState == EnemyAIState_RangedAttacking)
    {
        if (m_EnemiesInMeleeZone.size() > 0)
        {
            AcquireStateLock();
            if (m_EnemiesInMeleeZone.size() > 0)
            {
                EnterState("MeleeAttackState");
                m_bHasStateLock = false;
            }
        }
        else if (m_EnemiesInRangedZone.empty())
        {
            AcquireStateLock();
            if (m_EnemiesInMeleeZone.size() > 0)
            {
                EnterState("MeleeAttackState");
                m_bHasStateLock = false;
            }
        }
    }

    if (m_bHasStateLock)
    {
        EnterState("PatrolState");
    }
}

EnemyAIState EnemyAIComponent::GetCurrentState()
{
    for (auto stateIter : m_StateMap)
    {
        if (stateIter.second->IsActive())
        {
            return stateIter.second->VGetStateType();
        }
    }

    assert(false && "Could not find any state ?");

    return EnemyAIState_None;
}

bool EnemyAIComponent::HasState(std::string stateName)
{
    auto findIt = m_StateMap.find(stateName);
    return findIt != m_StateMap.end();
}