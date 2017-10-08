#include "GabrielEncounter.h"

#include "../EnemyAIComponent.h"
#include "../../AnimationComponent.h"
#include "../../PhysicsComponent.h"
#include "../../PositionComponent.h"
#include "../../RenderComponent.h"

#include "../../../../GameApp/BaseGameLogic.h"
#include "../../../../GameApp/BaseGameApp.h"
#include "../../../../Physics/ClawPhysics.h"
#include "../../ControllableComponent.h"
#include "../../RenderComponent.h"
#include "../../FollowableComponent.h"

#include "../../../../Events/EventMgr.h"
#include "../../../../Events/Events.h"

const char* GabrielAIStateComponent::g_Name = "GabrielAIStateComponent";
const char* GabrielCannonButtonComponent::g_Name = "GabrielCannonButtonComponent";
const char* GabrielCannonComponent::g_Name = "GabrielCannonComponent";

//=====================================================================================================================
// GabrielAIStateComponent
//=====================================================================================================================

GabrielAIStateComponent::GabrielAIStateComponent()
    :
    BaseBossAIStateComponennt("Gabriel"),
    m_ThrowBombAnimFrameIdx(0),
    m_FireCannonAnimFrameIdx(0),
    m_TimeSinceLastAction(0),
    m_CurrActionDelay(0)
{

}

GabrielAIStateComponent::~GabrielAIStateComponent()
{

}

bool GabrielAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    if (!BaseBossAIStateComponennt::VDelegateInit(pData))
    {
        return false;
    }

    assert(ParseValueFromXmlElem(&m_IdleAnim, pData->FirstChildElement("IdleAnim")));
    assert(ParseValueFromXmlElem(&m_ThrowBombAnim, pData->FirstChildElement("ThrowBombAnim")));
    assert(ParseValueFromXmlElem(&m_FireCannonAnim, pData->FirstChildElement("FireCannonAnim")));
    assert(ParseValueFromXmlElem(&m_SpawnPirateAnim, pData->FirstChildElement("SpawnPirateAnim")));

    assert(ParseValueFromXmlElem(&m_ThrowBombAnimFrameIdx, pData->FirstChildElement("ThrowBombAnimFrameIdx")));
    assert(ParseValueFromXmlElem(&m_FireCannonAnimFrameIdx, pData->FirstChildElement("FireCannonAnimFrameIdx")));

    assert(ParseValueFromXmlElem(&m_NumThrownBombs, pData->FirstChildElement("NumThrownBombs")));
    assert(ParseValueFromXmlElem(&m_NumSpawnedPirates, pData->FirstChildElement("NumSpawnedPirates")));
    assert(ParseValueFromXmlElem(&m_BombDamage, pData->FirstChildElement("BombDamage")));

    assert(ParseValueFromXmlElem(&m_ActionMinInterval, pData->FirstChildElement("ActionMinInterval")));
    assert(ParseValueFromXmlElem(&m_ActionMaxInterval, pData->FirstChildElement("ActionMaxInterval")));

    for (TiXmlElement* pSoundElem = pData->FirstChildElement("ThrowBombSound");
        pSoundElem != NULL;
        pSoundElem = pSoundElem->NextSiblingElement("ThrowBombSound"))
    {
        m_ThrowBombSoundList.push_back(pSoundElem->GetText());
    }

    for (TiXmlElement* pSoundElem = pData->FirstChildElement("SummonPirateSound");
        pSoundElem != NULL;
        pSoundElem = pSoundElem->NextSiblingElement("SummonPirateSound"))
    {
        m_SpawnPirateSoundList.push_back(pSoundElem->GetText());
    }

    for (TiXmlElement* pSoundElem = pData->FirstChildElement("FireCannonSound");
        pSoundElem != NULL;
        pSoundElem = pSoundElem->NextSiblingElement("FireCannonSound"))
    {
        m_FireCannonSoundList.push_back(pSoundElem->GetText());
    }

    return true;
}

void GabrielAIStateComponent::VPostInit()
{
    BaseBossAIStateComponennt::VPostInit();

    m_pAnimationComponent = m_pOwner->GetRawComponent<AnimationComponent>(true);
    m_pAnimationComponent->AddObserver(this);

    m_pQuestionMark = m_pOwner->GetRawComponent<FollowableComponent>(true);

    m_CurrActionDelay = Util::GetRandomNumber(m_ActionMinInterval, m_ActionMaxInterval);
    
    // Populate his actions, should be data-driven but this is unique instance
    GabrielAction throwBombAction;
    throwBombAction.actionType = GabrielActionType_ThrowBombs;
    throwBombAction.probability = 40;
    throwBombAction.reuseTime = 2500;
    throwBombAction.timeSinceLastUse = 0;

    GabrielAction summonPirateAction;
    summonPirateAction.actionType = GabrielActionType_SummonPirate;
    summonPirateAction.probability = 30;
    summonPirateAction.reuseTime = 4000;
    summonPirateAction.timeSinceLastUse = 0;

    GabrielAction fireCannonAction;
    fireCannonAction.actionType = GabrielActionType_FireCannon;
    fireCannonAction.probability = 30;
    fireCannonAction.reuseTime = 3000;
    fireCannonAction.timeSinceLastUse = 0;

    m_GabrielActionList.push_back(throwBombAction);
    m_GabrielActionList.push_back(summonPirateAction);
    m_GabrielActionList.push_back(fireCannonAction);
}

void GabrielAIStateComponent::VOnWorldFinishedLoading()
{
    StrongActorPtr pGabrielCannonActor = g_pApp->GetGameLogic()->FindActorByName("Level8_GabrielCannon", true);
    assert(pGabrielCannonActor != nullptr);

    m_pGabrielCannonComponent = pGabrielCannonActor->GetRawComponent<GabrielCannonComponent>(true);
}

void GabrielAIStateComponent::VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState)
{
    m_pAnimationComponent->SetAnimation(m_IdleAnim);

    m_IsActive = true;
}

void GabrielAIStateComponent::VOnStateLeave(BaseEnemyAIStateComponent* pNextState)
{
    m_IsActive = false;
}

void GabrielAIStateComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    if (!m_IsActive || !m_bBossFightStarted)
    {
        return;
    }
}

void GabrielAIStateComponent::VOnAnimationLooped(Animation* pAnimation)
{
    if (!m_IsActive || !m_bBossFightStarted)
    {
        return;
    }

    m_pAnimationComponent->SetAnimation(m_IdleAnim);
}

void GabrielAIStateComponent::VUpdate(uint32 msDiff)
{
    static const Point pirateSpawnPos(42666, 5130);

    if (!m_IsActive || !m_bBossFightStarted)
    {
        return;
    }

    m_TimeSinceLastAction += msDiff;
    if (m_TimeSinceLastAction > m_CurrActionDelay)
    {
        GabrielAction action = Util::GetRandomValueFromVector(m_GabrielActionList);
        std::string playedSound;
        switch (action.actionType)
        {
            case GabrielActionType_ThrowBombs:
                m_pAnimationComponent->SetAnimation(m_ThrowBombAnim);
                playedSound = Util::PlayRandomSoundFromList(m_ThrowBombSoundList);
                LOG("Should Throw Bomb(s)");
                break;

            case GabrielActionType_SummonPirate:
                m_pAnimationComponent->SetAnimation(m_SpawnPirateAnim);
                playedSound = Util::PlayRandomSoundFromList(m_SpawnPirateSoundList);

                
                ActorTemplates::CreateActor(ActorPrototype_Level8_GabrielPirate, pirateSpawnPos);

                LOG("Should Summon Pirate(s)");
                break;

            case GabrielActionType_FireCannon:
                // !!!! Recursion !!! - alternative was goto
                if (!m_pGabrielCannonComponent->IsReadyToFire())
                {
                    VUpdate(0);
                }

                m_pAnimationComponent->SetAnimation(m_FireCannonAnim);
                playedSound = Util::PlayRandomSoundFromList(m_FireCannonSoundList);
                LOG("Should Fire Cannon");

                m_pGabrielCannonComponent->Fire();
                break;

            default:
                LOG_ASSERT("Unknown Gabriel Action");
                break;
        }

        if (!playedSound.empty())
        {
            m_pQuestionMark->Activate(Util::GetSoundDurationMs(playedSound));
        }

        // Reset the timer
        m_CurrActionDelay = Util::GetRandomNumber(m_ActionMinInterval, m_ActionMaxInterval);
        m_TimeSinceLastAction = 0;
    }
}

void GabrielAIStateComponent::VOnBossFightStarted()
{
    BaseBossAIStateComponennt::VOnBossFightStarted();

    // To refresh contact list after Claw's death
    g_pApp->GetGameLogic()->VGetGamePhysics()->VActivate(m_pOwner->GetGUID());

    m_pEnemyAIComponent->EnterBestState(true);
}

void GabrielAIStateComponent::VOnBossFightEnded(bool isBossDead)
{
    BaseBossAIStateComponennt::VOnBossFightEnded(isBossDead);

    if (isBossDead)
    {
        SoundInfo soundInfo(SOUND_GAME_AMULET_RISE);
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));
    }
    else
    {
        // To refresh contact list after Claw's death
        g_pApp->GetGameLogic()->VGetGamePhysics()->VDeactivate(m_pOwner->GetGUID());

        m_TimeSinceLastAction = 0;
        m_CurrActionDelay = Util::GetRandomNumber(m_ActionMinInterval, m_ActionMaxInterval);
    }
}

void GabrielAIStateComponent::VOnHealthChanged(int32 oldHealth, int32 newHealth, DamageType damageType, Point impactPoint, int sourceActorId)
{
    BaseBossAIStateComponennt::VOnHealthChanged(oldHealth, newHealth, damageType, impactPoint, sourceActorId);
}

//=====================================================================================================================
// GabrielCannonButtonComponent
//=====================================================================================================================

GabrielCannonButtonComponent::GabrielCannonButtonComponent()
{

}

GabrielCannonButtonComponent::~GabrielCannonButtonComponent()
{

}

bool GabrielCannonButtonComponent::VInit(TiXmlElement* pData)
{
    assert(ParseValueFromXmlElem(&m_BlinkingAnim, pData->FirstChildElement("BlinkingAnim")));
    assert(ParseValueFromXmlElem(&m_PressedImageName, pData->FirstChildElement("PressedImageName")));
    assert(ParseValueFromXmlElem(&m_ButtonPressSound, pData->FirstChildElement("ButtonPressSound")));

    return true;
}

void GabrielCannonButtonComponent::VPostInit()
{
    m_pRenderComponent = m_pOwner->GetRawComponent<ActorRenderComponent>(true);
    m_pAnimationComponent = m_pOwner->GetRawComponent<AnimationComponent>(true);
    assert(m_pAnimationComponent->SetAnimation(m_BlinkingAnim));

    m_pOwner->GetRawComponent<TriggerComponent>(true)->AddObserver(this);
}

void GabrielCannonButtonComponent::VUpdate(uint32 msDiff)
{
    /*static int delay = 2000;

    delay -= msDiff;
    if (delay < 0)
    {
        if (m_pGabrielCannonComponent->IsReadyToFire())
        {
            m_pGabrielCannonComponent->Fire();
            delay = 5000;
            Reset();
        }
    }*/
}

void GabrielCannonButtonComponent::VOnWorldFinishedLoading()
{
    StrongActorPtr pGabrielCannonActor = g_pApp->GetGameLogic()->FindActorByName("Level8_GabrielCannon", true);
    assert(pGabrielCannonActor != nullptr);

    m_pGabrielCannonComponent = pGabrielCannonActor->GetRawComponent<GabrielCannonComponent>(true);
}

void GabrielCannonButtonComponent::VOnActorEnteredTrigger(Actor* pActorWhoEntered, FixtureType triggerType)
{
    if (triggerType != FixtureType_Trigger_GabrielButton)
    {
        return;
    }

    // Switch is red
    if (m_pAnimationComponent->GetCurrentAnimation()->IsPaused())
    {
        return;
    }

    m_pGabrielCannonComponent->Rise();
    m_pAnimationComponent->PauseAnimation();
    m_pRenderComponent->SetImage(m_PressedImageName);
    Util::PlaySimpleSound(m_ButtonPressSound);
}

void GabrielCannonButtonComponent::Reset()
{
    if (m_pAnimationComponent->GetCurrentAnimation()->IsPaused())
    {
        m_pAnimationComponent->ResetAnimation();
    }
}

//=====================================================================================================================
// GabrielCannonComponent
//=====================================================================================================================

GabrielCannonComponent::GabrielCannonComponent()
    :
    m_HorizontalFireAnimIdx(0),
    m_VerticalFireAnimIdx(0),
    m_bQueuedRise(false),
    m_bActive(false)
{
    IEventMgr::Get()->VAddListener(MakeDelegate(this, &GabrielCannonComponent::BossFightStartedDelegate), EventData_Boss_Fight_Started::sk_EventType);
    IEventMgr::Get()->VAddListener(MakeDelegate(this, &GabrielCannonComponent::BossFightEndedDelegate), EventData_Boss_Fight_Ended::sk_EventType);
}

GabrielCannonComponent::~GabrielCannonComponent()
{
    IEventMgr::Get()->VRemoveListener(MakeDelegate(this, &GabrielCannonComponent::BossFightStartedDelegate), EventData_Boss_Fight_Started::sk_EventType);
    IEventMgr::Get()->VRemoveListener(MakeDelegate(this, &GabrielCannonComponent::BossFightEndedDelegate), EventData_Boss_Fight_Ended::sk_EventType);
}

bool GabrielCannonComponent::VInit(TiXmlElement* pData)
{
    assert(ParseValueFromXmlElem(&m_HorizontalFireAnim, pData->FirstChildElement("HorizontalFireAnim")));
    assert(ParseValueFromXmlElem(&m_VerticalFireAnim, pData->FirstChildElement("VerticalFireAnim")));
    assert(ParseValueFromXmlElem(&m_ToVerticalAnim, pData->FirstChildElement("ToVerticalAnim")));
    assert(ParseValueFromXmlElem(&m_ToHorizontalAnim, pData->FirstChildElement("ToHorizontalAnim")));
    assert(ParseValueFromXmlElem(&m_IdleAnim, pData->FirstChildElement("IdleAnim")));

    assert(ParseValueFromXmlElem(&m_CannonRiseSound, pData->FirstChildElement("CannonRiseSound")));
    assert(ParseValueFromXmlElem(&m_CannonDropSound, pData->FirstChildElement("CannonDropSound")));

    assert(ParseValueFromXmlElem(&m_HorizontalFireAnimIdx, pData->FirstChildElement("HorizontalFireAnimIdx")));
    assert(ParseValueFromXmlElem(&m_VerticalFireAnimIdx, pData->FirstChildElement("VerticalFireAnimIdx")));

    return true;
}

void GabrielCannonComponent::VPostInit()
{
    m_pAnimationComponent = m_pOwner->GetRawComponent<AnimationComponent>(true);
    m_pAnimationComponent->AddObserver(this);
    m_pAnimationComponent->SetAnimation(m_IdleAnim);
}

void GabrielCannonComponent::VUpdate(uint32 msDiff)
{
    if (!m_bActive)
    {
        return;
    }
}

void GabrielCannonComponent::VOnWorldFinishedLoading()
{
    StrongActorPtr pGabrielCannonButtonActor = g_pApp->GetGameLogic()->FindActorByName("Level8_GabrielButton", true);
    assert(pGabrielCannonButtonActor != nullptr);

    m_pCannonButton = pGabrielCannonButtonActor->GetRawComponent<GabrielCannonButtonComponent>(true);
}

void GabrielCannonComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    if (pAnimation->GetName() == m_VerticalFireAnim && pNewFrame->idx == m_VerticalFireAnimIdx)
    {
        m_pCannonButton->Reset();
        LOG("Fire Vertical !")
    }
    else if (pAnimation->GetName() == m_HorizontalFireAnim && pNewFrame->idx == m_HorizontalFireAnimIdx)
    {
        m_pCannonButton->Reset();
        LOG("Fire Horizontal !")
    }
}

void GabrielCannonComponent::VOnAnimationLooped(Animation* pAnimation)
{
    if (pAnimation->GetName() == m_ToVerticalAnim)
    {
        m_pAnimationComponent->SetAnimation(m_VerticalFireAnim);
    }
    else if (pAnimation->GetName() == m_VerticalFireAnim)
    {
        m_pAnimationComponent->SetAnimation(m_ToHorizontalAnim);
        Util::PlaySimpleSound(m_CannonDropSound);
    }
    else if (pAnimation->GetName() == m_ToHorizontalAnim)
    {
        m_pAnimationComponent->SetAnimation(m_IdleAnim);
    }
    else if (pAnimation->GetName() == m_HorizontalFireAnim)
    {
        m_pAnimationComponent->SetAnimation(m_IdleAnim);
    }
}

void GabrielCannonComponent::VOnAnimationChanged(Animation* pOldAnimation, Animation* pNewAnimation)
{
    if (pNewAnimation->GetName() == m_IdleAnim && m_bQueuedRise)
    {
        m_pAnimationComponent->SetAnimation(m_ToVerticalAnim);
        Util::PlaySimpleSound(m_CannonRiseSound);
        m_bQueuedRise = false;
    }
}

void GabrielCannonComponent::VOnAnimationAtLastFrame(Animation* pAnimation)
{
    if (pAnimation->GetName() == m_ToVerticalAnim)
    {
        pAnimation->Pause();
    }
}

void GabrielCannonComponent::Fire()
{
    std::string currAnimName = m_pAnimationComponent->GetCurrentAnimationName();
    /*if (currAnimName != m_IdleAnim)
    {
        return;
    }

    m_pAnimationComponent->SetAnimation(m_HorizontalFireAnim);*/

    if (currAnimName == m_IdleAnim)
    {
        m_pAnimationComponent->SetAnimation(m_HorizontalFireAnim);
    }
    else if (currAnimName == m_ToVerticalAnim)
    {
        m_pAnimationComponent->SetAnimation(m_VerticalFireAnim);
    }
}

void GabrielCannonComponent::Rise()
{
    if (m_pAnimationComponent->GetCurrentAnimationName() == m_IdleAnim)
    {
        m_pAnimationComponent->SetAnimation(m_ToVerticalAnim);
        Util::PlaySimpleSound(m_CannonRiseSound);
    }
    else
    {
        m_bQueuedRise = true;
    }
}

void GabrielCannonComponent::BossFightStartedDelegate(IEventDataPtr pEvent)
{
    m_bActive = true;
    LOG_TRACE("Started !");
}

void GabrielCannonComponent::BossFightEndedDelegate(IEventDataPtr pEvent)
{
    m_bActive = false;
    m_pAnimationComponent->SetAnimation(m_IdleAnim);
    m_bQueuedRise = false;
}

bool GabrielCannonComponent::IsReadyToFire()
{
    return (
        (m_pAnimationComponent->GetCurrentAnimationName() == m_IdleAnim) ||
        (m_pAnimationComponent->GetCurrentAnimationName() == m_ToVerticalAnim && 
        m_pAnimationComponent->GetCurrentAnimation()->IsPaused())
        );
}