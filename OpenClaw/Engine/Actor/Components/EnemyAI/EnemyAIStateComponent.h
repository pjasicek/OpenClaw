#ifndef __ENEMY_AI_STATE_COMPONENT_H__
#define __ENEMY_AI_STATE_COMPONENT_H__

#include "../../../SharedDefines.h"
#include "../../ActorComponent.h"
#include "../AnimationComponent.h"
#include "../ControllerComponents/HealthComponent.h"
#include "../TriggerComponents/TriggerComponent.h"

enum EnemyAIState
{
    EnemyAIState_None,
    EnemyAIState_Patrolling,
    EnemyAIState_Parry,
    EnemyAIState_MeleeAttacking,
    EnemyAIState_DuckMeleeAttacking,
    EnemyAIState_RangedAttacking,
    EnemyAIState_DuckRangedAttacking,
    EnemyAIState_DiveAttacking,
    EnemyAIState_TakingDamage,
    EnemyAIState_Fleeing,
    EnemyAIState_Dying,
    EnemyAIState_Falling,
    EnemyAIState_Rolling,
    EnemyAIState_BrainLaRaux,
    EnemyAIState_BrainKatherine,
    EnemyAIState_BrainWolvington,
    EnemyAIState_PunkRat,
    EnemyAIState_BrainGabriel,
    EnemyAIState_BrainMarrow,
    EnemyAIState_BrainMarrowParrot,
    EnemyAIState_BrainAquatis,
    EnemyAIState_BrainRedTail,
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

template <typename T>
struct BossAction
{
    BossAction()
    {
        actionType = T(0);
        probability = 0;
        reuseTime = 0;
        timeSinceLastUse = 0;
    }

    bool CanBeUsed()
    {
        return reuseTime < timeSinceLastUse;
    }

    T actionType;
    int probability;
    int reuseTime;
    int timeSinceLastUse;
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
    BaseEnemyAIStateComponent(const std::string &stateName);
    virtual ~BaseEnemyAIStateComponent() { }

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual bool VDelegateInit(TiXmlElement* pData) = 0;
    virtual void VPostInit() override;

    bool IsActive() { return m_IsActive; }

    // EnemyAIStateComponent API

    // @pPreviousState can be NULL !
    virtual void VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState) = 0;

    // @pNextState can be NULL !
    virtual void VOnStateLeave(BaseEnemyAIStateComponent* pNextState) = 0;
    virtual EnemyAIState VGetStateType() const = 0;

    // Can enemy enter this state ?
    virtual bool VCanEnter() = 0;

    // Priority of this state - the higher, the more important priority
    virtual int VGetPriority() { return m_StatePriority; }

protected:
    bool IsActorWithinLOS(Actor* pActor);

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
    virtual bool VCanEnter() override { return false; }

    // EnemyAIStateComponent API
    virtual void VUpdate(uint32 msDiff) override;
    virtual void VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState) override;
    virtual void VOnStateLeave(BaseEnemyAIStateComponent* pNextState) override;
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_TakingDamage; }

    // AnimationObserver API
    virtual void VOnAnimationAtLastFrame(Animation* pAnimation) override;

private:
    TakeDamageAnimList m_TakeDamageAnimations;
};

//=====================================================================================================================
// FallAIStateComponent
//=====================================================================================================================
class FallAIStateComponent : public BaseEnemyAIStateComponent
{
public:
    FallAIStateComponent();
    virtual ~FallAIStateComponent() { }

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pData) override;

    // This state is entered manually
    virtual bool VCanEnter() override;

    // EnemyAIStateComponent API
    virtual void VUpdate(uint32 msDiff) override;
    virtual void VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState) override;
    virtual void VOnStateLeave(BaseEnemyAIStateComponent* pNextState) override;
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_Falling; }

private:
    std::string m_FallAnimation;
    bool m_bAlreadyFell;
};

//=====================================================================================================================
// PatrolEnemyAIStateComponent
//=====================================================================================================================
typedef std::map<std::string, EnemyAIAction> EnemyActionMap;
class PatrolEnemyAIStateComponent : public BaseEnemyAIStateComponent, public AnimationObserver, public TriggerObserver
{
public:
    PatrolEnemyAIStateComponent();
    virtual ~PatrolEnemyAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;
    virtual void VPostPostInit() override;

    virtual bool VCanEnter() override { return true; }

    // EnemyAIStateComponent API
    virtual void VUpdate(uint32 msDiff) override;
    virtual void VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState) override;
    virtual void VOnStateLeave(BaseEnemyAIStateComponent* pNextState) override;
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_Patrolling; }

    // AnimationObserver API
    virtual void VOnAnimationLooped(Animation* pAnimation) override;

    // TriggerObserver API
    virtual void VOnActorEnteredTrigger(Actor* pActorWhoEntered, FixtureType triggerType) override;
    virtual void VOnActorLeftTrigger(Actor* pActorWhoLeft, FixtureType triggerType) override;

    int GetLeftPatrolBorder() { return m_LeftPatrolBorder; }
    int GetRightPatrolBorder() { return m_RightPatrolBorder; }

private:
    void CalculatePatrolBorders();
    double FindClosestHole(const Point &center, int height, float maxSearchDistance);
    void ChangeDirection(Direction newDirection);
    void CommenceIdleBehaviour();
    bool TryChaseEnemy();

    bool m_bInitialized;
    bool m_bRetainDirection;
    bool m_bRecalculatePatrolBorders;

    bool m_bChaseEnemy;
    ActorFixtureDef m_ChaseEnemySensorFixtureDef;
    ActorList m_ChasedActorsList;

    int m_LeftPatrolBorder;
    int m_RightPatrolBorder;
    bool m_bIsFlying;

    double m_PatrolSpeed;

    bool m_IsAlwaysIdle;
    
    Direction m_Direction;

    std::unique_ptr<EnemyAIAction> m_pWalkAction;
    std::unique_ptr<EnemyAIAction> m_pIdleAction;

    std::shared_ptr<IGamePhysics> m_pPhysics;

    SoundList m_IdleSoundList;
    int m_IdleSpeechSoundMaxDistance;
    int m_IdleSpeechSoundPlayChance;
};

//=====================================================================================================================
// RollEnemyAIStateComponent
//=====================================================================================================================

class RollEnemyAIStateComponent : public BaseEnemyAIStateComponent, public AnimationObserver, public TriggerObserver, public HealthObserver
{
public:
    RollEnemyAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;
    virtual void VPostPostInit() override;
    virtual void VUpdate(uint32 msDiff) override;

    virtual bool VCanEnter() override;

    // EnemyAIStateComponent API
    virtual void VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState) override;
    virtual void VOnStateLeave(BaseEnemyAIStateComponent* pNextState) override;
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_Rolling; }

    // AnimationObserver
    virtual void VOnActorEnteredTrigger(Actor* pActorWhoEntered, FixtureType triggerType) override;
    virtual void VOnActorLeftTrigger(Actor* pActorWhoLeft, FixtureType triggerType) override;

    // TriggerObserver
    virtual bool VCanResistDamage(DamageType damageType, Point impactPoint) override;


    virtual void VOnAnimationLooped(Animation* pAnimation) override;

private:
    // XML Data
    double m_RollSpeed;
    std::string m_ForwardRollAnimation;
    std::string m_BackwardRollAnimation;
    ActorFixtureDef m_RollSensorFixtureDef;

    // Internal
    Direction DetermineRollDirection();

    Direction m_LastRollDirection;
    ActorList m_ActorsInLosList;
    int m_ReuseTimeLeft;
    PatrolEnemyAIStateComponent* m_pPatrolStateComponent;
    shared_ptr<IGamePhysics> m_pPhysics;
};

//=====================================================================================================================
// ParryEnemyAIStateComponent
//=====================================================================================================================

struct IgnoreParryInAnim
{
    IgnoreParryInAnim()
    {
        minAnimIdx = 0;
    }

    std::string animationName;
    int minAnimIdx;
};

typedef std::map<DamageType, int> ParryChanceMap;
typedef std::vector<IgnoreParryInAnim> IgnoreParryInAnimList;
class ParryEnemyAIStateComponent : public BaseEnemyAIStateComponent, public AnimationObserver
{
public:
    ParryEnemyAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;

    // Only on-demand
    virtual bool VCanEnter() override { return false; }

    // EnemyAIStateComponent API
    virtual void VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState) override;
    virtual void VOnStateLeave(BaseEnemyAIStateComponent* pNextState) override;
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
    IgnoreParryInAnimList m_IgnoreParryInAnimList;
};

//=====================================================================================================================
// BaseAttackAIStateComponent
//=====================================================================================================================

typedef std::vector<std::shared_ptr<EnemyAttackAction>> AttackActionList;
class BaseAttackAIStateComponent : public BaseEnemyAIStateComponent, public AnimationObserver
{
public:
    BaseAttackAIStateComponent(const std::string &stateName);
    virtual ~BaseAttackAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }
    virtual void VPostInit() override;
    virtual void VPostPostInit() override;

    virtual bool VDelegateInit(TiXmlElement* pData) override;

    // EnemyAIStateComponent API
    virtual void VUpdate(uint32 msDiff) override { };
    virtual void VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState) override;
    virtual void VOnStateLeave(BaseEnemyAIStateComponent* pNextState) override;
    virtual EnemyAIState VGetStateType() const override = 0;
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

    SoundList m_AttackSpeechSoundList;
    int m_AttackSpeechSoundPlayChance;

    int m_CurrentAttackActionIdx;

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
// DiveAttackAIStateComponent
//=====================================================================================================================
// Lots of duplicated code here, should have better design
class DiveAttackAIStateComponent : public BaseEnemyAIStateComponent
{
    enum DiveState
    {
        DiveState_None,
        DiveState_DivingIn,
        DiveState_DivingOut
    };

public:
    DiveAttackAIStateComponent();
    virtual ~DiveAttackAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }
    virtual void VPostPostInit() override;

    virtual bool VDelegateInit(TiXmlElement* pData) override;

    virtual void VUpdate(uint32 msDiff) override;

    // EnemyAIStateComponent API
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_DiveAttacking; }
    virtual void VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState) override;
    virtual void VOnStateLeave(BaseEnemyAIStateComponent* pNextState) override;
    virtual bool VCanEnter() override;

    void OnEnemyEnterAgroRange(Actor* pActor);
    void OnEnemyLeftAgroRange(Actor* pActor);

private:
    // XML
    std::string m_DiveSound;
    std::string m_DiveInAnimation;
    std::string m_DiveOutAnimation;
    float m_DiveSpeed;
    std::vector<ActorFixtureDef> m_DiveAreaSensorFixtureList;

    // Internal state
    DiveState m_DiveState;
    Point m_InitialPosition;
    Point m_Destination;

    ActorList m_EnemyAgroList;
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
    virtual void VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState) override = 0;
    virtual void VOnStateLeave(BaseEnemyAIStateComponent* pNextState) override = 0;
    virtual EnemyAIState VGetStateType() const override = 0;

    // Can enemy enter this state ?
    virtual bool VCanEnter() = 0;

    virtual void VOnHealthChanged(int32 oldHealth, int32 newHealth, DamageType damageType, Point impactPoint, int sourceActorId) override;
    virtual void VOnHealthBelowZero(DamageType damageType, int sourceActorId) override;

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
    virtual void VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState) override;
    virtual void VOnStateLeave(BaseEnemyAIStateComponent* pNextState) override;
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
    virtual void VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState) override;
    virtual void VOnStateLeave(BaseEnemyAIStateComponent* pNextState) override;
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_BrainKatherine; }

    // Can enemy enter this state ?
    virtual bool VCanEnter() override { return m_bBossFightStarted == false; }

protected:
    virtual void VOnBossFightStarted() override;
    virtual void VOnBossFightEnded(bool isBossDead) override;
};

//=====================================================================================================================
// WolvingtonBossAIStateComponent
//=====================================================================================================================
class WolvingtonBossAIStateComponent : public BaseBossAIStateComponennt
{
public:
    WolvingtonBossAIStateComponent();
    virtual ~WolvingtonBossAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pData) override;

    // EnemyAIStateComponent API
    virtual void VUpdate(uint32 msDiff) override { }
    virtual void VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState) override { }
    virtual void VOnStateLeave(BaseEnemyAIStateComponent* pNextState) override { }
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_BrainWolvington; }

    // Can enemy enter this state ?
    virtual bool VCanEnter() override { return m_bBossFightStarted == false; }

protected:
    virtual void VOnBossFightStarted() override;
    virtual void VOnBossFightEnded(bool isBossDead) override;
};

#endif
