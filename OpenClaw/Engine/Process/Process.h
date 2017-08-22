#ifndef ENGINE_PROCESS_H_
#define ENGINE_PROCESS_H_

#include <assert.h>
#include <stdint.h>
#include <memory>

class Process;
typedef std::shared_ptr<Process> StrongProcessPtr;
typedef std::weak_ptr<Process> WeakProcessPtr;

class Process
{
public:
    enum State
    {
        // Processes are neither dead nor alive
        UNITIALIZED = 0, // Created but not running
        REMOVED,         // Removed from the process list but not desstroyed

        // Living processes
        RUNNING,         // Initialized and running
        PAUSED,          // Initialized but paused

        // Dead processes
        SUCCEEDED,       // Completed successfully
        FAILED,          // Failed to complete
        ABORTED          // Aborted, my not have started
    };

    Process();
    virtual ~Process();

    inline void Succeed();
    inline void Fail();

    inline void Pause();
    inline void UnPause();

    State GetState() const { return _state; }
    void SetState(State newState) { _state = newState; }

    bool IsAlive() const { return _state == RUNNING || _state == PAUSED; }
    bool IsDead() const { return _state == SUCCEEDED || _state == FAILED || _state == ABORTED; }
    bool IsRemoved() const { return _state == REMOVED; }
    bool IsPaused() const { return _state == PAUSED; }

    // Child functions
    inline void AttachChild(StrongProcessPtr pChild);
    StrongProcessPtr RemoveChild();
    StrongProcessPtr PeekChild() { return _pChild; }

    // Interface
    virtual void VOnInit() { _state = RUNNING; }
    virtual void VOnUpdate(uint32_t msDiff) = 0;
    virtual void VOnSuccess() { }
    virtual void VOnFail() { }
    virtual void VOnAbort() { }

private:
    State _state;
    StrongProcessPtr _pChild;
};

//-----------------------------------------------------------------------------
// Inline function definitions
//-----------------------------------------------------------------------------

inline void Process::Succeed()
{
    assert(_state == RUNNING || _state == PAUSED);
    _state = SUCCEEDED;
}

inline void Process::Fail()
{
    assert(_state == RUNNING || _state == PAUSED);
    _state = FAILED;
}

inline void Process::Pause()
{
    if (_state == RUNNING)
    {
        _state = PAUSED;
    }
}

inline void Process::UnPause()
{
    if (_state == PAUSED)
    {
        _state = RUNNING;
    }
}

inline void Process::AttachChild(StrongProcessPtr pChild)
{
    if (_pChild)
    {
        _pChild->AttachChild(pChild);
    }
    else
    {
        _pChild = pChild;
    }
}

#endif // ENGINE_PROCESS_H_