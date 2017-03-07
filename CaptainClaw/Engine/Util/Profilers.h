#ifndef __PROFILERS_H__
#define __PROFILERS_H__

#include <stdint.h>
#include <string>

class CPU_PROFILER
{
public:
    CPU_PROFILER(std::string tag = 0);
    ~CPU_PROFILER();

private:
    uint64_t m_StartingTime;
    std::string m_Tag;
};

class MEMORY_PROFILER
{
public:
    MEMORY_PROFILER(std::string tag = 0);
    ~MEMORY_PROFILER();

private:
    std::string m_Tag;
    int32_t m_StartingMemory;
};

#endif