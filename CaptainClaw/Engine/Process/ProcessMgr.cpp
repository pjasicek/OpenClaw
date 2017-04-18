#include "ProcessMgr.h"

ProcessMgr::~ProcessMgr()
{
    ClearAllProcesses();
}

uint32_t ProcessMgr::UpdateProcesses(uint32_t msDiff)
{
    uint16_t successCount = 0;
    uint16_t failCount = 0;

    ProcessList::iterator processIter = _processList.begin();
    while (processIter != _processList.end())
    {
        StrongProcessPtr currentProcess = (*processIter);

        // Save the iterator and increment the old one in case we need to remove this process from the list
        ProcessList::iterator currentIter = processIter;
        ++processIter;

        if (currentProcess->GetState() == Process::UNITIALIZED)
        {
            currentProcess->VOnInit();
        }

        if (currentProcess->GetState() == Process::RUNNING)
        {
            currentProcess->VOnUpdate(msDiff);
        }

        if (currentProcess->IsDead())
        {
            // Run appropriate exit function
            switch (currentProcess->GetState())
            {
                case Process::SUCCEEDED:
                {
                    currentProcess->VOnSuccess();
                    StrongProcessPtr child = currentProcess->RemoveChild();
                    if (child)
                    {
                        AttachProcess(child);
                    }
                    else
                    {
                        ++successCount;
                    }
                    break;
                }
                case Process::FAILED:
                {
                    currentProcess->VOnFail();
                    ++failCount;
                    break;
                }
                case Process::ABORTED:
                {
                    currentProcess->VOnAbort();
                    ++failCount;
                    break;
                }
                default:
                {
                    break;
                }
            }

            // Remove the process and destroy it
            _processList.erase(currentIter);
        }
    }

    return ((successCount << 16) | failCount);
}

WeakProcessPtr ProcessMgr::AttachProcess(StrongProcessPtr process)
{
    _processList.push_front(process);
    return WeakProcessPtr(process);
}

void ProcessMgr::ClearAllProcesses()
{
    _processList.clear();
}

void ProcessMgr::AbortAllProcesses(bool immediate)
{
    ProcessList::iterator processIter = _processList.begin();
    while (processIter != _processList.end())
    {
        ProcessList::iterator currentIter = processIter;
        ++processIter;

        StrongProcessPtr currentProcess = (*currentIter);
        if (currentProcess->IsAlive())
        {
            currentProcess->SetState(Process::ABORTED);
            if (immediate)
            {
                currentProcess->VOnAbort();
                _processList.erase(currentIter);
            }
        }
    }
}
