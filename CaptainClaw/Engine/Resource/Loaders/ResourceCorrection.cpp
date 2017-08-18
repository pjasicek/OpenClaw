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
    else if (resourceName.find("/level2/images/powderkeg/frame") != std::string::npos)
    {
        pPid->offsetY += 20;
    }
    else if (resourceName.find("/level3/images/powderkeg/frame") != std::string::npos)
    {
        pPid->offsetY -= 24;
    }
    else if (resourceName.find("/level6/images/powderkeg/frame") != std::string::npos)
    {
        pPid->offsetY += 4;
    }
    else if (resourceName.find("/level3/images/ratbomb/") != std::string::npos)
    {
        pPid->offsetY -= 40;
    }
    else if (resourceName == "/claw/images/frame383.pid" ||
        resourceName == "/claw/images/frame384.pid" ||
        resourceName == "/claw/images/frame385.pid" ||
        resourceName == "/claw/images/frame386.pid" ||
        resourceName == "/claw/images/frame387.pid" ||
        resourceName == "/claw/images/frame388.pid" ||
        resourceName == "/claw/images/frame389.pid")
    {
        pPid->offsetY = 0;
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
    else if (resourceName == "/level2/anis/punkrat/strike.ani")
    {
        pAni->animationFrames[1].duration = 100;
        pAni->animationFrames[2].duration = 200;
    }
}