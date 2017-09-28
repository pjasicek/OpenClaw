#ifndef __LOCAL_AMBIENT_SOUND_COMPONENT_H__
#define __LOCAL_AMBIENT_SOUND_COMPONENT_H__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "TriggerComponents/TriggerComponent.h"

class LocalAmbientSoundComponent : public ActorComponent, public TriggerObserver
{
public:
    LocalAmbientSoundComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual TiXmlElement* VGenerateXml() override;

    virtual void VPostInit() override;
    virtual void VPostPostInit() override;

    virtual void VUpdate(uint32 msDiff) override;

    virtual void VOnActorEnteredTrigger(Actor* pActorWhoEntered, FixtureType triggerType) override;
    virtual void VOnActorLeftTrigger(Actor* pActorWhoLeft, FixtureType triggerType) override;

private:
    void PlayAmbientSound();
    void StopAmbientSound();
    void UpdateAmbientSound();

    // XML Properties
    LocalAmbientSoundDef m_Properties;

    // Internal properties
    int m_ActorsInTriggerArea;
    Actor* m_pActorInArea;
    double m_DiagonalLength;
    int m_SoundChannel;
};

#endif