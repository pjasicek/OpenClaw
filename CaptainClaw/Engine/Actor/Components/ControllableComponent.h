#ifndef CONTROLLABLECOMPONENT_H_
#define CONTROLLABLECOMPONENT_H_

#include "../../SharedDefines.h"
#include "../ActorComponent.h"

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
    ClawState_Attacking,
};

class ActorRenderComponent;
class AnimationComponent;
class ClawControllableComponent : public ControllableComponent
{
public:
    ClawControllableComponent();
    virtual ~ClawControllableComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VInitDelegate(TiXmlElement* data) override;
    virtual void VPostInit() override; 

    // Interface for subclasses
    virtual void VOnStartFalling() override;
    virtual void VOnLandOnGround() override;
    virtual void VOnStartJumping() override;
    virtual void VOnDirectionChange(Direction direction) override;
    virtual void VOnStopMoving() override;
    virtual void VOnRun() override;
    virtual void VOnClimb() override;
    virtual void VOnStopClimbing() override;

    virtual bool CanMove() override;

private:
    ClawState m_State;

    AnimationComponent* m_pClawAnimationComponent;
    ActorRenderComponent* m_pRenderComponent;
};

#endif