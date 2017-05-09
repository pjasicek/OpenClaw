#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <vector>

#include "../../SharedDefines.h"
#include <libwap.h>

class Image;
struct AnimationFrame
{
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

    static Animation* CreateAnimation(WapAni* wapAni, const char* animationName, const char* resourcePath, AnimationComponent* owner);
    static Animation* CreateAnimation(std::vector<AnimationFrame> animFrames, const char* animName, AnimationComponent* owner);
    static Animation* CreateAnimation(int numAnimFrames, int animFrameTime, const char* animName, AnimationComponent* owner);

    inline std::string GetName() const { return _name; }

    AnimationFrame* GetCurrentAnimationFrame() { return &_currentAnimationFrame; }

    void Update(uint32 msDiff);
    void Reset();
    void SetNextFrame();

    void Pause() { _paused = true; }
    void Resume() { _paused = false; }
    void SetDelay(uint32 delay) { _currentTime -= delay; }

    void SetReverseAnim(bool reverse) { _reversed = reverse; }

    uint32 GetAnimFramesSize() const { return _animationFrames.size(); }
    bool IsAtLastAnimFrame() const { return _currentAnimationFrame.idx + 1 == _animationFrames.size(); }
    bool IsAtFirstAnimFrame() const { return _currentAnimationFrame.idx == 0; }
    bool IsPaused() const { return _paused; }

private:
    void AddAnimationFrame(AnimationFrame& animFrame) { _animationFrames.push_back(animFrame); }
    void SetName(const char* name) { _name = name; }
    void SetOwner(AnimationComponent* owner) { assert(!_owner && owner); _owner = owner; }

    bool Initialize(WapAni* wapAni, const char* animationName, const char* resourcePath, AnimationComponent* owner);
    bool Initialize(std::vector<AnimationFrame> animFrames, const char* animationName, AnimationComponent* owner);
    bool Initialize(int numAnimFrames, int animFrameTime, const char* animName, AnimationComponent* owner);

    void PlayFrameSound(const std::string& sound);

    std::string _name;
    AnimationFrame _currentAnimationFrame;
    int32 _currentTime;
    bool _paused;
    bool _reversed;
    bool _isBeingReversed;

    AnimationComponent* _owner;

    std::vector<AnimationFrame> _animationFrames;
};

#endif