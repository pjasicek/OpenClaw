#ifndef ENGINE_PROCESSMGR_H_
#define ENGINE_PROCESSMGR_H_

#include <memory>
#include <list>
#include <stdint.h>

#include "Process.h"

typedef std::list<StrongProcessPtr> ProcessList;

class ProcessMgr
{
public:
    ProcessMgr() { }
    ~ProcessMgr();

    // Interface
    uint32_t UpdateProcesses(uint32_t msDiff);
    WeakProcessPtr AttachProcess(StrongProcessPtr process);
    void AbortAllProcesses(bool immediate);

    uint32_t GetProcessCount() const { return _processList.size(); }

private:
    void ClearAllProcesses();

    ProcessList _processList;
};

#endif