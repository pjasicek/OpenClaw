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
    //assert(pHealthComp);
    if (pHealthComp)
    {
        pHealthComp->AddObserver(this);
    }
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
    }

    if (newHealth < oldHealth)
    {
        ActorTemplates::CreateSingleAnimation(impactPoint, AnimationType_RedHitPoint);
        Util::PlayRandomHitSound();
    }
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

void EnemyAIComponent::EnterState(EnemyAIState state)
{
    LeaveAllStates();

    for (auto stateIter : m_StateMap)
    {
        if (stateIter.second->VGetStateType() == state)
        {
            stateIter.second->VOnStateEnter();
        }
    }

    assert(false && "Did not find given state");
}

void EnemyAIComponent::EnterState(BaseEnemyAIStateComponent* pState)
{
    assert(pState != NULL);

    LeaveAllStates();
    pState->VOnStateEnter();
}

void EnemyAIComponent::AcquireStateLock()
{
    LeaveAllStates();
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

    for (auto stateIter : m_StateMap)
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
        AcquireStateLock();
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
    for (auto stateIter : m_StateMap)
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
    for (auto stateIter : m_StateMap)
    {
        if (stateIter.second->VGetStateType() == state)
        {
            return stateIter.second;
        }
    }

    return NULL;
}

bool EnemyAIComponent::HasState(std::string stateName)
{
    auto findIt = m_StateMap.find(stateName);
    return findIt != m_StateMap.end();
}

bool EnemyAIComponent::HasState(EnemyAIState state)
{
    for (auto stateIter : m_StateMap)
    {
        if (stateIter.second->VGetStateType() == state)
        {
            return true;
        }
    }

    return false;
}