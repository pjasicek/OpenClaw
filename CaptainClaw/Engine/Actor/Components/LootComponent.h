#ifndef __LOOT_COMPONENT_H__
#define __LOOT_COMPONENT_H__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "ControllerComponents/HealthComponent.h"

class LootComponent : public ActorComponent, public HealthObserver
{
public:
    LootComponent() { }

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }
    virtual void VPostInit() override;

    virtual bool VInit(TiXmlElement* pData) override;
    virtual TiXmlElement* VGenerateXml() override;

    virtual void VOnHealthBelowZero() override;

private:
    std::vector<PickupType> m_Loot;
};

#endif