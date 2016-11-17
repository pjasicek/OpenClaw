#include "GameState.h"
#include "../Console.h"
#include "../Level/Level.h"
#include "../Collision/CollisionWorld.h"

#include "../Util/Util.h"
#include <iostream>
using namespace std;

GameState::GameState(LoadLevelInfo* loadLevelInfo)
{
    _loadLevelInfo = loadLevelInfo;
    _currentLevel = new Level(loadLevelInfo);
}

GameState::~GameState()
{

}

//
// Handle keyboard/mouse input event
//
IState* GameState::HandleEvent(SDL_Event* event)
{
    int count;
    const uint8_t* key_state;

    if (event->type == SDL_QUIT)
    {
        sGameApplication->Shutdown();
    }

    
    key_state = SDL_GetKeyboardState(&count);
    if (key_state[SDL_SCANCODE_ESCAPE])
    {
        sGameApplication->Shutdown();
    }

    return NULL;
}

//
// Update ingame state, called each frame
//
void GameState::Update(uint32_t msDiff)
{
    SDL_Renderer* renderer = sGameApplication->GetRenderer();
    Console* console = sGameApplication->GetConsole();

    // Clear everything
    SDL_RenderClear(renderer);

    double moveStep = CalculateMoveStep(msDiff);

    double moveX = 0;
    double moveY = 0;

    const uint8_t* key_state = SDL_GetKeyboardState(NULL);
    if (key_state[SDL_SCANCODE_LEFT]) moveX += -moveStep;
    if (key_state[SDL_SCANCODE_RIGHT]) moveX += moveStep;
    if (key_state[SDL_SCANCODE_UP]) moveY += -moveStep;
    if (key_state[SDL_SCANCODE_DOWN]) moveY += moveStep;

    Camera* camera = _currentLevel->GetCamera();
    int32_t cameraX = camera->GetX();
    int32_t cameraY = camera->GetY();
    float scaleX, scaleY;
    camera->GetScale(&scaleX, &scaleY);

    uint32_t cameraWidth, cameraHeight;
    camera->GetCameraSize(&cameraWidth, &cameraHeight);

    int32_t centerX = cameraX + int32_t((cameraWidth / 2) / scaleX);
    int32_t centerY = cameraY + int32_t((cameraHeight / 2) / scaleY);

    SDL_Rect rect = { centerX - 50 / 2, centerY - 110 / 2, 50, 110 };

    //cout << "moveStep: " << moveX << " " << moveY << endl;

    // TODOS
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //!!!!!! THIS WILL HAVE TO DEAL WITH DOUBLE VALUES NOT INTS, THIS CAUSES SCREEN MOVEMENT TEARING !!!!!!
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Ladder X Solid wall stuff: Happens ONLY when ladder AND wall are in the same spatial hash rectangle 
    // Resource loader - DO NOT STORE SHARED PTRS IN EXTRA DATA
    MovementInfo moveInfo = _currentLevel->GetCollisionWorld()->ResolveMovement(&rect, moveX, moveY);
    //LOG(ToStr(moveInfo.movementX) + " " + ToStr(moveInfo.movementY));

    camera->MoveX(moveInfo.movementX);
    camera->MoveY(moveInfo.movementY);

    _currentLevel->Update(msDiff);

    console->Update(msDiff, renderer);

    SDL_RenderPresent(renderer);
}

static int32_t modulo = 0;
double GameState::CalculateMoveStep(uint32_t msDiff)
{
    
    //cout << "msDiff=  " << msDiff << endl;
    /*SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(0, &mode);
    float frameLen = 1000.0f / (float)mode.refresh_rate;

    int32_t moveStep = 4;
    if (msDiff > frameLen)
    {
        moveStep *= (float)msDiff / frameLen;
        //cout << "moveStep = " << moveStep << ", msDiff = " << msDiff << endl;
    }*/

    //return moveStep;

    return (double)msDiff * 0.4f;
}