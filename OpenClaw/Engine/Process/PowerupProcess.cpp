#include "PowerupProcess.h"
#include "../Events/EventMgr.h"
#include "../Events/Events.h"

PowerupProcess::PowerupProcess(int32 msDuration)
    :
    m_MsTimeLeft(msDuration)
{ }

// Override Process API
void PowerupProcess::VOnUpdate(uint32 msDiff)
{
    int32 prevSecsLeft = m_MsTimeLeft % 1000;

    m_MsTimeLeft -= msDiff;
    if (m_MsTimeLeft <= 0)
    {
        SetState(SUCCEEDED);
        return;
    }

    int32 currSecsLeft = m_MsTimeLeft % 1000;
    if (prevSecsLeft != currSecsLeft)
    {
        // Raise event to update stopwatch HUD
    }
}

void PowerupProcess::VOnSuccess()
{
    // Raise event to hide stopwatch HUD, should be triggered, not queued
}

void PowerupProcess::VOnAbort()
{
    // Raise event to hide stopwatch HUD, should be triggered, not queued
}