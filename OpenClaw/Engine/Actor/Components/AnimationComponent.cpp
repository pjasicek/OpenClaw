#include "AnimationComponent.h"
#include "../Actor.h"
#include "../../GameApp/BaseGameApp.h"
#include "../../Resource/Loaders/AniLoader.h"
#include "RenderComponent.h"
#include "PositionComponent.h"

#include "RenderComponent.h"

const char* AnimationComponent::g_Name = "AnimationComponent";

AnimationComponent::AnimationComponent()
    :
    _currentAnimation(nullptr),
    m_PauseOnStart(false),
    m_PauseOnEnd(false)
{ }

AnimationComponent::~AnimationComponent()
{
    _animationMap.clear();
}

bool AnimationComponent::VInit(TiXmlElement* data)
{
    assert(data != nullptr);

    // Loop through all anim paths elements
    for (TiXmlElement* animPathElem = data->FirstChildElement("AnimationPath");
        animPathElem != nullptr; 
        animPathElem = animPathElem->NextSiblingElement("AnimationPath"))
    {
        const char* animationsPath = animPathElem->GetText();

        // TODO: Rework. Consult with RenderComponent.cpp for proper fast implementation.
        // Take the algo from there and make it general purpose, dont copy-paste stuff
        std::vector<std::string> matchingAnimNames =
            g_pApp->GetResourceCache()->Match(animationsPath);

        for (std::string& animPath : matchingAnimNames)
        {
            WapAni* wapAni = AniResourceLoader::LoadAndReturnAni(animPath.c_str());
            std::string animNameKey = StripPathAndExtension(animPath);

            // Check if we dont already have the animation loaded
            if (_animationMap.count(animNameKey) > 0)
            {
                LOG_WARNING("Trying to load existing animation: " + animPath);
                continue;
            }

            std::shared_ptr<Animation> animation = Animation::CreateAnimation(wapAni, animNameKey.c_str(), animPath.c_str(), this);
            if (!animation)
            {
                LOG_ERROR("Could not create animation: " + animPath);
                return false;
            }

            _animationMap.insert(std::make_pair(animNameKey, animation));
        }
    }

    for (TiXmlElement* animElem = data->FirstChildElement("Animation");
        animElem != nullptr; 
        animElem = animElem->NextSiblingElement("Animation"))
    {
        if (!animElem->Attribute("type"))
        {
            LOG_WARNING("Animation element is missing type attribute.");
            continue;
        }

        std::string animType = animElem->Attribute("type");
        
        m_SpecialAnimationRequestList.push_back(animType);
    }

    for (TiXmlElement* pSpecialAnimElem = data->FirstChildElement("SpecialAnimation");
        pSpecialAnimElem != nullptr;
        pSpecialAnimElem = pSpecialAnimElem->NextSiblingElement("SpecialAnimation"))
    {
        SpecialAnimation specialAnim;

        DO_AND_CHECK(ParseValueFromXmlElem(&specialAnim.type, pSpecialAnimElem->FirstChildElement("Type")));
        DO_AND_CHECK(ParseValueFromXmlElem(&specialAnim.frameDuration, pSpecialAnimElem->FirstChildElement("FrameDuration")));
        DO_AND_CHECK(ParseValueFromXmlElem(&specialAnim.setPositionDelay, pSpecialAnimElem->FirstChildElement("HasPositionDelay")));

        m_SpecialAnimationList.push_back(specialAnim);
    }

    ParseValueFromXmlElem(&m_PauseOnStart, data->FirstChildElement("PauseOnStart"));
    ParseValueFromXmlElem(&m_PauseOnEnd, data->FirstChildElement("PauseOnEnd"));

    if (_animationMap.empty() && m_SpecialAnimationRequestList.empty() && m_SpecialAnimationList.empty())
    {
        LOG_WARNING("Animation map for animation component is empty. Actor type: " + std::string(data->Parent()->ToElement()->Attribute("Type")));
    }

    return true;
}

void AnimationComponent::VPostInit()
{
    shared_ptr<ActorRenderComponent> pRenderComponent = MakeStrongPtr(m_pOwner->GetComponent<ActorRenderComponent>());
    if (!pRenderComponent)
    {
        pRenderComponent = MakeStrongPtr(m_pOwner->GetComponent<HUDRenderComponent>());
    }
    if (!pRenderComponent)
    {
        LOG_ERROR("Actor has existing animation component but not render component. Actor: " + m_pOwner->GetName());
        assert(false && "Actor has to have render component");
    }
    m_pActorRenderComponent = pRenderComponent;

    // TODO: Get rid of this. Obfuscated, unmaintanable...
    for (const std::string& animType : m_SpecialAnimationRequestList)
    {
        if (animType.find("cycle") != std::string::npos)
        {
            std::string cycleDurationStr = animType;
            cycleDurationStr.erase(0, 5);
            int cycleDuration = std::stoi(cycleDurationStr);

            // If there is only 1 or 0 frames, we do not need to animate it
            if (pRenderComponent->GetImagesCount() <= 1)
            {
                //LOG("Skipping creating " + animType + " animation for: " + m_pOwner->GetName());
                continue;
            }

            std::shared_ptr<Animation> pCycleAnim = Animation::CreateAnimation(pRenderComponent->GetImagesCount(), cycleDuration, animType.c_str(), this);
            if (!pCycleAnim)
            {
                LOG_ERROR("Failed to create " + animType + " animation.");
                continue;
            }

            // Set delay according to X coord
            shared_ptr<PositionComponent> pPositionComponent = m_pOwner->GetPositionComponent();
            if (!pPositionComponent)
            {
                LOG_ERROR("Actor is missing position component. Actor: " + m_pOwner->GetName());
                continue;
            }
            
            // HACK: TODO: Make special animation creation somewhat more general
            // so I dont have to use these hacks
            // This hack is specific to Toggle pegs which set their on delay
            if (cycleDuration != 75 && cycleDuration != 50 && cycleDuration != 99)
            {
                srand(pPositionComponent->GetX());
                pCycleAnim->SetDelay(rand() % 1000);
            }

            _animationMap.insert(std::make_pair(animType, pCycleAnim));
        }
        else
        {
            LOG_WARNING("Unknown special animation type: " +     animType);
        }
    }

    for (const SpecialAnimation& specialAnim : m_SpecialAnimationList)
    {
        assert(specialAnim.type == "cycle" && "Currently supporting only cycle special animation");

        if (specialAnim.type == "cycle")
        {

            // If there is only 1 or 0 frames, we do not need to animate it
            if (pRenderComponent->GetImagesCount() <= 1)
            {
                //LOG("Skipping creating " + animType + " animation for: " + m_pOwner->GetName());
                continue;
            }

            std::shared_ptr<Animation> pCycleAnim = Animation::CreateAnimation(
                pRenderComponent->GetImagesCount(), 
                specialAnim.frameDuration, 
                specialAnim.type.c_str(), 
                this);
            if (!pCycleAnim)
            {
                LOG_ERROR("Failed to create " + specialAnim.type + " animation.");
                continue;
            }

            if (specialAnim.setPositionDelay)
            {
                // Set delay according to X coord
                shared_ptr<PositionComponent> pPositionComponent = m_pOwner->GetPositionComponent();
                if (!pPositionComponent)
                {
                    LOG_ERROR("Actor is missing position component. Actor: " + m_pOwner->GetName());
                    continue;
                }

                srand(pPositionComponent->GetX());
                pCycleAnim->SetDelay(rand() % 1000);
            }

            _animationMap.insert(std::make_pair(specialAnim.type, pCycleAnim));
        }
    }

    if (_animationMap.empty())
    {
        //LOG_ERROR("No animations in animation component. Offending actor: " + m_pOwner->GetName());
        //assert(false && "No animations in AnimationComponent at all !");
    }

    if (!_animationMap.empty())
    {
        _currentAnimation = _animationMap.begin()->second;
    }

    if (m_PauseOnStart)
    {
        _currentAnimation->Pause();
    }
}

TiXmlElement* AnimationComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    //

    return baseElement;
}

void AnimationComponent::VUpdate(uint32 msDiff)
{
    if (_currentAnimation)
    {
        _currentAnimation->Update(msDiff);
    }
}

bool AnimationComponent::SetAnimation(const std::string& animationName)
{
    if (animationName == _currentAnimation->GetName())
    {
        //LOG("Trying to set the same animation: " + animationName);
        return true;
    }
    //LOG("Setting animation: " + animationName);

    auto findIt = _animationMap.find(animationName);
    if (findIt == _animationMap.end())
    {
        LOG_WARNING("Could not find animation: " + animationName + " for actor: " + m_pOwner->GetName());
        return false;
    }

    //LOG("Setting anim: " + animationName);

    std::shared_ptr<Animation> pOldAnimation = _currentAnimation;
    std::shared_ptr<Animation> pNewAnimation = findIt->second;

    pNewAnimation->Reset();
    _currentAnimation = pNewAnimation;
    OnAnimationFrameStarted(_currentAnimation->GetCurrentAnimationFrame());

    // If animation has only 1 anim lasting 0 ms, then it is just a placeholder for the image
    // TODO: Some anims are reliant on this and they set anim delay.... e.g. PatrolStateComponent
    if (_currentAnimation->GetAnimFramesSize() == 1 &&
        _currentAnimation->GetCurrentAnimationFrame()->duration == 0)
    {
        //_currentAnimation->Pause();
    }

    if (pOldAnimation && pNewAnimation)
    {
        NotifyAnimationChanged(pOldAnimation.get(), pNewAnimation.get());
    }

    return true;
}

bool AnimationComponent::HasAnimation(std::string& animName)
{ 
    return (_animationMap.count(animName) > 0);
}

void AnimationComponent::PauseAnimation()
{
    _currentAnimation->Pause();
    NotifyAnimationPaused(_currentAnimation.get());
}

void AnimationComponent::ResumeAnimation()
{
    _currentAnimation->Resume();
    NotifyAnimationResumed(_currentAnimation.get());
}

void AnimationComponent::ResetAnimation()
{
    _currentAnimation->Reset();
}

//-------------------------------------------------------------------------------------------------
// Animation listeners
//

void AnimationComponent::OnAnimationFrameFinished(AnimationFrame* frame)
{
    //if (!frame->eventName.empty())
    //{
        // Raise event
    //}
}

void AnimationComponent::OnAnimationFrameStarted(AnimationFrame* frame)
{
    //if (!frame->eventName.empty())
    //{
        // Raise event
    //}

    // Notify render component to change frame image
    shared_ptr<ActorRenderComponent> renderComponent = MakeStrongPtr(m_pActorRenderComponent);
    if (renderComponent)
    {
        renderComponent->SetImage(frame->imageName);
    }
    else
    {
        LOG_WARNING("Could not find render component for actor: " + m_pOwner->GetName());
    }
}

void AnimationComponent::OnAnimationFinished()
{

}

void AnimationComponent::OnAnimationFrameChanged(AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    NotifyAnimationFrameChanged(_currentAnimation.get(), pLastFrame, pNewFrame);
}

void AnimationComponent::OnAnimationLooped()
{
    NotifyAnimationLooped(_currentAnimation.get());
}

void AnimationComponent::OnAnimationAtLastFrame()
{
    NotifyAnimationAtLastFrame(_currentAnimation.get());

    if (m_PauseOnEnd)
    {
        _currentAnimation->Pause();
    }
}

void AnimationComponent::SetDelay(uint32 msDelay)
{
    _currentAnimation->SetDelay(msDelay);
}

void AnimationComponent::SetReverseAnimation(bool reverse)
{
    _currentAnimation->SetReverseAnim(reverse);
}

bool AnimationComponent::AddAnimation(const std::string &animName, std::shared_ptr<Animation> &pAnim)
{
    if (_animationMap.find(animName) != _animationMap.end())
    {
        return false;
    }

    _animationMap[animName] = pAnim;

    return true;
}

//=====================================================================================================================
// AnimationSubject implementation
//=====================================================================================================================

AnimationSubject::~AnimationSubject()
{
    m_AnimationObservers.clear();
}

void AnimationSubject::NotifyAnimationLooped(Animation* pAnimation)
{
    for (AnimationObserver* pSubject : m_AnimationObservers)
    {
        if (pSubject)
        {
            pSubject->VOnAnimationLooped(pAnimation);
        }
    }
}

void AnimationSubject::NotifyAnimationStarted(Animation* pAnimation)
{
    for (AnimationObserver* pSubject : m_AnimationObservers)
    {
        if (pSubject)
        {
            pSubject->VOnAnimationStarted(pAnimation);
        }
    }
}

void AnimationSubject::NotifyAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    for (AnimationObserver* pSubject : m_AnimationObservers)
    {
        if (pSubject)
        {
            pSubject->VOnAnimationFrameChanged(pAnimation, pLastFrame, pNewFrame);
        }
    }
}

void AnimationSubject::NotifyAnimationPaused(Animation* pAnimation)
{
    for (AnimationObserver* pSubject : m_AnimationObservers)
    {
        if (pSubject)
        {
            pSubject->VOnAnimationPaused(pAnimation);
        }
    }
}

void AnimationSubject::NotifyAnimationResumed(Animation* pAnimation)
{
    for (AnimationObserver* pSubject : m_AnimationObservers)
    {
        if (pSubject)
        {
            pSubject->VOnAnimationResumed(pAnimation);
        }
    }
}

void AnimationSubject::NotifyAnimationAtLastFrame(Animation* pAnimation)
{
    for (AnimationObserver* pSubject : m_AnimationObservers)
    {
        if (pSubject)
        {
            pSubject->VOnAnimationAtLastFrame(pAnimation);
        }
    }
}

void AnimationSubject::NotifyAnimationChanged(Animation* pOldAnimation, Animation* pNewAnimation)
{
    for (AnimationObserver* pSubject : m_AnimationObservers)
    {
        if (pSubject)
        {
            pSubject->VOnAnimationChanged(pOldAnimation, pNewAnimation);
        }
    }
}

void AnimationSubject::NotifyAnimationEndedDelay(Animation* pOldAnimation)
{
    for (AnimationObserver* pSubject : m_AnimationObservers)
    {
        if (pSubject)
        {
            pSubject->VOnAnimationEndedDelay(pOldAnimation);
        }
    }
}

void AnimationSubject::AddObserver(AnimationObserver* pObserver)
{
    m_AnimationObservers.push_back(pObserver);
}

void AnimationSubject::RemoveObserver(AnimationObserver* pObserver)
{
    m_AnimationObservers.erase(std::remove(m_AnimationObservers.begin(), m_AnimationObservers.end(), pObserver), m_AnimationObservers.end());
}
