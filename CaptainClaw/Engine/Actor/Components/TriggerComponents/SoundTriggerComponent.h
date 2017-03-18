#ifndef __SOUND_TRIGGER_COMPONENT__
#define __SOUND_TRIGGER_COMPONENT__

#include "../../ActorComponent.h"
#include "../PickupComponents//PickupComponent.h"
#include "TriggerComponent.h"

//=====================================================================================================================
// SoundTriggerComponent
//=====================================================================================================================

class SoundTriggerComponent : public PickupComponent
{
public:
    SoundTriggerComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VOnApply(Actor* pActorWhoPickedThis);

protected:
    virtual bool VDelegateInit(TiXmlElement* data);
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement);

private:
    bool m_bActivateDialog;
    int m_EnterCount;

};

#endif