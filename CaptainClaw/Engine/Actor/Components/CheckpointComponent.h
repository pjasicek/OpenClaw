#ifndef __CHECKPOINT_COMPONENT_H__
#define __CHECKPOINT_COMPONENT_H__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "PickupComponents/PickupComponent.h"
#include "AnimationComponent.h"

class CheckpointComponent : public PickupComponent, public AnimationObserver
{
public:
    CheckpointComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VOnApply(Actor* pActorWhoPickedThis) override;

    virtual void VOnAnimationAtLastFrame(Animation* pAnimation) override;

protected:
    virtual bool VDelegateInit(TiXmlElement* data) override;
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement) override;

private:
    Point m_SpawnPosition;
    bool m_IsSaveCheckpoint;
    uint32 m_SaveCheckpointNumber;
};

#endif