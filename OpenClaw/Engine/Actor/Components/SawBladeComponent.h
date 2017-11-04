#ifndef __SAW_BLADE_COMPONENT_H__
#define __SAW_BLADE_COMPONENT_H__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"

#include "AnimationComponent.h"

class DamageAuraComponent;
class SawBladeComponent : public ActorComponent, public AnimationObserver
{
public:
    SawBladeComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;
    virtual void VPostPostInit() override;

    virtual void VOnAnimationLooped(Animation* pAnimation) override;
    virtual void VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame) override;

private:
    // XML Data
    std::string m_UpAnimName;
    std::string m_DownAnimName;
    std::string m_SpinAnimName;
    int m_ActiveFrameIdx;
    int m_DeactivateFrameIdx;
    int m_TimeOff;
    int m_StartDelay;

    // State
    AnimationComponent* m_pAnimationComponent;
    DamageAuraComponent* m_pDamageAuraComponent;
    int m_CurrTimeOff;
};

#endif