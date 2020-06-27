#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <vector>

#include "../../SharedDefines.h"
#include <libwap.h>

class Image;
struct AnimationFrame
{
    AnimationFrame()
    {
        imageId = 0;
        imageName = "";
        idx = 0;
        duration = 0;
        eventName = "";
        hasEvent = false;
    }

    uint32 imageId;
    std::string imageName;
    uint32 idx;
    uint32 duration;
    std::string eventName;
    bool hasEvent;
};

// AnimationComponent and Animation are tightly coupled together
class AnimationComponent;
class Animation
{
public:
    Animation();
    ~Animation();

    static std::shared_ptr<Animation> CreateAnimation(WapAni* wapAni, const char* animationName, const char* resourcePath, AnimationComponent* owner);
    static std::shared_ptr<Animation> CreateAnimation(const std::vector<AnimationFrame> &animFrames, const char* animName, AnimationComponent* owner);
    static std::shared_ptr<Animation> CreateAnimation(int numAnimFrames, int animFrameTime, const char* animName, AnimationComponent* owner);

    inline std::string GetName() const { return _name; }

    AnimationFrame* GetCurrentAnimationFrame() { return &_currentAnimationFrame; }

    void Update(uint32 msDiff);
    void Reset();
    void SetNextFrame();
    void SetAnimationFrame(int idx) { assert(idx < (int)_animationFrames.size()); _currentAnimationFrame = _animationFrames[idx]; }

    void Pause() { _paused = true; }
    void Resume() { _paused = false; }
    void SetDelay(uint32 delay) { _delay = delay; }

    void SetReverseAnim(bool reverse) { _reversed = reverse; }

    const std::vector<AnimationFrame>& GetAnimFrames() const { return _animationFrames; }
    uint32 GetAnimFramesSize() const { return _animationFrames.size(); }
    bool IsAtLastAnimFrame() const { return _currentAnimationFrame.idx + 1 == _animationFrames.size(); }
    bool IsAtFirstAnimFrame() const { return _currentAnimationFrame.idx == 0; }
    bool IsPaused() const { return _paused; }

    const AnimationComponent* GetOwnerComponent() const { return m_pOwner; }

private:
    void AddAnimationFrame(AnimationFrame& animFrame) { _animationFrames.push_back(animFrame); }
    void SetName(const char* name) { _name = name; }
    void SetOwner(AnimationComponent* owner) { assert(!m_pOwner && owner); m_pOwner = owner; }

    bool Initialize(WapAni* wapAni, const char* animationName, const char* resourcePath, AnimationComponent* owner);
    bool Initialize(const std::vector<AnimationFrame> &animFrames, const char* animationName, AnimationComponent* owner);
    bool Initialize(int numAnimFrames, int animFrameTime, const char* animName, AnimationComponent* owner);

    void PlayFrameSound(const std::string& sound);

    std::string _name;
    AnimationFrame _currentAnimationFrame;
    int32 _currentTime;
    int32 _delay;
    bool _paused;
    bool _reversed;
    bool _isBeingReversed;

    AnimationComponent* m_pOwner;

    std::vector<AnimationFrame> _animationFrames;
};

#endif