#include <SDL.h>

#include "Audio\Audio.h"

#include "GameApplication.h"
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

// Resource loaders
#include "Engine\Resource\Loaders\DefaultLoader.h"
#include "Engine\Resource\Loaders\XmlLoader.h"
#include "Engine\Resource\Loaders\WwdLoader.h"
#include "Engine\Resource\Loaders\PalLoader.h"
#include "Engine\Resource\Loaders\PidLoader.h"
#include "Engine\Resource\Loaders\AniLoader.h"
#include "Engine\Resource\Loaders\WavLoader.h"

bool GameApplication::Initialize(const char* rezArchivePath, Console* console)
{
    START_QUERY_PERFORMANCE_TIMER;

    _currentState = NULL;
    _console = NULL;
    _window = NULL;
    _renderer = NULL;
    _font = NULL;
    _defaultPalette = NULL;
    _currentPalette = NULL;
    _resourceCache = NULL;

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "-------------------------------");
    if (!InitDisplay())
    {
        return false;
    }

    if (!InitAudio())
    {
        return false;
    }

    if (!InitResources(rezArchivePath))
    {
        return false;
    }

    if (!InitFont())
    {
        return false;
    }

    if (!InitConsole())
    {
        return false;
    }
    
    LOG("-------------------------------");

    LoadLevelInfo* loadLevelInfo = new LoadLevelInfo;
    (*loadLevelInfo) = { 1, false, 0, 3, 100, 0, 15, 10, 10};
    _currentState = new GameState(loadLevelInfo);

    

    END_QUERY_PERFORMANCE_TIMER; 

    return true;
}

void GameApplication::Terminate()
{
    //sAudioMgr->Terminate();
}

//
// Main game loop
// State design pattern is used to handle game states
//
void GameApplication::Run()
{
    SDL_Event event;
    uint32_t timeSinceLastFrame = 0;
    uint32_t ticks = 0;
    _running = true;

    uint32_t msCounter = 0;
    uint32_t framesCounter = 0;

    SDL_StartTextInput();

    while (_running)
    {
        static uint32 lastTime = SDL_GetTicks();
        uint32 elapsedTime = 0;
        uint32 now = SDL_GetTicks();

        elapsedTime = now - lastTime;
        lastTime = now;
        //cout << "time = " << timeSinceLastFrame << endl;

        // Handle all input events
        while (SDL_PollEvent(&event))
        {
            _console->ProcessEvents(event);

            IState* state = _currentState->HandleEvent(&event);
            if (state != NULL)
            {
                _currentState->OnExit();
                delete _currentState;

                _currentState = state;
                state->OnEnter();
            }
        }

        _currentState->Update(elapsedTime);
        //cout << "elapsedTime = " << elapsedTime << endl;

        /*timeSinceLastFrame = SDL_GetTicks() - ticks;
        if (timeSinceLastFrame > 0) cout << timeSinceLastFrame << endl;

        msCounter += timeSinceLastFrame;
        framesCounter++;
        if (msCounter > 1000)
        {
            cout << "FPS: " << framesCounter << endl;
            msCounter = 0;
            framesCounter = 0;
        }*/
    }

    SDL_StopTextInput();
}

//
// Handles executed console command
//
void GameApplication::HandleConsoleCommand(const char* command, void* userdata)
{
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Received command: %s", command);
    GameApplication* self = static_cast<GameApplication*>(userdata);

    if (self == NULL)
    {
        return;
    }


    std::string commandStr(command);
    std::vector<std::string> commandStrings;
    Util::SplitStringIntoVector(commandStr, commandStrings);
    if (commandStr.find("resize") == 0)
    {
        if (commandStrings.size() != 3)
        {
            self->GetConsole()->AddLine("Invalid parameters count. Usage: \"resize width height\"", COLOR_RED);
        }
        else
        {
            int32_t width = std::strtol(commandStrings[1].c_str(), NULL, 0);
            int32_t height = std::strtol(commandStrings[2].c_str(), NULL, 0);

            self->SetWindowSize(width, height);
            self->GetConsole()->Resize(width, height / 2);

            std::string response = "Setting window size to: " + std::to_string(width) + "x" + std::to_string(height);
            self->GetConsole()->AddLine(response, COLOR_GREEN);
        }
    }
    else if (commandStr.find("scale") == 0)
    {
        if (commandStrings.size() != 3)
        {
            self->GetConsole()->AddLine("Invalid parameters count. Usage: \"scale scaleX scaleY\"", COLOR_RED);
        }
        else
        {
            float scaleX = std::strtof(commandStrings[1].c_str(), NULL);
            float scaleY = std::strtof(commandStrings[2].c_str(), NULL);

            self->SetWindowScale(scaleX, scaleY);

            std::string response = "Setting scale to: " + std::to_string(scaleX) + "x" + std::to_string(scaleY);
            self->GetConsole()->AddLine(response, COLOR_GREEN);
        }
    }
    else if (commandStrings[0] == "vsync")
    {
        if (commandStrings.size() != 2)
        {
            self->GetConsole()->AddLine("Invalid parameters count. Usage: \"vsync {on|off}\"", COLOR_RED);
        }
        else if (commandStrings[1] == "on")
        {
            if (SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1") == SDL_TRUE)
            {
                self->GetConsole()->AddLine("Vertical synchronization ON", COLOR_GREEN);
            }
        }
        else if (commandStrings[1] == "off")
        {
            if (SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0") == SDL_TRUE)
            {
                self->GetConsole()->AddLine("Vertical synchronization OFF", COLOR_GREEN);
            }
        }
        else
        {
            self->GetConsole()->AddLine("Invalid parameter. Usage: \"vsync {on|off}\"", COLOR_RED);
        }
    }
    else
    {
        self->GetConsole()->AddLine(std::string(commandStr + ": Unknown command"), COLOR_RED);
    }
}

//-----------------------------------------------------------------------------
// Private implementations
//-----------------------------------------------------------------------------

//
// Initializes main window and creater renderer
//
bool GameApplication::InitDisplay()
{
    LOG(">>>>> Initializing display...");

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        LOG_ERROR("Failed to initialize SDL2 library");
        return false;
    }

    _window = SDL_CreateWindow("Captain Claw", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (_window == NULL)
    {
        LOG_ERROR("Failed to create main window");
        return false;
    }

    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (_renderer == NULL)
    {
        LOG_ERROR("Failed to create SDL2 Renderer. Error: %s" + std::string(SDL_GetError()));
        return false;
    }

    SDL_GetWindowSize(_window, &_windowWidth, &_windowHeight);
    _windowScale = 1.0f;
    SDL_RenderSetScale(_renderer, _windowScale, _windowScale);

    LOG("Display successfully initialized.");

    return true;
}

//
// Initializes audio
//
bool GameApplication::InitAudio()
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, ">>>>> Initializing audio...");

    Audio* audio = new Audio();
    if (!audio->Initialize(44100, 2, 2048, "MidiProc.exe"))
    {
        LOG_ERROR("Failed to initialize SDL Mixer audio subsystem");
        return false;
    }

    LOG("Audio successfully initialized.");

    return true;
}

#include <Psapi.h>

//
// Initializes assets - reads Claw REZ archive
//
bool GameApplication::InitResources(const char* rezArchivePath)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, ">>>>> Initializing resource cache...");

    ResourceRezArchive* resourceFile = new ResourceRezArchive(rezArchivePath);
    _resourceCache = new ResourceCache(50, resourceFile);
    if (!_resourceCache->Init())
    {
        LOG_ERROR("Failed to initialize resource cachce from resource file: " + std::string(rezArchivePath));
        return false;
    }

    _resourceCache->RegisterLoader(DefaultResourceLoader::Create());
    _resourceCache->RegisterLoader(XmlResourceLoader::Create());
    _resourceCache->RegisterLoader(WwdResourceLoader::Create());
    _resourceCache->RegisterLoader(PalResourceLoader::Create());
    _resourceCache->RegisterLoader(PidResourceLoader::Create());
    _resourceCache->RegisterLoader(AniResourceLoader::Create());
    _resourceCache->RegisterLoader(WavResourceLoader::Create());

    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    SIZE_T virtualMemUsedByMe = pmc.WorkingSetSize;
    LOG("Memory before preload: " + ToStr(virtualMemUsedByMe));

    /*_resourceCache->Preload("/GAME/*", NULL);
    _resourceCache->Preload("/STATES/*", NULL);
    _resourceCache->Preload("/CLAW/*", NULL);

    _resourceCache->Preload("/LEVEL1/*", NULL);*/

    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    virtualMemUsedByMe = pmc.WorkingSetSize;
    LOG("Memory after preload: " + ToStr(virtualMemUsedByMe));
    //exit(1);

    LOG("Resource cache successfully initialized");

    return true;
}

//
// Initializes console
//
bool GameApplication::InitConsole()
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, ">>>>> Initializing console...");

    int width, height;
    SDL_GetWindowSize(_window, &width, &height);

    SDL_Texture* backgroundTexture = NULL;
    backgroundTexture = IMG_LoadTexture(_renderer, "console02.tga");
    if (backgroundTexture == NULL)
    {
        LOG_WARNING("Failed to create texture so console background. Error: %s" + std::string(IMG_GetError()));
    }

    _console = new Console(width, height / 2, _font, backgroundTexture);
    if (_console == NULL)
    {
        LOG_ERROR("Failed to create console");
        return false;
    }

    _console->SetCommandHandler(GameApplication::HandleConsoleCommand, this);

    LOG("Console successfully initialized");

    return true;
}

//
// Initializes font used in game
//
bool GameApplication::InitFont()
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, ">>>>> Initializing font...");

    if (TTF_Init() < 0)
    {
        LOG_ERROR("Failed to initialize SDL TTF font subsystem");
        return false;
    }

    _font = TTF_OpenFont("clacon.ttf", 20);
    if (_font == NULL)
    {
        LOG_ERROR("Failed to load TTF font");
        return false;
    }

    LOG("Font successfully initialized...");

    return true;
}