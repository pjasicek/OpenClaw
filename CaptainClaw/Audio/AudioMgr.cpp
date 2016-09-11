#include <SDL.h>
#include <Windows.h>
#include <fstream>
#include <vector>

#include "AudioMgr.h"
#include "../../MidiProc/midiproc.h"

#include <iostream>
using namespace std;

const char MIDI_RPC_SERVER_EXE_NAME[] = "MidiProc.exe";
const uint32_t MIDI_RPC_MAX_HANDSHAKE_TRIES = 50;

//############################################
//################# API ######################
//############################################

AudioMgr* AudioMgr::Instance()
{
    static AudioMgr instance;
    return &instance;
}

bool AudioMgr::Initialize()
{
    _isServerInitialized = false;
    _isClientInitialized = false;
    _isMidiRpcInitialized = false;
    _isAudioInitialized = false;
    _rpcBindingString = NULL;

    if (!SDL_WasInit(SDL_INIT_AUDIO))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Attempted to initialize Audio subsystem before SDL2 was initialized");
        return false;
    }

    // Setup audio mode
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", Mix_GetError());
        return false;
    }

    _isMidiRpcInitialized = InitializeMidiRPC();
    if (!_isMidiRpcInitialized)
    {
        return false;
    }

    _isAudioInitialized = true;

    return true;
}

void AudioMgr::Terminate()
{
    TerminateMidiRPC();
}

void AudioMgr::PlayMusic(const char* musicData, size_t musicSize, bool looping)
{
    RpcTryExcept
    {
        MidiRPC_PrepareNewSong();
        MidiRPC_AddChunk(musicSize, (byte*)musicData);
        MidiRPC_PlaySong(looping);
    }
    RpcExcept(1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioMgr::PlayMusic: Failed due to RPC exception");
    }
    RpcEndExcept
}

void AudioMgr::PlayMusic(const char* musicPath, bool looping)
{
    std::ifstream musicFileStream(musicPath, std::ios::binary);
    if (!musicFileStream.is_open())
    {
        return;
    }

    // Read whole file
    std::vector<char> musicFileContents((std::istreambuf_iterator<char>(musicFileStream)), std::istreambuf_iterator<char>());
    if (!musicFileStream.good())
    {
        return;
    }

    PlayMusic(musicFileContents.data(), musicFileContents.size(), looping);
}

void AudioMgr::PauseMusic()
{
    RpcTryExcept
    {
        MidiRPC_PauseSong();
    }
        RpcExcept(1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioMgr::PauseMusic: Failed due to RPC exception");
    }
    RpcEndExcept
}

void AudioMgr::ResumeMusic()
{
    RpcTryExcept
    {
        MidiRPC_ResumeSong();
    }
        RpcExcept(1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioMgr::ResumeMusic: Failed due to RPC exception");
    }
    RpcEndExcept
}

void AudioMgr::StopMusic()
{
    RpcTryExcept
    {
        MidiRPC_StopSong();
    }
        RpcExcept(1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioMgr::StopMusic: Failed due to RPC exception");
    }
    RpcEndExcept
}

void AudioMgr::SetMusicVolume(uint32_t volume)
{
    RpcTryExcept
    {
        MidiRPC_ChangeVolume(volume);
    }
        RpcExcept(1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioMgr::SetMusicVolume: Failed due to RPC exception");
    }
    RpcEndExcept
}

void AudioMgr::PlaySound(const char* soundData, size_t soundSize, bool looping)
{
    SDL_RWops* soundRwOps = SDL_RWFromMem((void*)soundData, soundSize);
    Mix_Chunk* soundChunk = Mix_LoadWAV_RW(soundRwOps, 1);

    Mix_PlayChannel(-1, soundChunk, looping ? -1 : 1);
}

void AudioMgr::PlaySound(Mix_Chunk* sound, bool looping)
{
    Mix_PlayChannel(-1, sound, looping ? -1 : 1);
}

void AudioMgr::SetSoundVolume(uint32_t volume)
{
    Mix_Volume(-1, volume);
}

void AudioMgr::PauseAllSounds()
{
    Mix_Pause(-1);
    MidiRPC_PauseSong();
}

void AudioMgr::ResumeAllSounds()
{
    Mix_Resume(-1);
    MidiRPC_ResumeSong();
}

//############################################
//############## MIDI RPC ####################
//############################################

bool AudioMgr::InitializeMidiRPC()
{
    if (!InitializeMidiRPCServer())
    {
        return false;
    }

    if (!InitializeMidiRPCClient())
    {
        return false;
    }

    return true;
}

bool AudioMgr::InitializeMidiRPCServer()
{
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    BOOL doneCreateProc = CreateProcess(MIDI_RPC_SERVER_EXE_NAME, NULL, NULL, NULL, FALSE,
                                           0, NULL, NULL, &si, &pi);
    if (doneCreateProc)
    {
        _isServerInitialized = true;
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "MIDI RPC Server started. [%s]", MIDI_RPC_SERVER_EXE_NAME);
    }
    else
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "FAILED to start RPC MIDI Server. [%s]", MIDI_RPC_SERVER_EXE_NAME);
    }

    return (doneCreateProc != 0);
}

bool AudioMgr::InitializeMidiRPCClient()
{
    RPC_STATUS rpcStatus;

    if (!_isServerInitialized)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize RPC MIDI Client - server was was not initialized");
        return false;
    }

    rpcStatus = RpcStringBindingCompose(NULL,
                                       (RPC_CSTR)("ncalrpc"),
                                       NULL,
                                       (RPC_CSTR)("2d4dc2f9-ce90-4080-8a00-1cb819086970"),
                                       NULL,
                                       &_rpcBindingString);

    if (rpcStatus != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize RPC MIDI Client - RPC binding composition failed");
        return false;
    }

    rpcStatus = RpcBindingFromStringBinding(_rpcBindingString, &hMidiRPCBinding);

    if (rpcStatus != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize RPC MIDI Client - RPC client binding failed");
        return false;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "RPC Client successfully initialized");

    _isClientInitialized = true;

    bool isServerListening = IsRPCServerListening();
    if (!isServerListening)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Handshake between RPC Server and Client failed");
        return false;
    }
    else
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "RPC Server and Client successfully handshaked");
    }

    return true;
}

bool AudioMgr::IsRPCServerListening()
{
    if (!_isClientInitialized || !_isServerInitialized)
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

void AudioMgr::TerminateMidiRPC()
{
    RpcTryExcept
    {
        MidiRPC_StopServer();
    }
        RpcExcept(1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioMgr::SetMusicVolume: Failed due to RPC exception");
    }
    RpcEndExcept
}

AudioMgr::AudioMgr()
{

}

AudioMgr::~AudioMgr()
{

}