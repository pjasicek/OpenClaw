#ifndef __COMMAND_HANDLER_H__
#define __COMMAND_HANDLER_H__

class Console;
class CommandHandler
{
public:
    static void HandleCommand(const char* command, void* userdata);

private:
    static bool AddPowerup(PowerupType type, int duration, bool& executed, std::string command, Console* pConsole);
};

#endif