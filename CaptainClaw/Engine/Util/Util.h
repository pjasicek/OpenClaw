#ifndef UTIL_H_
#define UTIL_H_

#include <memory>
#include <vector>
#include <string>
#include <libwap.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

struct TileCollisionPrototype;
struct TileDescription;

namespace Util
{
    std::string ConvertToThreeDigitsString(int32_t num);

    SDL_Rect WwdRectToSDLRect(WwdRect& rect);

    void ParseCollisionRectanglesFromTile(TileCollisionPrototype* tilePrototype, TileDescription* tileDesc);

    void SplitStringIntoVector(std::string str, std::vector<std::string>& vec);

    void PrintRect(SDL_Rect rect, std::string comment);

    int GetRandomNumber(int fromRange, int toRange);

    void PlayRandomSoundFromList(const std::vector<std::string>& sounds, int volume = 100);

    int GetSoundDurationMs(const std::string& soundPath);
    int GetSoundDurationMs(Mix_Chunk* pSound);

    SDL_Texture* CreateSDLTextureRect(int width, int height, SDL_Color color, SDL_Renderer* pRenderer);

    void PlayRandomHitSound();

    uint32_t CalcCRC32(const char* pData, size_t dataLen);

    template<typename T>
    T GetRandomValueFromVector(const std::vector<T>& container)
    {
        int randIdx = GetRandomNumber(0, container.size() - 1);
        return container[randIdx];
    }
}

#endif