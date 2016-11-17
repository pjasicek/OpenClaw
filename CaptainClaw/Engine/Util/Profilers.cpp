#include "Profilers.h"
#include "../SharedDefines.h"

#include <Windows.h>
#include <Psapi.h>
#include <string>
#include <stdint.h>
#include <iostream>

CPU_PROFILER::CPU_PROFILER(std::string tag)
{
    m_Tag = tag;

    QueryPerformanceCounter(&m_StartingTime);
}

CPU_PROFILER::~CPU_PROFILER()
{
    LARGE_INTEGER EndingTime, ElapsedMicroseconds;
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&EndingTime);

    ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - m_StartingTime.QuadPart;
    ElapsedMicroseconds.QuadPart *= 1000000;
    ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

    if (!m_Tag.empty())
    {
        std::string s("[" + m_Tag + "]: Elapsed microseconds: " + ToStr((uint32)ElapsedMicroseconds.QuadPart));
        std::cout << s << std::endl;
    }
    else
    {
        std::string s("Elapsed microseconds: " + ToStr((uint32)ElapsedMicroseconds.QuadPart));
        std::cout << s << std::endl;
    }
}


MEMORY_PROFILER::MEMORY_PROFILER(std::string tag)
{
    m_Tag = tag;

    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    m_StartingMemory = pmc.WorkingSetSize;
}

MEMORY_PROFILER::~MEMORY_PROFILER()
{
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
}