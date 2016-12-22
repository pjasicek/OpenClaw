#ifndef __ENEMY_AI_COMPONENT_H__
#define __ENEMY_AI_COMPONENT_H__

#include "../../../SharedDefines.h"
#include "../../ActorComponent.h"

#include "../ControllerComponents/HealthComponent.h"

class BaseEnemyAIStateComponent;
class PositionComponent;
class ActorRenderComponent;

typedef std::map<std::string, BaseEnemyAIStateComponent*> EnemyStateMap;
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

private:
    bool m_bInitialized;
    bool m_bDead;
    EnemyStateMap m_StateMap;

    shared_ptr<PositionComponent> m_pPositionComponent;
    shared_ptr<ActorRenderComponent> m_pRenderComponent;
};

#endif