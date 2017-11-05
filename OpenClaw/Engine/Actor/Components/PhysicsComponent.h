#ifndef PHYSICSCOMPONENT_H_
#define PHYSICSCOMPONENT_H_

#include <Box2D/Box2D.h>
#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "../Actor.h"

class ControllableComponent;
class PhysicsComponent : public ActorComponent
{
public:
    PhysicsComponent();
    virtual ~PhysicsComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;
    virtual void VPostInit() override;
    virtual void VPostPostInit() override;

    virtual void VUpdate(uint32 msDiff) override;

    void Destroy() { m_pPhysics->VRemoveActor(m_pOwner->GetGUID()); }

    void SetControllableComponent(ControllableComponent* pComp);

    Direction GetDirection() { return m_Direction; }
    void SetDirection(Direction newDirection);

    bool CanBounce() const { return m_CanBounce; }
    void SetCanBounce(bool canBounce) { m_CanBounce = canBounce; }

    bool CanJump() const { return m_CanJump; }
    void SetCanJump(bool canJump) { m_CanJump = canJump; }

    int32 GetMaxJumpHeight() const { return m_MaxJumpHeight; }
    void SetMaxJumpHeight(int32 maxJumpHeight);

    void SetGravityScale(float gravityScale) { m_pPhysics->VSetGravityScale(m_pOwner->GetGUID(), gravityScale); }
    float GetGravityScale() { return m_GravityScale; }
    void RestoreGravityScale() { m_pPhysics->VSetGravityScale(m_pOwner->GetGUID(), m_GravityScale); }
    float GetFriction() { return m_Friction; }
    float GetDensity() { return m_Density; }

    Point GetBodySize() const { return m_ActorBodyDef.size; }
    double GetBodyWidth() const { return m_ActorBodyDef.size.x; }
    double GetBodyHeight() const { return m_ActorBodyDef.size.y; }

    void SetCurrentSpeed(Point speed) { m_CurrentSpeed = speed; }
    void SetConstantSpeed(Point speed) { m_ConstantSpeed = speed; }
    bool HasConstantSpeed() { return m_HasConstantSpeed; }
    void StopMovement() { m_CurrentSpeed = 0; m_ConstantSpeed = 0; }

    void RequestClimb(Point climbMovement);

    void OnBeginFootContact();
    void OnEndFootContact();
    void OnStartFalling();
    void OnStartJumping();

    void OnAttachedToRope();
    void OnDetachedFromRope();

    int32 GetNumFootContacts() { return m_NumFootContacts; }
    bool IsOnGround() { return m_NumFootContacts > 0; }
    bool IsInAir() { return m_IsFalling || m_IsJumping; }
    bool IsFalling() { return m_IsFalling; }
    bool IsJumping() { return m_IsJumping; }
    bool IsClimbing() { return m_IsClimbing; }
    bool IsOnLadder() { return m_OverlappingLaddersList.size() > 0; }
    Point GetClimbingSpeed() { return m_ClimbingSpeed; }

    void SetFalling(bool falling);
    void SetJumping(bool jumping);

    void AddJumpHeight(float deltaHeight) { m_HeightInAir += deltaHeight; }
    float GetHeightInAir() { return m_HeightInAir; }
    void SetHeightInAir(float heightInAir) { m_HeightInAir = heightInAir; }

    void AddFallHeight(float deltaFallHeight) { m_FallHeight += deltaFallHeight; }
    void SetFallHeight(float fallHeight) { m_FallHeight = fallHeight; }
    float GetFallHeight() { return m_FallHeight; }

    void SetForceFall();

    bool AttachToLadder();

    bool CheckOverlap(FixtureType withWhatFixture);

    void SetExternalSourceSpeed(const Point& externalSpeed) { m_ExternalSourceSpeed = externalSpeed; }
    void SetMovingPlatformContact(b2Contact* pContact) { m_pMovingPlatformContact = pContact; }

    // Communication with IGamePhysics
    Point GetVelocity() { return m_pPhysics->VGetVelocity(m_pOwner->GetGUID()); }
    void SetVelocity(Point velocity) { m_pPhysics->VSetLinearSpeed(m_pOwner->GetGUID(), velocity); }
    
    void ApplyLinearImpulse(Point impulse) { m_pPhysics->VApplyLinearImpulse(m_pOwner->GetGUID(), impulse); }
    void ApplyForce(Point force) { m_pPhysics->VApplyForce(m_pOwner->GetGUID(), force); }

    bool IsAwake() { return m_pPhysics->VIsAwake(m_pOwner->GetGUID()); }

    void SetIsForcedUp(bool isForcedUp, int forcedUpHeight);
    bool GetIsForcedUp() { return m_bIsForcedUp; }

    void AddOverlappingLadder(const b2Fixture* ladder) { m_OverlappingLaddersList.push_back(ladder); }
    void RemoveOverlappingLadder(const b2Fixture* ladder);

    void AddOverlappingGround(const b2Fixture* pGround);
    void RemoveOverlappingGround(const b2Fixture* pGround);

    void AddOverlappingKinematicBody(const b2Body* pBody);
    void RemoveOverlappingKinematicBody(const b2Body* pBody);

    void SetTopLadderContact(b2Contact* pContact) { m_pTopLadderContact = pContact; }
    b2Contact* GetTopLadderContact() { return m_pTopLadderContact; }

    void SetExternalConveyorBeltSpeed(const Point& extSpeed) { m_ExternalConveyorBeltSpeed = extSpeed; }

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

    float m_FallHeight;

    bool m_HasConstantSpeed;
    Point m_ConstantSpeed;

    Point m_CurrentSpeed;
    Point m_ExternalSourceSpeed;
    Point m_ExternalConveyorBeltSpeed;

    Point m_ClimbingSpeed;

    // Hackerino to prevent loop jumping with space pressed
    bool m_IgnoreJump;

    ControllableComponent* m_pControllableComponent;

    shared_ptr<IGamePhysics> m_pPhysics;

    int m_DoNothingTimeout;

    // Actor body definition for physics body creation
    ActorBodyDef m_ActorBodyDef;
    bool m_bClampToGround;

    // Spring caused us to go up
    bool m_bIsForcedUp;
    int m_ForcedUpHeight;

    b2Contact* m_pTopLadderContact;
    b2Contact* m_pMovingPlatformContact;

    std::vector<const b2Body*> m_OverlappingKinematicBodiesList;
    std::vector<const b2Fixture*> m_OverlappingLaddersList;
    std::vector<const b2Fixture*> m_OverlappingGroundsList;
};

#endif
