#ifndef __SPARKLE_COMPONENT_H__
#define __SPARKLE_COMPONENT_H__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "ControllerComponents/HealthComponent.h"

class PositionComponent;
class GlitterComponent : public ActorComponent, public HealthObserver
{
public:
    GlitterComponent();
    ~GlitterComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;
    virtual TiXmlElement* VGenerateXml() override;

    virtual void VUpdate(uint32 msDiff) override;

    void Deactivate();

private:
    // XML
    bool m_SpawnImmediate;
    bool m_FollowOwner;
    std::string m_GlitterType;

    PositionComponent* m_pPositonComponent;
    StrongActorPtr m_pGlitter;
    bool m_Active;
};

#endif