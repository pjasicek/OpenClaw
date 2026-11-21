#ifndef __SCREEN_ELEMENT_VIDEO_H__
#define __SCREEN_ELEMENT_VIDEO_H__

#include <cstdio>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "../Interfaces.h"

// Plays one of the original game's DVD cutscenes (MPEG-2 .VOB files).
//
// The engine has no video decoder, so this pipes the file through the ffmpeg
// binary as raw RGB24 frames and uploads each one to a streaming texture.
// ponytail: no decode thread and no A/V resync. ffmpeg decodes these 720x480
// clips far faster than realtime, so the pipe itself blocks and paces us.
// If a future clip is slow enough to stutter, move decoding to a thread.
class ScreenElementVideo : public IScreenElement
{
public:
    ScreenElementVideo(SDL_Renderer* pRenderer, const std::string& videoPath);
    virtual ~ScreenElementVideo();

    virtual void VOnLostDevice() override;
    virtual void VOnRender(uint32 msDiff) override;
    virtual void VOnUpdate(uint32 msDiff) override;

    virtual int32_t VGetZOrder() const override { return 10000; }
    virtual void VSetZOrder(int32_t const zOrder) override { }
    virtual bool VIsVisible() override { return m_IsVisible; }
    virtual void VSetVisible(bool visible) override { m_IsVisible = visible; }

    virtual bool VOnEvent(SDL_Event& evt) override;

    bool IsFinished() const { return m_IsFinished; }
    void Stop();

    static bool IsAvailable();

private:
    bool OpenVideo();
    bool DecodeNextFrame();
    void StartAudio();

    SDL_Renderer* m_pRenderer;
    std::string m_VideoPath;
    FILE* m_pPipe;
    SDL_Texture* m_pTexture;
    Mix_Music* m_pMusic;
    int m_Width;
    int m_Height;
    uint32 m_FrameDurationMs;
    uint32 m_AccumulatedMs;
    bool m_IsFinished;
    bool m_IsVisible;
};

#endif
