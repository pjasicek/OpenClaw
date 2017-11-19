#include "MarrowEncounter.h"
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

}

void MarrowAIStateComponent::VOnStateLeave(BaseEnemyAIStateComponent* pNextState)
{

}

void MarrowAIStateComponent::VUpdate(uint32 msDiff)
{

}

bool MarrowAIStateComponent::VCanEnter()
{
    return true;
}

void MarrowAIStateComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{

}

void MarrowAIStateComponent::VOnAnimationLooped(Animation* pAnimation)
{

}

void MarrowAIStateComponent::VOnHealthChanged(int32 oldHealth, int32 newHealth, DamageType damageType, Point impactPoint, int sourceActorId)
{

}

void MarrowAIStateComponent::OnCanActivateFloor()
{

}

void MarrowAIStateComponent::VOnBossFightStarted()
{

}

void MarrowAIStateComponent::VOnBossFightEnded(bool isBossDead)
{

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
    return true;
}

void MarrowParrotAIStateComponent::VPostInit()
{

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

}

void MarrowParrotAIStateComponent::VOnStateLeave(BaseEnemyAIStateComponent* pNextState)
{

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