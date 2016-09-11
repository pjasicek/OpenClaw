//#define SDL_MAIN_HANDLED
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <iostream>
#include <libwap.h>
#include <SDL.h>
#include <SDL_Image.h>
#include <SDL_ttf.h>

#include <Windows.h>

#include "Console.h"
#include "Audio\AudioMgr.h"

#include "Game.h"

int main(int argc, char* argv[])
{
    if (!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS))
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Failed to set high priority class to this process");
    }

    

    Game game("CLAW.REZ");
    game.Run();

    return 0;
}