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
    virtual const char* VGetName() const { return g_Name; }
    virtual void VPostInit() override;

    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;

    // Health observer
    virtual void VOnHealthBelowZero() override;

    // Animation observer
    virtual void VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame) override;

private:
    bool m_DeleteOnDestruction;
    std::vector<std::string> m_PossibleDestructionSounds;
    shared_ptr<IGamePhysics> m_pPhysics;
};

#endif