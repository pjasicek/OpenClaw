#ifndef PHYSICSCOMPONENT_H_
#define PHYSICSCOMPONENT_H_

#include <Box2D/Box2D.h>
#include "../../SharedDefines.h"
#include "../ActorComponent.h"

class ControllableComponent;
class PhysicsComponent : public ActorComponent
{
public:
    PhysicsComponent();
    virtual ~PhysicsComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;
    virtual void VPostInit() override;

    virtual void VUpdate(uint32 msDiff) override;

    void SetControllableComponent(ControllableComponent* pComp) { m_pControllableComponent = pComp; }

    bool CanBounce() const { return m_CanBounce; }
    void SetCanBounce(bool canBounce) { m_CanBounce = canBounce; }

    bool CanJump() const { return m_CanJump; }
    void SetCanJump(bool canJump) { m_CanJump = canJump; }

    int32 GetMaxJumpHeight() const { return m_MaxJumpHeight; }
    void SetMaxJumpHeight(int32 maxJumpHeight) { m_MaxJumpHeight = maxJumpHeight; }

    float GetGravityScale() { return m_GravityScale; }
    float GetFriction() { return m_Friction; }
    float GetDensity() { return m_Density; }

    Point GetBodySize() const { return m_BodySize; }
    double GetBodyWidth() const { return m_BodySize.x; }
    double GetBodyHeight() const { return m_BodySize.y; }

    void SetCurrentSpeed(Point speed) { m_CurrentSpeed = speed; }
    void SetConstantSpeed(Point speed) { m_ConstantSpeed = speed; }
    bool HasConstantSpeed() { return m_HasConstantSpeed; }
    void StopMovement() { m_CurrentSpeed = 0; m_ConstantSpeed = 0; }

    void RequestClimb(Point climbMovement);

    void OnBeginFootContact();
    void OnEndFootContact();
    void OnStartFalling();
    void OnStartJumping();

    int32 GetNumFootContacts() { return m_NumFootContacts; }
    bool IsOnGround() { return m_NumFootContacts > 0; }
    bool IsInAir() { return m_IsFalling || m_IsJumping; }
    bool IsFalling() { return m_IsFalling; }
    bool IsJumping() { return m_IsJumping; }

    void SetFalling(bool falling);
    void SetJumping(bool jumping);

    void AddJumpHeight(float deltaHeight) { m_HeightInAir += deltaHeight; }

    bool AttachToLadder();

    // Communication with IGamePhysics
    Point GetVelocity() { return m_pPhysics->VGetVelocity(_owner->GetGUID()); }
    void SetVelocity(Point velocity) { m_pPhysics->VSetLinearSpeed(_owner->GetGUID(), velocity); }
    
    void ApplyLinearImpulse(Point impulse) { m_pPhysics->VApplyLinearImpulse(_owner->GetGUID(), impulse); }
    void ApplyForce(Point force) { m_pPhysics->VApplyForce(_owner->GetGUID(), force); }

    void AddOverlappingLadder(const b2Fixture* ladder) { m_OverlappingLaddersList.push_back(ladder); }
    void RemoveOverlappingLadder(const b2Fixture* ladder);

    void AddOverlappingGround(const b2Fixture* pGround);
    void RemoveOverlappingGround(const b2Fixture* pGround);

    void AddOverlappingKinematicBody(const b2Body* pBody);
    void RemoveOverlappingKinematicBody(const b2Body* pBody);

private:
    // Configurable data read from XML
    bool m_CanClimb;
    bool m_CanBounce;
    bool m_CanJump;
    int32 m_MaxJumpHeight;
    float m_GravityScale;
    float m_Friction;
    float m_Density;

    Point m_BodySize;
    int32 m_NumFootContacts;

    bool m_IsClimbing;
    bool m_IsStopped;
    bool m_IsRunning;
    Direction m_Direction;

    bool m_IsFalling;
    bool m_IsJumping;
    float m_HeightInAir;

    bool m_HasConstantSpeed;
    Point m_ConstantSpeed;

    Point m_CurrentSpeed;

    Point m_ClimbingSpeed;

    // Hackerino to prevent loop jumping with space pressed
    bool m_IgnoreJump;

    ControllableComponent* m_pControllableComponent;

    shared_ptr<IGamePhysics> m_pPhysics;

    std::vector<const b2Body*> m_OverlappingKinematicBodiesList;
    std::vector<const b2Fixture*> m_OverlappingLaddersList;
    std::vector<const b2Fixture*> m_OverlappingGroundsList;
};

#endif