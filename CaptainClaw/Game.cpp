#include <SDL.h>

#include "Game.h"
#include "Console.h"

Game::Game(const char* rezArchivePath, Console* console)
{
    _currentState = GAME_STATE_INGAME;

    _clawRezArchive = WAP_LoadRezArchive(rezArchivePath);
    if (_clawRezArchive == NULL)
    {

    }
}

bool Game::Run()
{
    SDL_Event event;
    uint32_t timeSinceLastFrame = 0;
    uint32_t ticks = 0;

    while (true)
    {
        ProcessEvents();

        switch (_currentState)
        {

        }
    }

    return true;
}

void Game::ProcessEvents()
{

}

void Game::HandleConsoleCommand(const char* command, void* userdata)
{
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Received command: %s", command);

    Game* self = static_cast<Game*>(userdata);
}