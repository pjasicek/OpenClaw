#ifndef __DESTROYABLE_AI_COMPONENT_H__
#define __DESTROYABLE_AI_COMPONENT_H__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "ControllerComponents/HealthComponent.h"
#include "AnimationComponent.h"

class DestroyableComponent : public ActorComponent, public HealthObserver, public AnimationObserver
{
public:
    DestroyableComponent();
    virtual ~DestroyableComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }
    virtual void VPostInit() override;

    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;

    // Health observer
    virtual void VOnHealthBelowZero(DamageType damageType) override;

    // Animation observer
    virtual void VOnAnimationAtLastFrame(Animation* pAnimation) override;

private:
    // XML properties
    bool m_bDeleteOnDestruction;
    bool m_bRemoveFromPhysics;
    std::string m_DeathAnimationName;
    std::vector<std::string> m_PossibleDestructionSounds;

    // Internal members
    bool m_bIsDead;
    shared_ptr<IGamePhysics> m_pPhysics;
};

#endif