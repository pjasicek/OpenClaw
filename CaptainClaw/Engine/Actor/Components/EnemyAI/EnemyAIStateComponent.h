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
    std::string actionName;

    uint32 activeAnimIdx;
    std::vector<std::string> animations;
};

class PhysicsComponent;
class PositionComponent;

//=====================================================================================================================
// BaseEnemyAIStateComponent
//=====================================================================================================================
class BaseEnemyAIStateComponent : public ActorComponent
{
public:
    BaseEnemyAIStateComponent() : m_IsActive(false) { }

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
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

    PhysicsComponent* m_pPhysicsComponent;
    PositionComponent* m_pPositionComponent;
    AnimationComponent* m_pAnimationComponent;
};

//=====================================================================================================================
// PatrolEnemyAIStateComponent
//=====================================================================================================================
typedef std::map<std::string, EnemyAIAction> EnemyActionMap;
class PatrolEnemyAIStateComponent : public BaseEnemyAIStateComponent, public AnimationObserver
{
public:
    PatrolEnemyAIStateComponent();


    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;

    // EnemyAIStateComponent API
    virtual void VUpdate(uint32 msDiff);
    virtual void VOnStateEnter();
    virtual void VOnStateLeave();
    virtual EnemyAIState VGetStateType() const { return EnemyAIState_Patrolling; }

    // AnimationObserver API
    virtual void VOnAnimationLooped(Animation* pAnimation) override;

private:
    void CalculatePatrolBorders();

    int m_LeftPatrolBorder;
    int m_RightPatrolBorder;
    
    Direction m_Direction;

    EnemyAIAction* m_pCurrentAction;
    EnemyActionMap m_ActionMap;
};

#endif