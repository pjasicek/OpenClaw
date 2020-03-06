#include "TogglePegAIComponent.h"
#include "../../../GameApp/BaseGameApp.h"
#include "../../../GameApp/BaseGameLogic.h"

#include "../../../Graphics2D/Image.h"
#include "../RenderComponent.h"
#include "../PositionComponent.h"
#include "../AnimationComponent.h"
#include "../Animation.h"
#include "../../../Physics/ClawPhysics.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"

const char* TogglePegAIComponent::g_Name = "TogglePegAIComponent";

TogglePegAIComponent::TogglePegAIComponent()
    :
    m_pAnimationComponent(NULL),
    m_pPhysics(nullptr),
    m_PrevAnimframeIdx(0),
    m_OnDuration(0),
    m_OffDuration(0)
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

    m_Properties.LoadFromXml(pData, true);

    return true;
}

void TogglePegAIComponent::VPostInit()
{
    m_pAnimationComponent =
        MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>(AnimationComponent::g_Name)).get();
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

    std::shared_ptr<Animation> pCurrentAnimation = m_pAnimationComponent->GetCurrentAnimation();
    assert(pCurrentAnimation != NULL);

    int totalFrames = pCurrentAnimation->GetAnimFramesSize();
    // Assume that toggle pegs have cyclic anim
    int frameDuration = pCurrentAnimation->GetCurrentAnimationFrame()->duration;
    int framesOn = m_Properties.toggleFrameIdx;
    int framesOff = totalFrames - framesOn;

    m_OnDuration = m_Properties.timeOn - (framesOn * frameDuration);
    m_OffDuration = m_Properties.timeOff - (framesOff * frameDuration);
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

//struct DelayEventInfo
//{
//    DelayEventInfo(IEventDataPtr pEvent, uint32 msDelay)
//    {
//        this->pEvent = pEvent;
//        this->msDelay = msDelay;
//    }
//
//    IEventDataPtr pEvent;
//    uint32 msDelay;
//};
//
//static int SetupPlayMusicThread(void* pData)
//{
//    DelayEventInfo* pEventInfo = (DelayEventInfo*)pData;
//
//    SDL_Delay(pEventInfo->msDelay);
//    IEventMgr::Get()->VTriggerEvent(pEventInfo->pEvent);
//
//    return 0;
//}
//
//static void DelayEvent(IEventDataPtr pEvent, uint32 msDelay)
//{
//    DelayEventInfo* pDelayEventInfo = new DelayEventInfo(pEvent, msDelay);
//
//    // Playing music track takes ALOT of time for some reason so play it in another thread
//    SDL_Thread* pThread = SDL_CreateThread(SetupPlayMusicThread, "DelayedEventThread", (void*)pDelayEventInfo);
//    SDL_DetachThread(pThread);
//}

void TogglePegAIComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    bool didToggle = false;
    /*LOG(ToStr(m_pOwner->GetGUID()));
    LOG(ToStr(pLastFrame->idx) + " - " + ToStr(pNewFrame->idx));*/
    if ((pLastFrame->idx == (m_Properties.toggleFrameIdx - 1)) && 
        (pNewFrame->idx == m_Properties.toggleFrameIdx))
    {
        m_pPhysics->VDeactivate(m_pOwner->GetGUID());
        didToggle = true;
    }
    else if ((pLastFrame->idx == m_Properties.toggleFrameIdx) && 
            (pNewFrame->idx == (m_Properties.toggleFrameIdx - 1)))
    {
        m_pPhysics->VActivate(m_pOwner->GetGUID());
        didToggle = true;
    }

    if (pAnimation->IsAtLastAnimFrame())
    {
        pAnimation->SetDelay(m_OffDuration);
    }
    else if (pAnimation->IsAtFirstAnimFrame())
    {
        pAnimation->SetDelay(m_OnDuration);
    }

    if ((pNewFrame->idx == 1 && pLastFrame->idx == 0) ||
        (pNewFrame->idx == pAnimation->GetAnimFramesSize() - 2 && pLastFrame->idx == pAnimation->GetAnimFramesSize() - 1))
    {
        SoundInfo sound(m_Properties.toggleSound);
        sound.setDistanceEffect = true;
        sound.soundSourcePosition = m_pOwner->GetPositionComponent()->GetPosition();
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(sound)));
    }
}

void TogglePegAIComponent::VOnAnimationLooped(Animation* pAnimation)
{

}