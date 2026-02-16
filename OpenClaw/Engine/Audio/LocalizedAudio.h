#ifndef __LOCALIZED_AUDIO_H__
#define __LOCALIZED_AUDIO_H__

#include <string>

//=============================================================================
// LocalizedAudio - Helper for resolving language-specific audio paths
//
// This system allows the game to load localized voice/sound files based on
// the player's language setting. For example, Spanish audio files would be
// placed in a parallel directory structure:
//
//   Original (English): /CLAW/SOUNDS/1054.WAV
//   Spanish:            /CLAW_ES/SOUNDS/1054.WAV
//   German:             /CLAW_DE/SOUNDS/1054.WAV
//
// The system will first try to load from the localized path, and fall back
// to the original path if the localized version doesn't exist.
//
// Supported language codes:
//   "en" - English (default, uses original paths)
//   "es" - Spanish (Espanol)
//   "de" - German (Deutsch)
//   "fr" - French (Francais)
//   "pl" - Polish (Polski)
//   "ru" - Russian
//   "pt" - Portuguese
//
// To add Spanish support:
// 1. Extract Spanish audio from the original Captain Claw Spanish ISO
// 2. Place files in the appropriate directory structure (e.g., CLAW_ES/SOUNDS/)
// 3. Add to ASSETS.ZIP or a separate archive
// 4. Set language to "es" in config.xml
//=============================================================================

namespace LocalizedAudio
{
    // Transform an audio path to its localized version
    // Example: GetLocalizedPath("/CLAW/SOUNDS/1054.WAV", "es")
    //          returns "/CLAW_ES/SOUNDS/1054.WAV"
    //
    // Directories that get localized:
    //   /CLAW/SOUNDS/     -> /CLAW_XX/SOUNDS/
    //   /LEVEL1/SOUNDS/   -> /LEVEL1_XX/SOUNDS/ (for level-specific voice lines)
    //   /LEVEL2/SOUNDS/   -> /LEVEL2_XX/SOUNDS/
    //   etc.
    //
    // Non-voice sounds (like /GAME/SOUNDS/) are NOT localized since they
    // are sound effects, not voice lines.
    std::string GetLocalizedPath(const std::string& originalPath, const std::string& language);

    // Check if a path should be localized (voice/speech paths only)
    bool ShouldLocalize(const std::string& path);

    // Get the language suffix (e.g., "_ES" for Spanish)
    std::string GetLanguageSuffix(const std::string& language);

    // List of paths that contain voice audio and should be localized
    bool IsVoicePath(const std::string& path);
}

#endif // __LOCALIZED_AUDIO_H__
