#ifndef __PROJECTILE_SPAWNER_COMPONENT_H__
#define __PROJECTILE_SPAWNER_COMPONENT_H__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "TriggerComponents/TriggerComponent.h"
#include "AnimationComponent.h"

class ActorRenderComponent;
class ProjectileSpawnerComponent : public ActorComponent, public TriggerObserver, public AnimationObserver
{
public:
    ProjectileSpawnerComponent();
    virtual ~ProjectileSpawnerComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual TiXmlElement* VGenerateXml() override;

    virtual void VPostInit() override;
    virtual void VPostPostInit() override;

    virtual void VUpdate(uint32 msDiff) override;

    virtual void VOnActorEnteredTrigger(Actor* pActorWhoEntered, FixtureType triggerType) override;
    virtual void VOnActorLeftTrigger(Actor* pActorWhoLeft, FixtureType triggerType) override;

    virtual void VOnAnimationLooped(Animation* pAnimation) override;
    virtual void VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame) override;
    virtual void VOnAnimationEndedDelay(Animation* pAnimation) override;

private:
    bool TryToFire();
    bool IsFiring();
    void SpawnProjectile();
        
    ProjectileSpawnerDef m_Properties;

    int m_ActorsInTriggerArea;
    AnimationComponent* m_pAnimationComponent;
    ActorRenderComponent* m_pARC;

    bool m_bReady;
    int m_StartDelayLeft;
};



#endif