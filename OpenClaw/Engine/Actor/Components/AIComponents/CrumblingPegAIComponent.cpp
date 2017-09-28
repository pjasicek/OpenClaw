#include "CrumblingPegAIComponent.h"
#include "../../../GameApp/BaseGameApp.h"
#include "../../../GameApp/BaseGameLogic.h"

#include "../../../Graphics2D/Image.h"
#include "../RenderComponent.h"
#include "../PositionComponent.h"
#include "../AnimationComponent.h"
#include "../Animation.h"
#include "../../../Physics/ClawPhysics.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"

const char* CrumblingPegAIComponent::g_Name = "CrumblingPegAIComponent";

CrumblingPegAIComponent::CrumblingPegAIComponent()
    :
    m_pPhysics(nullptr)
{
    IEventMgr::Get()->VAddListener(MakeDelegate(this, &CrumblingPegAIComponent::ClawDiedDelegate), EventData_Claw_Died::sk_EventType);
}

CrumblingPegAIComponent::~CrumblingPegAIComponent()
{
    IEventMgr::Get()->VRemoveListener(MakeDelegate(this, &CrumblingPegAIComponent::ClawDiedDelegate), EventData_Claw_Died::sk_EventType);
}

bool CrumblingPegAIComponent::VInit(TiXmlElement* data)
{
    assert(data != NULL);

    m_pPhysics = g_pApp->GetGameLogic()->VGetGamePhysics();
    if (!m_pPhysics)
    {
        LOG_WARNING("Attemtping to create physics component without valid physics");
        return false;
    }

    m_Properties.LoadFromXml(data, true);

    return true;
}

void CrumblingPegAIComponent::VPostInit()
{
    shared_ptr<AnimationComponent> pAnimationComponent =
        MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>(AnimationComponent::g_Name));
    assert(pAnimationComponent && pAnimationComponent->GetCurrentAnimation());
    pAnimationComponent->AddObserver(this);
    pAnimationComponent->SetDelay(m_Properties.crumbleDelay);
}

TiXmlElement* CrumblingPegAIComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    return baseElement;
}

void CrumblingPegAIComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    if (pNewFrame->idx == m_Properties.crumbleFrameIdx)
    {
        m_pPhysics->VDeactivate(m_pOwner->GetGUID());
    }
    // TODO: VOnAnimationLooped not working prop
    if (pAnimation->IsAtLastAnimFrame())
    {
        pAnimation->Pause();
        auto pARC = MakeStrongPtr(m_pOwner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name));
        pARC->SetVisible(false);
    }
}

void CrumblingPegAIComponent::VOnAnimationLooped(Animation* pAnimation)
{

}

void CrumblingPegAIComponent::OnContact(b2Body* pBody)
{
    shared_ptr<AnimationComponent> pAnimationComponent =
        MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>(AnimationComponent::g_Name));
    assert(pAnimationComponent && pAnimationComponent->GetCurrentAnimation());
    pAnimationComponent->ResumeAnimation();

    SoundInfo soundInfo(m_Properties.crumbleSound);
    IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
        new EventData_Request_Play_Sound(soundInfo)));
}

// After claw dies, "recreate" crumbling pegs
void CrumblingPegAIComponent::ClawDiedDelegate(IEventDataPtr pEventData)
{
    shared_ptr<AnimationComponent> pAnimationComponent =
        MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>(AnimationComponent::g_Name));
    pAnimationComponent->GetCurrentAnimation()->Reset();
    pAnimationComponent->GetCurrentAnimation()->Pause();
    pAnimationComponent->SetDelay(m_Properties.crumbleDelay);

    auto pARC = MakeStrongPtr(m_pOwner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name));
    pARC->SetVisible(true);
    pARC->SetImage(pAnimationComponent->GetCurrentAnimation()->GetCurrentAnimationFrame()->imageName);

    m_pPhysics->VActivate(m_pOwner->GetGUID());
}