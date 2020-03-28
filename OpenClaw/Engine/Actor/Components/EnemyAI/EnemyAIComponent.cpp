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