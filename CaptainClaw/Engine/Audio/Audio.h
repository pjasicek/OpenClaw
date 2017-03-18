#ifndef AUDIO_H_
#define AUDIO_H_

#include <stdint.h>
#include <SDL2/SDL_mixer.h>

#include "../GameApp/BaseGameApp.h"

class Audio
{
public:
    Audio() { }
    ~Audio();

    bool Initialize(const GameOptions& config);
    void Terminate();

    void PlaySound(const char* soundData, size_t soundSize, int volumePercentage = 100, int loops = 0);
    void PlaySound(Mix_Chunk* sound, int volumePercentage = 100, int loops = 0);
    void SetSoundVolume(uint32_t volumePercentage); 

    void PlayMusic(const char* musicData, size_t musicSize, bool looping);
    void PlayMusic(const char* musicPath, bool looping);
    void PauseMusic();
    void ResumeMusic();
    void StopMusic();
    void SetMusicVolume(uint32_t volumePercentage);

    void PauseAllSounds();
    void ResumeAllSounds();

private:
    //##### Methods #####//
    bool InitializeMidiRPC(const std::string& midiRpcServerPath);
    bool InitializeMidiRPCServer(const std::string& midiRpcServerPath);
    bool InitializeMidiRPCClient();
    bool IsRPCServerListening();

    void TerminateMidiRPC();

    //##### Members #####//
    bool _isServerInitialized;
    bool _isClientInitialized;
    bool _isMidiRpcInitialized;
    bool _isAudioInitialized;
    unsigned char* _rpcBindingString;
    int m_SoundVolume;
    int m_MusicVolume;
};

#endif