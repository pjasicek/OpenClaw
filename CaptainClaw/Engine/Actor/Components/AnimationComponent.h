#ifndef ANIMATIONCOMPONENT_H_
#define ANIMATIONCOMPONENT_H_

#include <map>

#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "Animation.h"

class Image;
class Animation;

typedef std::map<std::string, Animation*> AnimationMap;

class AnimationComponent : public ActorComponent
{
public:
    AnimationComponent() { _currentAnimation = NULL; }
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

    // Events
    void OnAnimationFrameFinished(AnimationFrame* frame);
    void OnAnimationFrameStarted(AnimationFrame* frame);
    void OnAnimationFinished();

private:
    AnimationMap _animationMap;
    Animation* _currentAnimation;

    std::vector<std::string> m_SpecialAnimationRequestList;
};

#endif