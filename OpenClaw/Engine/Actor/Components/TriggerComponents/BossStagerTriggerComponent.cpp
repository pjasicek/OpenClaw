#include "BossStagerTriggerComponent.h"
#include "../ControllableComponent.h"
#include "../FollowableComponent.h"
#include "../PositionComponent.h"

#include "../../../GameApp/BaseGameApp.h"
#include "../../../GameApp/BaseGameLogic.h"
#include "../../../UserInterface/HumanView.h"
#include "../../../Audio/Audio.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"

//=====================================================================================================================
//
// BossStagerTriggerComponent Implementation
//
//=====================================================================================================================

const char* BossStagerTriggerComponent::g_Name = "BossStagerTriggerComponent";

BossStagerTriggerComponent::BossStagerTriggerComponent() 
:
    m_BossDistance(0),
    m_CameraSpeed(0),
    m_PopupTitleSpeed(0),
    m_pPopupTitleActor(NULL),
    m_bActivated(false),
    m_Delay(0),
    m_CurrentDelay(0),
    m_State(BossStagerState_None),
    m_ActorWhoEnteredId(INVALID_ACTOR_ID),
    m_pOverlappingActor(NULL)
{ 
    IEventMgr::Get()->VAddListener(MakeDelegate(this, &BossStagerTriggerComponent::BossFightEndedDelegate), EventData_Boss_Fight_Ended::sk_EventType);
    IEventMgr::Get()->VAddListener(MakeDelegate(this, &BossStagerTriggerComponent::ClawRespawnedDelegate), EventData_Claw_Respawned::sk_EventType);

    if (g_pApp->GetDebugOptions()->bSkipBossFightIntro)
    {
        m_State = BossStagerState_Done;
    }
}

BossStagerTriggerComponent::~BossStagerTriggerComponent()
{
    IEventMgr::Get()->VRemoveListener(MakeDelegate(this, &BossStagerTriggerComponent::BossFightEndedDelegate), EventData_Boss_Fight_Ended::sk_EventType);
    IEventMgr::Get()->VRemoveListener(MakeDelegate(this, &BossStagerTriggerComponent::ClawRespawnedDelegate), EventData_Claw_Respawned::sk_EventType);
}

bool BossStagerTriggerComponent::VInit(TiXmlElement* pData)
{
    assert(pData);

    DO_AND_CHECK(ParseValueFromXmlElem(&m_BossDistance, pData->FirstChildElement("BossDistance")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_CameraSpeed, pData->FirstChildElement("CameraSpeed")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_ClawDialogSound, pData->FirstChildElement("ClawDialogSound")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_BossDialogSound, pData->FirstChildElement("BossDialogSound")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_PopupTitleImageSet, pData->FirstChildElement("PopupTitleImageSet")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_PopupTitleSound, pData->FirstChildElement("PopupTitleSound")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_PopupTitleSpeed, pData->FirstChildElement("PopupTitleSpeed")));

    m_pCamera = g_pApp->GetHumanView()->GetCamera();
    assert(m_pCamera != nullptr);

    return true;
}

void BossStagerTriggerComponent::VPostInit()
{
    shared_ptr<TriggerComponent> pTriggerComponent =
        MakeStrongPtr(m_pOwner->GetComponent<TriggerComponent>(TriggerComponent::g_Name));
    assert(pTriggerComponent);

    pTriggerComponent->AddObserver(this);
}

TiXmlElement* BossStagerTriggerComponent::VGenerateXml()
{
    // TODO: Implement
    return NULL;
}

void BossStagerTriggerComponent::VUpdate(uint32 msDiff)
{
    if (!m_bActivated || (m_State == BossStagerState_Done))
    {
        return;
    }

    if (m_State == BossStagerState_None)
    {
        m_State = BossStagerState_MovingToBoss;
    }
    else if (m_State == BossStagerState_MovingToBoss)
    {
        double offsetIncrement = (double)m_CameraSpeed * ((double)msDiff / 1000.0);
        m_pCamera->AddCameraOffsetX(offsetIncrement);
        if (m_pCamera->GetCameraOffsetX() > m_BossDistance)
        {
            m_pCamera->SetCameraOffsetX((double)m_BossDistance);
            m_CurrentDelay = Util::GetSoundDurationMs(m_BossDialogSound);
            SoundInfo sound(m_BossDialogSound);
            IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Request_Play_Sound(sound)));

            m_State = BossStagerState_PlayingBossDialogSound;
        }
    }
    else if (m_State == BossStagerState_PlayingBossDialogSound)
    {
        m_CurrentDelay -= msDiff;
        if (m_CurrentDelay < 0)
        {
            m_State = BossStagerState_MovingToClaw;
        }
    }
    else if (m_State == BossStagerState_MovingToClaw)
    {
        double offsetIncrement = -1.0 * (double)m_CameraSpeed * ((double)msDiff / 1000.0);
        m_pCamera->AddCameraOffsetX(offsetIncrement);
        if (m_pCamera->GetCameraOffsetX() < 0)
        {
            m_pCamera->SetCameraOffsetX(0);
            m_CurrentDelay = Util::GetSoundDurationMs(m_ClawDialogSound);
            SoundInfo sound(m_ClawDialogSound);
            IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Request_Play_Sound(sound)));

            // Display Claw's exclamation speech image
            int clawSpeechDuration = Util::GetSoundDurationMs(m_ClawDialogSound);
            StrongActorPtr pClaw = g_pApp->GetGameLogic()->GetClawActor();
            assert(pClaw != nullptr);
            ((FollowableComponent*)pClaw->GetRawComponent<FollowableComponent>())
                    ->Activate(clawSpeechDuration);

            m_State = BossStagerState_PlayingClawDialogSound;
        }
    }
    else if (m_State == BossStagerState_PlayingClawDialogSound)
    {
        m_CurrentDelay -= msDiff;
        if (m_CurrentDelay < 0)
        {
            // Spawn the popup
            Point winSize = g_pApp->GetWindowSizeScaled();
            Point popupPosition = m_pCamera->GetPosition() + Point(winSize.x / 2, -200);
            AnimationDef dummy;

            StrongActorPtr pPopup = ActorTemplates::CreateActor_StaticImage(
                ActorPrototype_StaticImage,
                popupPosition,
                m_PopupTitleImageSet,
                dummy);
            assert(pPopup != nullptr);

            m_pPopupTitleActor = pPopup.get();
            
            SoundInfo sound("/GAME/SOUNDS/SDPT1.WAV");
            IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Request_Play_Sound(sound)));

            m_State = BossStagerState_MovingPopupDown;
        }
    }
    else if (m_State == BossStagerState_MovingPopupDown)
    {
        Point winSize = g_pApp->GetWindowSizeScaled();
        double destinationPositionY = m_pCamera->GetPosition().y + 250;

        double positionIncrement = (double)m_PopupTitleSpeed * ((double)msDiff / 1000.0);
        Point newPos = m_pPopupTitleActor->GetPositionComponent()->GetPosition();
        newPos.y += positionIncrement;

        if (newPos.y > destinationPositionY)
        {
            newPos.y = destinationPositionY;

            std::string popupSound = "/GAME/SOUNDS/SDPT2.WAV";
            m_CurrentDelay = Util::GetSoundDurationMs(popupSound);
            SoundInfo sound(popupSound);
            IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Request_Play_Sound(sound)));

            m_State = BossStagerState_PlayingPopupSound1;
        }

        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Move_Actor(m_pPopupTitleActor->GetGUID(), newPos)));
    }
    else if (m_State == BossStagerState_PlayingPopupSound1)
    {
        m_CurrentDelay -= msDiff;
        if (m_CurrentDelay < 0)
        {
            m_CurrentDelay = Util::GetSoundDurationMs(m_PopupTitleSound);
            SoundInfo sound(m_PopupTitleSound);
            IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Request_Play_Sound(sound)));

            m_State = BossStagerState_PlayingPopupSound2;
        }
    }
    else if (m_State == BossStagerState_PlayingPopupSound2)
    {
        m_CurrentDelay -= msDiff;
        if (m_CurrentDelay < 0)
        {
            m_State = BossStagerState_MovingPopupRight;
        }
    }
    else if (m_State == BossStagerState_MovingPopupRight)
    {
        Point winSize = g_pApp->GetWindowSizeScaled();
        double destPositionX = m_pCamera->GetPosition().x + winSize.x + 200;

        double positionIncrement = (double)(m_PopupTitleSpeed * 3) * ((double)msDiff / 1000.0);
        Point newPos = m_pPopupTitleActor->GetPositionComponent()->GetPosition();
        newPos.x += positionIncrement;

        if (newPos.x > destPositionX)
        {
            newPos.x = destPositionX;
            IEventMgr::Get()->VQueueEvent(IEventDataPtr(new EventData_Destroy_Actor(m_pPopupTitleActor->GetGUID())));

            m_State = BossStagerState_Done;
        }

        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Move_Actor(m_pPopupTitleActor->GetGUID(), newPos)));
    }
    
    if (m_State == BossStagerState_Done)
    {
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Boss_Fight_Started(m_ActorWhoEnteredId, INVALID_ACTOR_ID)));
    }
}

void BossStagerTriggerComponent::VOnActorEnteredTrigger(Actor* pActorWhoPickedThis, FixtureType triggerType)
{
    // Boss staging already commenced
    if (m_bActivated)
    {
        return;
    }
    
    g_pApp->GetAudio()->StopAllSounds();

    shared_ptr<ClawControllableComponent> pClaw =
        MakeStrongPtr(pActorWhoPickedThis->GetComponent<ClawControllableComponent>(ClawControllableComponent::g_Name));
    assert(pClaw != nullptr);

    m_ActorWhoEnteredId = pActorWhoPickedThis->GetGUID();

    IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Entered_Boss_Area(m_ActorWhoEnteredId, INVALID_ACTOR_ID)));

    // We already died to the boss at least once
    if (m_State == BossStagerState_Done)
    {
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Boss_Fight_Started(m_ActorWhoEnteredId, INVALID_ACTOR_ID)));
    }

    m_bActivated = true;

    m_pOverlappingActor = pActorWhoPickedThis;
}

void BossStagerTriggerComponent::VOnActorLeftTrigger(Actor* pActorWhoLeft, FixtureType triggerType)
{
    shared_ptr<ClawControllableComponent> pClaw =
        MakeStrongPtr(pActorWhoLeft->GetComponent<ClawControllableComponent>(ClawControllableComponent::g_Name));
    if (pClaw != nullptr)
    {
        m_pOverlappingActor = NULL;
    }
}

void BossStagerTriggerComponent::BossFightEndedDelegate(IEventDataPtr pEvent)
{
    shared_ptr<EventData_Boss_Fight_Ended> pCastEventData =
        static_pointer_cast<EventData_Boss_Fight_Ended>(pEvent);

    if (!pCastEventData->GetIsBossDead())
    {
        m_bActivated = false;
        m_State = BossStagerState_Done;
    }
    else
    {
        IEventMgr::Get()->VQueueEvent(IEventDataPtr(new EventData_Destroy_Actor(m_pOwner->GetGUID())));
    }
}

// Hack - no easy way to clear contact list, I need to force boss stager to go through its logic
void BossStagerTriggerComponent::ClawRespawnedDelegate(IEventDataPtr pEvent)
{
    shared_ptr<EventData_Claw_Respawned> pCastEventData =
        static_pointer_cast<EventData_Claw_Respawned>(pEvent);

    if (m_State == BossStagerState_Done)
    {
        // Make sure it is Claw
        StrongActorPtr pClawActor = MakeStrongPtr(g_pApp->GetGameLogic()->VGetActor(pCastEventData->GetActorId()));
        assert(pClawActor != nullptr);

        shared_ptr<ClawControllableComponent> pClaw =
            MakeStrongPtr(pClawActor->GetComponent<ClawControllableComponent>(ClawControllableComponent::g_Name));
        assert(pClaw != nullptr && "Not Claw ?!");

        VOnActorEnteredTrigger(pClawActor.get(), FixtureType_Trigger);
    }
}
