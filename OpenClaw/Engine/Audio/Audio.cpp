#include <SDL2/SDL.h>
#include <fstream>
#include <vector>

#include "Audio.h"
#include "../Events/EventMgr.h"
#include "../Events/Events.h"

#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#include "../../../MidiProc/midiproc.h"
#endif

#ifdef PlaySound
#undef PlaySound
#endif

using namespace std;

const uint32_t MIDI_RPC_MAX_HANDSHAKE_TRIES = 250;

//############################################
//################# API ######################
//############################################

Audio::Audio()
    :
    m_bIsServerInitialized(false),
    m_bIsClientInitialized(false),
    m_bIsMidiRpcInitialized(false),
    m_bIsAudioInitialized(false),
    m_RpcBindingString(NULL),
    m_SoundVolume(0),
    m_MusicVolume(0),
    m_bSoundOn(true),
    m_bMusicOn(true)
{

}

Audio::~Audio()
{
    Terminate();
}

bool Audio::Initialize(const GameOptions& config)
{
    if (!SDL_WasInit(SDL_INIT_AUDIO))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Attempted to initialize Audio subsystem before SDL2 was initialized");
        return false;
    }

    // Setup audio mode
    if (Mix_OpenAudio(config.frequency, MIX_DEFAULT_FORMAT, config.soundChannels, config.chunkSize) != 0)
    {
        LOG_ERROR(std::string(Mix_GetError()));
        return false;
    }

    Mix_AllocateChannels(config.mixingChannels);

    int reservedChannels = Mix_ReserveChannels(16);
#ifndef __EMSCRIPTEN__
    if (reservedChannels != 16)
    {
        LOG_ERROR(std::string(Mix_GetError()));
        return false;
    }

    Mix_GroupChannels(0, 15, 1);
#else
    // Mix_ReserveChannels returns nothing.
    if (reservedChannels != 0)
    {
        LOG_ERROR(std::string(Mix_GetError()));
        return false;
    }
    // TODO: [EMSCRIPTEN] Try to implement Mix_Group* functions
#endif

    m_SoundVolume = config.soundVolume;
    m_MusicVolume = config.musicVolume;
    m_bSoundOn = config.soundOn;
    m_bMusicOn = config.musicOn;

#ifdef _WIN32
    m_bIsMidiRpcInitialized = InitializeMidiRPC(config.midiRpcServerPath);
    if (!m_bIsMidiRpcInitialized)
    {
        return false;
    }
#endif //_WIN32

    SetSoundVolume(m_SoundVolume);
    SetMusicVolume(m_MusicVolume);

    SetSoundActive(m_bSoundOn);
    SetMusicActive(m_bMusicOn);

    m_bIsAudioInitialized = true;

    return true;
}

void Audio::Terminate()
{
#ifdef _WIN32
    TerminateMidiRPC();
#endif //_WIN32
}

struct _MusicInfo
{
    _MusicInfo(const char* pData, size_t size, bool isLooping, int volume)
    {
        pMusicData = pData;
        musicSize = size;
        looping = isLooping;
        musicVolume = volume;
    }

    const char* pMusicData;
    size_t musicSize;
    bool looping;
    int musicVolume;
};

static int SetupPlayMusicThread(void* pData)
{
    _MusicInfo* pMusicInfo = (_MusicInfo*)pData;

#ifdef _WIN32
    RpcTryExcept
    {
        MidiRPC_PrepareNewSong();
        MidiRPC_AddChunk(pMusicInfo->musicSize, (byte*)pMusicInfo->pMusicData);
        MidiRPC_PlaySong(pMusicInfo->looping);

        if (pMusicInfo->musicVolume == -1)
        {
            MidiRPC_PauseSong();
        }
        else
        {
            MidiRPC_ResumeSong();
            MidiRPC_ChangeVolume(pMusicInfo->musicVolume);
        }
    }
        RpcExcept(1)
    {
        //__LOG_ERROR("Audio::SetMusicVolume: Failed due to RPC exception");
    }
    RpcEndExcept;
#else
    SDL_RWops* pRWops = SDL_RWFromMem((void*)pMusicInfo->pMusicData, pMusicInfo->musicSize);
    Mix_Music* pMusic = Mix_LoadMUS_RW(pRWops, 0);
    if (!pMusic) {
        LOG_ERROR("Mix_LoadMUS_RW: " + std::string(Mix_GetError()));
    }
    Mix_PlayMusic(pMusic, pMusicInfo->looping ? -1 : 0);

    if (pMusicInfo->musicVolume == -1)
    {
        Mix_PauseMusic();
    }
    else
    {
        Mix_ResumeMusic();
    }
#endif //_WIN32

    SAFE_DELETE(pMusicInfo);

    return 0;
}

void Audio::PlayMusic(const char* musicData, size_t musicSize, bool looping)
{
    _MusicInfo* pMusicInfo = new _MusicInfo(musicData, musicSize, looping, m_bMusicOn ? m_MusicVolume : -1);

    // Playing music track takes ALOT of time for some reason so play it in another thread
    SDL_Thread* pThread = SDL_CreateThread(SetupPlayMusicThread, "SetupPlayMusicThread", (void*)pMusicInfo);
    SDL_DetachThread(pThread);
}

void Audio::PauseMusic()
{
#ifdef _WIN32
    RpcTryExcept
    {
        MidiRPC_PauseSong();
    }
        RpcExcept(1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Audio::PauseMusic: Failed due to RPC exception");
    }
    RpcEndExcept
#else
    Mix_PauseMusic();
#endif //_WIN32
}

void Audio::ResumeMusic()
{
#ifdef _WIN32
    RpcTryExcept
    {
        MidiRPC_ResumeSong();
        MidiRPC_ChangeVolume(m_MusicVolume);
    }
        RpcExcept(1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Audio::ResumeMusic: Failed due to RPC exception");
    }
    RpcEndExcept
#else
    Mix_ResumeMusic();
#endif //_WIN32
}

void Audio::StopMusic()
{
#ifdef _WIN32
    RpcTryExcept
    {
        MidiRPC_StopSong();
    }
        RpcExcept(1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioMgr::StopMusic: Failed due to RPC exception");
    }
    RpcEndExcept
#else
    Mix_HaltMusic();
#endif //_WIN32
}

void Audio::SetMusicVolume(int volumePercentage)
{
    // Music has ~ 5x more potency than sound, so max is 20 instead of 100
    volumePercentage = min(volumePercentage, 20);
    if (volumePercentage < 0)
    {
        volumePercentage = 0;
    }
    m_MusicVolume = (int)((((float)volumePercentage) / 100.0f) * (float)MIX_MAX_VOLUME);

#ifdef _WIN32
    RpcTryExcept
    {
        MidiRPC_ChangeVolume(m_MusicVolume);
    }
        RpcExcept(1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioMgr::SetMusicVolume: Failed due to RPC exception");
    }
    RpcEndExcept
#else
    Mix_VolumeMusic(m_MusicVolume);
#endif //_WIN32
}

int Audio::GetMusicVolume()
{
    return ceil(((float)m_MusicVolume / (float)MIX_MAX_VOLUME) * 100.0f);
}

bool Audio::PlaySound(const char* soundData, size_t soundSize, const SoundProperties& soundProperties)
{
    SDL_RWops* soundRwOps = SDL_RWFromMem((void*)soundData, soundSize);
    Mix_Chunk* soundChunk = Mix_LoadWAV_RW(soundRwOps, 1);

    return PlaySound(soundChunk, soundProperties);
}

bool Audio::PlaySound(Mix_Chunk* sound, const SoundProperties& soundProperties)
{
#ifndef __EMSCRIPTEN__
    int chunkVolume = (int)((((float)soundProperties.volume) / 100.0f) * (float)m_SoundVolume);

    Mix_VolumeChunk(sound, chunkVolume);

    int loops = soundProperties.loops;
#else
    // TODO: [EMSCRIPTEN] Try to implement Mix_VolumeChunk

    int loops = soundProperties.loops;
    // Emscripten SDL port supports infinite loops only
    if (loops != -1) {
        loops = 0;
    }
#endif
    int channel = Mix_PlayChannel(-1, sound, loops);
    if (channel == -1)
    {
        LOG_ERROR("Failed to play chunk: " + std::string(Mix_GetError()));
        return false;
    }

#ifndef __EMSCRIPTEN__
    if (!Mix_SetPosition(channel, soundProperties.angle, soundProperties.distance))
    {
        LOG_ERROR("Mix_SetPosition: " + std::string(Mix_GetError()));
        return false;
    }
#else
    // TODO: [EMSCRIPTEN] Try to implement Mix_SetPosition
#endif

    if (!m_bSoundOn)
    {
        Mix_Pause(channel);
    }

    return true;
}

void Audio::SetSoundVolume(int volumePercentage)
{
    volumePercentage = min(volumePercentage, 100);
    if (volumePercentage < 0)
    {
        volumePercentage = 0;
    }
    m_SoundVolume = (int)((((float)volumePercentage) / 100.0f) * (float)MIX_MAX_VOLUME);

    if (m_bSoundOn)
    {
        Mix_Volume(-1, m_SoundVolume);
    }
}

int Audio::GetSoundVolume()
{
    return ceil(((float)m_SoundVolume / (float)MIX_MAX_VOLUME) * 100.0f);
}

void Audio::StopAllSounds()
{
    Mix_HaltChannel(-1);
    StopMusic();
}

void Audio::PauseAllSounds()
{
    Mix_Pause(-1);
#ifdef _WIN32
    MidiRPC_PauseSong();
#endif //_WIN32
}

void Audio::ResumeAllSounds()
{
    Mix_Resume(-1);
#ifdef _WIN32
    MidiRPC_ResumeSong();
#endif //_WIN32
}

void Audio::SetSoundActive(bool active)
{ 
    if (m_bSoundOn == active)
    {
        return;
    }

    if (active)
    {
        Mix_Resume(-1);
        Mix_Volume(-1, m_SoundVolume);
    }
    else
    {
        Mix_Pause(-1);
    }

    m_bSoundOn = active; 
}

void Audio::SetMusicActive(bool active)
{ 
    if (m_bMusicOn == active)
    {
        return;
    }

    if (active)
    {
        ResumeMusic();
    }
    else
    {
        PauseMusic();
    }

    m_bMusicOn = active; 
}

#ifdef _WIN32
//############################################
//############## MIDI RPC ####################
//############################################

bool Audio::InitializeMidiRPC(const std::string& midiRpcServerPath)
{
    if (!InitializeMidiRPCServer(midiRpcServerPath))
    {
        return false;
    }

    if (!InitializeMidiRPCClient())
    {
        return false;
    }

    return true;
}

bool Audio::InitializeMidiRPCServer(const std::string& midiRpcServerPath)
{
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    BOOL doneCreateProc = CreateProcess(midiRpcServerPath.c_str(), NULL, NULL, NULL, FALSE,
                                           0, NULL, NULL, &si, &pi);
    if (doneCreateProc)
    {
        m_bIsServerInitialized = true;
        LOG("MIDI RPC Server started. [" + std::string(midiRpcServerPath) + "]");
    }
    else
    {
        LOG_ERROR("FAILED to start RPC MIDI Server. [" + std::string(midiRpcServerPath) + "]");
    }

    return (doneCreateProc != 0);
}

bool Audio::InitializeMidiRPCClient()
{
    RPC_STATUS rpcStatus;

    if (!m_bIsServerInitialized)
    {
        LOG_ERROR("Failed to initialize RPC MIDI Client - server was was not initialized");
        return false;
    }

    rpcStatus = RpcStringBindingCompose(NULL,
                                       (RPC_CSTR)("ncalrpc"),
                                       NULL,
                                       (RPC_CSTR)("2d4dc2f9-ce90-4080-8a00-1cb819086970"),
                                       NULL,
                                       &m_RpcBindingString);

    if (rpcStatus != 0)
    {
        LOG_ERROR("Failed to initialize RPC MIDI Client - RPC binding composition failed");
        return false;
    }

    rpcStatus = RpcBindingFromStringBinding(m_RpcBindingString, &hMidiRPCBinding);

    if (rpcStatus != 0)
    {
        LOG_ERROR("Failed to initialize RPC MIDI Client - RPC client binding failed");
        return false;
    }

    LOG("RPC Client successfully initialized");

    m_bIsClientInitialized = true;

    bool isServerListening = IsRPCServerListening();
    if (!isServerListening)
    {
        LOG_ERROR("Handshake between RPC Server and Client failed");
        return false;
    }
    else
    {
        LOG("RPC Server and Client successfully handshaked");
    }

    return true;
}

bool Audio::IsRPCServerListening()
{
    if (!m_bIsClientInitialized || !m_bIsServerInitialized)
    {
        return false;
    }

    uint16_t tries = 0;
    while (RpcMgmtIsServerListening(hMidiRPCBinding) != RPC_S_OK)
    {
        SDL_Delay(10);
        if (tries++ >= MIDI_RPC_MAX_HANDSHAKE_TRIES)
        {
            return false;
        }
    }

    return true;
}

void Audio::TerminateMidiRPC()
{
    RpcTryExcept
    {
        MidiRPC_StopServer();
    }
    RpcExcept(1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Audio::TerminateMidiRPC: Failed due to RPC exception");
    }
    RpcEndExcept;
}
#endif //_WIN32
