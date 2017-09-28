#ifndef __ACTOR_SPAWNER_COMPONENT_H__
#define __ACTOR_SPAWNER_COMPONENT_H__

#include "../ActorComponent.h"
#include "PickupComponents//PickupComponent.h"
#include "TriggerComponents/TriggerComponent.h"
#include "AnimationComponent.h"

//=====================================================================================================================
// ActorSpawnerComponent
//=====================================================================================================================

typedef std::vector<ActorSpawnInfo> ActorSpawnInfoList;

class ActorSpawnerComponent : public ActorComponent, public TriggerObserver, public AnimationObserver
{
public:
    ActorSpawnerComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual void VPostInit() override;

    virtual void VOnActorEnteredTrigger(Actor* pActorWhoEntered, FixtureType triggerType) override;

    virtual void VOnAnimationLooped(Animation* pAnimation) override;

private:
    ActorSpawnInfoList m_ActorSpawnInfoList;
    std::string m_SpawnAnimation;

    TriggerComponent* m_pTriggerComponent;
};

#endif