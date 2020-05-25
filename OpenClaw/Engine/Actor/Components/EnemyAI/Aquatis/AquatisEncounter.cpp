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
#include "AquatisEncounter.h"

const char* AquatisAIStateComponent::g_Name = "AquatisAIStateComponent";

//=====================================================================================================================
// GabrielAIStateComponent
//=====================================================================================================================

AquatisAIStateComponent::AquatisAIStateComponent()
    :
    BaseBossAIStateComponennt("AquatisAIStateComponent")
{

}

AquatisAIStateComponent::~AquatisAIStateComponent()
{

}

bool AquatisAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    if (!BaseBossAIStateComponennt::VDelegateInit(pData))
    {
        return false;
    }

    return true;
}

void AquatisAIStateComponent::VPostInit()
{
    BaseBossAIStateComponennt::VPostInit();

    m_pAnimationComponent = m_pOwner->GetRawComponent<AnimationComponent>(true);
    m_pAnimationComponent->AddObserver(this);

    //m_pQuestionMark = m_pOwner->GetRawComponent<FollowableComponent>(true);
}

void AquatisAIStateComponent::VOnWorldFinishedLoading()
{
    //m_pGabrielCannonComponent = pGabrielCannonActor->GetRawComponent<GabrielCannonComponent>(true);
}

void AquatisAIStateComponent::VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState)
{
    //m_pAnimationComponent->SetAnimation(m_IdleAnim);

    //m_IsActive = true;
}

void AquatisAIStateComponent::VOnStateLeave(BaseEnemyAIStateComponent* pNextState)
{
    m_IsActive = false;
}

void AquatisAIStateComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    if (!m_IsActive || !m_bBossFightStarted)
    {
        return;
    }
}

void AquatisAIStateComponent::VOnAnimationLooped(Animation* pAnimation)
{
    if (!m_IsActive || !m_bBossFightStarted)
    {
        return;
    }

    //m_pAnimationComponent->SetAnimation(m_IdleAnim);
}

void AquatisAIStateComponent::VUpdate(uint32 msDiff)
{
    if (!m_IsActive || !m_bBossFightStarted)
    {
        return;
    }
}

void AquatisAIStateComponent::VOnBossFightStarted()
{
    // To refresh contact list after Claw's death
    g_pApp->GetGameLogic()->VGetGamePhysics()->VActivate(m_pOwner->GetGUID());

    m_pEnemyAIComponent->EnterBestState(true);
}

void AquatisAIStateComponent::VOnBossFightEnded(bool isBossDead)
{
    if (isBossDead)
    {
        SoundInfo soundInfo(SOUND_GAME_AMULET_RISE);
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));

        StrongActorPtr pGem = ActorTemplates::CreateActor(
            ActorPrototype_Level12_BossGem,
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

void AquatisAIStateComponent::VOnHealthChanged(int32 oldHealth, int32 newHealth, DamageType damageType, Point impactPoint, int sourceActorId)
{
    BaseBossAIStateComponennt::VOnHealthChanged(oldHealth, newHealth, damageType, impactPoint, sourceActorId);
}

