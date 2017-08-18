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
    m_TimeOff(0),
    m_bIsStopped(false)
{ 
    IEventMgr::Get()->VAddListener(MakeDelegate(
        this, &GlobalAmbientSoundComponent::ActorEnteredBossAreaDelegate), EventData_Entered_Boss_Area::sk_EventType);
    IEventMgr::Get()->VAddListener(MakeDelegate(
        this, &GlobalAmbientSoundComponent::BossFightEndedDelegate), EventData_Boss_Fight_Ended::sk_EventType);
}

GlobalAmbientSoundComponent::~GlobalAmbientSoundComponent()
{
    IEventMgr::Get()->VRemoveListener(MakeDelegate(
        this, &GlobalAmbientSoundComponent::ActorEnteredBossAreaDelegate), EventData_Entered_Boss_Area::sk_EventType);
    IEventMgr::Get()->VRemoveListener(MakeDelegate(
        this, &GlobalAmbientSoundComponent::BossFightEndedDelegate), EventData_Boss_Fight_Ended::sk_EventType);
}

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
        IEventMgr::Get()->VQueueEvent(IEventDataPtr(
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
    if (m_IsLooping || m_bIsStopped)
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
        soundInfo.soundVolume = m_SoundVolume;
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));

        m_TimeOff = Util::GetRandomNumber(m_MinTimeOff, m_MaxTimeOff) + soundLoops * m_SoundDurationMs;

        m_CurrentTimeOff = 0;
    }
}

void GlobalAmbientSoundComponent::ActorEnteredBossAreaDelegate(IEventDataPtr pEventData)
{
    // Hack..
    m_bIsStopped = true;
}

void GlobalAmbientSoundComponent::BossFightEndedDelegate(IEventDataPtr pEvent)
{
    shared_ptr<EventData_Boss_Fight_Ended> pCastEventData =
        static_pointer_cast<EventData_Boss_Fight_Ended>(pEvent);

    if (!pCastEventData->GetIsBossDead())
    {
        m_bIsStopped = false;

        if (m_IsLooping)
        {
            SoundInfo soundInfo(m_Sound);
            soundInfo.loops = -1;
            soundInfo.soundVolume = m_SoundVolume;
            IEventMgr::Get()->VQueueEvent(IEventDataPtr(
                new EventData_Request_Play_Sound(soundInfo)));
        }
    }
}