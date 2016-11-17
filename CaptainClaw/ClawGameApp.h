#include <SDL.h>

#include "Engine/GameApp/BaseGameApp.h"

class ClawGameApp : public BaseGameApp
{
public:
    virtual char* VGetGameTitle() { return "Captain Claw"; }
    virtual char* VGetGameAppDirectory() { return SDL_GetBasePath(); }
    virtual BaseGameLogic* VCreateGameAndView();
};