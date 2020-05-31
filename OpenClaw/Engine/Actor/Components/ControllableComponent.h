#ifndef CONTROLLABLECOMPONENT_H_
#define CONTROLLABLECOMPONENT_H_

#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "AnimationComponent.h"
#include "ControllerComponents/HealthComponent.h"
#include "../../Util/PrimeSearch.h"

// Goal is to not hard bind claw to be the only controllable character
// It is solved by accessing this partially interface class
// and let concrete classes implement all the details
class ControllableComponent : public ActorComponent
{
public:
    ControllableComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual bool VInitDelegate(TiXmlElement* pData) { return true; }
    virtual TiXmlElement* VGenerateXml() override;
    virtual void VPostInit() override;

    bool IsActive() const { return m_Active; }
    void SetActive(bool active) { m_Active = active; }

    // Interface for subclasses
    virtual void VOnStartFalling() = 0;
    virtual void VOnLandOnGround(float fromHeight) = 0;
    virtual void VOnStartJumping() = 0;
    virtual bool VOnDirectionChange(Direction direction) = 0;
    virtual void VOnStopMoving() = 0;
    virtual void VOnRun() = 0;
    virtual void VOnClimb(bool bIsClimbingUp, bool bIsOnTopEdge) = 0;
    virtual void VOnStopClimbing() = 0;
    virtual void OnAttack() = 0;
    virtual void OnFire(bool outOfAmmo = false) = 0;
    virtual void OnFireEnded() = 0;
    virtual void OnDuck() = 0;
    virtual void OnStand() = 0;
    virtual void VOnAttachedToRope() = 0;
    virtual void VDetachFromRope() = 0;
    virtual bool VIsAttachedToRope() = 0;

    virtual bool CanMove() = 0;
    virtual bool IsActorFrozen() = 0;
    virtual bool IsDucking() = 0;

    virtual bool IsDying() = 0;
    virtual bool InPhysicsCapableState() = 0;

    virtual bool IsClimbing() = 0;

    virtual void SetFrozen(bool frozen) { m_bFrozen = frozen; }

    void SetMaxJumpHeight(int maxJumpHeight) { m_MaxJumpHeight = maxJumpHeight; }
    int GetMaxJumpHeight() { return m_MaxJumpHeight; }

    void SetLookingUpTime(int32 msTime) { m_LookingUpTime = msTime; }
    void AddLookingUpTime(int32 msTime) { m_LookingUpTime += msTime; }

    void SetFrozenTime(int32 msTime) { m_FrozenTime = msTime; }
    void AddFrozenTime(int32 msTime) { m_FrozenTime += msTime; }

    void SetDuckingTime(int32 msTime) { m_DuckingTime = msTime; }
    void AddDuckingTime(int32 msTime) { m_DuckingTime += msTime; }

protected:
    bool m_Active;
    bool m_bFrozen;

    int m_DuckingTime;
    int m_LookingUpTime;
    int m_FrozenTime;

    int m_MaxJumpHeight;
};

enum ClawState
{
    ClawState_None,
    ClawState_Standing,
    ClawState_Walking,
    ClawState_Jumping,
    ClawState_Falling,
    ClawState_Climbing,
    ClawState_Ducking,
    ClawState_Shooting,
    ClawState_DuckShooting,
    ClawState_JumpShooting,
    ClawState_Attacking,
    ClawState_DuckAttacking,
    ClawState_JumpAttacking,
    ClawState_TakingDamage,
    ClawState_Dying,
    ClawState_HoldingRope,
    ClawState_Frozen,
    ClawState_Idle
};

class PositionComponent;
class PhysicsComponent;
class ActorRenderComponent;
class AnimationComponent;
class AmmoComponent;
class PowerupComponent;
class HealthComponent;
class FollowableComponent;
class ClawControllableComponent : 
    public ControllableComponent, 
    public AnimationObserver, 
    public HealthObserver
{
public:
    ClawControllableComponent();
    virtual ~ClawControllableComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInitDelegate(TiXmlElement* data) override;
    virtual void VPostInit() override; 
    virtual void VPostPostInit() override;
    virtual void VUpdate(uint32 msDiff) override;

    // Interface for subclasses
    virtual void VOnStartFalling() override;
    virtual void VOnLandOnGround(float fromHeight) override;
    virtual void VOnStartJumping() override;
    virtual bool VOnDirectionChange(Direction direction) override;
    virtual void VOnStopMoving() override;
    virtual void VOnRun() override;
    virtual void VOnClimb(bool bIsClimbingUp, bool bIsOnTopEdge) override;
    virtual void VOnStopClimbing() override;
    virtual void OnAttack() override;
    virtual void OnFire(bool outOfAmmo = false) override;
    virtual void OnFireEnded() override;
    virtual void OnDuck() override;

    virtual void VOnAttachedToRope() override;
    virtual void VDetachFromRope() override;
    virtual bool VIsAttachedToRope() override { return m_State == ClawState_HoldingRope; }

    virtual bool CanMove() override;
    virtual bool IsActorFrozen() override;
    virtual bool IsDucking() override;
    virtual void OnStand() override;

    virtual bool IsDying() override { return m_State == ClawState_Dying; }
    virtual bool InPhysicsCapableState() override { return (m_State != ClawState_Dying && m_State != ClawState_TakingDamage); }

    // Feels abit hacky
    virtual bool IsClimbing() override;

    bool IsHoldingRope() { return m_State == ClawState_HoldingRope; }

    void OnClawKilledEnemy(DamageType killDamageType, Actor* pKilledEnemyActor);

    // AnimationObserver API
    virtual void VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame) override;
    virtual void VOnAnimationLooped(Animation* pAnimation) override;

    // HealthObserver API
    virtual void VOnHealthBelowZero(DamageType damageType, int sourceActorId) override;
    virtual void VOnHealthChanged(int32 oldHealth, int32 newHealth, DamageType damageType, Point impactPoint, int sourceActorId) override;

private:
    void BossFightStartedDelegate(IEventDataPtr pEvent);
    void BossFightEndedDelegate(IEventDataPtr pEvent);

    void SetCurrentPhysicsState();
    bool IsAttackingOrShooting();

    bool m_bIsInBossFight;

    int m_TakeDamageDuration;
    int m_TakeDamageTimeLeftMs;

    int m_ThrowingTime;

    ClawState m_State;
    ClawState m_LastState;

    std::vector<std::string> m_TakeDamageSoundList;
    std::vector<std::string> m_IdleQuoteSoundList;
    uint32 m_IdleTime;

    unique_ptr<PrimeSearch> m_pIdleQuotesSequence;

    Direction m_Direction;
    PhysicsComponent* m_pPhysicsComponent;
    AnimationComponent* m_pClawAnimationComponent;
    ActorRenderComponent* m_pRenderComponent;
    PositionComponent* m_pPositionComponent;
    AmmoComponent* m_pAmmoComponent;
    PowerupComponent* m_pPowerupComponent;
    HealthComponent* m_pHealthComponent;
    FollowableComponent* m_pExclamationMark;
};

#endif
