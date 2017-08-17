#ifndef __DESTROYABLE_AI_COMPONENT_H__
#define __DESTROYABLE_AI_COMPONENT_H__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "ControllerComponents/HealthComponent.h"
#include "AnimationComponent.h"

class ActorRenderComponent;
class DestroyableComponent : public ActorComponent, public HealthObserver, public AnimationObserver
{
public:
    DestroyableComponent();
    virtual ~DestroyableComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }
    virtual void VPostInit() override;

    virtual void VUpdate(uint32 msDiff) override;

    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;

    // Health observer
    virtual void VOnHealthBelowZero(DamageType damageType, int sourceActorId) override;

    // Animation observer
    virtual void VOnAnimationAtLastFrame(Animation* pAnimation) override;

private:
    void DeleteActor();

    // XML properties
    int m_DeleteDelay;
    bool m_bDeleteImmediately;
    bool m_bBlinkOnDestruction;
    bool m_bDeleteOnDestruction;
    bool m_bRemoveFromPhysics;
    std::string m_DeathAnimationName;
    std::vector<std::string> m_PossibleDestructionSounds;

    // Internal members
    bool m_bIsDead;
    int m_DeleteDelayTimeLeft;
    shared_ptr<IGamePhysics> m_pPhysics;

    ActorRenderComponent* m_pRenderComponent;
};

#endif