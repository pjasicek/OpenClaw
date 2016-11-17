#include "Process.h"

Process::Process()
{
    _state = UNITIALIZED;
}

Process::~Process()
{
    if (_pChild)
    {
        _pChild->VOnAbort();
    }
}

StrongProcessPtr Process::RemoveChild()
{
    if (_pChild)
    {
        StrongProcessPtr pChild = _pChild;
        _pChild.reset();

        return pChild;
    }

    return NULL;
}