#include "PowerupComponent.h"
#include "../PositionComponent.h"
#include "../RenderComponent.h"
#include "../PowerupSparkleAIComponent.h"
#include "../PhysicsComponent.h"
#include "../../ActorTemplates.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"

const char* PowerupComponent::g_Name = "PowerupComponent";

PowerupComponent::PowerupComponent()
    :
    m_RemainingPowerupTime(0),
    m_ActivePowerup(PowerupType_None)
{ 
    IEventMgr::Get()->VAddListener(MakeDelegate(
        this, &PowerupComponent::ClawDiedDelegate), EventData_Claw_Died::sk_EventType);
}

PowerupComponent::~PowerupComponent()
{
    for (const auto &pSparkle : m_PowerupSparkles)
    {
        pSparkle->Destroy();
    }

    m_PowerupSparkles.clear();

    IEventMgr::Get()->VRemoveListener(MakeDelegate(
        this, &PowerupComponent::ClawDiedDelegate), EventData_Claw_Died::sk_EventType);
}

bool PowerupComponent::VInit(TiXmlElement* pData)
{
    return true;
}

void PowerupComponent::VPostInit()
{
    if (HasPowerup())
    {
        BroadcastPowerupStatusUpdated(m_pOwner->GetGUID(), m_ActivePowerup, false);
        BroadcastPowerupTimeUpdated(m_pOwner->GetGUID(), m_ActivePowerup, m_RemainingPowerupTime);
    }

    // Create powerup sparkles. Is this a good place ?
    m_PowerupSparkles.reserve(30);
    for (int i = 0; i < 30; i++)
    {
        StrongActorPtr pPowerupSparkle = ActorTemplates::CreatePowerupSparkleActor(75);
        assert(pPowerupSparkle);

        shared_ptr<PositionComponent> pPositionComponent = m_pOwner->GetPositionComponent();
        assert(pPositionComponent);

        shared_ptr<PhysicsComponent> pPhysicsComponent = m_pOwner->GetPhysicsComponent();
        assert(pPhysicsComponent);

        shared_ptr<PowerupSparkleAIComponent> pPowerupSparkleAIComponent =
            MakeStrongPtr(pPowerupSparkle->GetComponent<PowerupSparkleAIComponent>(PowerupSparkleAIComponent::g_Name));
        assert(pPowerupSparkleAIComponent);

        pPowerupSparkleAIComponent->SetTargetPositionComponent(pPositionComponent.get());
        pPowerupSparkleAIComponent->SetTargetSize(pPhysicsComponent->GetBodySize());

        m_PowerupSparkles.push_back(pPowerupSparkle);
    }
}

TiXmlElement* PowerupComponent::VGenerateXml()
{
    // TODO:
    return NULL;
}

void PowerupComponent::VUpdate(uint32 msDiff)
{
    if (HasPowerup())
    {
        int32 oldSecsRemaining = m_RemainingPowerupTime / 1000;
        m_RemainingPowerupTime -= msDiff;
        int32 currentSecsRemainig = m_RemainingPowerupTime / 1000;

        if (m_RemainingPowerupTime <= 0)
        {
            BroadcastPowerupStatusUpdated(m_pOwner->GetGUID(), m_ActivePowerup, true);
            m_ActivePowerup = PowerupType_None;
            m_RemainingPowerupTime = 0;

            SetPowerupSparklesVisibility(false);
        }
        else if (oldSecsRemaining != currentSecsRemainig)
        {
            BroadcastPowerupTimeUpdated(m_pOwner->GetGUID(), m_ActivePowerup, currentSecsRemainig);
        }
    }
}

void PowerupComponent::ApplyPowerup(PowerupType powerupType, int32 msDuration)
{
    if (msDuration > 0 && powerupType != PowerupType_None)
    {
        if (m_ActivePowerup != PowerupType_None)
        {
            // Clear internal state
            SetPowerupSparklesVisibility(false);
        }

        m_ActivePowerup = powerupType;
        m_RemainingPowerupTime = msDuration;
        int32 secondsRemaining = m_RemainingPowerupTime / 1000;

        BroadcastPowerupStatusUpdated(m_pOwner->GetGUID(), m_ActivePowerup, false);
        BroadcastPowerupTimeUpdated(m_pOwner->GetGUID(), m_ActivePowerup, secondsRemaining);

        if (m_ActivePowerup == PowerupType_Catnip)
        {
            SetPowerupSparklesVisibility(true);
        }
    }
}

void PowerupComponent::SetPowerupSparklesVisibility(bool visible)
{
    for (const auto &pSparkle : m_PowerupSparkles)
    {
        shared_ptr<ActorRenderComponent> pRenderComponent =
            MakeStrongPtr(pSparkle->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name));
        assert(pRenderComponent);

        pRenderComponent->SetVisible(visible);
    }
}

void PowerupComponent::BroadcastPowerupTimeUpdated(uint32 actorId, PowerupType powerupType, int32 secondsRemaining)
{
    shared_ptr<EventData_Updated_Powerup_Time> pEvent(new EventData_Updated_Powerup_Time(actorId, powerupType, secondsRemaining));
    IEventMgr::Get()->VTriggerEvent(pEvent);
}

void PowerupComponent::BroadcastPowerupStatusUpdated(uint32 actorId, PowerupType powerupType, bool isPowerupFinished)
{
    shared_ptr<EventData_Updated_Powerup_Status> pEvent(new EventData_Updated_Powerup_Status(actorId, powerupType, isPowerupFinished));
    IEventMgr::Get()->VTriggerEvent(pEvent);

    NotifyPowerupStatusUpdated(powerupType, isPowerupFinished);
}

void PowerupComponent::ClawDiedDelegate(IEventDataPtr pEventData)
{
    if (HasPowerup())
    {
        BroadcastPowerupStatusUpdated(m_pOwner->GetGUID(), m_ActivePowerup, true);

        m_RemainingPowerupTime = 0;
        SetPowerupSparklesVisibility(false);
        m_ActivePowerup = PowerupType_None;
    }
}

//=====================================================================================================================
// PowerupSubject implementation
//=====================================================================================================================

void PowerupSubject::NotifyPowerupStatusUpdated(PowerupType powerupType, bool isPowerupFinished)
{
    for (PowerupObserver* pObserver : m_Observers)
    {
        pObserver->VOnPowerupStatusChanged(powerupType, isPowerupFinished);
    }
}