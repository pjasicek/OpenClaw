#ifndef __SOUND_TRIGGER_COMPONENT__
#define __SOUND_TRIGGER_COMPONENT__

#include "../../ActorComponent.h"
#include "../PickupComponents//PickupComponent.h"
#include "TriggerComponent.h"

//=====================================================================================================================
// SoundTriggerComponent
//=====================================================================================================================

class SoundTriggerComponent : public ActorComponent, public TriggerObserver
{
public:
    SoundTriggerComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual void VPostInit() override;
    virtual TiXmlElement* VGenerateXml() override;

    virtual void VOnActorEnteredTrigger(Actor* pActorWhoEntered, FixtureType triggerType) override;

private:
    std::string m_TriggerSound;
    bool m_bActivateDialog;
    int m_EnterCount;
    bool m_bIsInfinite;

};

#endif