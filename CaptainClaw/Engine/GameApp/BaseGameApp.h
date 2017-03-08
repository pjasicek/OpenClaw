#ifndef __BASEGAMEAPP_H__
#define __BASEGAMEAPP_H__

#include <tinyxml.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../SharedDefines.h"

#include "../UserInterface/Console.h"
#include "CommandHandler.h"

const int DEFAULT_SCREEN_WIDTH = 1280;
const int DEFAULT_SCREEN_HEIGHT = 768;

struct GameOptions
{
    void SetDefaults()
    {
        windowWidth = 1280;
        windowHeight = 780;
        scale = 1.0f;
        useVerticalSync = true;
        isFullscreen = false;
        isFullscreenDesktop = false;

        frequency = 44100;
        channels = 2;
        chunkSize = 2048;
        midiRpcServerPath = "MidiProc.exe";

        fontNames.push_back("clacon.ttf");
        consoleFontName = "clacon.ttf";
        consoleFontSize = 20;

        resourceFileNames.push_back("CLAW.REZ");
        resourceCacheSize = 50;

        startupCommandsFile = "startup_commands.txt";
    }

    GameOptions()
    {
        startupCommandsFile = "startup_commands.txt";
    }

    // Display options
    int windowWidth;
    int windowHeight;
    double scale;
    bool useVerticalSync;
    bool isFullscreen;
    bool isFullscreenDesktop;

    // Audio
    int frequency;
    uint32 channels;
    int chunkSize;
    const char* midiRpcServerPath;

    // Font
    std::vector<const char*> fontNames;
    const char* consoleFontName;
    int consoleFontSize;

    // Assets
    std::vector<const char*> resourceFileNames;
    int resourceCacheSize;

    // Console config
    ConsoleConfig consoleConfig;

    // File with prewritten commands which are executed upon startup of the game
    std::string startupCommandsFile;
};

// Cheats and stuff
struct GameCheats
{
    GameCheats()
    {
        showPhysicsDebug = false;

        clawInfiniteAmmo = false;
        clawInvincible = false;
        clawInfiniteJump = false;
    }

    // Environment
    bool showPhysicsDebug;

    // Claw
    bool clawInfiniteAmmo;
    bool clawInvincible;
    bool clawInfiniteJump;
};

class EventMgr;
class BaseGameLogic;
class HumanView;
class ResourceCache;
class Audio;

typedef std::map<std::string, std::string> LocalizedStringsMap;
typedef std::map<std::string, TTF_Font*> FontMap;

class BaseGameApp
{
    // Command handler should have unlimited access
    friend class CommandHandler;

public:
    BaseGameApp();

    // Muset be defined in inherited class
    virtual char* VGetGameTitle() = 0;
    virtual char* VGetGameAppDirectory() = 0;
    virtual BaseGameLogic* VCreateGameAndView() = 0;

    // Icon ?

    virtual bool Initialize(int argc, char** argv);
    virtual void VPostInitialize() { }
    virtual void Terminate();

    // HW Events
    void OnEvent(SDL_Event& event);
    void OnDisplayChange(int newWidth, int newHeight);
    void VOnRestore();
    void VOnMinimized();

    // Main loop
    int32 Run();

    // This is provided to be used the engine
    bool LoadStrings(std::string language);
    std::string GetString(std::string stringId);

    inline SDL_Renderer* GetRenderer() const { return m_pRenderer; }
    inline WapPal* GetCurrentPalette() const { return m_pPalette; }
    void SetCurrentPalette(WapPal* palette) { m_pPalette = palette; }
    inline ResourceCache* GetResourceCache() const { return m_pResourceCache; }

    BaseGameLogic* GetGameLogic() const { return m_pGame; }
    HumanView* GetHumanView() const;

    SDL_Window* GetWindow() const { return m_pWindow; }
    Point GetWindowSize() { return m_WindowSize; }
    void RequestWindowSizeChange(Point newSize, bool fullscreen);

    inline EventMgr* GetEventMgr() const { return m_pEventMgr; }

    TTF_Font* GetConsoleFont() const { return m_pConsoleFont; }

    Audio* GetAudio() const { return m_pAudio; }

    bool LoadGameOptions(const char* inConfigFile = "config.xml");
    void SaveGameOptions(const char* outConfigFile = "config.xml");

    bool LoadLevel(const char* levelResource);

    GameCheats* GetGameCheats() { return &m_GameCheats; }

    const ConsoleConfig* GetConsoleConfig() const { return &m_GameOptions.consoleConfig; }

    const GameOptions* GetGameConfig() const { return &m_GameOptions; }

protected:
    virtual void VRegisterGameEvents() { }

    BaseGameLogic* m_pGame;
    ResourceCache* m_pResourceCache;
    EventMgr* m_pEventMgr;
    TTF_Font* m_pConsoleFont;
    Audio* m_pAudio;

    TiXmlDocument m_XmlConfiguration;

    LocalizedStringsMap m_LocalizedStringsMap;
    FontMap m_FontMap;

    GameOptions m_GameOptions;

private:
    bool InitializeDisplay(GameOptions& gameOptions);
    bool InitializeAudio(GameOptions& gameOptions);
    bool InitializeResources(GameOptions& gameOptions);
    bool InitializeFont(GameOptions& gameOptions);
    bool InitializeLocalization(GameOptions& gameOptions);
    bool InitializeEventMgr();
    bool ReadConsoleConfig();

    void RegisterEngineEvents();

    TiXmlDocument CreateAndReturnDefaultConfig(const char* inConfigFile);

    SDL_Window* m_pWindow;
    SDL_Renderer* m_pRenderer;
    WapPal* m_pPalette;
    
    bool m_IsRunning;
    bool m_QuitRequested;
    bool m_IsQuitting;

    Point m_WindowSize;

    GameCheats m_GameCheats;
};

extern BaseGameApp* g_pApp;

#endif