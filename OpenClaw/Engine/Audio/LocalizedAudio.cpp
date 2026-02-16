#include "LocalizedAudio.h"
#include <algorithm>
#include <cctype>

namespace LocalizedAudio
{
    // Convert string to uppercase for comparison
    static std::string ToUpper(const std::string& str)
    {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }

    std::string GetLanguageSuffix(const std::string& language)
    {
        if (language.empty() || language == "en" || language == "EN")
        {
            return "";  // English uses original paths
        }

        // Convert to uppercase for the suffix
        std::string suffix = "_";
        for (char c : language)
        {
            suffix += std::toupper(c);
        }
        return suffix;
    }

    bool IsVoicePath(const std::string& path)
    {
        std::string upperPath = ToUpper(path);

        // Captain Claw voice lines
        if (upperPath.find("/CLAW/SOUNDS/") != std::string::npos)
        {
            return true;
        }

        // Level-specific voice lines (enemy voices, boss voices, etc.)
        // These follow the pattern /LEVELX/SOUNDS/
        for (int level = 1; level <= 14; ++level)
        {
            std::string levelPath = "/LEVEL" + std::to_string(level) + "/SOUNDS/";
            if (upperPath.find(levelPath) != std::string::npos)
            {
                // Check if it's likely a voice file (character subdirectories)
                // Voice paths typically have subdirectories like /SOLDIER/, /OFFICER/, /RAUX/
                if (upperPath.find("/SOLDIER/") != std::string::npos ||
                    upperPath.find("/OFFICER/") != std::string::npos ||
                    upperPath.find("/RAT/") != std::string::npos ||
                    upperPath.find("/RAUX/") != std::string::npos ||
                    upperPath.find("/BOSS/") != std::string::npos ||
                    upperPath.find("/TIGER/") != std::string::npos ||
                    upperPath.find("/CUTTHROAT/") != std::string::npos ||
                    upperPath.find("/ROBBER/") != std::string::npos ||
                    upperPath.find("/TOWNGUARD/") != std::string::npos ||
                    upperPath.find("/BEAR/") != std::string::npos ||
                    upperPath.find("/REDTAIL/") != std::string::npos ||
                    upperPath.find("/GABRIEL/") != std::string::npos ||
                    upperPath.find("/MARROW/") != std::string::npos ||
                    upperPath.find("/AQUATIS/") != std::string::npos ||
                    upperPath.find("/SEAGULL/") != std::string::npos ||
                    upperPath.find("/CHAMELEON/") != std::string::npos ||
                    upperPath.find("/MERCAT/") != std::string::npos ||
                    upperPath.find("/SIREN/") != std::string::npos)
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ShouldLocalize(const std::string& path)
    {
        return IsVoicePath(path);
    }

    std::string GetLocalizedPath(const std::string& originalPath, const std::string& language)
    {
        // If English or no language set, return original
        if (language.empty() || language == "en" || language == "EN")
        {
            return originalPath;
        }

        // Only localize voice paths
        if (!ShouldLocalize(originalPath))
        {
            return originalPath;
        }

        std::string suffix = GetLanguageSuffix(language);
        std::string localizedPath = originalPath;
        std::string upperPath = ToUpper(originalPath);

        // Transform /CLAW/SOUNDS/ to /CLAW_XX/SOUNDS/
        size_t pos = upperPath.find("/CLAW/");
        if (pos != std::string::npos)
        {
            localizedPath = originalPath.substr(0, pos) + "/CLAW" + suffix + originalPath.substr(pos + 5);
            return localizedPath;
        }

        // Transform /LEVELX/SOUNDS/ to /LEVELX_XX/SOUNDS/
        for (int level = 1; level <= 14; ++level)
        {
            std::string levelStr = "/LEVEL" + std::to_string(level) + "/";
            pos = upperPath.find(levelStr);
            if (pos != std::string::npos)
            {
                size_t endPos = pos + levelStr.length() - 1;  // Position of the trailing /
                localizedPath = originalPath.substr(0, endPos) + suffix + originalPath.substr(endPos);
                return localizedPath;
            }
        }

        return originalPath;
    }
}
