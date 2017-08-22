#ifndef __CRUMBLINGPEGAICOMPONENT_H__
#define __CRUMBLINGPEGAICOMPONENT_H__

#include "../../../SharedDefines.h"
#include "../../ActorComponent.h"
#include "../AnimationComponent.h"

#include <Box2D/Box2D.h>

class AnimationComponent;
class CrumblingPegAIComponent : public ActorComponent, public AnimationObserver
{
public:
    CrumblingPegAIComponent();
    virtual ~CrumblingPegAIComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }
    virtual void VPostInit() override;

    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;

    void OnContact(b2Body* pBody);

    // AnimationObserver interface
    virtual void VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame) override;
    virtual void VOnAnimationLooped(Animation* pAnimation) override;

    void ClawDiedDelegate(IEventDataPtr pEventData);

private:
    shared_ptr<IGamePhysics> m_pPhysics;

    CrumblingPegDef m_Properties;
};

#endif
