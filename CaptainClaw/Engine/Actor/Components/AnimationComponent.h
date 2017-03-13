#ifndef ANIMATIONCOMPONENT_H_
#define ANIMATIONCOMPONENT_H_

#include <map>

#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "Animation.h"

class Animation;
class AnimationObserver;
class AnimationSubject
{
public:
    void NotifyAnimationLooped(Animation* pAnimation);
    void NotifyAnimationStarted(Animation* pAnimation);
    void NotifyAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame);
    void NotifyAnimationPaused(Animation* pAnimation);
    void NotifyAnimationResumed(Animation* pAnimation);
    void NotifyAnimationAtLastFrame(Animation* pAnimation);
    void AddObserver(AnimationObserver* pObserver);
    void RemoveObserver(AnimationObserver* pObserver);

private:
    std::vector<AnimationObserver*> m_AnimationObservers;
};

class AnimationObserver
{
public:
    virtual void VOnAnimationLooped(Animation* pAnimation) { }
    virtual void VOnAnimationStarted(Animation* pAnimation) { }
    virtual void VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame) { }
    virtual void VOnAnimationPaused(Animation* pAnimation) { }
    virtual void VOnAnimationResumed(Animation* pAnimation) { }
    virtual void VOnAnimationAtLastFrame(Animation* pAnimation) { }
};

typedef std::map<std::string, Animation*> AnimationMap;

class Image;
class AnimationComponent : public ActorComponent, public AnimationSubject
{
    friend class Animation;

public:
    AnimationComponent();
    virtual ~AnimationComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;

    virtual void VPostInit();

    virtual void VUpdate(uint32 msDiff) override;

    // API
    bool SetAnimation(std::string animationName);
    bool HasAnimation(std::string& animName);
    void PauseAnimation();
    void ResumeAnimation();
    void ResetAnimation();
    void SetDelay(uint32 msDelay);
    void SetReverseAnimation(bool reverse);
    Animation* GetCurrentAnimation() const { return _currentAnimation; }
    std::string GetCurrentAnimationName() const;

private:

    bool m_PauseOnStart;

    // Animation events
    void OnAnimationFrameFinished(AnimationFrame* frame);
    void OnAnimationFrameStarted(AnimationFrame* frame);
    void OnAnimationFinished();
    void OnAnimationFrameChanged(AnimationFrame* pLastFrame, AnimationFrame* pNewFrame);
    void OnAnimationLooped();
    void OnAnimationAtLastFrame();


    AnimationMap _animationMap;
    Animation* _currentAnimation;

    std::vector<std::string> m_SpecialAnimationRequestList;
};

#endif