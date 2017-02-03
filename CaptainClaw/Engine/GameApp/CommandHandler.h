#ifndef __COMMAND_HANDLER_H__
#define __COMMAND_HANDLER_H__

class CommandHandler
{
public:
    static void HandleCommand(const char* command, void* userdata);
};

#endif