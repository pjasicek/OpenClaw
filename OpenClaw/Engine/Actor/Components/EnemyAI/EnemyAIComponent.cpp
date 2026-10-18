#include "EnemyAIComponent.h"
#include "EnemyAIStateComponent.h"
#include "../RenderComponent.h"
#include "../PositionComponent.h"
#include "../ControllerComponents/HealthComponent.h"
#include "../PhysicsComponent.h"
#include "../AnimationComponent.h"
#include "../FollowableComponent.h"

#include "../ControllableComponent.h"

#include "../../../GameApp/BaseGameApp.h"
#include "../../../GameApp/BaseGameLogic.h"
#include "../../../UserInterface/HumanView.h"
#include "../../../Scene/SceneNodes.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"

const char* EnemyAIComponent::g_Name = "EnemyAIComponent";

EnemyAIComponent::EnemyAIComponent()
    :
    m_bInitialized(false),
    m_bDead(false),
    m_bHasStateLock(true),
    m_bLifted(false),
    m_bThrown(false),
    m_ThrownTime(0),
    m_ThrowerActorId(INVALID_ACTOR_ID),
    m_LastThrownVelocityY(0.0),
    m_TimeSinceLastSpeechSound(0),
    m_MinTimeIntervalForSpeechSound(0)
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
    
    if (TiXmlElement* pSoundsElem = pData->FirstChildElement("Sounds"))
    {
        for (TiXmlElement* pElem = pSoundsElem->FirstChildElement("Sound");
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
    }

    ParseValueFromXmlElem(&m_MinTimeIntervalForSpeechSound, pData->FirstChildElement("MinTimeIntervalForSpeechSound"));

    return true;
}

void EnemyAIComponent::VPostInit()
{
    m_pRenderComponent = MakeStrongPtr(m_pOwner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name));
    m_pPositionComponent = m_pOwner->GetPositionComponent();
    assert(m_pRenderComponent);
    assert(m_pPositionComponent);

    shared_ptr<HealthComponent> pHealthComp = 
        MakeStrongPtr(m_pOwner->GetComponent<HealthComponent>(HealthComponent::g_Name));
    assert(pHealthComp);

    pHealthComp->AddObserver(this);
}

void EnemyAIComponent::VPostPostInit()
{
    assert(!m_StateMap.empty());
    EnterBestState(true);
}

void EnemyAIComponent::VUpdate(uint32 msDiff)
{
    if (m_bThrown)
    {
        UpdateThrown(msDiff);
        return;
    }
    else if (m_bLifted)
    {
        // Claw holds him above his head - he can only wriggle
        return;
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
                shared_ptr<EventData_Move_Actor> pEvent(new EventData_Move_Actor(m_pOwner->GetGUID(), m_pPositionComponent->GetPosition()));
                IEventMgr::Get()->VTriggerEvent(pEvent);

                SDL_Rect renderRect = m_pRenderComponent->VGetPositionRect();
                SDL_Rect cameraRect = pCamera->GetCameraRect();
                if (!SDL_HasIntersection(&renderRect, &cameraRect))
                {
                    shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(m_pOwner->GetGUID()));
                    IEventMgr::Get()->VQueueEvent(pEvent);

                    // This is really weird... but it is in original game exactly like this
                    SoundInfo splashSound("/GAME/SOUNDS/SPLASH.WAV");
                    IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
                        new EventData_Request_Play_Sound(splashSound)));
                }
            }
            else
            {
                LOG_ERROR("Could not retrieve camera");
            }
        }

        if (!m_DeathAnimation.empty())
        {
            auto pAnimComp = MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>(AnimationComponent::g_Name));
            assert(pAnimComp);

            pAnimComp->SetAnimation(m_DeathAnimation);
        }
    }
    else
    {
        m_TimeSinceLastSpeechSound += msDiff;
    }
}

void EnemyAIComponent::VOnHealthBelowZero(DamageType damageType, int sourceActorId)
{
    m_bDead = true;
    m_bLifted = false;
    m_bThrown = false;
    for (const auto &stateComponentIter : m_StateMap)
    {
        if (stateComponentIter.second->IsActive())
        {
            stateComponentIter.second->VOnStateLeave(NULL);
        }
    }

    // Play death sound
    Util::PlayRandomSoundFromList(m_DeathSounds);

    shared_ptr<PhysicsComponent> pPhysicsComponent = m_pOwner->GetPhysicsComponent();
    assert(pPhysicsComponent);

    pPhysicsComponent->Destroy();

    // HACK: This should be general like Subject/Observer but this is the only place so far
    // that the killed-by information is used
    StrongActorPtr pKiller = MakeStrongPtr(g_pApp->GetGameLogic()->VGetActor(sourceActorId));
    if (pKiller != nullptr)
    {
        shared_ptr<ClawControllableComponent> pClaw = MakeStrongPtr(pKiller->GetComponent<ClawControllableComponent>());
        if (pClaw)
        {
            pClaw->OnClawKilledEnemy(damageType, m_pOwner.get());
        }
    }
}

void EnemyAIComponent::VOnHealthChanged(int32 oldHealth, int32 newHealth, DamageType damageType, Point impactPoint, int sourceActorId)
{
    // If he died we do not care
    if (newHealth >= 0)
    {
        if (HasState("TakeDamageState"))
        {
            EnterState("TakeDamageState");
        }
    }

    if (newHealth < oldHealth)
    {
        ActorTemplates::CreateSingleAnimation(impactPoint, AnimationType_RedHitPoint);
        Util::PlayRandomHitSound();
    }
}

void EnemyAIComponent::VOnResistDamage(DamageType damageType, Point impactPoint)
{
    if (HasState(EnemyAIState_Parry))
    {
        AcquireStateLock(GetState(EnemyAIState_Parry));
        EnterState(EnemyAIState_Parry);
    }
}

bool EnemyAIComponent::VCanResistDamage(DamageType damageType, Point impactPoint)
{
    if (HasState(EnemyAIState_Parry))
    {
        shared_ptr<ParryEnemyAIStateComponent> pParryStateComponent =
            MakeStrongPtr(m_pOwner->GetComponent<ParryEnemyAIStateComponent>());
        assert(pParryStateComponent);

        assert(GetCurrentState() != NULL);

        return pParryStateComponent->CanParry(damageType, GetCurrentState()->VGetStateType());
    }

    return false;
}

void EnemyAIComponent::LeaveAllStates(BaseEnemyAIStateComponent* pNextState)
{
    for (const auto& stateIter : m_StateMap)
    {
        if (stateIter.second->IsActive())
        {
            stateIter.second->VOnStateLeave(pNextState);
        }
    }
}

void EnemyAIComponent::EnterState(const std::string &stateName)
{
    BaseEnemyAIStateComponent* pCurrentState = GetCurrentState();

    auto findIt = m_StateMap.find(stateName);
    assert(findIt != m_StateMap.end());

    LeaveAllStates(findIt->second);
    findIt->second->VOnStateEnter(pCurrentState);
}

void EnemyAIComponent::EnterState(EnemyAIState state)
{
    BaseEnemyAIStateComponent* pCurrentState = GetCurrentState();

    for (const auto &stateIter : m_StateMap)
    {
        if (stateIter.second->VGetStateType() == state)
        {
            LeaveAllStates(stateIter.second);
            stateIter.second->VOnStateEnter(pCurrentState);
            return;
        }
    }

    assert(false && "Did not find given state");
}

void EnemyAIComponent::EnterState(BaseEnemyAIStateComponent* pState)
{
    assert(pState != NULL);
    BaseEnemyAIStateComponent* pCurrentState = GetCurrentState();

    LeaveAllStates(pState);
    pState->VOnStateEnter(pCurrentState);
}

void EnemyAIComponent::AcquireStateLock(BaseEnemyAIStateComponent* pNewState)
{
    LeaveAllStates(pNewState);
    m_bHasStateLock = true;
}

bool EnemyAIComponent::EnterBestState(bool canForceEnter)
{
    BaseEnemyAIStateComponent* pCurrentState = GetCurrentState();

    // If some other state has state lock we will not force him to exit.
    // If no state is set at the moment, then force it aswell
    // Maybe this can revisited in future
    //LOG("m_bHasStateLock: " + ToStr(m_bHasStateLock) + ", canForceEnter: " + ToStr(canForceEnter));
    if ((!m_bHasStateLock && !canForceEnter && (pCurrentState != NULL)) /*&&
        pCurrentState->VCanEnter()*/)
    {
        return false;
    }

    BaseEnemyAIStateComponent* pBestState = NULL;
    int bestStatePrio = -1;

    for (const auto &stateIter : m_StateMap)
    {
        BaseEnemyAIStateComponent* pState = stateIter.second;
        if (pState == pCurrentState)
        {
            continue;
        }
        
        if (pState->VCanEnter() && 
            pState->VGetPriority() > bestStatePrio)
        {
            pBestState = pState;
            bestStatePrio = pState->VGetPriority();
        }
    }

    /*assert(pBestState != NULL);
    assert(bestStatePrio >= 0);*/

    if ((pCurrentState == NULL) ||
        !pCurrentState->VCanEnter() ||
        (pCurrentState->VGetPriority() < bestStatePrio))
    {
        AcquireStateLock(pBestState);
        // If best prio has some positive values, then we can only force switch it
        if (bestStatePrio > 0)
        {
            m_bHasStateLock = false;
        }
        EnterState(pBestState);
        return true;
    }

    return false;
}

BaseEnemyAIStateComponent* EnemyAIComponent::GetCurrentState()
{
    for (const auto &stateIter : m_StateMap)
    {
        if (stateIter.second->IsActive())
        {
            return stateIter.second;
        }
    }

    //assert(false && "Could not find any state ?");

    return NULL;
}

BaseEnemyAIStateComponent* EnemyAIComponent::GetState(EnemyAIState state)
{
    for (const auto &stateIter : m_StateMap)
    {
        if (stateIter.second->VGetStateType() == state)
        {
            return stateIter.second;
        }
    }

    return NULL;
}

bool EnemyAIComponent::HasState(const std::string &stateName)
{
    return m_StateMap.count(stateName) > 0;
}

bool EnemyAIComponent::HasState(EnemyAIState state)
{
    for (const auto &stateIter : m_StateMap)
    {
        if (stateIter.second->VGetStateType() == state)
        {
            return true;
        }
    }

    return false;
}

bool EnemyAIComponent::TryPlaySpeechSound(int chance, const SoundList& speechSounds)
{
    if (Util::RollDice(chance) && (m_TimeSinceLastSpeechSound > m_MinTimeIntervalForSpeechSound))
    {
        std::string playedSound = Util::PlayRandomSoundFromList(speechSounds);
        m_TimeSinceLastSpeechSound = 0;

        // Exclamation mark
        shared_ptr<FollowableComponent> pExclamationMark = 
            MakeStrongPtr(m_pOwner->GetComponent<FollowableComponent>());
        assert(pExclamationMark != nullptr);

        pExclamationMark->Activate(Util::GetSoundDurationMs(playedSound));

        return true;
    }

    return false;
}

/*bool EnemyAIComponent::TryFindClosestHostileActorOffset(Point& outOffset)
{
    outOffset = Point(DBL_MAX, DBL_MAX);
    bool found = false;

    for (auto stateIter : m_StateMap)
    {
        if (BaseAttackAIStateComponent* pEnemyState = dynamic_cast<BaseAttackAIStateComponent*>(stateIter.second))
        {
            if (pEnemyState->FindClosestHostileActorOffset().Length() < outOffset.Length())
            {
                outOffset = pEnemyState->FindClosestHostileActorOffset();
                found = true;
            }
        }
    }

    return found;
}*/
//=====================================================================================================================
// Lifted / thrown by Claw (see ClawControllableComponent::TryLift() for the whole picture)
//
// Enemies which have the original "lifted2" (or "lifted") animation can be lifted - bosses only during the fight.
// While lifted, all AI states are left and the enemy only wriggles. When thrown, the enemy flies until he hits
// another enemy, wall or ground:
//  - ordinary enemy dies (with his usual death animation and loot)
//  - boss loses 25 health and continues fighting
// Enemy hit by the thrown one loses 50 health. Damage of type DamageType_Thrown cannot be parried.
//=====================================================================================================================

bool EnemyAIComponent::HasAnimation(const std::string& animName)
{
    shared_ptr<AnimationComponent> pAnimComp = MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>(AnimationComponent::g_Name));
    std::string name = animName;
    return pAnimComp && pAnimComp->HasAnimation(name);
}

void EnemyAIComponent::SetAnimationIfExists(const std::string& animName)
{
    shared_ptr<AnimationComponent> pAnimComp = MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>(AnimationComponent::g_Name));
    std::string name = animName;
    if (pAnimComp && pAnimComp->HasAnimation(name))
    {
        pAnimComp->SetAnimation(name);
        pAnimComp->ResumeAnimation();
    }
}

// Most enemies wriggle in "lifted2" animation, some (e.g. Marrow) only have "lifted"
std::string EnemyAIComponent::GetLiftedAboveHeadAnimation()
{
    return HasAnimation("lifted2") ? "lifted2" : "lifted";
}

bool EnemyAIComponent::IsBoss()
{
    for (const auto &stateIter : m_StateMap)
    {
        EnemyAIState stateType = stateIter.second->VGetStateType();
        if (stateType >= EnemyAIState_BrainLaRaux && stateType != EnemyAIState_PunkRat)
        {
            return true;
        }
    }

    return false;
}

bool EnemyAIComponent::CanBeLifted()
{
    if (m_bDead || m_bLifted || m_bThrown)
    {
        return false;
    }

    // Only enemies which have the original lifted animations can be lifted
    if (!HasAnimation("lifted2") && !HasAnimation("lifted"))
    {
        return false;
    }

    // Bosses can be lifted only during the fight, not while they are talking before it
    if (IsBoss())
    {
        BaseEnemyAIStateComponent* pCurrentState = GetCurrentState();
        if (pCurrentState == NULL ||
            (pCurrentState->VGetStateType() >= EnemyAIState_BrainLaRaux && pCurrentState->VGetStateType() != EnemyAIState_PunkRat))
        {
            return false;
        }
    }

    return true;
}

void EnemyAIComponent::OnLifted()
{
    m_bLifted = true;
    LeaveAllStates(NULL);

    // Grabbed pose: "lifted1" if it exists, otherwise "lifted" if it is not used for wriggling above head
    if (HasAnimation("lifted1"))
    {
        SetAnimationIfExists("lifted1");
    }
    else
    {
        SetAnimationIfExists("lifted");
    }
}

void EnemyAIComponent::OnLiftedAboveHead()
{
    SetAnimationIfExists(GetLiftedAboveHeadAnimation());
}

void EnemyAIComponent::OnDropped()
{
    m_bLifted = false;
    EnterBestState(true);
}

void EnemyAIComponent::OnThrown(int throwerActorId)
{
    m_bLifted = false;
    m_bThrown = true;
    m_ThrownTime = 0;
    m_ThrowerActorId = throwerActorId;
    m_LastThrownVelocityY = 0.0;

    SetAnimationIfExists(HasAnimation("thrown") ? "thrown" : GetLiftedAboveHeadAnimation());
}

void EnemyAIComponent::UpdateThrown(uint32 msDiff)
{
    m_ThrownTime += msDiff;

    shared_ptr<IGamePhysics> pPhysics = g_pApp->GetGameLogic()->VGetGamePhysics();
    const uint32 myId = m_pOwner->GetGUID();
    const Point pos = m_pPositionComponent->GetPosition();
    const Point velocity = pPhysics->VGetVelocity(myId);

    // Hit other enemy in the flight direction
    const double dir = velocity.x < 0 ? -1.0 : 1.0;
    RaycastResult hit = pPhysics->VRayCast(pos, Point(pos.x + 60.0 * dir, pos.y), CollisionFlag_DynamicActor);
    if (hit.foundIntersection && hit.actorId != INVALID_ACTOR_ID && hit.actorId != myId)
    {
        StrongActorPtr pHitActor = MakeStrongPtr(g_pApp->GetGameLogic()->VGetActor(hit.actorId));
        if (pHitActor)
        {
            shared_ptr<EnemyAIComponent> pHitEnemy = MakeStrongPtr(pHitActor->GetComponent<EnemyAIComponent>(EnemyAIComponent::g_Name));
            shared_ptr<HealthComponent> pHitHealth = MakeStrongPtr(pHitActor->GetComponent<HealthComponent>(HealthComponent::g_Name));
            if (pHitEnemy && !pHitEnemy->m_bDead && !pHitEnemy->IsLiftedOrThrown() && pHitHealth)
            {
                pHitHealth->AddHealth(-50, DamageType_Thrown, Point(pos.x + 30.0 * dir, pos.y), m_ThrowerActorId);
                OnThrowImpact();
                return;
            }
        }
    }

    // Hit wall (horizontal speed is gone), landed on ground (stopped falling) or flies for too long
    const bool hitWall = m_ThrownTime > 100 && fabs(velocity.x) < 0.5;
    const bool landed = m_LastThrownVelocityY > 1.0 && fabs(velocity.y) < 0.2;
    m_LastThrownVelocityY = velocity.y;
    if (hitWall || landed || m_ThrownTime > 3000)
    {
        OnThrowImpact();
    }
}

void EnemyAIComponent::OnThrowImpact()
{
    m_bThrown = false;

    shared_ptr<HealthComponent> pHealth = MakeStrongPtr(m_pOwner->GetComponent<HealthComponent>(HealthComponent::g_Name));
    if (!pHealth)
    {
        return;
    }
    pHealth->SetInvulnerable(false);

    if (!IsBoss())
    {
        // Ordinary enemy dies upon impact
        pHealth->AddHealth(-(pHealth->GetHealth() + 1), DamageType_Thrown, m_pPositionComponent->GetPosition(), m_ThrowerActorId);
        return;
    }

    // Boss loses 25 health, falls down and continues fighting
    shared_ptr<IGamePhysics> pPhysics = g_pApp->GetGameLogic()->VGetGamePhysics();
    pPhysics->VSetLinearSpeed(m_pOwner->GetGUID(), Point(0, pPhysics->VGetVelocity(m_pOwner->GetGUID()).y));

    pHealth->AddHealth(-25, DamageType_Thrown, m_pPositionComponent->GetPosition(), m_ThrowerActorId);
    if (!m_bDead && GetCurrentState() == NULL)
    {
        EnterBestState(true);
    }
}
