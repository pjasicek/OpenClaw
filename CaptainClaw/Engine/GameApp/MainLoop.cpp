#include "BaseGameApp.h"

#include "MainLoop.h"

#include <fstream>

int RunGameEngine(int argc, char** argv)
{
    if (SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH) != 0)
    {
        LOG_WARNING("Failed to set high priority class to this process");
    }

    std::string configDir = "";

#if (__ANDROID__ == 1)
    configDir = "/sdcard/claw/";
#elif (__LINUX__ == 1)
    configDir = "/usr/share/captainclaw/config.xml";
#elif (__WINDOWS__ == 1)
    configDir = "";
#endif

    // Temporary hack - always prefer config in the same folder as binary to default config
    std::ifstream f("config.xml");
    if (f.good())
    {
        configDir = "";
        f.close();
    }

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
