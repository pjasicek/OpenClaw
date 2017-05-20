#ifndef __GLOBAL_AMBIENT_SOUND_COMPONENT_H__
#define __GLOBAL_AMBIENT_SOUND_COMPONENT_H__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"

class GlobalAmbientSoundComponent : public ActorComponent
{
public:
    GlobalAmbientSoundComponent();
    virtual ~GlobalAmbientSoundComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual TiXmlElement* VGenerateXml() override;

    virtual void VUpdate(uint32 msDiff) override;

private:
    void ActorEnteredBossAreaDelegate(IEventDataPtr pEventData);
    void BossFightEndedDelegate(IEventDataPtr pEvent);

    // Data properties
    std::string m_Sound;
    int m_SoundVolume;
    int m_MinTimeOff;
    int m_MaxTimeOff;
    int m_MinTimeOn;
    int m_MaxTimeOn;
    bool m_IsLooping;

    // Internal properties
    int m_SoundDurationMs;
    int m_CurrentTimeOff;
    int m_TimeOff;
    bool m_bIsStopped;
};

#endif //__GLOBAL_AMBIENT_SOUND_COMPONENT_H__