#include "GameState.h"
#include "../Console.h"
#include "../Level/Level.h"

#include <iostream>
using namespace std;

GameState::GameState(LoadLevelInfo* loadLevelInfo, Game* game) : State(game)
{
    _loadLevelInfo = loadLevelInfo;
    cout << "eeehere" << endl;
    _currentLevel = new Level(loadLevelInfo, game);
    cout << "here" << endl;
}

GameState::~GameState()
{

}

//
// Handle keyboard/mouse input event
//
State* GameState::HandleEvent(SDL_Event* event)
{
    int count;
    const uint8_t* key_state;

    if (event->type == SDL_QUIT)
    {
        _game->Shutdown();
    }

    key_state = SDL_GetKeyboardState(&count);
    if (key_state[SDL_SCANCODE_ESCAPE])
    {
        _game->Shutdown();
    }

    return NULL;
}

//
// Update ingame state, called each frame
//
void GameState::Update(uint32_t msDiff)
{
    SDL_Renderer* renderer = _game->GetRenderer();
    Console* console = _game->GetConsole();

    // Clear everything
    SDL_RenderClear(renderer);

    _currentLevel->Update(msDiff);

    console->Update(msDiff, renderer);


    SDL_RenderPresent(renderer);
}