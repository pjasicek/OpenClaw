#include <SDL2/SDL.h>

#include "Engine/GameApp/BaseGameApp.h"

class ClawGameApp : public BaseGameApp
{
public:
    virtual const char* VGetGameTitle() { return "Captain Claw"; }
    virtual const char* VGetGameAppDirectory() { return SDL_GetBasePath(); }
    virtual BaseGameLogic* VCreateGameAndView();
};
