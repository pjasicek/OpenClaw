#include "PunkRatAIStateComponent.h"
#include "../../Actor.h"
#include "../PositionComponent.h"
#include "../RenderComponent.h"

#include "../../../GameApp/BaseGameLogic.h"
#include "../../../GameApp/BaseGameApp.h"
#include "../../../UserInterface/HumanView.h"

const char* PunkRatAIStateComponent::g_Name = "PunkRatAIStateComponent";

PunkRatAIStateComponent::PunkRatAIStateComponent()
    :
    m_pAnimationComponent(NULL),
    m_pCannonActor(NULL),
    m_pCannonAnimationComponent(NULL),
    m_RatFireAnimFrameIdx(-1),
    m_CannonFireAnimFrameIdx(-1),
    m_bIsMirrored(false),
    m_CannonActorProto(ActorPrototype_None),
    m_ProjectileActorPrototype(ActorPrototype_None),
    BaseEnemyAIStateComponent("PunkRatAI")
{

}

PunkRatAIStateComponent::~PunkRatAIStateComponent()
{
    if (m_pCannonAnimationComponent) {
        m_pCannonAnimationComponent->RemoveObserver(this);
        m_pCannonAnimationComponent->SetAnimation(m_CannonIdleAnim);
    }
}

bool PunkRatAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    std::string cannonActorProtoStr;
    DO_AND_CHECK(ParseValueFromXmlElem(&cannonActorProtoStr, pData->FirstChildElement("CannonActorPrototype")));
    m_CannonActorProto = StringToEnum_ActorPrototype(cannonActorProtoStr);

    DO_AND_CHECK(ParseValueFromXmlElem(&m_CannonSpawnOffset, pData->FirstChildElement("CannonSpawnOffset"), "x", "y"));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_RatFireAnim, pData->FirstChildElement("RatFireAnim")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_RatRecoilAnim, pData->FirstChildElement("RatRecoilAnim")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_RatFireAnimFrameIdx, pData->FirstChildElement("RatFireAnimFrameIdx")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_CannonFireAnim, pData->FirstChildElement("CannonFireAnim")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_CannonIdleAnim, pData->FirstChildElement("CannonIdleAnim")));

    std::string projectileActorProtoStr;
    DO_AND_CHECK(ParseValueFromXmlElem(&projectileActorProtoStr, pData->FirstChildElement("ProjectileActorPrototype")));
    m_ProjectileActorPrototype = StringToEnum_ActorPrototype(projectileActorProtoStr);

    DO_AND_CHECK(ParseValueFromXmlElem(&m_CannonFireAnimFrameIdx, pData->FirstChildElement("CannonFireAnimFrameIdx")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_ProjectileSpawnOffset, pData->FirstChildElement("ProjectileSpawnOffset"), "x", "y"));

    return true;
}

void PunkRatAIStateComponent::VPostInit()
{
    BaseEnemyAIStateComponent::VPostInit();

    shared_ptr<ActorRenderComponent> pARC = MakeStrongPtr(m_pOwner->GetComponent<ActorRenderComponent>());
    assert(pARC != nullptr);
    m_bIsMirrored = pARC->IsMirrored();

    if (m_bIsMirrored)
    {
        m_CannonSpawnOffset.x *= -1.0;
    }

    // Create the cannon
    Point cannonSpawn = m_pOwner->GetPositionComponent()->GetPosition() + m_CannonSpawnOffset;
    StrongActorPtr pCannonActor = ActorTemplates::CreateActor(
        m_CannonActorProto,
        cannonSpawn);
    m_pCannonActor = pCannonActor.get();

    m_pAnimationComponent = MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>()).get();
    m_pCannonAnimationComponent = MakeStrongPtr(m_pCannonActor->GetComponent<AnimationComponent>()).get();
    assert(m_pAnimationComponent != nullptr);
    assert(m_pCannonAnimationComponent != nullptr);

    m_pAnimationComponent->AddObserver(this);
    m_pCannonAnimationComponent->AddObserver(this);

    m_pCannonAnimationComponent->SetAnimation(m_CannonIdleAnim);

    
    shared_ptr<ActorRenderComponent> pCannonARC = MakeStrongPtr(m_pCannonActor->GetComponent<ActorRenderComponent>());
    assert(pCannonARC != nullptr);
    pCannonARC->SetMirrored(m_bIsMirrored);

    if (m_bIsMirrored)
    {
        m_ProjectileSpawnOffset.x *= -1.0;
    }
}

void PunkRatAIStateComponent::VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState)
{
    Point zeroSpeed(0, 0);
    g_pApp->GetGameLogic()->VGetGamePhysics()->VSetLinearSpeed(m_pOwner->GetGUID(), zeroSpeed);

    m_pAnimationComponent->SetAnimation(m_RatFireAnim);

    m_IsActive = true;
}

void PunkRatAIStateComponent::VOnStateLeave(BaseEnemyAIStateComponent* pNextState)
{
    m_IsActive = false;
}

bool PunkRatAIStateComponent::VCanEnter()
{
    return true;
}

void PunkRatAIStateComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    if (pAnimation->GetOwnerComponent() == m_pAnimationComponent)
    {
        if ((pAnimation->GetName() == m_RatFireAnim) && (pNewFrame->idx == m_RatFireAnimFrameIdx))
        {
            m_pCannonAnimationComponent->SetAnimation(m_CannonFireAnim);
            m_pCannonAnimationComponent->ResumeAnimation();
        }
    }
    else // Cannon
    {
        if ((pAnimation->GetName() == m_CannonFireAnim) && (pNewFrame->idx == m_CannonFireAnimFrameIdx))
        {
            // Spawn is relative to the cannon, no the rat
            Point projectilePos = m_pCannonActor->GetPositionComponent()->GetPosition() + m_ProjectileSpawnOffset;
            if (g_pApp->GetHumanView()->GetCamera()->IntersectsWithPoint(projectilePos, 1.25f))
            {
                Direction dir = Direction_Right;
                if (m_bIsMirrored)
                {
                    dir = Direction_Left;
                }

                ActorTemplates::CreateActor_Projectile(
                    m_ProjectileActorPrototype,
                    projectilePos,
                    dir,
                    m_pOwner->GetGUID());
            }
        }
    }
}

void PunkRatAIStateComponent::VOnAnimationLooped(Animation* pAnimation)
{
    if (pAnimation->GetOwnerComponent() == m_pAnimationComponent)
    {
        if (pAnimation->GetName() == m_RatFireAnim)
        {
            m_pAnimationComponent->SetAnimation(m_RatRecoilAnim);
        }
        else if (pAnimation->GetName() == m_RatRecoilAnim)
        {
            m_pAnimationComponent->SetAnimation(m_RatFireAnim);
            m_pAnimationComponent->SetDelay(Util::GetRandomNumber(300, 1000));
        }
    }
    else // Cannon
    {
        if (pAnimation->GetName() == m_CannonFireAnim)
        {
            m_pCannonAnimationComponent->SetAnimation(m_CannonIdleAnim);
        }
    }
}