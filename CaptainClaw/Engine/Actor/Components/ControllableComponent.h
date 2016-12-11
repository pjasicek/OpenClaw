#ifndef CONTROLLABLECOMPONENT_H_
#define CONTROLLABLECOMPONENT_H_

#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "AnimationComponent.h"
#include "ControllerComponents/HealthComponent.h"

// Goal is to not hard bind claw to be the only controllable character
// It is solved by accessing this partially interface class
// and let concrete classes implement all the details
class ControllableComponent : public ActorComponent
{
public:
    ControllableComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual bool VInitDelegate(TiXmlElement* pData) { return true; }
    virtual TiXmlElement* VGenerateXml() override;
    virtual void VPostInit() override;

    bool IsActive() const { return m_Active; }
    void SetActive(bool active) { m_Active = active; }

    // Interface for subclasses
    virtual void VOnStartFalling() = 0;
    virtual void VOnLandOnGround() = 0;
    virtual void VOnStartJumping() = 0;
    virtual void VOnDirectionChange(Direction direction) = 0;
    virtual void VOnStopMoving() = 0;
    virtual void VOnRun() = 0;
    virtual void VOnClimb() = 0;
    virtual void VOnStopClimbing() = 0;
    virtual void OnAttack() = 0;
    virtual void OnFire(bool outOfAmmo = false) = 0;
    virtual void OnDuck() = 0;

    virtual bool CanMove() = 0;

private:
    bool m_Active;
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
    ClawState_Dying
};

class PositionComponent;
class PhysicsComponent;
class ActorRenderComponent;
class AnimationComponent;
class AmmoComponent;
class PowerupComponent;
class ClawControllableComponent : public ControllableComponent, public AnimationObserver, public HealthObserver
{
public:
    ClawControllableComponent();
    virtual ~ClawControllableComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VInitDelegate(TiXmlElement* data) override;
    virtual void VPostInit() override; 
    virtual void VUpdate(uint32 msDiff) override;

    // Interface for subclasses
    virtual void VOnStartFalling() override;
    virtual void VOnLandOnGround() override;
    virtual void VOnStartJumping() override;
    virtual void VOnDirectionChange(Direction direction) override;
    virtual void VOnStopMoving() override;
    virtual void VOnRun() override;
    virtual void VOnClimb() override;
    virtual void VOnStopClimbing() override;
    virtual void OnAttack() override;
    virtual void OnFire(bool outOfAmmo = false) override;
    virtual void OnDuck() override;

    virtual bool CanMove() override;

    // AnimationObserver API
    virtual void VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame) override;
    virtual void VOnAnimationLooped(Animation* pAnimation) override;

    // HealthObserver API
    virtual void VOnHealthBelowZero() override;

private:
    void SetCurrentPhysicsState();
    bool IsAttackingOrShooting();

    ClawState m_State;

    Direction m_Direction;
    PhysicsComponent* m_pPhysicsComponent;
    AnimationComponent* m_pClawAnimationComponent;
    ActorRenderComponent* m_pRenderComponent;
    PositionComponent* m_pPositionComponent;
    AmmoComponent* m_pAmmoComponent;
    PowerupComponent* m_pPowerupComponent;
};

#endif