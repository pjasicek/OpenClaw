#include "ResourceCorrection.h"

void OnPidLoaded(const char* resource, WapPid* pPid)
{
    const std::string resourceName(resource);

    if (resourceName == "/level2/images/towercannonleft/frame002.pid")
    {
        pPid->offsetX -= 2;
    }
    else if (resourceName == "/level2/images/towercannonleft/frame004.pid")
    {
        pPid->offsetX -= 2;
    }
    else if (resourceName == "/level2/images/towercannonleft/frame005.pid")
    {
        pPid->offsetX -= 2;
    }
}

void OnAniLoaded(const char* resource, WapAni* pAni)
{
    const std::string resourceName(resource);
}