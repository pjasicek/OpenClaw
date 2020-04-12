#include "SpringBoardComponent.h"
#include "PositionComponent.h"
#include "PhysicsComponent.h"
#include "TriggerComponents/TriggerComponent.h"

#include "../../GameApp/BaseGameApp.h"
#include "../../GameApp/BaseGameLogic.h"

#include "../../Events/Events.h"
#include "../../Events/EventMgr.h"

const char* SpringBoardComponent::g_Name = "SpringBoardComponent";

SpringBoardComponent::SpringBoardComponent()
    :
    m_bIsSteppedOn(false),
    m_pAnimationComponent(NULL)
{

}

bool SpringBoardComponent::VInit(TiXmlElement* pData)
{
    assert(pData != NULL);

    m_pPhysics = g_pApp->GetGameLogic()->VGetGamePhysics();
    assert(m_pPhysics != nullptr);

    m_Properties.LoadFromXml(pData, true);

    return true;
}

void SpringBoardComponent::VPostInit()
{
    m_pAnimationComponent = MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>()).get();
    assert(m_pAnimationComponent != nullptr);

    if (m_Properties.idleAnimName == "NONE")
    {
        std::shared_ptr<Animation> pIdleAnim = Animation::CreateAnimation(1, 0, "NONE", m_pAnimationComponent);
        assert(pIdleAnim);
        DO_AND_CHECK(m_pAnimationComponent->AddAnimation("NONE", pIdleAnim));
    }

    m_pAnimationComponent->AddObserver(this);
    m_pAnimationComponent->SetAnimation(m_Properties.idleAnimName);

    if (auto pTriggerComp = MakeStrongPtr(m_pOwner->GetComponent<TriggerComponent>()))
    {
        pTriggerComp->AddObserver(this);
    }
}

void SpringBoardComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    if (pNewFrame->idx > pLastFrame->idx)
    {
        if ((pAnimation->GetName() == m_Properties.springAnimName) && 
            (pNewFrame->idx == m_Properties.springFrameIdx))
        {
            // Only does anything for Claw I assume
            for (Actor* pStandingActor : m_StandingActorsList)
            {
                auto pPC = pStandingActor->GetPhysicsComponent();
                if (pPC)
                {
                    pPC->SetIsForcedUp(true, (int)m_Properties.springHeight);
                }
            }

            Util::PlaySimpleSound(m_Properties.springSound);
        }
        if (pAnimation->IsAtLastAnimFrame())
        {
            if (m_StandingActorsList.empty())
            {
                m_pAnimationComponent->SetAnimation(m_Properties.idleAnimName);
            }
        }
    }
}

void SpringBoardComponent::VOnActorEnteredTrigger(Actor* pActorWhoEntered, FixtureType triggerType)
{
    OnActorBeginContact(pActorWhoEntered);
}

void SpringBoardComponent::VOnActorLeftTrigger(Actor* pActorWhoLeft, FixtureType triggerType)
{
    OnActorEndContact(pActorWhoLeft);
}

void SpringBoardComponent::VOnAnimationLooped(Animation* pAnimation)
{
    if (pAnimation->GetName() == m_Properties.springAnimName)
    {
        pAnimation->SetDelay(m_Properties.springDelay);
    }
}

void SpringBoardComponent::OnActorBeginContact(Actor* pActor)
{
    if (m_StandingActorsList.empty())
    {
        m_pAnimationComponent->SetAnimation(m_Properties.springAnimName);
        m_pAnimationComponent->SetDelay(m_Properties.springDelay);
    }

    m_StandingActorsList.push_back(pActor);
}

void SpringBoardComponent::OnActorEndContact(Actor* pActor)
{
    for (auto actorIter = m_StandingActorsList.begin(); actorIter != m_StandingActorsList.end(); ++actorIter)
    {
        if (pActor == (*actorIter))
        {
            m_StandingActorsList.erase(actorIter);
            return;
        }
    }
}