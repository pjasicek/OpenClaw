#include <SDL.h>

#include "Audio\AudioMgr.h"

#include "Game.h"
#include "Console.h"

#include "States\State.h"
#include "States\GameState.h"
#include "States\MenuState.h"
#include "States\CinematicState.h"
#include "States\ScoreboardState.h"

#include "Util\Util.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 764

#include <iostream>
using namespace std;

Game::Game(const char* rezArchivePath, Console* console)
{
    START_QUERY_PERFORMANCE_TIMER;

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "-------------------------------");
    if (!InitDisplay())
    {
        exit(-1);
    }

    if (!InitResources(rezArchivePath))
    {
        exit(-1);
    }

    if (!InitFont())
    {
        exit(-1);
    }

    if (!InitConsole())
    {
        exit(-1);
    }

    if (!InitAudio())
    {
        exit(-1);
    }
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "-------------------------------");

    cout << "here" << endl;

    SDL_GetWindowSize(_window, &_windowWidth, &_windowHeight);
    _windowScale = 1.0f;
    SDL_RenderSetScale(_renderer, _windowScale, _windowScale);

    LoadLevelInfo* loadLevelInfo = new LoadLevelInfo;
    (*loadLevelInfo) = { 1, false, 0, 3, 100, 0, 15, 10, 10};
    _currentState = new GameState(loadLevelInfo, this);

    cout << "here" << endl;

    cout << "here" << endl;

    END_QUERY_PERFORMANCE_TIMER;
}

Game::~Game()
{
    sAudioMgr->Terminate();
}

//
// Main game loop
// State design pattern is used to handle game states
//
void Game::Run()
{
    SDL_Event event;
    uint32_t timeSinceLastFrame = 0;
    uint32_t ticks = 0;
    _running = true;

    SDL_StartTextInput();

    while (_running)
    {
        ticks = SDL_GetTicks();

        // Handle all input events
        while (SDL_PollEvent(&event))
        {
            _console->ProcessEvents(event);

            State* state = _currentState->HandleEvent(&event);
            if (state != NULL)
            {
                _currentState->OnExit();
                delete _currentState;

                _currentState = state;
                state->OnEnter();
            }
        }

        _currentState->Update(timeSinceLastFrame);

        timeSinceLastFrame = SDL_GetTicks() - ticks;
    }

    SDL_StopTextInput();
}

//
// Handles executed console command
//
void Game::HandleConsoleCommand(const char* command, void* userdata)
{
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Received command: %s", command);
    Game* self = static_cast<Game*>(userdata);
}

//-----------------------------------------------------------------------------
// Private implementations
//-----------------------------------------------------------------------------

//
// Initializes main window and creater renderer
//
bool Game::InitDisplay()
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, ">>>>> Initializing display...");

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize SDL2 library");
        return false;
    }

    _window = SDL_CreateWindow("Captain Claw", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (_window == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create main window");
        return false;
    }

    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (_renderer == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create SDL2 Renderer. Error: %s", SDL_GetError());
        return false;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Display successfully initialized.");

    return true;
}

//
// Initializes audio
//
bool Game::InitAudio()
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, ">>>>> Initializing audio...");

    if (!sAudioMgr->Initialize())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize SDL Mixer audio subsystem");
        return false;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Audio successfully initialized.");

    return true;
}

//
// Initializes assets - reads Claw REZ archive
//
bool Game::InitResources(const char* rezArchivePath)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, ">>>>> Initializing resources...");

    _clawRezArchive = WAP_LoadRezArchive("CLAW.REZ");
    if (_clawRezArchive == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load rez archive CLAW.REZ");
        return false;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Resources successfully initialized");

    return true;
}

//
// Initializes console
//
bool Game::InitConsole()
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, ">>>>> Initializing console...");

    int width, height;
    SDL_GetWindowSize(_window, &width, &height);

    SDL_Texture* backgroundTexture = NULL;
    backgroundTexture = IMG_LoadTexture(_renderer, "console02.tga");
    if (backgroundTexture == NULL)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Failed to create texture so console background. Error: %s", IMG_GetError());
    }

    _console = new Console(width, height / 2, _font, backgroundTexture);
    if (_console == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create console");
        return false;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Console successfully initialized");

    return true;
}

//
// Initializes font used in game
//
bool Game::InitFont()
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, ">>>>> Initializing font...");

    if (TTF_Init() < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize SDL TTF font subsystem");
        return false;
    }

    _font = TTF_OpenFont("clacon.ttf", 20);
    if (_font == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load TTF font");
        return false;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Font successfully initialized...");

    return true;
}