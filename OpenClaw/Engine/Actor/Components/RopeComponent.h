#ifndef __ROPE_COMPONENT_H__
#define __ROPE_COMPONENT_H__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "TriggerComponents/TriggerComponent.h"
#include "AnimationComponent.h"

class RopeComponent : public ActorComponent, public TriggerObserver, public AnimationObserver
{
public:
    RopeComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;
    virtual void VPostPostInit() override;

    virtual void VUpdate(uint32 msDiff) override;

    virtual TiXmlElement* VGenerateXml() override { assert(false && "Unimplemented"); return NULL; }

    virtual void VOnActorEnteredTrigger(Actor* pActorWhoEntered, FixtureType triggerType) override;
    virtual void VOnActorLeftTrigger(Actor* pActorWhoLeft, FixtureType triggerType) override;

    virtual void VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame) override;

private:
    void UpdateAttachedActorPosition(const Point& newPosition);
    void DetachActor();

    // Internal state
    int m_TimeStanceAttach;
    Actor* m_pAttachedActor;
    Actor* m_pRopeEndTriggerActor;
};

#endif