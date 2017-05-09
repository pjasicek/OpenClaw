#ifndef __RESOURCE_CORRECTION_H__
#define __RESOURCE_CORRECTION_H__

#include "../../SharedDefines.h"

// Not all resources from Monolith are perfect - some have screwed up offsets
// and stuff like that
// This is meant to be a hook in which certain resources can be corrected

void OnPidLoaded(const char* resource, WapPid* pPid);
void OnAniLoaded(const char* resource, WapAni* pAni);

#endif