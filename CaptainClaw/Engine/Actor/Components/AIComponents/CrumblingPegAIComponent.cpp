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
    m_Size(Point(0, 0)),
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

    if (TiXmlElement* pElem = data->FirstChildElement("Size"))
    {
        pElem->Attribute("width", &m_Size.x);
        pElem->Attribute("height", &m_Size.y);
    }

    return true;
}

void CrumblingPegAIComponent::VPostInit()
{
    shared_ptr<AnimationComponent> pAnimationComponent =
        MakeStrongPtr(_owner->GetComponent<AnimationComponent>(AnimationComponent::g_Name));
    assert(pAnimationComponent && pAnimationComponent->GetCurrentAnimation());
    pAnimationComponent->AddObserver(this);
}

TiXmlElement* CrumblingPegAIComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    return baseElement;
}

void CrumblingPegAIComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    if (pNewFrame->idx == 9)
    {
        m_pPhysics->VDeactivate(_owner->GetGUID());
    }
    // TODO: VOnAnimationLooped not working prop
    if (pAnimation->IsAtLastAnimFrame())
    {
        pAnimation->Pause();
        auto pARC = MakeStrongPtr(_owner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name));
        pARC->SetHidden(true);
    }
}

void CrumblingPegAIComponent::VOnAnimationLooped(Animation* pAnimation)
{

}

void CrumblingPegAIComponent::OnContact(b2Body* pBody)
{
    shared_ptr<AnimationComponent> pAnimationComponent =
        MakeStrongPtr(_owner->GetComponent<AnimationComponent>(AnimationComponent::g_Name));
    assert(pAnimationComponent && pAnimationComponent->GetCurrentAnimation());
    pAnimationComponent->ResumeAnimation();
}

// After claw dies, "recreate" crumbling pegs
void CrumblingPegAIComponent::ClawDiedDelegate(IEventDataPtr pEventData)
{
    shared_ptr<AnimationComponent> pAnimationComponent =
        MakeStrongPtr(_owner->GetComponent<AnimationComponent>(AnimationComponent::g_Name));
    pAnimationComponent->GetCurrentAnimation()->Reset();
    pAnimationComponent->GetCurrentAnimation()->Pause();

    auto pARC = MakeStrongPtr(_owner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name));
    pARC->SetHidden(false);
    pARC->SetImage(pAnimationComponent->GetCurrentAnimation()->GetCurrentAnimationFrame()->imageName);

    m_pPhysics->VActivate(_owner->GetGUID());
}