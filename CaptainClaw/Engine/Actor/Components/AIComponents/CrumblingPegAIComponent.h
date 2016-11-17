#ifndef __CRUMBLINGPEGAICOMPONENT_H__
#define __CRUMBLINGPEGAICOMPONENT_H__

#include "../../../SharedDefines.h"
#include "../../ActorComponent.h"

#include <Box2D/Box2D.h>

class AnimationComponent;
class CrumblingPegAIComponent : public ActorComponent
{
public:
    CrumblingPegAIComponent();
    virtual ~CrumblingPegAIComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }
    virtual void VPostInit() override;
    virtual void VUpdate(uint32 msDiff) override;

    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;

    void OnContact(b2Body* pBody);

private:
    Point m_Size;

    uint32 m_PrevAnimframeIdx;

    AnimationComponent* m_pAnimationComponent;
    shared_ptr<IGamePhysics> m_pPhysics;
};

#endif