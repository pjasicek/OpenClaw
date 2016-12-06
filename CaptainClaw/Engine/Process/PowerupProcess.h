#ifndef __POWERUP_PROCESS_H__
#define __POWERUP_PROCESS_H__

#include "../SharedDefines.h"
#include "Process.h"

class PowerupProcess : public Process
{
public:
    PowerupProcess(int32 msDuration);

    // Override Process API
    virtual void VOnUpdate(uint32 msDiff) override;
    virtual void VOnSuccess() override;
    virtual void VOnAbort() override;

protected:
    int32 m_MsTimeLeft;
};

#endif