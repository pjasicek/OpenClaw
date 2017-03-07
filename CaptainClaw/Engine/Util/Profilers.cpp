#include "Profilers.h"
#include "../SharedDefines.h"

// For memory profiler
#ifdef _WIN32
#include <Windows.h>
#include <Psapi.h>
#endif

#include <string>
#include <stdint.h>
#include <iostream>
#include <SDL2/SDL.h>

CPU_PROFILER::CPU_PROFILER(std::string tag)
{
    m_Tag = tag;

    m_StartingTime = SDL_GetPerformanceCounter();
}

CPU_PROFILER::~CPU_PROFILER()
{
    uint64_t now = SDL_GetPerformanceCounter();

    if (!m_Tag.empty())
    {
        std::string s("[" + m_Tag + "]: Elapsed microseconds: " + ToStr((unsigned long)(((now - m_StartingTime) * 1000000) / SDL_GetPerformanceFrequency())));
        std::cout << s << std::endl;
    }
    else
    {
        std::string s("Elapsed microseconds: " + ToStr((unsigned long)(((now - m_StartingTime) * 1000000) / SDL_GetPerformanceFrequency())));
        std::cout << s << std::endl;
    }
}


MEMORY_PROFILER::MEMORY_PROFILER(std::string tag)
{
    m_Tag = tag;
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    m_StartingMemory = pmc.WorkingSetSize;
#else
    LOG_ERROR("Memory profiler not supported on this platform !");
#endif
}

MEMORY_PROFILER::~MEMORY_PROFILER()
{
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    SIZE_T currentMemory = pmc.WorkingSetSize;

    int32_t memoryDiff = currentMemory - m_StartingMemory;

    if (!m_Tag.empty())
    {
        std::string s("[" + m_Tag + "]: Memory difference: " + ToStr(memoryDiff));
        std::cout << s << std::endl;
    }
    else
    {
        std::string s("Memory difference: " + ToStr(memoryDiff));
        std::cout << s << std::endl;
    }
#endif
}