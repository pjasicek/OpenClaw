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
    m_Size(Point(0, 0)),
    m_TimeOff(0),
    m_TimeOn(0),
    m_Delay(0),
    m_IsAlwaysActive(false),
    m_pAnimationComponent(NULL),
    m_pPhysics(nullptr),
    m_PrevAnimframeIdx(0)
{

}

TogglePegAIComponent::~TogglePegAIComponent()
{

}

bool TogglePegAIComponent::VInit(TiXmlElement* data)
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
    if (TiXmlElement* pElem = data->FirstChildElement("AlwaysOn"))
    {
        m_IsAlwaysActive = std::string(pElem->GetText()) == "true";
    }
    if (TiXmlElement* pElem = data->FirstChildElement("TimeOff"))
    {
        m_TimeOff = std::stod(pElem->GetText());
    }
    if (TiXmlElement* pElem = data->FirstChildElement("TimeOn"))
    {
        m_TimeOn = std::stod(pElem->GetText());
    }
    if (TiXmlElement* pElem = data->FirstChildElement("Delay"))
    {
        m_Delay = std::stod(pElem->GetText());
    }

    assert(ParseValueFromXmlElem(&m_ToggleFrameIdx, data->FirstChildElement("ToggleFrameIdx")));

    return true;
}

void TogglePegAIComponent::VPostInit()
{
    // Set size from current image if necessary
    if (fabs(m_Size.x) < DBL_EPSILON || fabs(m_Size.y) < DBL_EPSILON)
    {
        shared_ptr<ActorRenderComponent> pRenderComponent =
            MakeStrongPtr(_owner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name));
        assert(pRenderComponent);

        shared_ptr<Image> pImage = MakeStrongPtr(pRenderComponent->GetCurrentImage());

        m_Size.x = pImage->GetWidth();
        m_Size.y = pImage->GetHeight();
    }

    shared_ptr<AnimationComponent> pAnimationComponent =
        MakeStrongPtr(_owner->GetComponent<AnimationComponent>(AnimationComponent::g_Name));
    assert(pAnimationComponent);

    if (m_IsAlwaysActive)
    {
        pAnimationComponent->PauseAnimation();
    }
    else
    {
        pAnimationComponent->SetReverseAnimation(true);
        pAnimationComponent->SetDelay(m_Delay);
        pAnimationComponent->AddObserver(this);
    }

    // Set size from current image if necessary
    if (fabs(m_Size.x) < DBL_EPSILON || fabs(m_Size.y) < DBL_EPSILON)
    {
        shared_ptr<ActorRenderComponent> pRenderComponent =
            MakeStrongPtr(_owner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name));
        assert(pRenderComponent);

        shared_ptr<Image> pImage = MakeStrongPtr(pRenderComponent->GetCurrentImage());

        m_Size.x = pImage->GetWidth();
        m_Size.y = pImage->GetHeight();
    }

    m_pPhysics->VAddStaticBody(_owner, m_Size, CollisionType_Ground);
}

TiXmlElement* TogglePegAIComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    //

    return baseElement;
}

void TogglePegAIComponent::VUpdate(uint32 msDiff)
{
    /*if (m_IsAlwaysActive)
    {
        m_pAnimationComponent->PauseAnimation();
        return;
    }

    // TODO: HACK: This is polling, drains cpu for no reason, should be event based
    if (Animation* pAnimation = m_pAnimationComponent->GetCurrentAnimation())
    {
        if (m_PrevAnimframeIdx != pAnimation->GetCurrentAnimationFrame()->idx)
        {
            if (pAnimation->IsAtLastAnimFrame())
            {
                pAnimation->SetDelay(m_TimeOff - 500);
            }
            else if (pAnimation->IsAtFirstAnimFrame())
            {
                pAnimation->SetDelay(m_TimeOn - 500);
            }

            m_PrevAnimframeIdx = pAnimation->GetCurrentAnimationFrame()->idx;
        }

        if (pAnimation->GetCurrentAnimationFrame()->idx > 8)
        {
            m_pPhysics->VDeactivate(_owner->GetGUID());
        }
        else
        {
            m_pPhysics->VActivate(_owner->GetGUID());
        }
    }*/
}

void TogglePegAIComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    /*LOG(ToStr(_owner->GetGUID()));
    LOG(ToStr(pLastFrame->idx) + " - " + ToStr(pNewFrame->idx));*/
    if ((pLastFrame->idx == (m_ToggleFrameIdx - 1)) && (pNewFrame->idx == m_ToggleFrameIdx))
    {
        m_pPhysics->VDeactivate(_owner->GetGUID());
    }
    else if ((pLastFrame->idx == m_ToggleFrameIdx) && (pNewFrame->idx == (m_ToggleFrameIdx - 1)))
    {
        m_pPhysics->VActivate(_owner->GetGUID());
    }

    if (pAnimation->IsAtLastAnimFrame())
    {
        pAnimation->SetDelay(m_TimeOff - 500);
    }
    else if (pAnimation->IsAtFirstAnimFrame())
    {
        pAnimation->SetDelay(m_TimeOn - 500);
    }
}

void TogglePegAIComponent::VOnAnimationLooped(Animation* pAnimation)
{

}