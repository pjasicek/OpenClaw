#ifndef GAME_H_
#define GAME_H_

#include <libwap.h>

const uint16_t MAXIMUM_LEVEL = 14;

enum GAME_STATE
{
    GAME_STATE_MENU,
    GAME_STATE_INGAME,
    GAME_STATE_FINISH_SCORE,
    GAME_STATE_CUTSCENE
};

struct KeyboardState
{
    bool isDownPressed;
    bool isUpPressed;
    bool isLeftPressed;
    bool isRightPressed;
    bool isCtrlPressed;
    bool isEnterPressed;
    bool isReturnPressed;
    bool isShiftPressed;
    bool isLiftPressed;
    bool isJumpPressed;
};

class Console;

class Game
{
public:
    Game(const char* rezArchivePath, Console* console = NULL);
    ~Game();

    bool Run();

    static void HandleConsoleCommand(const char* command, void* userdata);

private:
    void ProcessEvents();

    void ProcessCutsceneEvents();
    void ProcessFinishScoreEvents();
    void ProcessMenuEvents();
    void ProcessIngameEvents();

    RezArchive* _clawRezArchive;
    Console* _console;

    uint16_t _currentState;
    uint16_t _currentLevelNum;
};

#endif