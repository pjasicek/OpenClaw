#ifndef UTIL_H_
#define UTIL_H_

#include <vector>
#include <string>
#include <libwap.h>
#include <SDL.h>
#include "../Level/Level.h"

namespace Util
{
    std::string ConvertToThreeDigitsString(int32_t num);

    SDL_Rect WwdRectToSDLRect(WwdRect& rect);

    void ParseCollisionRectanglesFromTile(TilePrototype* tilePrototype, WwdTileDescription* tileDesc, int32_t tileId);
}






#define NOMINMAX
#include <Windows.h>
#include <iostream>

#define START_QUERY_PERFORMANCE_TIMER \
    LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds; \
    LARGE_INTEGER Frequency; \
    QueryPerformanceFrequency(&Frequency); \
    QueryPerformanceCounter(&StartingTime); \

#define END_QUERY_PERFORMANCE_TIMER \
    QueryPerformanceCounter(&EndingTime); \
    ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart; \
    ElapsedMicroseconds.QuadPart *= 1000000; \
    ElapsedMicroseconds.QuadPart /= Frequency.QuadPart; \
    std::cout << "Elapsed microseconds: " << ElapsedMicroseconds.QuadPart << std::endl; \

#endif