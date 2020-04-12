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
    ~AnimationSubject();

    void NotifyAnimationLooped(Animation* pAnimation);
    void NotifyAnimationStarted(Animation* pAnimation);
    void NotifyAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame);
    void NotifyAnimationPaused(Animation* pAnimation);
    void NotifyAnimationResumed(Animation* pAnimation);
    void NotifyAnimationAtLastFrame(Animation* pAnimation);
    void NotifyAnimationChanged(Animation* pOldAnimation, Animation* pNewAnimation);
    void NotifyAnimationEndedDelay(Animation* pAnimation);
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
    virtual void VOnAnimationChanged(Animation* pOldAnimation, Animation* pNewAnimation) { }
    virtual void VOnAnimationEndedDelay(Animation* pAnimation) { }
};

struct SpecialAnimation
{
    SpecialAnimation()
    {
        frameDuration = 0;
        setPositionDelay = false;
    }

    std::string type;
    int frameDuration;
    bool setPositionDelay;
};

typedef std::map<std::string, std::shared_ptr<Animation>> AnimationMap;

class Image;
class ActorRenderComponent;
class AnimationComponent : public ActorComponent, public AnimationSubject
{
    friend class Animation;

public:
    AnimationComponent();
    virtual ~AnimationComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;

    virtual void VPostInit() override;

    virtual void VUpdate(uint32 msDiff) override;

    // API
    bool SetAnimation(const std::string& animationName);
    bool HasAnimation(std::string& animName);
    void PauseAnimation();
    void ResumeAnimation();
    void ResetAnimation();
    void SetDelay(uint32 msDelay);
    void SetReverseAnimation(bool reverse);
    inline std::shared_ptr<Animation> GetCurrentAnimation() const { return _currentAnimation; }
    inline std::string GetCurrentAnimationName() const { return _currentAnimation->GetName(); }
    bool AddAnimation(const std::string &animName, std::shared_ptr<Animation> &pAnim);

private:

    bool m_PauseOnStart;
    bool m_PauseOnEnd;

    // Animation events
    void OnAnimationFrameFinished(AnimationFrame* frame);
    void OnAnimationFrameStarted(AnimationFrame* frame);
    void OnAnimationFinished();
    void OnAnimationFrameChanged(AnimationFrame* pLastFrame, AnimationFrame* pNewFrame);
    void OnAnimationLooped();
    void OnAnimationAtLastFrame();


    AnimationMap _animationMap;
    std::shared_ptr<Animation> _currentAnimation;

    // TODO: Get rid of this..
    std::vector<std::string> m_SpecialAnimationRequestList;

    std::vector<SpecialAnimation> m_SpecialAnimationList;

    std::weak_ptr<ActorRenderComponent> m_pActorRenderComponent;
};

#endif
