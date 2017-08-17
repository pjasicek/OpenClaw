#ifndef __EXPLODEABLE_COMPONENT_H__
#define __EXPLODEABLE_COMPONENT_H__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "ControllerComponents/HealthComponent.h"

class ExplodeableComponent : public ActorComponent, public HealthObserver
{
public:
    ExplodeableComponent();
    virtual ~ExplodeableComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }
    virtual void VPostInit() override;

    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;

    // Health observer
    virtual void VOnHealthBelowZero(DamageType damageType, int sourceActorId) override;

private:
    Point m_ExplosionSize;
    int32 m_ExplodingTime;
    int32 m_Damage;
};

#endif