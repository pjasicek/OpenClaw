#include "ControllableComponent.h"
#include "../../Events/Events.h"
#include "../../Events/EventMgr.h"
#include "AnimationComponent.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "PositionComponent.h"
#include "../ActorTemplates.h"
#include "ControllerComponents/AmmoComponent.h"
#include "ControllerComponents/PowerupComponent.h"
#include "ControllerComponents/HealthComponent.h"
#include "ControllerComponents/LifeComponent.h"
#include "FollowableComponent.h"
#include "EnemyAI/EnemyAIComponent.h"
#include "../../Graphics2D/Image.h"

#include "../../GameApp/BaseGameApp.h"
#include "../../GameApp/BaseGameLogic.h"
#include "../../UserInterface/HumanView.h"
#include "../../Scene/SceneNodes.h"

#include "../../Util/ClawLevelUtil.h"

const char* ControllableComponent::g_Name = "ControllableComponent";
const char* ClawControllableComponent::g_Name = "ClawControllableComponent";

ControllableComponent::ControllableComponent()
    :
    m_Active(false),
    m_DuckingTime(0),
    m_LookingUpTime(0),
    m_FrozenTime(0),
    m_bFrozen(false),
    m_MaxJumpHeight(0)
{ }

bool ControllableComponent::VInit(TiXmlElement* data)
{
    assert(data != NULL);

    if (TiXmlElement* isActiveElement = data->FirstChildElement("IsActive"))
    {
        m_Active = std::string(isActiveElement->GetText()) == "true";
    }
    else
    {
        m_Active = false;
    }

    return VInitDelegate(data);
}

void ControllableComponent::VPostInit()
{
    shared_ptr<PhysicsComponent> pPhysicsComponent = m_pOwner->GetPhysicsComponent();
    if (pPhysicsComponent)
    {
        pPhysicsComponent->SetControllableComponent(this);
    }

    if (m_Active)
    {
        shared_ptr<EventData_Attach_Actor> pEvent(new EventData_Attach_Actor(m_pOwner->GetGUID()));
        IEventMgr::Get()->VTriggerEvent(pEvent);
    }
}

TiXmlElement* ControllableComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    //

    return baseElement;
}

//=====================================================================================================================

ClawControllableComponent::ClawControllableComponent()
{
    m_pClawAnimationComponent = NULL;
    m_pRenderComponent = NULL;
    m_State = ClawState_None;
    m_LastState = ClawState_None;
    m_Direction = Direction_Right;
    m_IdleTime = 0;
    m_TakeDamageDuration = 500; // TODO: Data drive
    m_TakeDamageTimeLeftMs = 0;
    m_bIsInBossFight = false;
    m_ThrowingTime = 0;
    m_CarriedActorId = INVALID_ACTOR_ID;
    m_bIsCarryingEnemy = false;
    m_bIsLifting = false;
    m_bIsThrowing = false;
    m_bIsChargingThrow = false;
    m_ThrowChargeTime = 0;

    IEventMgr::Get()->VAddListener(MakeDelegate(this, &ClawControllableComponent::BossFightStartedDelegate), EventData_Boss_Fight_Started::sk_EventType);
    IEventMgr::Get()->VAddListener(MakeDelegate(this, &ClawControllableComponent::BossFightEndedDelegate), EventData_Boss_Fight_Ended::sk_EventType);
}

ClawControllableComponent::~ClawControllableComponent()
{
    IEventMgr::Get()->VRemoveListener(MakeDelegate(this, &ClawControllableComponent::BossFightStartedDelegate), EventData_Boss_Fight_Started::sk_EventType);
    IEventMgr::Get()->VRemoveListener(MakeDelegate(this, &ClawControllableComponent::BossFightEndedDelegate), EventData_Boss_Fight_Ended::sk_EventType);
}

bool ClawControllableComponent::VInitDelegate(TiXmlElement* data)
{
    return true;
}

void ClawControllableComponent::VPostInit()
{
    ControllableComponent::VPostInit();

    m_pRenderComponent = MakeStrongPtr(m_pOwner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name)).get();
    m_pClawAnimationComponent = MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>(AnimationComponent::g_Name)).get();
    m_pPositionComponent = m_pOwner->GetPositionComponent().get();
    m_pAmmoComponent = MakeStrongPtr(m_pOwner->GetComponent<AmmoComponent>(AmmoComponent::g_Name)).get();
    m_pPowerupComponent = MakeStrongPtr(m_pOwner->GetComponent<PowerupComponent>(PowerupComponent::g_Name)).get();
    m_pHealthComponent = MakeStrongPtr(m_pOwner->GetComponent<HealthComponent>(HealthComponent::g_Name)).get();
    m_pExclamationMark = MakeStrongPtr(m_pOwner->GetComponent<FollowableComponent>()).get();
    assert(m_pClawAnimationComponent);
    assert(m_pRenderComponent);
    assert(m_pPositionComponent);
    assert(m_pAmmoComponent);
    assert(m_pPowerupComponent);
    assert(m_pHealthComponent);
    assert(m_pExclamationMark);
    m_pClawAnimationComponent->AddObserver(this);

    auto pHealthComponent = MakeStrongPtr(m_pOwner->GetComponent<HealthComponent>(HealthComponent::g_Name));
    pHealthComponent->AddObserver(this);

    m_pPhysicsComponent = m_pOwner->GetPhysicsComponent().get();

    // Sounds that play when claw takes some damage
    m_TakeDamageSoundList.push_back(SOUND_CLAW_TAKE_DAMAGE1);
    m_TakeDamageSoundList.push_back(SOUND_CLAW_TAKE_DAMAGE2);
    m_TakeDamageSoundList.push_back(SOUND_CLAW_TAKE_DAMAGE3);
    m_TakeDamageSoundList.push_back(SOUND_CLAW_TAKE_DAMAGE4);

    // Sounds that play when claw is idle for some time
    m_IdleQuoteSoundList.push_back(SOUND_CLAW_IDLE1);
    //m_IdleQuoteSoundList.push_back(SOUND_CLAW_IDLE2);
    m_IdleQuoteSoundList.push_back(SOUND_CLAW_IDLE3);
    m_IdleQuoteSoundList.push_back(SOUND_CLAW_IDLE4);
    m_IdleQuoteSoundList.push_back(SOUND_CLAW_IDLE5);
    m_IdleQuoteSoundList.push_back(SOUND_CLAW_IDLE6);
    m_IdleQuoteSoundList.push_back(SOUND_CLAW_IDLE7);
    m_IdleQuoteSoundList.push_back(SOUND_CLAW_IDLE8);
    m_IdleQuoteSoundList.push_back(SOUND_CLAW_IDLE9);
    m_IdleQuoteSoundList.push_back(SOUND_CLAW_IDLE10);
    m_IdleQuoteSoundList.push_back(SOUND_CLAW_IDLE11);
    m_IdleQuoteSoundList.push_back(SOUND_CLAW_IDLE12);

    m_pIdleQuotesSequence.reset(new PrimeSearch(m_IdleQuoteSoundList.size()));

    // No existing animation for the top-ladder climb...
    {
        std::vector<AnimationFrame> climbAnimFrames;
        climbAnimFrames.reserve(389 - 383 + 1);
        for (int i = 0, climbImageId = 383; climbImageId <= 389; climbImageId++, i++)
        {
            AnimationFrame frame;
            frame.idx = i;
            frame.imageId = climbImageId;
            frame.imageName = "frame" + ToStr(climbImageId);
            frame.duration = 55;
            climbAnimFrames.push_back(frame);
        }
        std::shared_ptr<Animation> pClimbAnim = Animation::CreateAnimation(climbAnimFrames, "topclimb", m_pClawAnimationComponent);
        assert(pClimbAnim);
        DO_AND_CHECK(m_pClawAnimationComponent->AddAnimation("topclimb", pClimbAnim));
    }

    {
        std::vector<AnimationFrame> climbAnimFrames;
        climbAnimFrames.reserve(389 - 383 + 1);
        for (int i = 0, climbImageId = 389; climbImageId >= 383; climbImageId--, i++)
        {
            AnimationFrame frame;
            frame.idx = i;
            frame.imageId = climbImageId;
            frame.imageName = "frame" + ToStr(climbImageId);
            frame.duration = 55;
            climbAnimFrames.push_back(frame);
        }
        std::shared_ptr<Animation> pClimbAnim = Animation::CreateAnimation(climbAnimFrames, "topclimbdown", m_pClawAnimationComponent);
        assert(pClimbAnim);
        DO_AND_CHECK(m_pClawAnimationComponent->AddAnimation("topclimbdown", pClimbAnim));
    }

    {
        AnimationFrame frame;
        frame.idx = 0;
        frame.imageId = 100;
        frame.imageName = "frame100";
        frame.duration = 500;
        std::vector<AnimationFrame> freezeAnimFrames = {frame};

        std::shared_ptr<Animation> pFreezeAnim = Animation::CreateAnimation(freezeAnimFrames, "freeze", m_pClawAnimationComponent);
        assert(pFreezeAnim);
        DO_AND_CHECK(m_pClawAnimationComponent->AddAnimation("freeze", pFreezeAnim));
    }

    {
        AnimationFrame frame;
        frame.idx = 0;
        frame.imageId = 401;
        frame.imageName = "frame401";
        frame.duration = 2000;
        std::vector<AnimationFrame> highFallAnimFrames = {frame};

        std::shared_ptr<Animation> pHighFallAnim = Animation::CreateAnimation(highFallAnimFrames, "highfall", m_pClawAnimationComponent);
        assert(pHighFallAnim);
        DO_AND_CHECK(m_pClawAnimationComponent->AddAnimation("highfall", pHighFallAnim));
    }

    // No existing animation for leaning back while charging the throw of carried keg / enemy.
    // Frame 44 is not used by any of Claw's animations, it is the pose between holding (43) and throwing (45)
    {
        AnimationFrame frame;
        frame.idx = 0;
        frame.imageId = 44;
        frame.imageName = "frame044";
        frame.duration = 500;
        std::vector<AnimationFrame> liftChargeAnimFrames = {frame};

        std::shared_ptr<Animation> pLiftChargeAnim = Animation::CreateAnimation(liftChargeAnimFrames, "liftcharge", m_pClawAnimationComponent);
        assert(pLiftChargeAnim);
        DO_AND_CHECK(m_pClawAnimationComponent->AddAnimation("liftcharge", pLiftChargeAnim));
    }
}

void ClawControllableComponent::VPostPostInit()
{
    // So that Claw can attach to rope
    ActorFixtureDef fixtureDef;
    fixtureDef.collisionShape = "Rectangle";
    fixtureDef.fixtureType = FixtureType_RopeSensor;
    fixtureDef.size = Point(25, 25);
    fixtureDef.collisionMask = CollisionFlag_Rope;
    fixtureDef.collisionFlag = CollisionFlag_RopeSensor;
    fixtureDef.isSensor = true;

    g_pApp->GetGameLogic()->VGetGamePhysics()->VAddActorFixtureToBody(m_pOwner->GetGUID(), &fixtureDef);
}

void ClawControllableComponent::VUpdate(uint32 msDiff)
{
    if (m_State == ClawState_Standing ||
        m_State == ClawState_Idle)
    {
        m_IdleTime += msDiff;
        if (m_IdleTime > g_pApp->GetGlobalOptions()->idleSoundQuoteIntervalMs)
        {
            // This is to prevent the same sound to play multiple times in a row
            int idleQuoteSoundIdx = m_pIdleQuotesSequence->GetNext();
            if (idleQuoteSoundIdx == -1)
            {
                idleQuoteSoundIdx = m_pIdleQuotesSequence->GetNext(true);
            }

            SoundInfo soundInfo(m_IdleQuoteSoundList[idleQuoteSoundIdx]);
            IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
                new EventData_Request_Play_Sound(soundInfo)));

            m_pExclamationMark->Activate(2000);

            SetClawAnimation("idle");

            m_State = ClawState_Idle;
            m_IdleTime = 0;
        }
    }
    else
    {
        m_IdleTime = 0;
    }

    // Check if invulnerability caused by previously taking damage is gone
    if (m_TakeDamageTimeLeftMs > 0)
    {
        m_TakeDamageTimeLeftMs -= msDiff;
        if (m_TakeDamageTimeLeftMs <= 0)
        {
            if (!m_pPowerupComponent->HasPowerup(PowerupType_Invulnerability))
            {
                m_pHealthComponent->SetInvulnerable(false);
            }
            m_TakeDamageTimeLeftMs = 0;
        }
    }

    assert(g_pApp->GetHumanView() && g_pApp->GetHumanView()->GetCamera());
    shared_ptr<CameraNode> pCamera = g_pApp->GetHumanView()->GetCamera();

    if (m_DuckingTime > g_pApp->GetGlobalOptions()->startLookUpOrDownTime)
    {
        if (pCamera->GetCameraOffsetY() < g_pApp->GetGlobalOptions()->maxLookUpOrDownDistance)
        {
            // Pixels per milisecond
            double cameraOffsetSpeed = g_pApp->GetGlobalOptions()->lookUpOrDownSpeed / 1000.0;
            pCamera->AddCameraOffsetY(cameraOffsetSpeed * msDiff);
        }
    }
    else if (m_LookingUpTime > g_pApp->GetGlobalOptions()->startLookUpOrDownTime)
    {
        if (pCamera->GetCameraOffsetY() > -1.0 * g_pApp->GetGlobalOptions()->maxLookUpOrDownDistance)
        {
            // Pixels per milisecond
            double cameraOffsetSpeed = -1.0 * (g_pApp->GetGlobalOptions()->lookUpOrDownSpeed / 1000.0);
            pCamera->AddCameraOffsetY(cameraOffsetSpeed * msDiff);
        }
        SetClawAnimation("lookup");
    }
    else
    {
        if (m_State != ClawState_Ducking)
        {
            m_DuckingTime = 0;
        }
        if (m_State != ClawState_Standing)
        {
            m_LookingUpTime = 0;
        }

        if (fabs(pCamera->GetCameraOffsetY()) > DBL_EPSILON)
        {
            double cameraOffsetSpeed = ((double)g_pApp->GetGlobalOptions()->lookUpOrDownSpeed * 2.0) / 1000.0;
            if (pCamera->GetCameraOffsetY() > DBL_EPSILON)
            {
                // Camera should move back up
                double cameraMovePx = -1.0 * cameraOffsetSpeed * msDiff;
                pCamera->AddCameraOffsetY(cameraMovePx);
                if (pCamera->GetCameraOffsetY() < DBL_EPSILON)
                {
                    pCamera->SetCameraOffsetY(0.0);
                }
            }
            else if (pCamera->GetCameraOffsetY() < (-1.0 * DBL_EPSILON))
            {
                // Camera should move back down
                double cameraMovePx = cameraOffsetSpeed * msDiff;
                pCamera->AddCameraOffsetY(cameraMovePx);
                if (pCamera->GetCameraOffsetY() > DBL_EPSILON)
                {
                    pCamera->SetCameraOffsetY(0.0);
                }
            }
        }
    }

    if (m_bFrozen)
    {
        SetClawAnimation("stand");
        m_IdleTime = 0;
        m_LookingUpTime = 0;
        m_DuckingTime = 0;
    }

    if (m_State == ClawState_HoldingRope)
    {
        m_IdleTime = 0;
        m_LookingUpTime = 0;
        m_DuckingTime = 0;
    }

    if (m_pPhysicsComponent->IsFalling() && 
        m_pClawAnimationComponent->GetCurrentAnimationName() == "fall" &&
        m_pPhysicsComponent->GetFallHeight() > g_pApp->GetGlobalOptions()->clawMinFallHeight)
    {
        SetClawAnimation("highfall");
    }

    if (m_pClawAnimationComponent->GetCurrentAnimationName().find("predynamite") != std::string::npos ||
        m_pClawAnimationComponent->GetCurrentAnimationName().find("postdynamite") != std::string::npos)
    {
        if (m_pClawAnimationComponent->GetCurrentAnimationName().find("predynamite") != std::string::npos)
        {
            m_ThrowingTime += msDiff;
            if (m_ThrowingTime >= 2000)
            {
                if (IsDucking())
                {
                    m_pClawAnimationComponent->SetAnimation("duckpostdynamite");
                }
                else
                {
                    m_pClawAnimationComponent->SetAnimation("postdynamite");
                }
            }
        }
    }
    else
    {
        m_ThrowingTime = 0;
    }

    if (m_bIsChargingThrow)
    {
        m_ThrowChargeTime += msDiff;
    }

    UpdateCarriedActorPosition();

    //LOG("State: " + ToStr((int)m_State));

    m_LastState = m_State;
}

// Interface for subclasses
void ClawControllableComponent::VOnStartFalling()
{
    if (m_State == ClawState_JumpShooting ||
        m_State == ClawState_JumpAttacking)
    {
        return;
    }

    SetClawAnimation("fall");
    m_State = ClawState_Falling;
}

void ClawControllableComponent::VOnLandOnGround(float fromHeight)
{
    if (fromHeight > g_pApp->GetGlobalOptions()->clawMinFallHeight)
    {
        SetClawAnimation("land");
    }
    else
    {
        SetClawAnimation("stand");

        SoundInfo soundInfo(SOUND_CLAW_LAND_SHORT);
        soundInfo.soundVolume = 150;
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));
    }

    m_State = ClawState_Standing;
}

void ClawControllableComponent::VOnStartJumping()
{
    if (m_State == ClawState_JumpShooting ||
        m_State == ClawState_JumpAttacking ||
        m_bFrozen)
    {
        return;
    }

    SetClawAnimation("jump");
    m_State = ClawState_Jumping;
}

bool ClawControllableComponent::VOnDirectionChange(Direction direction)
{
    if (m_bFrozen/* || (IsAttackingOrShooting() && fabs(m_pPhysicsComponent->GetVelocity().x) < DBL_EPSILON)*/)
    {
        return false;
    }

    m_pRenderComponent->SetMirrored(direction == Direction_Left);
    m_Direction = direction;

    return true;
}

void ClawControllableComponent::VOnStopMoving()
{
    if (m_State == ClawState_Shooting ||
        m_State == ClawState_Idle ||
        IsDucking())
    {
        return;
    }

    if (m_pClawAnimationComponent->GetCurrentAnimationName() != "land")
    {
        SetClawAnimation("stand");
    }
    
    m_State = ClawState_Standing;
}

void ClawControllableComponent::VOnRun()
{
    if (m_State == ClawState_Shooting ||
        m_pClawAnimationComponent->GetCurrentAnimationName() == "land")
    {
        return;
    }
    SetClawAnimation("walk");
    m_LookingUpTime = 0;
    m_DuckingTime = 0;
    m_State = ClawState_Walking;
}

void ClawControllableComponent::VOnClimb(bool bIsClimbingUp, bool bIsOnTopEdge)
{
    DropCarried();

    // TODO: Decide whether to keep it
    static bool useAlternativeClimbAnim = true;
    if (bIsOnTopEdge && useAlternativeClimbAnim)
    {
        if (!bIsClimbingUp)
        {
            m_pClawAnimationComponent->SetAnimation("topclimbdown");
        }
        else
        {
            m_pClawAnimationComponent->SetAnimation("topclimb");
        }
    }
    else
    {
        if (!bIsClimbingUp)
        {
            m_pClawAnimationComponent->SetAnimation("climbdown");
        }
        else
        {
            m_pClawAnimationComponent->SetAnimation("climb");
        }
    }
    
    m_pClawAnimationComponent->ResumeAnimation();
    m_State = ClawState_Climbing;
}

void ClawControllableComponent::VOnStopClimbing()
{
    m_pClawAnimationComponent->PauseAnimation();
}

void ClawControllableComponent::OnAttack()
{
    if (IsCarrying() || m_bIsLifting || m_bIsThrowing)
    {
        OnQuickThrow();
        return;
    }

    if (IsAttackingOrShooting() ||
        m_State == ClawState_Climbing ||
        m_State == ClawState_TakingDamage ||
        m_State == ClawState_Frozen ||
        m_State == ClawState_Dying ||
        m_State == ClawState_HoldingRope ||
        m_bFrozen)
    {
        return;
    }

    if (m_State == ClawState_Falling ||
        m_State == ClawState_Jumping)
    {
        m_pClawAnimationComponent->SetAnimation("jumpswipe");
        m_State = ClawState_JumpAttacking;
    }
    else if (IsDucking())
    {
        m_pClawAnimationComponent->SetAnimation("duckswipe");
        m_State = ClawState_DuckAttacking;
    }
    else
    {
        // Fire/Ice/Lightning sword powerups have always swipe anim
        if (m_pPowerupComponent->HasPowerup(PowerupType_FireSword) ||
            m_pPowerupComponent->HasPowerup(PowerupType_FrostSword) ||
            m_pPowerupComponent->HasPowerup(PowerupType_LightningSword))
        {
            m_pClawAnimationComponent->SetAnimation("swipe");
        }
        else
        {
            int attackType = rand() % 5;
            if (attackType == 0)
            {
                m_pClawAnimationComponent->SetAnimation("kick");
            }
            else if (attackType == 1)
            {
                m_pClawAnimationComponent->SetAnimation("uppercut");
            }
            else
            {
                m_pClawAnimationComponent->SetAnimation("swipe");
            }
        }
        
        m_State = ClawState_Attacking;
    }

    // If its one of the magic swords, play its corresponding sound

    if (m_pPowerupComponent->HasPowerup(PowerupType_FireSword))
    {
        SoundInfo soundInfo(SOUND_CLAW_FIRE_SWORD);
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));
    }
    else if (m_pPowerupComponent->HasPowerup(PowerupType_FrostSword))
    {
        SoundInfo soundInfo(SOUND_CLAW_FROST_SWORD);
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));
    }
    else if (m_pPowerupComponent->HasPowerup(PowerupType_LightningSword))
    {
        SoundInfo soundInfo(SOUND_CLAW_LIGHTNING_SWORD);
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));
    }
}

void ClawControllableComponent::OnFire(bool outOfAmmo)
{
    if (IsCarrying() || m_bIsLifting || m_bIsThrowing)
    {
        OnQuickThrow();
        return;
    }

    if (IsAttackingOrShooting() ||
        m_State == ClawState_Climbing ||
        m_State == ClawState_Dying || 
        m_State == ClawState_TakingDamage ||
        m_State == ClawState_Frozen ||
        m_State == ClawState_HoldingRope ||
        m_bFrozen)
    {
        return;
    }

    AmmoType activeAmmoType = m_pAmmoComponent->GetActiveAmmoType();
    if (m_State == ClawState_Falling ||
        m_State == ClawState_Jumping)
    {
        if (activeAmmoType == AmmoType_Pistol)
        {
            if (m_pAmmoComponent->CanFire())
            {
                m_pClawAnimationComponent->SetAnimation("jumppistol");
            }
            else
            {
                m_pClawAnimationComponent->SetAnimation("emptyjumppistol");
            }
        }
        else if (activeAmmoType == AmmoType_Magic)
        {
            if (m_pAmmoComponent->CanFire())
            {
                m_pClawAnimationComponent->SetAnimation("jumpmagic");
            }
            else
            {
                m_pClawAnimationComponent->SetAnimation("emptyjumpmagic");
            }
        }
        else if (activeAmmoType == AmmoType_Dynamite)
        {
            if (m_pAmmoComponent->CanFire())
            {
                m_pClawAnimationComponent->SetAnimation("jumpdynamite");
            }
            else
            {
                m_pClawAnimationComponent->SetAnimation("emptyjumpdynamite");
            }
        }
        m_State = ClawState_JumpShooting;
    }
    else if (IsDucking())
    {
        if (activeAmmoType == AmmoType_Pistol)
        {
            if (m_pAmmoComponent->CanFire())
            {
                m_pClawAnimationComponent->SetAnimation("duckpistol");
            }
            else
            {
                m_pClawAnimationComponent->SetAnimation("duckemptypistol");
            }
        }
        else if (activeAmmoType == AmmoType_Magic)
        {
            if (m_pAmmoComponent->CanFire())
            {
                m_pClawAnimationComponent->SetAnimation("duckmagic");
            }
            else
            {
                m_pClawAnimationComponent->SetAnimation("duckemptymagic");
            }
        }
        else if (activeAmmoType == AmmoType_Dynamite)
        {
            if (m_pAmmoComponent->CanFire())
            {
                m_pClawAnimationComponent->SetAnimation("duckpredynamite");
            }
            else
            {
                m_pClawAnimationComponent->SetAnimation("duckemptydynamite");
            }
        }
        m_State = ClawState_DuckAttacking;
    }
    else
    {
        if (activeAmmoType == AmmoType_Pistol)
        {
            if (m_pAmmoComponent->CanFire())
            {
                m_pClawAnimationComponent->SetAnimation("pistol");
            }
            else
            {
                m_pClawAnimationComponent->SetAnimation("emptypistol");
            }
        }
        else if (activeAmmoType == AmmoType_Magic)
        {
            if (m_pAmmoComponent->CanFire())
            {
                m_pClawAnimationComponent->SetAnimation("magic");
            }
            else
            {
                m_pClawAnimationComponent->SetAnimation("emptymagic");
            }
        }
        else if (activeAmmoType == AmmoType_Dynamite)
        {
            if (m_pAmmoComponent->CanFire())
            {
                m_pClawAnimationComponent->SetAnimation("predynamite");
            }
            else
            {
                m_pClawAnimationComponent->SetAnimation("emptydynamite");
            }
        }
        m_State = ClawState_Shooting;
    }
}

void ClawControllableComponent::OnFireEnded()
{
    if (m_pClawAnimationComponent->GetCurrentAnimationName().find("predynamite") != std::string::npos)
    {
        if (IsDucking())
        {
            m_pClawAnimationComponent->SetAnimation("duckpostdynamite");
        }
        else
        {
            m_pClawAnimationComponent->SetAnimation("postdynamite");
        }
    }
}

bool ClawControllableComponent::CanMove()
{
    if (m_bIsLifting ||
        m_bIsThrowing ||
        (m_State == ClawState_Shooting && m_pClawAnimationComponent->GetCurrentAnimationName() != "predynamite") ||
        m_State == ClawState_Attacking ||
        m_State == ClawState_Dying ||
        m_State == ClawState_DuckAttacking ||
        m_State == ClawState_DuckShooting ||
        m_State == ClawState_TakingDamage ||
        m_State == ClawState_Frozen ||
        m_pClawAnimationComponent->GetCurrentAnimationName() == "land" ||
        (m_LookingUpTime > g_pApp->GetGlobalOptions()->startLookUpOrDownTime) ||
        m_bFrozen)
    {
        return false;
    }

    return true;
}

bool ClawControllableComponent::IsActorFrozen()
{
    if (m_FrozenTime && m_FrozenTime < g_pApp->GetGlobalOptions()->freezeTime)
    {
        return true;
    }

    if (m_State == ClawState_Frozen)
    {
        m_State = ClawState_None;
    }

    return false;
}

void ClawControllableComponent::SetCurrentPhysicsState()
{
    if (m_pPhysicsComponent->IsFalling())
    {
        SetClawAnimation("fall");
        m_State = ClawState_Falling;
    }
    else if (m_pPhysicsComponent->IsJumping())
    {
        SetClawAnimation("jump");
        m_State = ClawState_Jumping;
    }
    else if (IsDucking())
    {
        SetClawAnimation("duck");
        m_State = ClawState_Ducking;
    }
    else if (m_pPhysicsComponent->IsOnGround())
    {
        SetClawAnimation("stand");
        m_State = ClawState_Standing;
    }
    else if (m_State == ClawState_HoldingRope)
    {
        m_pClawAnimationComponent->SetAnimation("swing");
        m_pPhysicsComponent->SetGravityScale(0.0f);
        return;
    }
    else
    {
        SetClawAnimation("fall");
        m_State = ClawState_Standing;
        //LOG_ERROR("Unknown physics state. Assume falling");
    }

    m_pPhysicsComponent->RestoreGravityScale();
}

void ClawControllableComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    const std::string animName = pAnimation->GetName();

    if (animName.find("predynamite") != std::string::npos && pAnimation->IsAtLastAnimFrame())
    {
        pAnimation->Pause();
        return;
    }

    // Shooting, only magic and pistol supported at the moment
    if (((animName.find("pistol") != std::string::npos) && pNewFrame->hasEvent) ||
        ((animName.find("magic") != std::string::npos) && pNewFrame->hasEvent) ||
        ((animName.find("dynamite") != std::string::npos) && pNewFrame->hasEvent))
    {
        if (m_pAmmoComponent->CanFire())
        {
            AmmoType projectileType = m_pAmmoComponent->GetActiveAmmoType();
            int32 offsetX = 50;
            if (m_Direction == Direction_Left) { offsetX *= -1; }
            int32 offsetY = -20;
            if (IsDucking()) { offsetY += 40; }

            // Dynamite hacks
            Point initialImpulse(0, 0);
            if (animName.find("postdynamite") != std::string::npos)
            {
                initialImpulse.Set(10, -10);
                float factor = (float)m_ThrowingTime / (float)2000.0f;
                initialImpulse.x *= factor;
                initialImpulse.y *= factor;
            }
            else if (animName == "jumpdynamite")
            {
                initialImpulse.Set(3.2, 0);
            }

            ActorTemplates::CreateClawProjectile(
                projectileType, 
                m_Direction, 
                Point(m_pPositionComponent->GetX() + offsetX, 
                m_pPositionComponent->GetY() + offsetY),
                m_pOwner->GetGUID(),
                initialImpulse);

            int soundPlayChance = 33;
            if (!m_pExclamationMark->IsActive() && 
                Util::RollDice(soundPlayChance) &&
                animName.find("duck") == std::string::npos &&
                animName.find("jump") == std::string::npos)
            {
                if (projectileType == AmmoType_Pistol)
                {
                    SoundInfo soundInfo(SOUND_CLAW_KILL_PISTOL1);
                    soundInfo.soundVolume = 200;
                    IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
                        new EventData_Request_Play_Sound(soundInfo)));
                    m_pExclamationMark->Activate(Util::GetSoundDurationMs(SOUND_CLAW_KILL_PISTOL1));
                }
                else if ((projectileType == AmmoType_Dynamite) && Util::RollDice(soundPlayChance))
                {
                    SoundInfo soundInfo(SOUND_CLAW_SCREW_ALL_THIS);
                    soundInfo.soundVolume = 200;
                    IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
                        new EventData_Request_Play_Sound(soundInfo)));
                    m_pExclamationMark->Activate(Util::GetSoundDurationMs(SOUND_CLAW_SCREW_ALL_THIS));
                }
            }

            if (IsDucking())
            {
                SetCurrentPhysicsState();
            }

            if (!g_pApp->GetGameCheats()->clawInfiniteAmmo)
            {
                // TODO: Better name of this method ? 
                m_pAmmoComponent->OnFired();
            }
        }
    }

    if (((animName.find("pistol") != std::string::npos) || 
        (animName.find("magic") != std::string::npos) || 
        (animName.find("dynamite") != std::string::npos))
        && pAnimation->IsAtLastAnimFrame())
    {
        SetCurrentPhysicsState();
    }
    else if ((animName == "swipe" ||
            animName == "kick" ||
            animName == "uppercut" ||
            animName == "jumpswipe" ||
            animName == "duckswipe"))
    {
        if (pAnimation->GetCurrentAnimationFrame()->hasEvent ||
            (animName == "swipe" && pNewFrame->idx == 3))
        {
            Point position = m_pPositionComponent->GetPosition();
            Point positionOffset(60, 20);
            if (animName == "jumpswipe")
            {
                positionOffset.y -= 10;
                positionOffset.x += 5;
            }
            if (m_Direction == Direction_Left)
            {
                positionOffset = Point(-1.0 * positionOffset.x, positionOffset.y);
            }
            position += positionOffset;

            if (m_pPowerupComponent->HasPowerup(PowerupType_FireSword))
            {
                ActorTemplates::CreateActor_Projectile(
                    ActorPrototype_FireSwordProjectile,
                    position,
                    m_Direction,
                    m_pOwner->GetGUID());
            }
            else if (m_pPowerupComponent->HasPowerup(PowerupType_FrostSword))
            {
                ActorTemplates::CreateActor_Projectile(
                    ActorPrototype_FrostSwordProjectile,
                    position,
                    m_Direction,
                    m_pOwner->GetGUID());
            }
            else if (m_pPowerupComponent->HasPowerup(PowerupType_LightningSword))
            {
                ActorTemplates::CreateActor_Projectile(
                    ActorPrototype_LightningSwordProjectile,
                    position,
                    m_Direction,
                    m_pOwner->GetGUID());
            }
            else
            {
                int damage = 10;

                // When Claw is ducking he deals 1/2 damage
                if (IsDucking())
                {
                    damage = 5;
                }
                if (m_pPowerupComponent->HasPowerup(PowerupType_Catnip))
                {
                    damage = 100;
                }

                ActorTemplates::CreateAreaDamage(
                    position,
                    Point(50, 25),
                    damage,
                    CollisionFlag_ClawAttack,
                    "Rectangle",
                    DamageType_MeleeAttack,
                    m_Direction,
                    m_pOwner->GetGUID());
            }
        }
        if (pAnimation->IsAtLastAnimFrame())
        {
            SetCurrentPhysicsState();
        }
    }

    if (animName == "lookup" && pAnimation->IsAtLastAnimFrame())
    {
        pAnimation->Pause();
    }
}

void ClawControllableComponent::VOnAnimationLooped(Animation* pAnimation)
{
    std::string animName = pAnimation->GetName();
    if (pAnimation->GetName().find("death") != std::string::npos)
    {
        shared_ptr<LifeComponent> pClawLifeComponent = MakeStrongPtr(m_pOwner->GetComponent<LifeComponent>());
        assert(pClawLifeComponent != nullptr);

        shared_ptr<EventData_Claw_Died> pEvent(new EventData_Claw_Died(
            m_pOwner->GetGUID(), 
            m_pPositionComponent->GetPosition(), 
            pClawLifeComponent->GetLives() - 1));
        IEventMgr::Get()->VTriggerEvent(pEvent);

        SetCurrentPhysicsState();
        m_pPhysicsComponent->RestoreGravityScale();
        m_pRenderComponent->SetVisible(true);
    }
    else if (animName == "damage1" ||
             animName == "damage2")
    {
        SetCurrentPhysicsState();
    }
    else if (animName == "land")
    {
        SetClawAnimation("stand");
    }
    else if (animName == "lift" && m_bIsLifting)
    {
        m_bIsLifting = false;
        SetCurrentPhysicsState();

        if (m_bIsCarryingEnemy)
        {
            StrongActorPtr pEnemy = MakeStrongPtr(g_pApp->GetGameLogic()->VGetActor(m_CarriedActorId));
            shared_ptr<EnemyAIComponent> pEnemyAI = pEnemy ? MakeStrongPtr(pEnemy->GetComponent<EnemyAIComponent>(EnemyAIComponent::g_Name)) : nullptr;
            if (pEnemyAI)
            {
                pEnemyAI->OnLiftedAboveHead();
            }
        }
    }
    else if (animName == "throw" && m_bIsThrowing)
    {
        m_bIsThrowing = false;
        SetCurrentPhysicsState();
    }
}

bool ClawControllableComponent::IsAttackingOrShooting()
{
    if (m_State == ClawState_Shooting ||
        m_State == ClawState_JumpShooting ||
        m_State == ClawState_DuckShooting ||
        m_State == ClawState_Attacking ||
        m_State == ClawState_DuckAttacking ||
        m_State == ClawState_JumpAttacking)
    {
        return true;
    }

    return false;
}

void ClawControllableComponent::VOnHealthBelowZero(DamageType damageType, int sourceActorId)
{
    if (m_State == ClawState_Dying)
    {
        return;
    }

    DropCarried();

    IEventMgr::Get()->VQueueEvent(IEventDataPtr(new EventData_Claw_Health_Below_Zero(m_pOwner->GetGUID())));

    // TODO: Track how exactly claw died
    if (m_pClawAnimationComponent->GetCurrentAnimationName() != "spikedeath")
    {
        m_pClawAnimationComponent->SetAnimation("spikedeath");
        m_pPhysicsComponent->SetGravityScale(0.0f);
        m_State = ClawState_Dying;

        std::string deathSound = SOUND_CLAW_DEATH_SPIKES;

        // TODO: When claw dies, in the original game he falls off the screen and respawns
        if (damageType == DamageType_DeathTile)
        {
            int currentLevel = g_pApp->GetGameLogic()->GetCurrentLevelData()->GetLevelNumber();

            // Sound should be always like this
            deathSound = "/LEVEL" + ToStr(currentLevel) + "/SOUNDS/DEATHTILE.WAV";

            // Check if we fell into some stuff (tar, water) - if yes, hide Claw's body since he is underwater 
            // or something similar
            if (ClawLevelUtil::CreateSpecialDeathEffect(m_pPositionComponent->GetPosition(), currentLevel) != nullptr)
            {
                m_pRenderComponent->SetVisible(false);
            }
        }
        
        SoundInfo soundInfo(deathSound);
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));
    }

    if (m_bIsInBossFight)
    {
        IEventMgr::Get()->VQueueEvent(IEventDataPtr(new EventData_Boss_Fight_Ended(false)));
    }
}

void ClawControllableComponent::VOnHealthChanged(int32 oldHealth, int32 newHealth, DamageType damageType, Point impactPoint, int sourceActorId)
{
    // If claw still has health
    if (newHealth <= 0) {
        return;
    } 

    if (oldHealth > newHealth || damageType == DamageType_SirenProjectile)
    {
        DropCarried();
    }

    if (damageType == DamageType_SirenProjectile) {
        m_pClawAnimationComponent->SetAnimation("freeze");
        AddFrozenTime(1);
        m_State = ClawState_Frozen;
        // Since this is abit hacky, return as soon as possible into the method
        return;
    }

    // When claw takes damage but does not actually die
    if (oldHealth > newHealth)
    {

        // When Claw is holding rope his animation does not change
        if (m_State != ClawState_HoldingRope)
        {
            if (Util::GetRandomNumber(0, 1) == 0)
            {
                m_pClawAnimationComponent->SetAnimation("damage1");
            }
            else
            {
                m_pClawAnimationComponent->SetAnimation("damage2");
            }
        }

        if (damageType == DamageType_Trident) {
            Point explosionPoint(m_pPositionComponent->GetX() + 50, m_pPositionComponent->GetY());
            ActorTemplates::CreateSingleAnimation(explosionPoint, AnimationType_TridentExplosion);
        }

        // Play random "take damage" sound
        int takeDamageSoundIdx = Util::GetRandomNumber(0, m_TakeDamageSoundList.size() - 1);
        SoundInfo soundInfo(m_TakeDamageSoundList[takeDamageSoundIdx]);
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));

        Point knockback(-10, 0);
        if (m_pRenderComponent->IsMirrored())
        {
            knockback = Point(knockback.x * -1.0, knockback.y);
        }

        // TODO: How to make this work well with enemy damage auras ?
        /*m_pPositionComponent->SetPosition(m_pPositionComponent->GetX() + knockback.x, m_pPositionComponent->GetY() + knockback.y);

        shared_ptr<EventData_Teleport_Actor> pEvent(new EventData_Teleport_Actor
            (m_pOwner->GetGUID(), m_pPositionComponent->GetPosition()));
        IEventMgr::Get()->VQueueEvent(pEvent);*/

        m_pHealthComponent->SetInvulnerable(true);
        m_TakeDamageTimeLeftMs = m_TakeDamageDuration;
        m_pPhysicsComponent->SetGravityScale(0.0f);
        
        if (m_State != ClawState_HoldingRope)
        {
            m_State = ClawState_TakingDamage;
        }

        // Spawn graphics in the hit point
        ActorTemplates::CreateSingleAnimation(impactPoint, AnimationType_BlueHitPoint);
        Util::PlayRandomHitSound();
    }
}

bool ClawControllableComponent::IsDucking()
{
    return (m_State == ClawState_Ducking ||
        m_State == ClawState_DuckAttacking ||
        m_State == ClawState_DuckShooting);
}

void ClawControllableComponent::OnDuck()
{
    if (!IsDucking())
    {
        SetClawAnimation("duck");
        m_State = ClawState_Ducking;
    }
}

void ClawControllableComponent::OnStand()
{
    m_State = ClawState_Standing;
    SetCurrentPhysicsState();
}

bool ClawControllableComponent::IsClimbing()
{
    return m_pClawAnimationComponent->GetCurrentAnimationName().find("climb") != std::string::npos &&
        !m_pClawAnimationComponent->GetCurrentAnimation()->IsPaused();
}

void ClawControllableComponent::VOnAttachedToRope()
{
    DropCarried();

    m_pClawAnimationComponent->SetAnimation("swing");
    m_pPhysicsComponent->SetGravityScale(0.0f);

    m_State = ClawState_HoldingRope;

    m_pPhysicsComponent->OnAttachedToRope();
}

void ClawControllableComponent::VDetachFromRope()
{
    m_State = ClawState_Jumping;
    m_pPhysicsComponent->RestoreGravityScale();

    SetCurrentPhysicsState();

    m_pPhysicsComponent->OnDetachedFromRope();
}

void ClawControllableComponent::BossFightStartedDelegate(IEventDataPtr pEvent)
{
    m_TakeDamageDuration = 500;

    m_bIsInBossFight = true;
}

void ClawControllableComponent::BossFightEndedDelegate(IEventDataPtr pEvent)
{
    m_TakeDamageDuration = 500;

    m_bIsInBossFight = false;
}

void ClawControllableComponent::OnClawKilledEnemy(DamageType killDamageType, Actor* pKilledEnemyActor)
{
    assert(pKilledEnemyActor != NULL);

    static std::vector<std::string> s_OnEnemyKillSoundList =
    {
        SOUND_CLAW_KILL_MELEE1,
        SOUND_CLAW_KILL_MELEE2,
        SOUND_CLAW_KILL_MELEE3,
        SOUND_CLAW_KILL_MELEE4,
        SOUND_CLAW_KILL_MELEE5,
        SOUND_CLAW_KILL_MELEE6,
        SOUND_CLAW_LAND_LOVER1_SHORT,
        SOUND_CLAW_LAND_LOVER2_LONG
    };

    int soundPlayChance = 36;
    if ((killDamageType == DamageType_MeleeAttack) && 
        !m_pExclamationMark->IsActive() &&
        Util::RollDice(soundPlayChance))
    {
        std::string snd = Util::PlayRandomSoundFromList(s_OnEnemyKillSoundList, 260);
        m_pExclamationMark->Activate(Util::GetSoundDurationMs(snd));
    }
}
//=====================================================================================================================
// Lifting and throwing powder kegs and enemies
//
// Mimics the original game:
//  1) Lift key (Z) pressed while standing next to a powder keg / liftable enemy -> TryLift()
//     Claw plays "lift" animation, the lifted actor's Box2D body is deactivated (so it stops colliding,
//     falling and dealing aura damage) and every frame it is teleported above Claw's head
//     (UpdateCarriedActorPosition()). Enemy AI is put on hold (EnemyAIComponent::OnLifted()).
//  2) While carrying, Claw walks with "liftwalk" / stands with "lift2" (see SetClawAnimation()),
//     he cannot duck, jump, attack or shoot.
//  3) Lift key pressed again starts charging the throw (Claw leans back - "liftcharge" animation),
//     releasing it throws (Throw()). The longer it was held, the further it flies.
//     Attack / fire key throws immediately with the shortest distance.
//     - Keg is replaced by a projectile actor (ActorTemplates::CreateThrownPowderKeg()) which explodes upon impact.
//     - Enemy's body is reactivated with initial speed, EnemyAIComponent takes care of the impact.
//  4) When Claw takes damage / climbs / grabs a rope, he drops what he carries (DropCarried()).
//=====================================================================================================================

// Keg images which the original game uses while keg is held above Claw's head / flying
static const int KEG_LIFTED_IMAGE_ID = 14;

// Horizontal speed of the furthest throw. Vertical speed is always the same so the throw distance is linear to this.
static const double MAX_THROW_SPEED_X = 7.0;
static const double THROW_SPEED_Y = -3.0;

static AnimationComponent* GetAnimationComponent(Actor* pActor)
{
    return MakeStrongPtr(pActor->GetComponent<AnimationComponent>(AnimationComponent::g_Name)).get();
}

void ClawControllableComponent::SetClawAnimation(const std::string& animName)
{
    // Lift and throw animations have to finish first
    if (m_bIsLifting || m_bIsThrowing)
    {
        return;
    }

    if (IsCarrying())
    {
        const bool isMovementAnim = (animName == "walk" || animName == "jump" || animName == "fall" ||
            animName == "highfall" || animName == "stand" || animName == "idle" || animName == "land" || animName == "lookup");

        if (isMovementAnim && m_bIsChargingThrow)
        {
            m_pClawAnimationComponent->SetAnimation("liftcharge");
            return;
        }
        else if (animName == "walk")
        {
            m_pClawAnimationComponent->SetAnimation("liftwalk");
            return;
        }
        else if (isMovementAnim)
        {
            m_pClawAnimationComponent->SetAnimation("lift2");
            return;
        }
    }

    m_pClawAnimationComponent->SetAnimation(animName);
}

void ClawControllableComponent::OnLiftPressed()
{
    if (m_bIsLifting ||
        m_bIsThrowing ||
        m_bFrozen ||
        m_State == ClawState_Dying ||
        m_State == ClawState_TakingDamage ||
        m_State == ClawState_Frozen)
    {
        return;
    }

    if (IsCarrying())
    {
        // Claw leans back while the key is held
        m_bIsChargingThrow = true;
        m_ThrowChargeTime = 0;
        m_pClawAnimationComponent->SetAnimation("liftcharge");
        return;
    }

    // Kegs / enemies can only be lifted when Claw stands on ground and is not busy
    if ((m_State != ClawState_Standing &&
         m_State != ClawState_Walking &&
         m_State != ClawState_Idle) ||
        !m_pPhysicsComponent->IsOnGround())
    {
        return;
    }

    TryLift();
}

void ClawControllableComponent::OnLiftReleased()
{
    if (!m_bIsChargingThrow)
    {
        return;
    }

    m_bIsChargingThrow = false;
    if (IsCarrying() && !m_bIsLifting && !m_bIsThrowing)
    {
        Throw(m_ThrowChargeTime);
    }
}

void ClawControllableComponent::OnQuickThrow()
{
    if (IsCarrying() && !m_bIsLifting && !m_bIsThrowing && !m_bFrozen)
    {
        m_bIsChargingThrow = false;
        Throw(0);
    }
}

bool ClawControllableComponent::TryLift()
{
    shared_ptr<IGamePhysics> pPhysics = g_pApp->GetGameLogic()->VGetGamePhysics();

    // Look for keg or enemy in front of Claw
    const Point clawPos = m_pPositionComponent->GetPosition();
    const double dir = (m_Direction == Direction_Left) ? -1.0 : 1.0;
    const double rayOffsetsY[] = { 20.0, 45.0 };
    const uint32 liftableFlags[] = { CollisionFlag_PowderKeg, CollisionFlag_DynamicActor };

    for (uint32 flag : liftableFlags)
    {
        for (double offsetY : rayOffsetsY)
        {
            RaycastResult result = pPhysics->VRayCast(
                Point(clawPos.x - 20.0 * dir, clawPos.y + offsetY),
                Point(clawPos.x + 75.0 * dir, clawPos.y + offsetY),
                flag);
            if (!result.foundIntersection || result.actorId == INVALID_ACTOR_ID)
            {
                continue;
            }

            StrongActorPtr pActor = MakeStrongPtr(g_pApp->GetGameLogic()->VGetActor(result.actorId));
            if (!pActor)
            {
                continue;
            }

            shared_ptr<EnemyAIComponent> pEnemyAI = MakeStrongPtr(pActor->GetComponent<EnemyAIComponent>(EnemyAIComponent::g_Name));
            if (flag == CollisionFlag_DynamicActor && (!pEnemyAI || !pEnemyAI->CanBeLifted()))
            {
                continue;
            }

            // Lifted actor's body stops being simulated while Claw holds it
            pPhysics->VDeactivate(result.actorId);

            m_CarriedActorId = result.actorId;
            m_bIsCarryingEnemy = (pEnemyAI != nullptr);
            if (pEnemyAI)
            {
                pEnemyAI->OnLifted();
            }

            m_bIsLifting = true;
            m_State = ClawState_Standing;
            m_pClawAnimationComponent->SetAnimation("lift");

            return true;
        }
    }

    return false;
}

void ClawControllableComponent::UpdateCarriedActorPosition()
{
    if (!IsCarrying())
    {
        return;
    }

    StrongActorPtr pCarried = MakeStrongPtr(g_pApp->GetGameLogic()->VGetActor(m_CarriedActorId));
    if (!pCarried)
    {
        // Carried actor is gone (e.g. destroyed by explosion)
        m_CarriedActorId = INVALID_ACTOR_ID;
        m_bIsChargingThrow = false;
        return;
    }

    // While Claw bends down to grab it, it stays where it is
    if (m_bIsLifting && m_pClawAnimationComponent->GetCurrentAnimation()->GetCurrentAnimationFrame()->idx == 0)
    {
        return;
    }

    // Lifted keg is shown lying on its side
    if (!m_bIsCarryingEnemy)
    {
        AnimationComponent* pKegAnimComp = GetAnimationComponent(pCarried.get());
        std::string liftedAnim = "lifted";
        if (pKegAnimComp && !pKegAnimComp->HasAnimation(liftedAnim))
        {
            AnimationFrame frame;
            frame.idx = 0;
            frame.imageId = KEG_LIFTED_IMAGE_ID;
            frame.imageName = "frame" + Util::ConvertToThreeDigitsString(KEG_LIFTED_IMAGE_ID);
            frame.duration = 1000;
            std::vector<AnimationFrame> frames = { frame };
            std::shared_ptr<Animation> pLiftedAnim = Animation::CreateAnimation(frames, liftedAnim.c_str(), pKegAnimComp);
            if (pLiftedAnim)
            {
                pKegAnimComp->AddAnimation(liftedAnim, pLiftedAnim);
            }
        }
        if (pKegAnimComp && pKegAnimComp->HasAnimation(liftedAnim))
        {
            pKegAnimComp->SetAnimation(liftedAnim);
        }
    }

    // Held above Claw's head - bottom of the image is right above Claw's hands.
    // Image is drawn centered at its body position + image offset, so compensate it.
    Point carriedPos(m_pPositionComponent->GetX(), m_pPositionComponent->GetY() - 44);
    shared_ptr<ActorRenderComponent> pRenderComponent = MakeStrongPtr(pCarried->GetComponent<ActorRenderComponent>());
    if (pRenderComponent)
    {
        if (shared_ptr<Image> pImage = MakeStrongPtr(pRenderComponent->GetCurrentImage()))
        {
            carriedPos.Set(
                carriedPos.x - pImage->GetOffsetX(),
                carriedPos.y - pImage->GetHeight() / 2 - pImage->GetOffsetY());
        }
    }

    g_pApp->GetGameLogic()->VGetGamePhysics()->VSetPosition(m_CarriedActorId, carriedPos);
}

void ClawControllableComponent::Throw(int chargeTimeMs)
{
    StrongActorPtr pCarried = MakeStrongPtr(g_pApp->GetGameLogic()->VGetActor(m_CarriedActorId));
    const uint32 carriedId = m_CarriedActorId;
    const bool isEnemy = m_bIsCarryingEnemy;
    m_CarriedActorId = INVALID_ACTOR_ID;
    m_bIsCarryingEnemy = false;
    if (!pCarried)
    {
        return;
    }

    // The longer the lift key was held, the further it flies
    //  Keg:   < 1s = 1/2, 1-2s = 3/4, > 2s = full distance
    //  Enemy: < 2s = 1/2, > 2s = 3/4 of full distance
    double distanceFactor = 0.5;
    if (isEnemy)
    {
        distanceFactor = (chargeTimeMs >= 2000) ? 0.75 : 0.5;
    }
    else
    {
        if (chargeTimeMs >= 2000) { distanceFactor = 1.0; }
        else if (chargeTimeMs >= 1000) { distanceFactor = 0.75; }
    }

    const double dir = (m_Direction == Direction_Left) ? -1.0 : 1.0;
    const Point throwSpeed(MAX_THROW_SPEED_X * distanceFactor * dir, THROW_SPEED_Y);

    shared_ptr<IGamePhysics> pPhysics = g_pApp->GetGameLogic()->VGetGamePhysics();
    if (isEnemy)
    {
        // Enemy flies on his own and dies upon impact
        pPhysics->VActivate(carriedId);
        pPhysics->VSetLinearSpeed(carriedId, throwSpeed);

        shared_ptr<EnemyAIComponent> pEnemyAI = MakeStrongPtr(pCarried->GetComponent<EnemyAIComponent>(EnemyAIComponent::g_Name));
        if (pEnemyAI)
        {
            pEnemyAI->OnThrown(m_pOwner->GetGUID());
        }
    }
    else
    {
        // Visible keg is replaced by a projectile which explodes upon impact
        Point kegImagePos = pCarried->GetPositionComponent()->GetPosition();
        shared_ptr<ActorRenderComponent> pKegRenderComponent = MakeStrongPtr(pCarried->GetComponent<ActorRenderComponent>());
        if (pKegRenderComponent)
        {
            if (shared_ptr<Image> pKegImage = MakeStrongPtr(pKegRenderComponent->GetCurrentImage()))
            {
                kegImagePos.Set(kegImagePos.x + pKegImage->GetOffsetX(), kegImagePos.y + pKegImage->GetOffsetY());
            }
        }
        const std::string kegImageSet = pCarried->GetName();

        pPhysics->VRemoveActor(carriedId);
        IEventMgr::Get()->VQueueEvent(IEventDataPtr(new EventData_Destroy_Actor(carriedId)));

        ActorTemplates::CreateThrownPowderKeg(
            kegImageSet,
            Point(kegImagePos.x + 15.0 * dir, kegImagePos.y),
            throwSpeed,
            m_pOwner->GetGUID());
    }

    m_bIsThrowing = true;
    m_pClawAnimationComponent->SetAnimation("throw");
}

void ClawControllableComponent::DropCarried()
{
    m_bIsLifting = false;
    m_bIsThrowing = false;
    m_bIsChargingThrow = false;

    if (!IsCarrying())
    {
        return;
    }

    // Falls down from Claw's hands - keg lands as an ordinary keg, enemy continues as usual
    shared_ptr<IGamePhysics> pPhysics = g_pApp->GetGameLogic()->VGetGamePhysics();
    pPhysics->VActivate(m_CarriedActorId);
    pPhysics->VSetLinearSpeed(m_CarriedActorId, Point(0, 0));

    StrongActorPtr pCarried = MakeStrongPtr(g_pApp->GetGameLogic()->VGetActor(m_CarriedActorId));
    if (pCarried)
    {
        if (m_bIsCarryingEnemy)
        {
            shared_ptr<EnemyAIComponent> pEnemyAI = MakeStrongPtr(pCarried->GetComponent<EnemyAIComponent>(EnemyAIComponent::g_Name));
            if (pEnemyAI)
            {
                pEnemyAI->OnDropped();
            }
        }
        else if (AnimationComponent* pKegAnimComp = GetAnimationComponent(pCarried.get()))
        {
            // Back to the paused first frame of the explode animation, as at the level start
            pKegAnimComp->SetAnimation("explode");
            pKegAnimComp->PauseAnimation();
        }
    }

    m_CarriedActorId = INVALID_ACTOR_ID;
    m_bIsCarryingEnemy = false;
}
