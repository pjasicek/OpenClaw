#ifndef GAME_H_
#define GAME_H_

#include <libwap.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include "Level\Level.h"
#include <assert.h>

const uint16_t MAXIMUM_LEVEL = 14;

enum GAME_STATE
{
    GAME_STATE_MENU,
    GAME_STATE_INGAME,
    GAME_STATE_FINISH_SCORE,
    GAME_STATE_CUTSCENE
};

struct KeyboardState
{
    bool isDownPressed;
    bool isUpPressed;
    bool isLeftPressed;
    bool isRightPressed;
    bool isCtrlPressed;
    bool isEnterPressed;
    bool isReturnPressed;
    bool isShiftPressed;
    bool isLiftPressed;
    bool isJumpPressed;
};

class Console;
class State;

struct DisplayContext
{
    SDL_Window* window;
    SDL_Renderer* renderer;
};

struct ResourceContext
{
    RezArchive* rezArchive;
};

struct GameContext
{
    DisplayContext displayContext;
    ResourceContext resourceContext;
};

struct LoadLevelInfo
{
    // Info about whichi level to load and if it is loaded saved level 
    uint32_t levelNumber;
    bool loadLevel;
    uint16_t checkpointNumber;

    // Default new or loaded stats
    uint32_t lives;
    uint32_t health;
    uint32_t score;
    uint32_t gunpowderCount;
    uint32_t magicCount;
    uint32_t dynamiteCount;
};

class Game
{
public:
    Game(const char* rezArchivePath, Console* console = NULL);
    ~Game();

    void Run();
    void Shutdown() { _running = false; }

    inline Console* GetConsole() { return _console; }
    inline SDL_Renderer* GetRenderer() { return _renderer; }
    inline SDL_Window* GetWindow() { return _window;}
    inline RezArchive* GetRezArchive() { return _clawRezArchive; }

    uint32_t GetWindowWidth() { return _windowWidth; }
    uint32_t GetWindowHeight() { return _windowHeight; }

    double GetWindowScale() { return _windowScale; }

    static void HandleConsoleCommand(const char* command, void* userdata);

private:
    bool InitDisplay();
    bool InitAudio();
    bool InitResources(const char* rezArchivePath);
    bool InitConsole();
    bool InitFont();

    int _windowWidth;
    int _windowHeight;

    float _windowScale;

    State* _currentState;

    RezArchive* _clawRezArchive;
    Console* _console;

    // Video
    SDL_Window* _window;
    SDL_Renderer* _renderer;

    // Font used
    TTF_Font* _font;

    bool _running;

    Level* _currentLevel;
};

#endif