#include "Logger.h"

namespace Logger
{
    void GetOutputString(std::string& outOutputBuffer, const std::string& tag, const std::string& message, const char* funcName, const char* sourceFile, unsigned int lineNum)
    {
        if (funcName != NULL && sourceFile != NULL)
        {
            outOutputBuffer = "[" + std::string(sourceFile) + ", " + funcName + "] ";
        }
        else if (funcName != NULL)
        {
            outOutputBuffer = "[" + std::string(funcName) + "] ";
        }
        else if (sourceFile != NULL)
        {
            outOutputBuffer = "[" + std::string(sourceFile) + "] ";
        }

        outOutputBuffer += message;

        /*if (lineNum != 0)
        {
            outOutputBuffer += "\nLine: ";
            char lineNumBuffer[11];
            memset(lineNumBuffer, 0, sizeof(char));
            outOutputBuffer += _itoa_s(lineNum, lineNumBuffer, 10);
        }*/

        outOutputBuffer += "\n";
    }
}