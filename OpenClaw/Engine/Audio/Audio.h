#ifndef AUDIO_H_
#define AUDIO_H_

#include <stdint.h>
#include <SDL2/SDL_mixer.h>

#include "../GameApp/BaseGameApp.h"

class Audio
{
public:
    Audio();
    ~Audio();

    bool Initialize(const GameOptions& config);
    void Terminate();

    bool PlaySound(const char* soundData, size_t soundSize, const SoundProperties& soundProperties);
    bool PlaySound(Mix_Chunk* sound, const SoundProperties& soundProperties);
    void SetSoundVolume(int volumePercentage); 

    void PlayMusic(const char* musicData, size_t musicSize, bool looping);
    void PauseMusic();
    void ResumeMusic();
    void StopMusic();
    void SetMusicVolume(int volumePercentage);

    void StopAllSounds();

    void PauseAllSounds();
    void ResumeAllSounds();

    bool IsSoundActive() { return m_bSoundOn; }
    bool IsMusicActive() { return m_bMusicOn; }

    void SetSoundActive(bool active);
    void SetMusicActive(bool active);

    int GetSoundVolume();
    int GetMusicVolume();

private:
    //##### Methods #####//
    bool InitializeMidiRPC(const std::string& midiRpcServerPath);
    bool InitializeMidiRPCServer(const std::string& midiRpcServerPath);
    bool InitializeMidiRPCClient();
    bool IsRPCServerListening();

    void TerminateMidiRPC();

    //##### Members #####//
    bool m_bIsServerInitialized;
    bool m_bIsClientInitialized;
    bool m_bIsMidiRpcInitialized;
    bool m_bIsAudioInitialized;
    unsigned char* m_RpcBindingString;
    int m_SoundVolume;
    int m_MusicVolume;
    bool m_bSoundOn;
    bool m_bMusicOn;
};

#endif