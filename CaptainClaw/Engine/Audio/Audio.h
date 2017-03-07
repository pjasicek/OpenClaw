#ifndef AUDIO_H_
#define AUDIO_H_

#include <stdint.h>
#include <SDL2/SDL_mixer.h>

class Audio
{
public:
    Audio() { }
    ~Audio();

    bool Initialize(int frequency, int channels, int chunkSize, const char* midiRpcPath);
    void Terminate();

    void PlaySound(const char* soundData, size_t soundSize, bool looping);
    void PlaySound(Mix_Chunk* sound, bool looping);
    void SetSoundVolume(uint32_t volume); 

    void PlayMusic(const char* musicData, size_t musicSize, bool looping);
    void PlayMusic(const char* musicPath, bool looping);
    void PauseMusic();
    void ResumeMusic();
    void StopMusic();
    void SetMusicVolume(uint32_t volume);

    void PauseAllSounds();
    void ResumeAllSounds();

private:
    //##### Methods #####//
    bool InitializeMidiRPC(const char* midiRpcServerPath);
    bool InitializeMidiRPCServer(const char* midiRpcServerPath);
    bool InitializeMidiRPCClient();
    bool IsRPCServerListening();

    void TerminateMidiRPC();

    //##### Members #####//
    bool _isServerInitialized;
    bool _isClientInitialized;
    bool _isMidiRpcInitialized;
    bool _isAudioInitialized;
    unsigned char* _rpcBindingString;
};

#endif