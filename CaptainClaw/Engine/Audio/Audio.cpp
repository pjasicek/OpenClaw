#include <SDL2/SDL.h>
#include <fstream>
#include <vector>

#include "Audio.h"

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

Audio::~Audio()
{
    Terminate();
}

bool Audio::Initialize(int frequency, int channels, int chunkSize, const char* midiRpcPath)
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
    if (Mix_OpenAudio(frequency, MIX_DEFAULT_FORMAT, channels, chunkSize) != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", Mix_GetError());
        return false;
    }

    /*if (Mix_AllocateChannels(16))
    {

    }*/

#ifdef _WIN32
    _isMidiRpcInitialized = InitializeMidiRPC(midiRpcPath);
    if (!_isMidiRpcInitialized)
    {
        return false;
    }
#endif //_WIN32

    _isAudioInitialized = true;

    return true;
}

void Audio::Terminate()
{
#ifdef _WIN32
    TerminateMidiRPC();
#endif //_WIN32
}

void Audio::PlayMusic(const char* musicData, size_t musicSize, bool looping)
{
#ifdef _WIN32
    RpcTryExcept
    {
        MidiRPC_PrepareNewSong();
        MidiRPC_AddChunk(musicSize, (byte*)musicData);
        MidiRPC_PlaySong(looping);
    }
    RpcExcept(1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Audio::PlayMusic: Failed due to RPC exception");
    }
    RpcEndExcept
#else
    SDL_RWops* pRWops = SDL_RWFromMem((void*)musicData, musicSize);
    Mix_Music* pMusic = Mix_LoadMUS_RW(pRWops, 0);
    Mix_PlayMusic(pMusic, looping ? -1 : 1);
#endif //_WIN32
}

// This is probably slow as fuck, should be removed, only used for debugging afaik
void Audio::PlayMusic(const char* musicPath, bool looping)
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

void Audio::SetMusicVolume(uint32_t volume)
{
#ifdef _WIN32
    RpcTryExcept
    {
        MidiRPC_ChangeVolume(volume);
    }
        RpcExcept(1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "AudioMgr::SetMusicVolume: Failed due to RPC exception");
    }
    RpcEndExcept
#else
    Mix_VolumeMusic(volume);
#endif //_WIN32
}

void Audio::PlaySound(const char* soundData, size_t soundSize, bool looping)
{
    SDL_RWops* soundRwOps = SDL_RWFromMem((void*)soundData, soundSize);
    Mix_Chunk* soundChunk = Mix_LoadWAV_RW(soundRwOps, 1);

    Mix_PlayChannel(-1, soundChunk, looping ? -1 : 0);
}

void Audio::PlaySound(Mix_Chunk* sound, bool looping)
{
    Mix_PlayChannel(-1, sound, looping ? -1 : 0);
}

void Audio::SetSoundVolume(uint32_t volume)
{
    Mix_Volume(-1, volume);
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

#ifdef _WIN32
//############################################
//############## MIDI RPC ####################
//############################################

bool Audio::InitializeMidiRPC(const char* midiRpcServerPath)
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

bool Audio::InitializeMidiRPCServer(const char* midiRpcServerPath)
{
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    BOOL doneCreateProc = CreateProcess(midiRpcServerPath, NULL, NULL, NULL, FALSE,
                                           0, NULL, NULL, &si, &pi);
    if (doneCreateProc)
    {
        _isServerInitialized = true;
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "MIDI RPC Server started. [%s]", midiRpcServerPath);
    }
    else
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "FAILED to start RPC MIDI Server. [%s]", midiRpcServerPath);
    }

    return (doneCreateProc != 0);
}

bool Audio::InitializeMidiRPCClient()
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

bool Audio::IsRPCServerListening()
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

void Audio::TerminateMidiRPC()
{
    RpcTryExcept
    {
        MidiRPC_StopServer();
    }
        RpcExcept(1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Audio::SetMusicVolume: Failed due to RPC exception");
    }
    RpcEndExcept
}
#endif //_WIN32