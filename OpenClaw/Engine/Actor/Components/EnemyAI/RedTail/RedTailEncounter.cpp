#include "../EnemyAIComponent.h"
#include "../../AnimationComponent.h"
#include "../../PhysicsComponent.h"
#include "../../PositionComponent.h"
#include "../../RenderComponent.h"

#include "../../../../GameApp/BaseGameLogic.h"
#include "../../../../GameApp/BaseGameApp.h"
#include "../../../../Physics/ClawPhysics.h"
#include "../../ControllableComponent.h"
#include "../../RenderComponent.h"
#include "../../FollowableComponent.h"

#include "../../../../Events/EventMgr.h"
#include "../../../../Events/Events.h"
#include "RedTailEncounter.h"

const char* RedTailAIStateComponent::g_Name = "RedTailAIStateComponent";

//=====================================================================================================================
// GabrielAIStateComponent
//=====================================================================================================================

RedTailAIStateComponent::RedTailAIStateComponent()
    :
    BaseBossAIStateComponennt("RedTailAIStateComponent")
{

}

RedTailAIStateComponent::~RedTailAIStateComponent()
{

}

bool RedTailAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    if (!BaseBossAIStateComponennt::VDelegateInit(pData))
    {
        return false;
    }

    return true;
}

void RedTailAIStateComponent::VPostInit()
{
    BaseBossAIStateComponennt::VPostInit();

    m_pAnimationComponent = m_pOwner->GetRawComponent<AnimationComponent>(true);
    m_pAnimationComponent->AddObserver(this);

    //m_pQuestionMark = m_pOwner->GetRawComponent<FollowableComponent>(true);
}

void RedTailAIStateComponent::VOnWorldFinishedLoading()
{
}

void RedTailAIStateComponent::VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState)
{
    m_IsActive = true;
}

void RedTailAIStateComponent::VOnStateLeave(BaseEnemyAIStateComponent* pNextState)
{
    m_IsActive = false;
}

void RedTailAIStateComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    if (!m_IsActive)
    {
        return;
    }
}

void RedTailAIStateComponent::VOnAnimationLooped(Animation* pAnimation)
{
    if (!m_IsActive)
    {
        return;
    }
}

void RedTailAIStateComponent::VUpdate(uint32 msDiff)
{
    if (!m_IsActive)
    {
        return;
    }
}

void RedTailAIStateComponent::VOnBossFightStarted()
{
    // To refresh contact list after Claw's death
    g_pApp->GetGameLogic()->VGetGamePhysics()->VActivate(m_pOwner->GetGUID());

    m_pEnemyAIComponent->EnterBestState(true);
}

void RedTailAIStateComponent::VOnBossFightEnded(bool isBossDead)
{
    if (isBossDead)
    {
        SoundInfo soundInfo(SOUND_GAME_AMULET_RISE);
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));

        StrongActorPtr pGem = ActorTemplates::CreateActor(
            ActorPrototype_Level13_BossGem,
            m_pOwner->GetPositionComponent()->GetPosition());

        /*Point gemForce(-3.5, -7);
        g_pApp->GetGameLogic()->VGetGamePhysics()->VApplyLinearImpulse(pGem->GetGUID(), gemForce);*/
    }
    else
    {
        // To refresh contact list after Claw's death
        g_pApp->GetGameLogic()->VGetGamePhysics()->VDeactivate(m_pOwner->GetGUID());

        /*m_TimeSinceLastAction = 0;
        m_CurrActionDelay = Util::GetRandomNumber(m_ActionMinInterval, m_ActionMaxInterval);*/
    }
}

void RedTailAIStateComponent::VOnHealthChanged(int32 oldHealth, int32 newHealth, DamageType damageType, Point impactPoint, int sourceActorId)
{
    BaseBossAIStateComponennt::VOnHealthChanged(oldHealth, newHealth, damageType, impactPoint, sourceActorId);
}

