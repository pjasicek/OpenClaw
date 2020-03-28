#include "Animation.h"
#include "AnimationComponent.h"
#include "../../Util/Util.h"

#include "PositionComponent.h"
#include "../Actor.h"

#include "../../Events/EventMgr.h"
#include "../../Events/Events.h"

Animation::Animation() :
    _name("Unknown"),
    _currentTime(0),
    _delay(0),
    _paused(false),
    _reversed(false),
    _isBeingReversed(false),
    m_pOwner(NULL)
{ }

Animation::~Animation()
{
    _animationFrames.clear();
}

std::shared_ptr<Animation> Animation::CreateAnimation(WapAni* wapAni, const char* animationName, const char* resourcePath, AnimationComponent* owner)
{
    std::shared_ptr<Animation> animation{new Animation()};
    if (!animation->Initialize(wapAni, animationName, resourcePath, owner))
    {
        return nullptr;
    }

    return animation;
}

std::shared_ptr<Animation> Animation::CreateAnimation(const std::vector<AnimationFrame> &animFrames, const char* animName, AnimationComponent* owner)
{
    std::shared_ptr<Animation> animation{new Animation()};
    if (!animation->Initialize(animFrames, animName, owner))
    {
        return nullptr;
    }

    return animation;
}

std::shared_ptr<Animation> Animation::CreateAnimation(int numAnimFrames, int animFrameTime, const char* animName, AnimationComponent* owner)
{
    std::shared_ptr<Animation> animation{new Animation()};
    if (!animation->Initialize(numAnimFrames, animFrameTime, animName, owner))
    {
        return nullptr;
    }

    return animation;
}

bool Animation::Initialize(WapAni* wapAni, const char* animationName, const char* resourcePath, AnimationComponent* owner)
{
    _name = animationName;
    m_pOwner = owner;

    // Load animation frame from WapAni
    uint32 numAnimFrames = wapAni->animationFramesCount;
    AniAnimationFrame* aniAnimFrames = wapAni->animationFrames;
    _animationFrames.reserve(numAnimFrames);
    for (uint32 frameIdx = 0; frameIdx < numAnimFrames; ++frameIdx)
    {
        AnimationFrame animFrame;
        animFrame.idx = frameIdx;
        animFrame.imageId = aniAnimFrames[frameIdx].imageFileId;
        animFrame.imageName = "frame" + Util::ConvertToThreeDigitsString(animFrame.imageId);
        animFrame.duration = aniAnimFrames[frameIdx].duration;
         
        // if pWapAni->unk0 == 1, then skip all sounds in this animation
        if (aniAnimFrames[frameIdx].eventFilePath != NULL &&
            wapAni->unk0 != 1)
        {
            std::string resourcePathStr(resourcePath);
            std::string soundPath(aniAnimFrames[frameIdx].eventFilePath);

            std::replace(soundPath.begin(), soundPath.end(), '_', '/');

            // If the sound path from ANI has "LEVEL" in it, then take the level number
            // from the animation resource path
            if (soundPath.find("LEVEL/") != std::string::npos)
            {
                soundPath = soundPath.substr(soundPath.find("/"));

                // Remove "/" at the beginning
                resourcePathStr.erase(0, 1);
                std::string rootDir = resourcePathStr.substr(0, resourcePathStr.find("/"));

                soundPath = "/" + rootDir + "/SOUNDS" + soundPath + ".WAV";
            }
            else
            {
                // Else just replace it with /[game|state|claw]/sounds/
                soundPath.insert(soundPath.find("/"), "/sounds");
                soundPath.insert(0, "/");
                soundPath += ".wav";
            }

            std::transform(soundPath.begin(), soundPath.end(), soundPath.begin(), ::tolower);

            //LOG("Sound: " + soundPath);

            animFrame.hasEvent = true;
            animFrame.eventName = soundPath;
        }
        else
        {
            animFrame.hasEvent = false;
            animFrame.eventName = "";
        }

        // HACK: For specific reason, dynamite jump throw takes too long
        if (_name == "jumpdynamite")
        {
            animFrame.duration = 60;
        }

        _animationFrames.push_back(animFrame);
    }

    if (_animationFrames.empty())
    {
        LOG_ERROR("Animation: " + _name + " has no animation frames");
        return false;
    }

    _currentAnimationFrame = _animationFrames[0];

    return true;
}

bool Animation::Initialize(const std::vector<AnimationFrame> &animFrames, const char* animationName, AnimationComponent* owner)
{
    if (animFrames.empty())
    {
        LOG_ERROR("Animation: " + std::string(animationName) + " has no animation frames");
        return false;
    }

    _name = animationName;
    m_pOwner = owner;
    _animationFrames = animFrames;

    _currentAnimationFrame = _animationFrames[0];

    return true;
}

bool Animation::Initialize(int numAnimFrames, int animFrameTime, const char* animName, AnimationComponent* owner)
{
    _animationFrames.reserve(numAnimFrames);
    for (int frameIdx = 0; frameIdx < numAnimFrames; ++frameIdx)
    {
        AnimationFrame animFrame;
        animFrame.idx = frameIdx;
        animFrame.imageId = frameIdx + 1;
        animFrame.imageName = "frame" + Util::ConvertToThreeDigitsString(animFrame.imageId);
        animFrame.duration = animFrameTime;
        animFrame.hasEvent = false;
        animFrame.eventName = "";

        _animationFrames.push_back(animFrame);
    }
    _name = animName;
    m_pOwner = owner;

    if (_animationFrames.empty())
    {
        LOG_ERROR("Animation: " + _name + " has no animation frames");
        return false;
    }

    _currentAnimationFrame = _animationFrames[0];

    return true;
}

void Animation::Update(uint32 msDiff)
{
    if (_paused)
    {
        return;
    }
    
    if (_delay > 0)
    {
        _delay -= msDiff;
        if (_delay > 0)
        {
            return;
        }
        else
        {
            m_pOwner->NotifyAnimationEndedDelay(this);
            msDiff = abs(_delay);
            _delay = 0;
        }
    }

    // Hack for now
    if (_currentAnimationFrame.hasEvent)
    {
        if (_currentAnimationFrame.idx == 0 && _currentTime == 0)
        {
            PlayFrameSound(_currentAnimationFrame.eventName);
        }
    }

    _currentTime += msDiff;

    int32 currentFrameDuration = _currentAnimationFrame.duration;
    if (_currentTime >= currentFrameDuration)
    {
        _currentTime = _currentTime - currentFrameDuration;

        if (m_pOwner)
        {
            m_pOwner->OnAnimationFrameFinished(&_currentAnimationFrame);
        }

        SetNextFrame();
    }
}

void Animation::Reset()
{
    _currentAnimationFrame = _animationFrames[0];
    _delay = 0;
    _currentTime = 0;
    _paused = false;
}

void Animation::SetNextFrame()
{
    uint32 countAnimationFrames = _animationFrames.size();

    bool looped = false;
    // Certain animations play in loop while being reversed - e.g.: 0,1,2,3,4,3,2,1,0,1,....
    if (_reversed)
    {
        if (_currentAnimationFrame.idx == (_animationFrames.size() - 1))
        {
            _isBeingReversed = true;
            looped = true;
        }
        else if (_isBeingReversed && _currentAnimationFrame.idx == 0)
        {
            _isBeingReversed = false;
            looped = true;
        }
    }
    else
    {
        if (IsAtLastAnimFrame())
        {
            looped = true;
        }
        // If next frame will be last
        else if (_currentAnimationFrame.idx + 2 == _animationFrames.size())
        {
            m_pOwner->OnAnimationAtLastFrame();
        }
    }

    int32 delta = 0;
    _isBeingReversed ? delta-- : delta++;

    AnimationFrame* lastAnimFrame = &_animationFrames[_currentAnimationFrame.idx];
    _currentAnimationFrame = _animationFrames[(_currentAnimationFrame.idx + delta) % countAnimationFrames];    

    m_pOwner->OnAnimationFrameStarted(&_currentAnimationFrame);

    m_pOwner->OnAnimationFrameChanged(lastAnimFrame, &_currentAnimationFrame);
    if (looped)
    {
        m_pOwner->OnAnimationLooped();
    }

    if (_currentAnimationFrame.idx != 0 && _currentAnimationFrame.hasEvent)
    {
        PlayFrameSound(_currentAnimationFrame.eventName);
    }
}

void Animation::PlayFrameSound(const std::string& sound)
{
    assert(m_pOwner && m_pOwner->m_pOwner && m_pOwner->m_pOwner->GetPositionComponent());

    //LOG("Sound: " + sound + ", Owner: " + m_pOwner->m_pOwner->GetName());

    SoundInfo soundInfo(sound);
    soundInfo.setDistanceEffect = true;
    soundInfo.soundSourcePosition = m_pOwner->m_pOwner->GetPositionComponent()->GetPosition();
    IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
        new EventData_Request_Play_Sound(soundInfo)));
}