#include "SawBladeComponent.h"
#include "../Actor.h"

#include "AuraComponents/AuraComponent.h"

const char* SawBladeComponent::g_Name = "SawBladeComponent";

SawBladeComponent::SawBladeComponent() 
    :
    m_ActiveFrameIdx(0),
    m_DeactivateFrameIdx(0),
    m_TimeOff(0),
    m_StartDelay(0),
    m_pAnimationComponent(NULL),
    m_pDamageAuraComponent(NULL)
{

}

bool SawBladeComponent::VInit(TiXmlElement* pData)
{
    DO_AND_CHECK(ParseValueFromXmlElem(&m_UpAnimName, pData->FirstChildElement("UpAnimName")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_DownAnimName, pData->FirstChildElement("DownAnimName")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_SpinAnimName, pData->FirstChildElement("SpinAnimName")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_ActiveFrameIdx, pData->FirstChildElement("ActiveFrameIdx")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_DeactivateFrameIdx, pData->FirstChildElement("DeactivateFrameIdx")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_TimeOff, pData->FirstChildElement("TimeOff")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_StartDelay, pData->FirstChildElement("StartDelay")));

    return true;
}

void SawBladeComponent::VPostInit()
{
    m_pAnimationComponent = m_pOwner->GetRawComponent<AnimationComponent>(true);
    m_pDamageAuraComponent = m_pOwner->GetRawComponent<DamageAuraComponent>(true);
}

void SawBladeComponent::VPostPostInit()
{
    m_pAnimationComponent->SetAnimation(m_UpAnimName);
    m_pAnimationComponent->SetDelay(m_StartDelay);
    m_pAnimationComponent->AddObserver(this);
}


void SawBladeComponent::VOnAnimationLooped(Animation* pAnimation)
{
    const std::string& animName = pAnimation->GetName();

    if (animName == m_UpAnimName)
    {
        m_pDamageAuraComponent->SetEnabled(true);
        m_pAnimationComponent->SetAnimation(m_SpinAnimName);
    }
    else if (animName == m_SpinAnimName)
    {
        m_pDamageAuraComponent->SetEnabled(false);
        m_pAnimationComponent->SetAnimation(m_DownAnimName);
    }
    else if (animName == m_DownAnimName)
    {
        m_pDamageAuraComponent->SetEnabled(false);
        m_pAnimationComponent->SetAnimation(m_UpAnimName);
        m_pAnimationComponent->SetDelay(m_TimeOff);
    }
}

void SawBladeComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    /*const std::string& animName = pAnimation->GetName();

    if (animName == m_UpAnimName && pNewFrame->idx == m_ActiveFrameIdx)
    {
        m_pDamageAuraComponent->SetEnabled(true);
    }
    else if (animName == m_DownAnimName && pNewFrame->idx == m_DeactivateFrameIdx)
    {
        m_pDamageAuraComponent->SetEnabled(false);
    }*/
}