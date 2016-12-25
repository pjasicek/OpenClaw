#ifndef __ENEMY_AI_COMPONENT_H__
#define __ENEMY_AI_COMPONENT_H__

#include "../../../SharedDefines.h"
#include "../../ActorComponent.h"

#include "../ControllerComponents/HealthComponent.h"
#include "EnemyAIStateComponent.h"

class BaseEnemyAIStateComponent;
class PositionComponent;
class ActorRenderComponent;

typedef std::map<std::string, BaseEnemyAIStateComponent*> EnemyStateMap;
typedef std::vector<Actor*> ActorList;
class EnemyAIComponent : public ActorComponent, public HealthObserver
{
public:
    EnemyAIComponent();
    ~EnemyAIComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;
    virtual void VUpdate(uint32 msDiff) override;

    virtual TiXmlElement* VGenerateXml() { return NULL; }

    void RegisterState(std::string stateName, BaseEnemyAIStateComponent* pState) { m_StateMap[stateName] = pState; }

    virtual void VOnHealthBelowZero() override;

    void OnEnemyEnteredMeleeZone(Actor* pEnemy);
    void OnEnemyLeftMeleeZone(Actor* pEnemy);

    void OnEnemyEnteredRangedZone(Actor* pEnemy);
    void OnEnemyLeftRangedZone(Actor* pEnemy);

    Point FindClosestHostileActorOffset();

    void OnStateCanFinish();

private:
    void LeaveAllStates();
    bool HasState(std::string stateName);
    void EnterState(std::string stateName);
    void AcquireStateLock();
    EnemyAIState GetCurrentState();

    bool m_bInitialized;
    bool m_bDead;
    bool m_bHasStateLock;
    EnemyStateMap m_StateMap;

    std::string m_DeathAnimation;

    shared_ptr<PositionComponent> m_pPositionComponent;
    shared_ptr<ActorRenderComponent> m_pRenderComponent;

    ActorList m_EnemiesInMeleeZone;
    ActorList m_EnemiesInRangedZone;
};

#endif