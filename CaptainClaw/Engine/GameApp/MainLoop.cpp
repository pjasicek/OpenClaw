#include "BaseGameApp.h"

#include "MainLoop.h"

int RunGameEngine(int argc, char** argv)
{
    int tmpDebugFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    tmpDebugFlag |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(tmpDebugFlag);

    if (!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS))
    {
        LOG_WARNING("Failed to set high priority class to this process");
    }

    // Load options
    if (!g_pApp->LoadGameOptions("config.xml"))
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