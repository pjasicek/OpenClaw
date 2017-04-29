#include "TogglePegAIComponent.h"
#include "../../../GameApp/BaseGameApp.h"
#include "../../../GameApp/BaseGameLogic.h"

#include "../../../Graphics2D/Image.h"
#include "../RenderComponent.h"
#include "../PositionComponent.h"
#include "../AnimationComponent.h"
#include "../Animation.h"
#include "../../../Physics/ClawPhysics.h"

const char* TogglePegAIComponent::g_Name = "TogglePegAIComponent";

TogglePegAIComponent::TogglePegAIComponent()
    :
    m_pAnimationComponent(NULL),
    m_pPhysics(nullptr),
    m_PrevAnimframeIdx(0)
{

}

TogglePegAIComponent::~TogglePegAIComponent()
{

}

bool TogglePegAIComponent::VInit(TiXmlElement* pData)
{
    assert(pData != NULL);

    m_pPhysics = g_pApp->GetGameLogic()->VGetGamePhysics();
    if (!m_pPhysics)
    {
        LOG_WARNING("Attemtping to create physics component without valid physics");
        return false;
    }

    assert(ParseValueFromXmlElem(&m_Properties.timeOn, pData->FirstChildElement("TimeOn")));
    assert(ParseValueFromXmlElem(&m_Properties.timeOff, pData->FirstChildElement("TimeOff")));
    assert(ParseValueFromXmlElem(&m_Properties.delay, pData->FirstChildElement("Delay")));
    assert(ParseValueFromXmlElem(&m_Properties.toggleFrameIdx, pData->FirstChildElement("ToggleFrameIdx")));
    assert(ParseValueFromXmlElem(&m_Properties.isAlwaysOn, pData->FirstChildElement("AlwaysOn")));

    return true;
}

void TogglePegAIComponent::VPostInit()
{
    m_pAnimationComponent =
        MakeStrongPtr(_owner->GetComponent<AnimationComponent>(AnimationComponent::g_Name)).get();
    assert(m_pAnimationComponent);

    if (m_Properties.isAlwaysOn)
    {
        m_pAnimationComponent->PauseAnimation();
    }
    else
    {
        m_pAnimationComponent->SetReverseAnimation(true);
        m_pAnimationComponent->SetDelay(m_Properties.delay);
        m_pAnimationComponent->AddObserver(this);
    }
}

TiXmlElement* TogglePegAIComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    //

    return baseElement;
}

void TogglePegAIComponent::VUpdate(uint32 msDiff)
{
    
}

void TogglePegAIComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    /*LOG(ToStr(_owner->GetGUID()));
    LOG(ToStr(pLastFrame->idx) + " - " + ToStr(pNewFrame->idx));*/
    if ((pLastFrame->idx == (m_Properties.toggleFrameIdx - 1)) && 
        (pNewFrame->idx == m_Properties.toggleFrameIdx))
    {
        m_pPhysics->VDeactivate(_owner->GetGUID());
    }
    else if ((pLastFrame->idx == m_Properties.toggleFrameIdx) && 
            (pNewFrame->idx == (m_Properties.toggleFrameIdx - 1)))
    {
        m_pPhysics->VActivate(_owner->GetGUID());
    }

    if (pAnimation->IsAtLastAnimFrame())
    {
        pAnimation->SetDelay(m_Properties.timeOff - 500);
    }
    else if (pAnimation->IsAtFirstAnimFrame())
    {
        pAnimation->SetDelay(m_Properties.timeOn - 500);
    }
}

void TogglePegAIComponent::VOnAnimationLooped(Animation* pAnimation)
{

}