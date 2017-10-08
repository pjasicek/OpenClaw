#ifndef LOGGER_H_
#define LOGGER_H_

#include <SDL2/SDL.h>
#include <string>
#include <memory.h>

namespace Logger
{
    void GetOutputString(std::string& outOutputBuffer, const std::string& tag, const std::string& message, const char* funcName, const char* sourceFile, unsigned int lineNum);
}


// Errors are bad and potentially fatal.  They are presented as a dialog with Abort, Retry, and Ignore.  Abort will
// break into the debugger, retry will continue the game, and ignore will continue the game and ignore every subsequent 
// call to this specific error.  They are ignored completely in release mode.
#define LOG_ERROR(str) \
do \
{ \
    std::string s((str)); \
    std::string out; \
    Logger::GetOutputString(out, "", s, __FUNCTION__, NULL, __LINE__); \
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", out.c_str()); \
} \
while (0); \

#define LOG_ASSERT(str) \
do \
{ \
    std::string s((str)); \
    std::string out; \
    Logger::GetOutputString(out, "", s, __FUNCTION__, NULL, __LINE__); \
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", out.c_str()); \
    assert(false); \
} \
while (0); \

// Warnings are recoverable.  They are just logs with the "WARNING" tag that displays calling information.  The flags
// are initially set to WARNINGFLAG_DEFAULT (defined in debugger.cpp), but they can be overridden normally.
#define LOG_WARNING(str) \
do \
{ \
    std::string s((str)); \
    std::string out; \
    Logger::GetOutputString(out, std::string(""), s, __FUNCTION__, NULL, __LINE__); \
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", out.c_str()); \
} \
while (0);\

#define LOG_TAG(tag, str) \
do \
{ \
    std::string s((str)); \
    std::string out; \
    Logger::GetOutputString(out, tag, s, NULL, NULL, 0); \
    SDL_Log("%s", out.c_str()); \
} \
    while (0);\

#define LOG(str) \
do \
{ \
    std::string s((str)); \
    std::string out; \
    Logger::GetOutputString(out, "", s, NULL, NULL, 0); \
    SDL_Log("%s", out.c_str()); \
} \
while (0);\

#define LOG_TRACE(str) \
do \
{ \
    std::string s((str)); \
    std::string out; \
    Logger::GetOutputString(out, "", s, __FUNCTION__, NULL, __LINE__); \
    SDL_Log("%s", out.c_str()); \
} \
while (0); \

#endif