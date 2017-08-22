#ifndef __PROJECTILE_SPAWNER_COMPONENT_H__
#define __PROJECTILE_SPAWNER_COMPONENT_H__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "TriggerComponents/TriggerComponent.h"
#include "AnimationComponent.h"

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

    virtual void VOnActorEnteredTrigger(Actor* pActorWhoEntered) override;
    virtual void VOnActorLeftTrigger(Actor* pActorWhoLeft) override;

    virtual void VOnAnimationLooped(Animation* pAnimation) override;
    virtual void VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame) override;

private:
    bool TryToFire();
    bool IsFiring() { return m_pAnimationComponent->GetCurrentAnimationName() == m_Properties.fireAnim; }
        
    ProjectileSpawnerDef m_Properties;

    int m_ActorsInTriggerArea;
    AnimationComponent* m_pAnimationComponent;

    bool m_bReady;
    int m_StartDelayLeft;
};



#endif