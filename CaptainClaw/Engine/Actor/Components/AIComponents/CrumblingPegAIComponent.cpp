#include <Tinyxml\tinyxml.h>
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
{ }

CrumblingPegAIComponent::~CrumblingPegAIComponent()
{

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

    //

    return baseElement;
}

/*void CrumblingPegAIComponent::VUpdate(uint32 msDiff)
{
    // TODO: HACK: This is polling, drains cpu for no reason, should be event based
    if (Animation* pAnimation = m_pAnimationComponent->GetCurrentAnimation())
    {
        if (m_PrevAnimframeIdx != pAnimation->GetCurrentAnimationFrame()->idx)
        {
            if (pAnimation->GetCurrentAnimationFrame()->idx == 9)
            {
                m_pPhysics->VRemoveActor(_owner->GetGUID());
            }

            if (pAnimation->IsAtLastAnimFrame())
            {
                shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(_owner->GetGUID()));
                IEventMgr::Get()->VQueueEvent(pEvent);
            }

            m_PrevAnimframeIdx = pAnimation->GetCurrentAnimationFrame()->idx;
        }
    }
}*/

void CrumblingPegAIComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    if (pNewFrame->idx == 9)
    {
        m_pPhysics->VRemoveActor(_owner->GetGUID());
    }
}

void CrumblingPegAIComponent::VOnAnimationLooped(Animation* pAnimation)
{
    shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(_owner->GetGUID()));
    IEventMgr::Get()->VQueueEvent(pEvent);
}

void CrumblingPegAIComponent::OnContact(b2Body* pBody)
{
    shared_ptr<AnimationComponent> pAnimationComponent =
        MakeStrongPtr(_owner->GetComponent<AnimationComponent>(AnimationComponent::g_Name));
    assert(pAnimationComponent && pAnimationComponent->GetCurrentAnimation());
    pAnimationComponent->ResumeAnimation();
}