#ifndef __TOGGLEPEGAICOMPONENT_H__
#define __TOGGLEPEGAICOMPONENT_H__

#include "../../../SharedDefines.h"
#include "../../ActorComponent.h"

class AnimationComponent;
class TogglePegAIComponent : public ActorComponent
{
public:
    TogglePegAIComponent();
    virtual ~TogglePegAIComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }
    virtual void VPostInit() override;
    virtual void VUpdate(uint32 msDiff) override;

    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;

private:
    Point m_Size;
    double m_TimeOff;
    double m_TimeOn;
    uint32 m_Delay;
    bool m_IsAlwaysActive;

    uint32 m_PrevAnimframeIdx;

    AnimationComponent* m_pAnimationComponent;
    shared_ptr<IGamePhysics> m_pPhysics;
};

#endif