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
    if (resourceName == "/level2/images/towercannonright/frame002.pid")
    {
        pPid->offsetX += 2;
    }
    else if (resourceName == "/level2/images/towercannonright/frame004.pid")
    {
        pPid->offsetX += 2;
    }
    else if (resourceName == "/level2/images/towercannonright/frame005.pid")
    {
        pPid->offsetX += 2;
    }
}

void OnAniLoaded(const char* resource, WapAni* pAni)
{
    const std::string resourceName(resource);

    /*for (int animFrameIdx = 0; animFrameIdx < pAni->animationFramesCount; ++animFrameIdx)
    {
        if (pAni->animationFrames[animFrameIdx].eventFilePath != NULL)
        {
            std::string soundPath(pAni->animationFrames[animFrameIdx].eventFilePath);
            LOG("Resource: " + resourceName + " - Sound: " + soundPath);
            pAni->
        }
    }*/

    if (resourceName == "/level1/anis/rat/dead.ani")
    {
        // Disable all sounds for this animation
        pAni->unk0 = 1;
    }
}