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
    BaseEnemyAIStateComponent(std::string stateName) : m_IsActive(false), m_StateName(stateName) { }
    virtual ~BaseEnemyAIStateComponent() { }

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual bool VDelegateInit(TiXmlElement* pData) = 0;
    virtual void VPostInit() override;

    virtual TiXmlElement* VGenerateXml() { return NULL; }

    bool IsActive() { return m_IsActive; }

    // EnemyAIStateComponent API
    virtual void VUpdate(uint32 msDiff) = 0;
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
// PatrolEnemyAIStateComponent
//=====================================================================================================================
typedef std::map<std::string, EnemyAIAction> EnemyActionMap;
class PatrolEnemyAIStateComponent : public BaseEnemyAIStateComponent, public AnimationObserver
{
public:
    PatrolEnemyAIStateComponent();
    virtual ~PatrolEnemyAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pData) override;
    virtual void VPostInit();

    // EnemyAIStateComponent API
    virtual void VUpdate(uint32 msDiff);
    virtual void VOnStateEnter();
    virtual void VOnStateLeave();
    virtual EnemyAIState VGetStateType() const { return EnemyAIState_Patrolling; }

    // AnimationObserver API
    virtual void VOnAnimationLooped(Animation* pAnimation) override;

private:
    void CalculatePatrolBorders();
    double FindClosestHole(Point center, int height, float maxSearchDistance);
    void ChangeDirection(Direction newDirection);
    void CommenceIdleBehaviour();

    bool m_bInitialized;

    int m_LeftPatrolBorder;
    int m_RightPatrolBorder;

    double m_PatrolSpeed;
    
    Direction m_Direction;

    std::unique_ptr<EnemyAIAction> m_pWalkAction;
    std::unique_ptr<EnemyAIAction> m_pIdleAction;

    std::shared_ptr<IGamePhysics> m_pPhysics;

    //EnemyAIAction* m_pCurrentAction;
    //EnemyActionMap m_ActionMap;
};

#endif