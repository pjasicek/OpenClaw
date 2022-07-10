#include "MarrowEncounter.h"
#include "../EnemyAIComponent.h"
#include "../../../Actor.h"
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

const char* MarrowAIStateComponent::g_Name = "MarrowAIStateComponent";
const char* MarrowParrotAIStateComponent::g_Name = "MarrowParrotAIStateComponent";
const char* MarrowFloorComponent::g_Name = "MarrowFloorComponent";

//=====================================================================================================================
// MarrowAIStateComponent
//=====================================================================================================================

MarrowAIStateComponent::MarrowAIStateComponent()
    :
    BaseBossAIStateComponennt("MarrowAIStateComponent")
{

}

MarrowAIStateComponent::~MarrowAIStateComponent()
{

}

bool MarrowAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    if (!BaseBossAIStateComponennt::VDelegateInit(pData))
    {
        return false;
    }

    return true;
}

void MarrowAIStateComponent::VPostInit()
{
    BaseBossAIStateComponennt::VPostInit();

    m_pAnimationComponent = m_pOwner->GetRawComponent<AnimationComponent>(true);
    m_pAnimationComponent->AddObserver(this);
}

void MarrowAIStateComponent::VOnWorldFinishedLoading()
{
    StrongActorPtr pParrotActor = g_pApp->GetGameLogic()->FindActorByName("Level10_MarrowParrot", true);
    assert(pParrotActor != nullptr);

    m_pParrot = pParrotActor->GetRawComponent<MarrowParrotAIStateComponent>(true);

    m_MarrowFloorActorList = g_pApp->GetGameLogic()->FindActorByName("Level10_MarrowFloor");
    assert(m_MarrowFloorActorList.size() == 2);
}

void MarrowAIStateComponent::VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState)
{
    m_IsActive = true;
}

void MarrowAIStateComponent::VOnStateLeave(BaseEnemyAIStateComponent* pNextState)
{
    m_IsActive = false;
}

void MarrowAIStateComponent::VUpdate(uint32 msDiff)
{
    if (!m_IsActive)
    {
        return;
    }
}

bool MarrowAIStateComponent::VCanEnter()
{
    return m_bBossFightStarted == false;
}

void MarrowAIStateComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    if (!m_IsActive)
    {
        return;
    }
}

void MarrowAIStateComponent::VOnAnimationLooped(Animation* pAnimation)
{
    if (!m_IsActive)
    {
        return;
    }
}

void MarrowAIStateComponent::VOnHealthChanged(int32 oldHealth, int32 newHealth, DamageType damageType, Point impactPoint, int sourceActorId)
{
    BaseBossAIStateComponennt::VOnHealthChanged(oldHealth, newHealth, damageType, impactPoint, sourceActorId);
}

void MarrowAIStateComponent::OnCanActivateFloor()
{

}

void MarrowAIStateComponent::VOnBossFightStarted()
{
    BaseBossAIStateComponennt::VOnBossFightStarted();

    // To refresh contact list after Claw's death
    g_pApp->GetGameLogic()->VGetGamePhysics()->VActivate(m_pOwner->GetGUID());

    m_pOwner->GetRawComponent<EnemyAIComponent>(true)->EnterBestState(true);
}

void MarrowAIStateComponent::VOnBossFightEnded(bool isBossDead)
{
    BaseBossAIStateComponennt::VOnBossFightEnded(isBossDead);

    if (isBossDead)
    {
        SoundInfo soundInfo(SOUND_GAME_AMULET_RISE);
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));

        StrongActorPtr pGem = ActorTemplates::CreateActor(
            ActorPrototype_Level10_BossGem,
            m_pOwner->GetPositionComponent()->GetPosition());

        Point gemForce(3.5, -7);
        g_pApp->GetGameLogic()->VGetGamePhysics()->VApplyLinearImpulse(pGem->GetGUID(), gemForce);
    }
    else
    {
        // To refresh contact list after Claw's death
        g_pApp->GetGameLogic()->VGetGamePhysics()->VDeactivate(m_pOwner->GetGUID());
    }
}

void MarrowAIStateComponent::OnQuarterHealthGone()
{

}

//=====================================================================================================================
// MarrowAIStateComponent
//=====================================================================================================================

MarrowParrotAIStateComponent::MarrowParrotAIStateComponent()
    :
    BaseEnemyAIStateComponent("MarrowParrotAIStateComponent")
{

}

MarrowParrotAIStateComponent::~MarrowParrotAIStateComponent()
{

}

bool MarrowParrotAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    DO_AND_CHECK(ParseValueFromXmlElem(&m_IdleAnim, pData->FirstChildElement("IdleAnim")));

    return true;
}

void MarrowParrotAIStateComponent::VPostInit()
{
    BaseEnemyAIStateComponent::VPostInit();

    m_pAnimationComponent = m_pOwner->GetRawComponent<AnimationComponent>(true);
    m_pAnimationComponent->SetAnimation(m_IdleAnim);

    LOG("I am here !");
}

void MarrowParrotAIStateComponent::VUpdate(uint32 msDiff)
{

}

void MarrowParrotAIStateComponent::VOnWorldFinishedLoading()
{
    StrongActorPtr pMarrowActor = g_pApp->GetGameLogic()->FindActorByName("Level10_Marrow", true);
    assert(pMarrowActor != nullptr);

    m_pMarrow = pMarrowActor->GetRawComponent<MarrowAIStateComponent>(true);
}

void MarrowParrotAIStateComponent::VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState)
{
    m_IsActive = true;
}

void MarrowParrotAIStateComponent::VOnStateLeave(BaseEnemyAIStateComponent* pNextState)
{
    m_IsActive = false;
}

void MarrowParrotAIStateComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{

}

void MarrowParrotAIStateComponent::VOnAnimationLooped(Animation* pAnimation)
{

}

void MarrowParrotAIStateComponent::VOnAnimationAtLastFrame(Animation* pAnimation)
{

}

void MarrowParrotAIStateComponent::VOnAnimationChanged(Animation* pOldAnimation, Animation* pNewAnimation)
{

}

void MarrowParrotAIStateComponent::SitOnPerch()
{

}

void MarrowParrotAIStateComponent::SitOnShoulder(const Point& shoulderLocation)
{

}

void MarrowParrotAIStateComponent::DoAttack()
{

}

bool MarrowParrotAIStateComponent::VCanEnter()
{
    return true;
}

void MarrowParrotAIStateComponent::ActivateMarrowFloors()
{

}

void MarrowParrotAIStateComponent::OnGotHit(DamageType damageType)
{

}

//=====================================================================================================================
// MarrowFloorComponent
//=====================================================================================================================

MarrowFloorComponent::MarrowFloorComponent()
{

}

MarrowFloorComponent::~MarrowFloorComponent()
{

}

bool MarrowFloorComponent::VInit(TiXmlElement* pData)
{
    return true;
}

void MarrowFloorComponent::VPostInit()
{

}

void MarrowFloorComponent::VUpdate(uint32 msDiff)
{

}

void MarrowFloorComponent::VOnWorldFinishedLoading()
{

}

void MarrowFloorComponent::Activate()
{

}

void MarrowFloorComponent::Deactivate()
{

}
