#ifndef __STEPPING_GROUND_COMPONENT_H__
#define __STEPPING_GROUND_COMPONENT_H__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "AnimationComponent.h"

class SteppingGroundComponent : public ActorComponent, public AnimationObserver
{
public:
    SteppingGroundComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;

    virtual TiXmlElement* VGenerateXml() override { assert(false && "Unimplemented"); return NULL; }

    void OnActorContact(Actor* pActor);

    virtual void VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame) override;
    virtual void VOnAnimationAtLastFrame(Animation* pAnimation) override;

private:
    // XML Data
    SteppingGroundDef m_Properties;

    // Internal properties
    bool m_bIsSteppedOn;
    int m_TimeBeforeToggleLeft;
    int m_TimeOffLeft;

    shared_ptr<IGamePhysics> m_pPhysics;
    AnimationComponent* m_pAnimationComponent;
};

#endif