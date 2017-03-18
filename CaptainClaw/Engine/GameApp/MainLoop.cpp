#include "BaseGameApp.h"

#include "MainLoop.h"

int RunGameEngine(int argc, char** argv)
{
    if (SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH) != 0)
    {
        LOG_WARNING("Failed to set high priority class to this process");
    }

#ifndef ANDROID
    std::string configDir = "";
#else
    std::string configDir = "/sdcard/claw/";
#endif

    // Load options
    if (!g_pApp->LoadGameOptions(std::string(configDir + "config.xml").c_str()))
    {
        LOG_ERROR("Could not load game options. Exiting.");
        return -1;
    }

    // Initialize game instance
    if (!g_pApp->Initialize(argc, argv))
    {
        LOG_ERROR("Failed to initialize. Exiting.");
        return -1;
    }

    // Run the game
    return g_pApp->Run();
}
