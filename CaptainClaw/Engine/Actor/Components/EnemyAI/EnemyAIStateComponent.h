#ifndef __ENEMY_AI_STATE_COMPONENT_H__
#define __ENEMY_AI_STATE_COMPONENT_H__

#include "../../../SharedDefines.h"
#include "../../ActorComponent.h"
#include "../AnimationComponent.h"

enum EnemyAIState
{
    EnemyAIState_None,
    EnemyAIState_Patrolling,
    EnemyAIState_MeleeAttacking,
    EnemyAIState_RangedAttacking,
    EnemyAIState_TakingDamage,
    EnemyAIState_Fleeing,
    EnemyAIState_Dying,
};

struct EnemyAIAction
{
    EnemyAIAction()
    {
        activeAnimIdx = 0;
        actionName = "Unknown";
        isActive = false;
        animDelay = 0;
    }

    bool IsAtLastAnimation()
    {
        return (activeAnimIdx == (animations.size() - 1));
    }

    std::string actionName;
    uint32 animDelay;
    bool isActive;
    bool retainDirection;
    uint32 activeAnimIdx;
    std::vector<std::string> animations;
};

class PhysicsComponent;
class PositionComponent;
class EnemyAIComponent;
class ActorRenderComponent;

//=====================================================================================================================
// BaseEnemyAIStateComponent
//=====================================================================================================================
class BaseEnemyAIStateComponent : public ActorComponent
{
public:
    BaseEnemyAIStateComponent(std::string stateName);
    virtual ~BaseEnemyAIStateComponent() { }

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual bool VDelegateInit(TiXmlElement* pData) = 0;
    virtual void VPostInit() override;

    virtual TiXmlElement* VGenerateXml() override { return NULL; }

    bool IsActive() { return m_IsActive; }

    // EnemyAIStateComponent API
    virtual void VUpdate(uint32 msDiff) override = 0;
    virtual void VOnStateEnter() = 0;
    virtual void VOnStateLeave() = 0;
    virtual EnemyAIState VGetStateType() const = 0;

protected:
    bool m_IsActive;

    std::string m_StateName;

    PhysicsComponent* m_pPhysicsComponent;
    PositionComponent* m_pPositionComponent;
    AnimationComponent* m_pAnimationComponent;
    EnemyAIComponent* m_pEnemyAIComponent;
    ActorRenderComponent* m_pRenderComponent;
};

//=====================================================================================================================
// TakeDamageAIStateComponent
//=====================================================================================================================
typedef std::vector<std::string> TakeDamageAnimList;
class TakeDamageAIStateComponent : public BaseEnemyAIStateComponent, public AnimationObserver
{
public:
    TakeDamageAIStateComponent();
    virtual ~TakeDamageAIStateComponent() { }

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }
    virtual void VPostInit() override;

    virtual bool VDelegateInit(TiXmlElement* pData) override;

    // EnemyAIStateComponent API
    virtual void VUpdate(uint32 msDiff) override;
    virtual void VOnStateEnter() override;
    virtual void VOnStateLeave() override;
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_TakingDamage; }

    // AnimationObserver API
    virtual void VOnAnimationAtLastFrame(Animation* pAnimation) override;

private:
    TakeDamageAnimList m_TakeDamageAnimations;
};

//=====================================================================================================================
// PatrolEnemyAIStateComponent
//=====================================================================================================================
typedef std::map<std::string, EnemyAIAction> EnemyActionMap;
class PatrolEnemyAIStateComponent : public BaseEnemyAIStateComponent, public AnimationObserver
{
public:
    PatrolEnemyAIStateComponent();
    virtual ~PatrolEnemyAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;

    // EnemyAIStateComponent API
    virtual void VUpdate(uint32 msDiff) override;
    virtual void VOnStateEnter() override;
    virtual void VOnStateLeave() override;
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_Patrolling; }

    // AnimationObserver API
    virtual void VOnAnimationLooped(Animation* pAnimation) override;

private:
    void CalculatePatrolBorders();
    double FindClosestHole(Point center, int height, float maxSearchDistance);
    void ChangeDirection(Direction newDirection);
    void CommenceIdleBehaviour();

    bool m_bInitialized;
    bool m_bRetainDirection;

    int m_LeftPatrolBorder;
    int m_RightPatrolBorder;

    double m_PatrolSpeed;

    bool m_IsAlwaysIdle;
    
    Direction m_Direction;

    std::unique_ptr<EnemyAIAction> m_pWalkAction;
    std::unique_ptr<EnemyAIAction> m_pIdleAction;

    std::shared_ptr<IGamePhysics> m_pPhysics;

    //EnemyAIAction* m_pCurrentAction;
    //EnemyActionMap m_ActionMap;
};

//=====================================================================================================================
// MeleeAttackAIStateComponent
//=====================================================================================================================
typedef std::vector<std::shared_ptr<EnemyAttackAction>> AttackActionList;
class MeleeAttackAIStateComponent : public BaseEnemyAIStateComponent, public AnimationObserver
{
public:
    MeleeAttackAIStateComponent();
    virtual ~MeleeAttackAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }
    virtual void VPostInit() override;

    virtual bool VDelegateInit(TiXmlElement* pData) override;

    // EnemyAIStateComponent API
    virtual void VUpdate(uint32 msDiff) override;
    virtual void VOnStateEnter() override;
    virtual void VOnStateLeave() override;
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_MeleeAttacking; }

    // AnimationObserver API
    virtual void VOnAnimationLooped(Animation* pAnimation) override;
    virtual void VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame) override;

private:
    void ExecuteMeleeAttack();

    AttackActionList m_MeleeAttackActions;
};

//=====================================================================================================================
// RangedAttackAIStateComponent
//=====================================================================================================================
class RangedAttackAIStateComponent : public BaseEnemyAIStateComponent, public AnimationObserver
{
public:
    RangedAttackAIStateComponent();
    virtual ~RangedAttackAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }
    virtual void VPostInit() override;

    virtual bool VDelegateInit(TiXmlElement* pData) override;

    // EnemyAIStateComponent API
    virtual void VUpdate(uint32 msDiff) override;
    virtual void VOnStateEnter() override;
    virtual void VOnStateLeave() override;
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_RangedAttacking; }

    // AnimationObserver API
    virtual void VOnAnimationLooped(Animation* pAnimation) override;
    virtual void VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame) override;

private:
    void ExecuteRangedAttack();

    AttackActionList m_RangedAttackActions;
};

#endif
