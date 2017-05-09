#include "GlobalAmbientSoundComponent.h"

#include "../../Resource/Loaders/WavLoader.h"

#include "../../Events/EventMgr.h"
#include "../../Events/Events.h"

//=====================================================================================================================
//
// GlobalAmbientSoundComponent Implementation
//
//=====================================================================================================================

const char* GlobalAmbientSoundComponent::g_Name = "GlobalAmbientSoundComponent";

GlobalAmbientSoundComponent::GlobalAmbientSoundComponent() :
    m_SoundVolume(0),
    m_MinTimeOff(0),
    m_MaxTimeOff(0),
    m_MinTimeOn(0),
    m_MaxTimeOn(0),
    m_IsLooping(false),
    m_SoundDurationMs(0),
    m_CurrentTimeOff(0),
    m_TimeOff(0)
{ }

bool GlobalAmbientSoundComponent::VInit(TiXmlElement* pData)
{
    assert(pData);

    ParseValueFromXmlElem(&m_Sound, pData->FirstChildElement("Sound"));
    ParseValueFromXmlElem(&m_SoundVolume, pData->FirstChildElement("SoundVolume"));
    ParseValueFromXmlElem(&m_MinTimeOff, pData->FirstChildElement("MinTimeOff"));
    ParseValueFromXmlElem(&m_MaxTimeOff, pData->FirstChildElement("MaxTimeOff"));
    ParseValueFromXmlElem(&m_MinTimeOn, pData->FirstChildElement("MinTimeOn"));
    ParseValueFromXmlElem(&m_MaxTimeOn, pData->FirstChildElement("MaxTimeOn"));
    ParseValueFromXmlElem(&m_IsLooping, pData->FirstChildElement("IsLooping"));

    if (!m_IsLooping)
    {
        assert(m_MinTimeOff != 0 && m_MaxTimeOff != 0 && m_MinTimeOn != 0 && m_MaxTimeOn != 0);
    }

    shared_ptr<Mix_Chunk> pSound = WavResourceLoader::LoadAndReturnSound(m_Sound.c_str());
    m_SoundDurationMs = Util::GetSoundDurationMs(pSound.get());
    assert(m_SoundDurationMs > 0);

    m_TimeOff = Util::GetRandomNumber(m_MinTimeOff, m_MaxTimeOff);

    if (m_IsLooping)
    {
        SoundInfo soundInfo(m_Sound);
        soundInfo.loops = -1;
        soundInfo.soundVolume = m_SoundVolume;
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));
    }

    return true;
}


TiXmlElement* GlobalAmbientSoundComponent::VGenerateXml()
{
    // TODO: Implement
    return NULL;
}

void GlobalAmbientSoundComponent::VUpdate(uint32 msDiff)
{
    if (m_IsLooping)
    {
        return;
    }

    m_CurrentTimeOff += msDiff;
    if (m_CurrentTimeOff >= m_TimeOff)
    {
        int timeOn = Util::GetRandomNumber(m_MinTimeOn, m_MaxTimeOn);
        int soundLoops = timeOn / m_SoundDurationMs;

        SoundInfo soundInfo(m_Sound);
        soundInfo.loops = soundLoops;
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));

        m_TimeOff = Util::GetRandomNumber(m_MinTimeOff, m_MaxTimeOff) + soundLoops * m_SoundDurationMs;

        m_CurrentTimeOff = 0;
    }
}