#include "ResourceCorrection.h"

void OnPidLoaded(const char* resource, WapPid* pPid)
{
    const std::string resourceName(resource);

    if (resourceName == "/level2/images/towercannonleft/frame002.pid" ||
        resourceName == "/level2/images/towercannonleft/frame004.pid" ||
        resourceName == "/level2/images/towercannonleft/frame005.pid")
    {
        pPid->offsetX -= 2;
    }
    else if (resourceName == "/level2/images/towercannonright/frame002.pid" ||
             resourceName == "/level2/images/towercannonright/frame004.pid" ||
             resourceName == "/level2/images/towercannonright/frame005.pid")
    {
        pPid->offsetX += 2;
    }
    else if (resourceName.find("/images/powderkeg/frame") != std::string::npos)
    {
        pPid->offsetY -= 45;
    }
    else if (resourceName.find("/level2/images/cannon/frame") != std::string::npos)
    {
        pPid->offsetY -= 25;
    }
    else if (resourceName.find("/level8/images/cannon/frame") != std::string::npos)
    {
        pPid->offsetY -= 45;
    }
    else if (resourceName.find("/level8/images/gabrielcannon/frame") != std::string::npos)
    {
        pPid->offsetY -= 80;
    }
    else if (resourceName.find("/images/ratbomb/frame") != std::string::npos)
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
    else if (resourceName.find("/images/breakjem/") != std::string::npos)
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
    else if (resourceName == "/level9/anis/springbox/spring.ani")
    {
        pAni->animationFrames[0].imageFileId = 1;
        pAni->animationFrames[1].duration = 100;
        pAni->animationFrames[2].duration = 100;
        pAni->animationFrames[3].duration = 100;
    }
    else if (resourceName.find("/level8/anis/gabrielcannon") != std::string::npos)
    {
        int factor = 4;
        if (resourceName == "/level8/anis/gabrielcannon/horzifire.ani")
        {
            factor = 8;
        }
        for (int idx = 0; idx < pAni->animationFramesCount; idx++)
        {
            pAni->animationFrames[idx].duration *= factor;
        }
    }
    else if (resourceName.find("/level9/anis/sawblade") != std::string::npos)
    {
        int factor = 2;
        for (int idx = 0; idx < pAni->animationFramesCount; idx++)
        {
            pAni->animationFrames[idx].duration *= factor;
        }
    }
    else if (resourceName.find("/level10/anis/parrot/idle") != std::string::npos)
    {
        int factor = 3;
        for (int idx = 0; idx < pAni->animationFramesCount; idx++)
        {
            pAni->animationFrames[idx].duration *= factor;
        }
    }
}
