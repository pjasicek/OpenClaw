#ifndef __ENEMY_AI_STATE_COMPONENT_H__
#define __ENEMY_AI_STATE_COMPONENT_H__

#include "../../../SharedDefines.h"
#include "../../ActorComponent.h"
#include "../AnimationComponent.h"
#include "../ControllerComponents/HealthComponent.h"

enum EnemyAIState
{
    EnemyAIState_None,
    EnemyAIState_Patrolling,
    EnemyAIState_Parry,
    EnemyAIState_MeleeAttacking,
    EnemyAIState_DuckMeleeAttacking,
    EnemyAIState_RangedAttacking,
    EnemyAIState_DuckRangedAttacking,
    EnemyAIState_TakingDamage,
    EnemyAIState_Fleeing,
    EnemyAIState_Dying,
    EnemyAIState_BrainLaRaux,
    EnemyAIState_BrainKatherine
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
    virtual void VOnStateEnter() = 0;
    virtual void VOnStateLeave() = 0;
    virtual EnemyAIState VGetStateType() const = 0;

    // Can enemy enter this state ?
    virtual bool VCanEnter() = 0;

    // Priority of this state - the higher, the more important priority
    virtual int VGetPriority() { return m_StatePriority; }

protected:
    bool m_IsActive;
    int m_StatePriority;

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

    // This state is entered manually
    virtual bool VCanEnter() { return false; }

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
    virtual void VPostPostInit() override;

    virtual bool VCanEnter() { return true; }

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
// ParryEnemyAIStateComponent
//=====================================================================================================================
typedef std::map<DamageType, int> ParryChanceMap;
class ParryEnemyAIStateComponent : public BaseEnemyAIStateComponent, public AnimationObserver
{
public:
    ParryEnemyAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;

    // Only on-demand
    virtual bool VCanEnter() { return false; }

    // EnemyAIStateComponent API
    virtual void VOnStateEnter() override;
    virtual void VOnStateLeave() override;
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_Parry; }

    // AnimationObserver API
    virtual void VOnAnimationLooped(Animation* pAnimation) override;

    // Component specific
    bool CanParry(DamageType damageType, EnemyAIState currentState);

private:
    int m_ParryAnimFrameIdx;
    std::string m_ParryAnimation;
    std::vector<std::string> m_ParrySoundList;
    ParryChanceMap m_ParryChanceMap;
};

//=====================================================================================================================
// BaseAttackAIStateComponent
//=====================================================================================================================

typedef std::vector<std::shared_ptr<EnemyAttackAction>> AttackActionList;
class BaseAttackAIStateComponent : public BaseEnemyAIStateComponent, public AnimationObserver
{
public:
    BaseAttackAIStateComponent(std::string stateName);
    virtual ~BaseAttackAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }
    virtual void VPostInit() override;
    virtual void VPostPostInit() override;

    virtual bool VDelegateInit(TiXmlElement* pData) override;

    // EnemyAIStateComponent API
    virtual void VUpdate(uint32 msDiff) override { };
    virtual void VOnStateEnter() override;
    virtual void VOnStateLeave() override;
    virtual EnemyAIState VGetStateType() const = 0;
    virtual bool VCanEnter() override;

    virtual void VOnAnimationLooped(Animation* pAnimation) override;
    virtual void VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame) override;

    // BaseAttackAIStateComponent API
    virtual void VExecuteAttack();
    virtual void VOnAttackFrame(std::shared_ptr<EnemyAttackAction> pAttack, Direction dir, const Point& offset) = 0;

    void OnEnemyEnterAgroRange(Actor* pEnemy);
    void OnEnemyLeftAgroRange(Actor* pEnemy);

    Actor* FindClosestHostileActor();
    Point FindClosestHostileActorOffset();

private:
    void ClawHealthBelowZeroDelegate(IEventDataPtr pEventData);

    int m_AttackDelay;

protected:
    AttackActionList m_AttackActions;
    ActorList m_EnemyAgroList;
};

//=====================================================================================================================
// MeleeAttackAIStateComponent
//=====================================================================================================================
class MeleeAttackAIStateComponent : public BaseAttackAIStateComponent
{
public:
    MeleeAttackAIStateComponent();
    virtual ~MeleeAttackAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pData) override;

    // EnemyAIStateComponent API
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_MeleeAttacking; }

    // BaseAttackAIStateComponent API
    virtual void VOnAttackFrame(std::shared_ptr<EnemyAttackAction> pAttack, Direction dir, const Point& offset) override;
};

//=====================================================================================================================
// DuckMeleeAttackAIStateComponent
//=====================================================================================================================
class DuckMeleeAttackAIStateComponent : public BaseAttackAIStateComponent
{
public:
    DuckMeleeAttackAIStateComponent();
    virtual ~DuckMeleeAttackAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pData) override;

    // EnemyAIStateComponent API
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_DuckMeleeAttacking; }

    // BaseAttackAIStateComponent API
    virtual void VOnAttackFrame(std::shared_ptr<EnemyAttackAction> pAttack, Direction dir, const Point& offset) override;
};

//=====================================================================================================================
// RangedAttackAIStateComponent
//=====================================================================================================================
class RangedAttackAIStateComponent : public BaseAttackAIStateComponent
{
public:
    RangedAttackAIStateComponent();
    virtual ~RangedAttackAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pData) override;

    // EnemyAIStateComponent API
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_RangedAttacking; }

    // BaseAttackAIStateComponent API
    virtual void VOnAttackFrame(std::shared_ptr<EnemyAttackAction> pAttack, Direction dir, const Point& offset) override;
};

//=====================================================================================================================
// DuckRangedAttackAIStateComponent
//=====================================================================================================================
class DuckRangedAttackAIStateComponent : public BaseAttackAIStateComponent
{
public:
    DuckRangedAttackAIStateComponent();
    virtual ~DuckRangedAttackAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pData) override;

    // EnemyAIStateComponent API
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_DuckRangedAttacking; }

    // BaseAttackAIStateComponent API
    virtual void VOnAttackFrame(std::shared_ptr<EnemyAttackAction> pAttack, Direction dir, const Point& offset) override;
    virtual bool VCanEnter() override;
};

//=====================================================================================================================
// BaseBossAIStateComponennt
//=====================================================================================================================
class BaseBossAIStateComponennt : public BaseEnemyAIStateComponent, public HealthObserver
{
public:
    BaseBossAIStateComponennt(std::string stateName);
    virtual ~BaseBossAIStateComponennt();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;

    // EnemyAIStateComponent API
    virtual void VUpdate(uint32 msDiff) override = 0;
    virtual void VOnStateEnter() = 0;
    virtual void VOnStateLeave() = 0;
    virtual EnemyAIState VGetStateType() const = 0;

    // Can enemy enter this state ?
    virtual bool VCanEnter() = 0;

    virtual void VOnHealthChanged(int32 oldHealth, int32 newHealth, DamageType damageType, Point impactPoint) override;
    virtual void VOnHealthBelowZero(DamageType damageType) override;

private:
    void ActorEnteredBossAreaDelegate(IEventDataPtr pEvent);
    void BossFightStartedDelegate(IEventDataPtr pEvent);
    void BossFightEndedDelegate(IEventDataPtr pEvent);

    HealthComponent* m_pHealthComponent;

protected:
    virtual void VOnActorEnteredBossArea() { }
    virtual void VOnBossFightStarted() { }
    virtual void VOnBossFightEnded(bool isBossDead) { }

    bool m_bBossFightStarted;
    std::string m_BossDialogAnimation;
    Point m_DefaultPosition;
};

//=====================================================================================================================
// LaRauxBossAIStateComponent
//=====================================================================================================================
class LaRauxBossAIStateComponent : public BaseBossAIStateComponennt
{
public:
    LaRauxBossAIStateComponent();
    virtual ~LaRauxBossAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pData) override;

    // EnemyAIStateComponent API
    virtual void VUpdate(uint32 msDiff) override;
    virtual void VOnStateEnter() override;
    virtual void VOnStateLeave() override;
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_BrainLaRaux; }

    // Can enemy enter this state ?
    virtual bool VCanEnter() override { return m_bBossFightStarted == false; }

protected:
    virtual void VOnBossFightStarted() override;
    virtual void VOnBossFightEnded(bool isBossDead) override;
};

//=====================================================================================================================
// KatherineBossAIStateComponent
//=====================================================================================================================
class KatherineBossAIStateComponent : public BaseBossAIStateComponennt
{
public:
    KatherineBossAIStateComponent();
    virtual ~KatherineBossAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pData) override;

    // EnemyAIStateComponent API
    virtual void VUpdate(uint32 msDiff) override;
    virtual void VOnStateEnter() override;
    virtual void VOnStateLeave() override;
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_BrainKatherine; }

    // Can enemy enter this state ?
    virtual bool VCanEnter() override { return m_bBossFightStarted == false; }

protected:
    virtual void VOnBossFightStarted() override;
    virtual void VOnBossFightEnded(bool isBossDead) override;
};

#endif
