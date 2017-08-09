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
    m_pAnimationComponent = MakeStrongPtr(_owner->GetComponent<AnimationComponent>()).get();
    assert(m_pAnimationComponent != NULL);

    m_pAnimationComponent->AddObserver(this);
    m_pAnimationComponent->SetAnimation(m_Properties.idleAnimName);

    if (auto pTriggerComp = MakeStrongPtr(_owner->GetComponent<TriggerComponent>()))
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
                auto pPC = MakeStrongPtr(pStandingActor->GetComponent<PhysicsComponent>());
                if (pPC)
                {
                    pPC->SetIsForcedUp(true, (int)m_Properties.springHeight);
                }
            }
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

void SpringBoardComponent::VOnActorEnteredTrigger(Actor* pActorWhoEntered)
{
    OnActorBeginContact(pActorWhoEntered);
}

void SpringBoardComponent::VOnActorLeftTrigger(Actor* pActorWhoLeft)
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