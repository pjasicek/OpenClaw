//#define SDL_MAIN_HANDLED
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "Engine/GameApp/MainLoop.h"
#include "ClawGameApp.h"

ClawGameApp testApp;

int main(int argc, char* argv[])
{
    return RunGameEngine(argc, argv);
}