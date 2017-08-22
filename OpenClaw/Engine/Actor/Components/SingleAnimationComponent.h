#ifndef __SINGLE_ANIMATION_COMPONENT__
#define __SINGLE_ANIMATION_COMPONENT__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "AnimationComponent.h"

class SingleAnimationComponent : public ActorComponent, public AnimationObserver
{
public:
    SingleAnimationComponent() { }

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual TiXmlElement* VGenerateXml() override;

    virtual void VPostInit() override;
    virtual void VPostPostInit() override;

    virtual void VOnAnimationAtLastFrame(Animation* pAnimation) override;

private:

};

#endif
