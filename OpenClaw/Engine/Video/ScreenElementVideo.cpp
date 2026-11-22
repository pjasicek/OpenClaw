#include "ScreenElementVideo.h"

#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <sys/stat.h>

#include "../Audio/Audio.h"
#include "../GameApp/BaseGameApp.h"
#include "../Logger/Logger.h"

#ifdef _WIN32
// MSVC only exposes the POSIX pipe calls with an underscore prefix.
#define popen _popen
#define pclose _pclose
#endif

// All shipped cutscenes are 720x480 MPEG-2 at 29.97 fps. We let ffmpeg do the
// scaling so the frame size is known up front and no probing is needed.
static const int kVideoWidth = 720;
static const int kVideoHeight = 480;
static const uint32 kFrameDurationMs = 33;

static std::string QuotePath(const std::string& path)
{
    std::string quoted = "'";
    for (char c : path)
    {
        if (c == '\'')
        {
            quoted += "'\\''";
        }
        else
        {
            quoted += c;
        }
    }
    quoted += "'";
    return quoted;
}

static bool FileExists(const std::string& path)
{
    struct stat st;
    return stat(path.c_str(), &st) == 0;
}

// ffmpeg is not guaranteed to be on PATH when the game is launched from Finder.
static std::string FindFfmpeg()
{
    static const char* candidates[] = {
        "ffmpeg",
        "/opt/homebrew/bin/ffmpeg",
        "/usr/local/bin/ffmpeg",
        "/usr/bin/ffmpeg",
    };

    for (const char* candidate : candidates)
    {
        if (strchr(candidate, '/') == nullptr)
        {
            // Bare name: let popen/system resolve it through PATH.
            return candidate;
        }
        if (FileExists(candidate))
        {
            return candidate;
        }
    }

    return "";
}

bool ScreenElementVideo::IsAvailable()
{
    return !FindFfmpeg().empty();
}

ScreenElementVideo::ScreenElementVideo(SDL_Renderer* pRenderer, const std::string& videoPath)
    : m_pRenderer(pRenderer),
      m_VideoPath(videoPath),
      m_pPipe(nullptr),
      m_pTexture(nullptr),
      m_pMusic(nullptr),
      m_Width(kVideoWidth),
      m_Height(kVideoHeight),
      m_FrameDurationMs(kFrameDurationMs),
      m_AccumulatedMs(0),
      m_IsFinished(false),
      m_IsVisible(true)
{
    if (!OpenVideo())
    {
        m_IsFinished = true;
        return;
    }

    StartAudio();
}

ScreenElementVideo::~ScreenElementVideo()
{
    Stop();
}

bool ScreenElementVideo::OpenVideo()
{
    if (!FileExists(m_VideoPath))
    {
        LOG_ERROR("Video file not found: " + m_VideoPath);
        return false;
    }

    const std::string ffmpeg = FindFfmpeg();
    if (ffmpeg.empty())
    {
        LOG_ERROR("ffmpeg binary not found, cannot play cutscenes");
        return false;
    }

    m_pTexture = SDL_CreateTexture(m_pRenderer, SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING, m_Width, m_Height);
    if (!m_pTexture)
    {
        LOG_ERROR("Could not create video texture: " + std::string(SDL_GetError()));
        return false;
    }

    std::string command = ffmpeg +
        " -v error -i " + QuotePath(m_VideoPath) +
        " -an -vf scale=" + std::to_string(m_Width) + ":" + std::to_string(m_Height) +
        " -vsync 0 -f rawvideo -pix_fmt rgb24 -";

    m_pPipe = popen(command.c_str(), "r");
    if (!m_pPipe)
    {
        LOG_ERROR("Could not start ffmpeg for: " + m_VideoPath);
        return false;
    }

    LOG("Playing video: " + m_VideoPath);
    return DecodeNextFrame();
}

bool ScreenElementVideo::DecodeNextFrame()
{
    if (!m_pPipe || !m_pTexture)
    {
        return false;
    }

    const size_t frameSize = (size_t)m_Width * (size_t)m_Height * 3;
    std::vector<char> buffer(frameSize);
    if (fread(buffer.data(), 1, frameSize, m_pPipe) != frameSize)
    {
        m_IsFinished = true;
        return false;
    }

    SDL_UpdateTexture(m_pTexture, nullptr, buffer.data(), m_Width * 3);
    return true;
}

void ScreenElementVideo::StartAudio()
{
    if (!g_pApp || !g_pApp->GetAudio())
    {
        return;
    }

    // SDL2_mixer cannot decode the AC3 track, so extract it once to a WAV in the
    // user directory and let the mixer stream that. Missing file = silent video.
    const std::string baseName = m_VideoPath.substr(m_VideoPath.find_last_of("/\\") + 1);
    const std::string wavPath = g_pApp->GetGameConfig()->userDirectory + baseName + ".wav";
    if (!FileExists(wavPath))
    {
        const std::string ffmpeg = FindFfmpeg();
        if (ffmpeg.empty())
        {
            return;
        }

        const std::string command = ffmpeg +
            " -v error -y -i " + QuotePath(m_VideoPath) +
            " -vn -ac 2 -ar 44100 " + QuotePath(wavPath);
        if (system(command.c_str()) != 0 || !FileExists(wavPath))
        {
            LOG_WARNING("Could not extract audio from: " + m_VideoPath);
            return;
        }
    }

    m_pMusic = Mix_LoadMUS(wavPath.c_str());
    if (!m_pMusic)
    {
        LOG_WARNING("Could not load cutscene audio: " + std::string(Mix_GetError()));
        return;
    }

    g_pApp->GetAudio()->StopMusic();
    Mix_VolumeMusic((g_pApp->GetAudio()->GetSoundVolume() * MIX_MAX_VOLUME) / 100);
    Mix_PlayMusic(m_pMusic, 0);
}

void ScreenElementVideo::VOnUpdate(uint32 msDiff)
{
    if (m_IsFinished)
    {
        return;
    }

    m_AccumulatedMs += msDiff;
    if (m_AccumulatedMs < m_FrameDurationMs)
    {
        return;
    }

    m_AccumulatedMs -= m_FrameDurationMs;
    DecodeNextFrame();
}

void ScreenElementVideo::VOnRender(uint32 msDiff)
{
    if (m_IsFinished || !m_pTexture)
    {
        return;
    }

    int outWidth = 0;
    int outHeight = 0;
    SDL_GetRendererOutputSize(m_pRenderer, &outWidth, &outHeight);

    // The DVD clips are 4:3 despite the 720x480 storage resolution.
    SDL_Rect destRect;
    if (outHeight * 4 / 3 <= outWidth)
    {
        destRect.h = outHeight;
        destRect.w = outHeight * 4 / 3;
    }
    else
    {
        destRect.w = outWidth;
        destRect.h = outWidth * 3 / 4;
    }
    destRect.x = (outWidth - destRect.w) / 2;
    destRect.y = (outHeight - destRect.h) / 2;

    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(m_pRenderer);
    SDL_RenderCopy(m_pRenderer, m_pTexture, nullptr, &destRect);
}

bool ScreenElementVideo::VOnEvent(SDL_Event& evt)
{
    if (m_IsFinished)
    {
        return false;
    }

    // Any of these skips the cutscene.
    if (evt.type == SDL_KEYDOWN || evt.type == SDL_MOUSEBUTTONDOWN)
    {
        Stop();
    }

    // Swallow everything while the video is on screen.
    return true;
}

void ScreenElementVideo::VOnLostDevice()
{
    // Nothing to do: the texture is recreated with the renderer.
}

void ScreenElementVideo::Stop()
{
    if (m_pPipe)
    {
        pclose(m_pPipe);
        m_pPipe = nullptr;
    }

    if (m_pMusic)
    {
        Mix_HaltMusic();
        Mix_FreeMusic(m_pMusic);
        m_pMusic = nullptr;
    }

    if (m_pTexture)
    {
        SDL_DestroyTexture(m_pTexture);
        m_pTexture = nullptr;
    }

    m_IsFinished = true;
}
