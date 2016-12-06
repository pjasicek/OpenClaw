#ifndef __CONVERTERS_H__
#define __CONVERTERS_H__

#include <algorithm>
#include <libwap.h>
#include "SharedDefines.h"
#include "Actor/ActorTemplates.h"

#define INSERT_POSITION_COMPONENT(x, y, rootElem) \
{ \
   TiXmlElement* elem = new TiXmlElement("PositionComponent"); \
   rootElem->LinkEndChild(elem); \
   XML_ADD_2_PARAM_ELEMENT("Position", "x", x, "y", y, elem); \
} \

#define INSERT_COLLISION_COMPONENT(width, height, rootElem) \
{ \
   TiXmlElement* elem = new TiXmlElement("CollisionComponent"); \
   rootElem->LinkEndChild(elem); \
   XML_ADD_2_PARAM_ELEMENT("CollisionSize", "width", width, "height", height, elem); \
} \

inline TiXmlElement* CreatePositionComponent(int x, int y)
{
    TiXmlElement* elem = new TiXmlElement("PositionComponent");
    XML_ADD_2_PARAM_ELEMENT("Position", "x", x, "y", y, elem);

    return elem;
}

inline TiXmlElement* CreateCollisionComponent(int width, int height)
{
    TiXmlElement* elem = new TiXmlElement("CollisionComponent");
    XML_ADD_2_PARAM_ELEMENT("CollisionSize", "width", width, "height", height, elem);

    return elem;
}

inline TiXmlElement* CreatePhysicsComponent(bool canClimb, bool canBounce, bool canJump, int maxJumpHeight, int width, int height, double gravityScale, double friction, double density)
{
    TiXmlElement* elem = new TiXmlElement("PhysicsComponent");

    XML_ADD_TEXT_ELEMENT("CanClimb", ToStr(canClimb).c_str(), elem);
    XML_ADD_TEXT_ELEMENT("CanBounce", ToStr(canBounce).c_str(), elem);
    XML_ADD_TEXT_ELEMENT("CanJump", ToStr(canJump).c_str(), elem);
    XML_ADD_TEXT_ELEMENT("JumpHeight", ToStr(maxJumpHeight).c_str(), elem);
    XML_ADD_2_PARAM_ELEMENT("CollisionSize", "width", width, "height", height, elem);
    XML_ADD_TEXT_ELEMENT("GravityScale", ToStr(gravityScale).c_str(), elem);
    XML_ADD_TEXT_ELEMENT("Friction", ToStr(friction).c_str(), elem);
    XML_ADD_TEXT_ELEMENT("Density", ToStr(density).c_str(), elem);

    return elem;
}

inline TiXmlElement* CreateControllableComponent(bool active)
{
    TiXmlElement* elem = new TiXmlElement("ClawControllableComponent");
    
    XML_ADD_TEXT_ELEMENT("IsActive", ToStr(active).c_str(), elem);

    return elem;
}

inline TiXmlElement* CreateAnimationComponent(const char* aniPath)
{
    TiXmlElement* elem = new TiXmlElement("AnimationComponent");

    XML_ADD_TEXT_ELEMENT("AnimationPath", aniPath, elem);

    return elem;
}

inline TiXmlElement* CreateSoundComponent(const char* soundPath)
{
    TiXmlElement* elem = new TiXmlElement("SoundComponent");

    XML_ADD_TEXT_ELEMENT("SoundPath", soundPath, elem);

    return elem;
}

inline TiXmlElement* CreateActorRenderComponent(const char* imagesPath, int32 zCoord, bool visible = true)
{
    TiXmlElement* elem = new TiXmlElement("ActorRenderComponent");

    XML_ADD_TEXT_ELEMENT("ImagePath", imagesPath, elem);
    XML_ADD_TEXT_ELEMENT("ZCoord", ToStr(zCoord).c_str(), elem);

    return elem;
}

TiXmlElement* CreatePlayAreaElement(int x, int y, int w, int h)
{
    TiXmlElement* pPlayAreaElement = new TiXmlElement("PlayArea");
    pPlayAreaElement->SetAttribute("x", x);
    pPlayAreaElement->SetAttribute("y", y);
    pPlayAreaElement->SetAttribute("width", w);
    pPlayAreaElement->SetAttribute("height", h);
    return pPlayAreaElement;
}


//=====================================================================================================================

TiXmlElement* CreateAnimationComponent(std::string aniPath)
{
    TiXmlElement* animationComponent = new TiXmlElement("AnimationComponent");
    XML_ADD_TEXT_ELEMENT("AnimationPath", aniPath.c_str(), animationComponent);
    return animationComponent;
}

TiXmlElement* CreateCycleAnimation(int animFrameTime)
{
    TiXmlElement* cycleElem = new TiXmlElement("Animation");
    std::string cycleStr = "cycle" + ToStr(animFrameTime);
    cycleElem->SetAttribute("type", cycleStr.c_str());
    //XML_ADD_1_PARAM_ELEMENT("Animation", "type", cycleStr.c_str(), cycleElem);

    return cycleElem;
}

TiXmlElement* CreateTriggerComponent(int enterCount, bool onceALife, bool isStatic)
{
    TiXmlElement* pTriggerComponent = new TiXmlElement("TriggerComponent");
    if (onceALife)
    {
        XML_ADD_TEXT_ELEMENT("TriggerOnce", "true", pTriggerComponent);
    }
    else if (enterCount > 0)
    {
        XML_ADD_TEXT_ELEMENT("TriggerFinitedTimes", ToStr(enterCount).c_str(), pTriggerComponent);
    }
    else
    {
        XML_ADD_TEXT_ELEMENT("TriggerUnlimited", "true", pTriggerComponent);
    }
    if (isStatic)
    {
        XML_ADD_TEXT_ELEMENT("IsStatic", "true", pTriggerComponent);
    }

    return pTriggerComponent;
}

//=====================================================================================================================

// This is really ugly as fuck
TiXmlElement* AmbientSoundToXml(WwdObject* pWwdObject)
{
    TiXmlElement* pAmbientSoundElem = new TiXmlElement("AmbientSound");

    std::string logicName = pWwdObject->logic;
    if (logicName == "GlobalAmbientSound" ||
        logicName == "AmbientSound")
    {
        if (pWwdObject->hitRect.right != 0 && pWwdObject->hitRect.bottom != 0)
        {
            pAmbientSoundElem->LinkEndChild(CreatePlayAreaElement(pWwdObject->hitRect.left, pWwdObject->hitRect.top,
                pWwdObject->hitRect.right - pWwdObject->hitRect.left, pWwdObject->hitRect.bottom - pWwdObject->hitRect.top));
        }
        else
        {
            XML_ADD_TEXT_ELEMENT("Global", "true", pAmbientSoundElem);
        }
    }
    else if (logicName == "SpotAmbientSound")
    {
        if (pWwdObject->maxX != 0 && pWwdObject->maxY != 0)
        {
            pAmbientSoundElem->LinkEndChild(CreatePlayAreaElement(pWwdObject->minX, pWwdObject->minY,
                pWwdObject->maxX - pWwdObject->minX, pWwdObject->maxY - pWwdObject->minY));
        }
        if (pWwdObject->attackRect.top != 0 && pWwdObject->attackRect.left != 0)
        {
            pAmbientSoundElem->LinkEndChild(CreatePlayAreaElement(pWwdObject->attackRect.left, pWwdObject->attackRect.top,
                pWwdObject->attackRect.right - pWwdObject->attackRect.left, pWwdObject->attackRect.bottom - pWwdObject->attackRect.top));
        }
    } 
    else
    {
        LOG_WARNING("Unknown AmbientSound logic: " + logicName);
    }

    // Sound play interval
    if (pWwdObject->moveRect.right != 0 && pWwdObject->moveRect.bottom != 0)
    {
        TiXmlElement* pPlayIntervalElem = new TiXmlElement("PlayInterval");
        pPlayIntervalElem->SetAttribute("minTimeOn", pWwdObject->moveRect.left);
        pPlayIntervalElem->SetAttribute("maxTimeOn", pWwdObject->moveRect.right);
        pPlayIntervalElem->SetAttribute("minTimeOff", pWwdObject->moveRect.top);
        pPlayIntervalElem->SetAttribute("maxTimeOff", pWwdObject->moveRect.bottom);
        pAmbientSoundElem->LinkEndChild(pPlayIntervalElem);
    }
    else
    {
        XML_ADD_TEXT_ELEMENT("Looping", "true", pAmbientSoundElem);
    }

    // Volume
    int soundVolume = 100;
    if (pWwdObject->damage != 0)
    {
        soundVolume = pWwdObject->damage;
    }

    XML_ADD_TEXT_ELEMENT("Volume", ToStr(soundVolume).c_str(), pAmbientSoundElem);

    XML_ADD_TEXT_ELEMENT("Sound", pWwdObject->sound, pAmbientSoundElem);

    return pAmbientSoundElem;
}

TiXmlElement* SoundTriggerToXml(WwdObject* pWwdObject)
{
    TiXmlElement* pSoundTriggerElem = new TiXmlElement("SoundTrigger");


    return pSoundTriggerElem;
}

TiXmlElement* StackedCratesToXml(WwdObject* pWwdObject)
{
    
}

TiXmlElement* CrateToXml(WwdObject* pWwdObject)
{

}

TiXmlElement* EyeCandyToXml(WwdObject* pWwdObject)
{
    if (std::string(pWwdObject->logic).find("AniCandy") != std::string::npos)
    {
        TiXmlElement* animElem = new TiXmlElement("AnimationComponent");
        XML_ADD_1_PARAM_ELEMENT("Animation", "type", "cycle150", animElem);

        return animElem;
    }

    return NULL;
}

TiXmlElement* CrumblingPegToXml(WwdObject* pWwdObject)
{
    TiXmlElement* pCrumblingPegAIComponent = new TiXmlElement("CrumblingPegAIComponent");

    XML_ADD_TEXT_ELEMENT("FloorOffset", "10", pCrumblingPegAIComponent);
    XML_ADD_TEXT_ELEMENT("CrumbleFrameIdx", "10", pCrumblingPegAIComponent);

    return pCrumblingPegAIComponent;
}

TiXmlElement* ElevatorToXml(WwdObject* pWwdObject)
{
    //----- [Level::Actors::ActorProperties::AI_ElevatorComponent]
    TiXmlElement* aiElevatorComponent = new TiXmlElement("KinematicComponent");
    aiElevatorComponent->SetAttribute("Type", pWwdObject->logic);
    aiElevatorComponent->SetAttribute("resource", "created");

    std::string logic = pWwdObject->logic;
    if (logic == "TriggerElevator")
    {
        XML_ADD_TEXT_ELEMENT("TriggeredBehaviour", "true", aiElevatorComponent);
    }
    else if (logic == "StartElevator")
    {
        XML_ADD_TEXT_ELEMENT("StartBehaviour", "true", aiElevatorComponent);
    }
    else if (logic == "StopElevator")
    {
        XML_ADD_TEXT_ELEMENT("StopBehaviour", "true", aiElevatorComponent);
    }
    else if (logic == "Elevator")
    {
        XML_ADD_TEXT_ELEMENT("StandardBehaviour", "true", aiElevatorComponent);
    }
    else
    {
        LOG_WARNING("Unknown Elevator: " + logic)

    }

    Point speed(0, 0);
    bool hasHorizontalMovement = false;
    bool hasVerticalMovement = false;
    if (pWwdObject->minX > 0 && pWwdObject->maxX > 0)
    {
        hasHorizontalMovement = true;
    }
    if (pWwdObject->minY > 0 && pWwdObject->maxY > 0)
    {
        hasVerticalMovement = true;
    }

    if (hasHorizontalMovement)
    {
        if (pWwdObject->speedX > 0)
        {
            speed.x = pWwdObject->speedX;
        }
        else
        {
            speed.x = 125;
        }
    }
    if (hasVerticalMovement)
    {
        if (pWwdObject->speedY > 0)
        {
            speed.y = pWwdObject->speedY;
        }
        else if (pWwdObject->speedX > 0)
        {
            speed.y = pWwdObject->speedX;
        }
        else
        {
            speed.y = 125;
        }
    }

    XML_ADD_2_PARAM_ELEMENT("Speed", "x", ToStr(speed.x).c_str(), "y", ToStr(speed.y).c_str(), aiElevatorComponent);
    XML_ADD_2_PARAM_ELEMENT("MinPosition", "x", ToStr(pWwdObject->minX).c_str(), "y", ToStr(pWwdObject->minY).c_str(), aiElevatorComponent);
    XML_ADD_2_PARAM_ELEMENT("MaxPosition", "x", ToStr(pWwdObject->maxX).c_str(), "y", ToStr(pWwdObject->maxY).c_str(), aiElevatorComponent);

    return aiElevatorComponent;
}

TiXmlElement* SuperCheckpointToXml(WwdObject* pWwdObject)
{

}

TiXmlElement* CheckpointToXml(WwdObject* pWwdObject)
{

}

TiXmlElement* PowderKegToXml(WwdObject* pWwdObject)
{

}

TiXmlElement* OfficerToXml(WwdObject* pWwdObject)
{

}

TiXmlElement* SoldierToXml(WwdObject* pWwdObject)
{

}

TiXmlElement* RatToXml(WwdObject* pWwdObject)
{

}

TiXmlElement* AmmoToXml(WwdObject* pWwdObject)
{

}

TiXmlElement* TreasureToXml(WwdObject* pWwdObject)
{
    if (std::string(pWwdObject->imageSet) == "GAME_TREASURE_COINS")
    {
        TiXmlElement* animElem = new TiXmlElement("AnimationComponent");
        //animElem->LinkEndChild(CreateCycleAnimation(100));
        XML_ADD_1_PARAM_ELEMENT("Animation", "type", "cycle100", animElem);

        return animElem;
    }



    return NULL;
}

TiXmlElement* SpecialPowerupToXml(WwdObject* pWwdObject)
{

}

TiXmlElement* TogglePegToXml(WwdObject* pWwdObject)
{
    TiXmlElement* pTogglePegAIElem = new TiXmlElement("TogglePegAIComponent");
    pTogglePegAIElem->SetAttribute("Type", pWwdObject->logic);
    pTogglePegAIElem->SetAttribute("resource", "created");

    std::string logic = pWwdObject->logic;

    uint32 delay = 0;
    if (logic == "TogglePeg")
    {
        delay = 0;
    }
    else if (logic == "TogglePeg2")
    {
        delay = 750;
    }
    else if (logic == "TogglePeg3")
    {
        delay = 1500;
    }
    else if (logic == "TogglePeg4")
    {
        delay = 2250;
    }
    else
    {
        LOG_WARNING("Unknown TogglePeg: " + logic)
    }

    if (pWwdObject->speed > 0)
    {
        delay = pWwdObject->speed;
    }

    XML_ADD_TEXT_ELEMENT("Delay", ToStr(delay).c_str(), pTogglePegAIElem);

    uint32 timeOn = 0;
    uint32 timeOff = 0;
    if (pWwdObject->speedX > 0)
    {
        timeOn = pWwdObject->speedX;
    }
    else
    {
        timeOn = 1500;
    }
    if (pWwdObject->speedY > 0)
    {
        timeOff = pWwdObject->speedY;
    }
    else
    {
        timeOff = 1500;
    }

    if (pWwdObject->smarts & 0x1)
    {
        XML_ADD_TEXT_ELEMENT("AlwaysOn", "true", pTogglePegAIElem);
    }
    else
    {
        XML_ADD_TEXT_ELEMENT("TimeOn", ToStr(timeOn).c_str(), pTogglePegAIElem);
        XML_ADD_TEXT_ELEMENT("TimeOff", ToStr(timeOff).c_str(), pTogglePegAIElem);
    }

    return pTogglePegAIElem;
}

TiXmlElement* WwdObjectToXml(WwdObject* wwdObject, std::string& imagesRootPath)
{
    TiXmlElement* pActorElem = new TiXmlElement("Actor");
    pActorElem->SetAttribute("Type", wwdObject->logic);

    std::string logic = wwdObject->logic;
    std::string imageSet = wwdObject->imageSet;

    // Common components for all actors

    // Position component
    INSERT_POSITION_COMPONENT(wwdObject->x, wwdObject->y, pActorElem);

    // ActorRenderComponent
    //----- [Level::Actors::ActorProperties::ActorRenderComponent]
    TiXmlElement* actorRenderComponent = new TiXmlElement("ActorRenderComponent");
    pActorElem->LinkEndChild(actorRenderComponent);

    // For debug
    XML_ADD_TEXT_ELEMENT("imgpath", wwdObject->imageSet, actorRenderComponent);

    XML_ADD_TEXT_ELEMENT("Visible", ToStr(true).c_str(), actorRenderComponent);
    XML_ADD_TEXT_ELEMENT("Mirrored", ToStr((wwdObject->drawFlags & WAP_OBJECT_DRAW_FLAG_MIRROR) != 0).c_str(), actorRenderComponent);
    XML_ADD_TEXT_ELEMENT("Inverted", ToStr((wwdObject->drawFlags & WAP_OBJECT_DRAW_FLAG_INVERT) != 0).c_str(), actorRenderComponent);
    XML_ADD_TEXT_ELEMENT("ZCoord", ToStr(wwdObject->z).c_str(), actorRenderComponent);

    std::string tmpImagesRootPath = imagesRootPath;
    std::string tmpImageSet = wwdObject->imageSet;
    bool imageSetValid = false;

    if (tmpImageSet.find("LEVEL_") == 0)
    {
        // Remove "LEVEL_" from tmpImageSet, e.g. "LEVEL_SOLDIER" -> "SOLDIER"
        tmpImageSet.erase(0, strlen("LEVEL_"));
        imageSetValid = true;
    }
    else if (tmpImageSet.find("GAME_") == 0)
    {
        // Remove "GAME_" from tmpImageSet, e.g. "GAME_TREASURE_COINS" -> "TREASURE_COINS"
        tmpImageSet.erase(0, strlen("GAME_"));
        tmpImagesRootPath = std::string("/GAME/IMAGES/");
        std::replace(tmpImageSet.begin(), tmpImageSet.end(), '_', '/');
        imageSetValid = true;

    }
    else
    {
        LOG_WARNING("Unknown actor image path: " + std::string(wwdObject->imageSet));
    }

    if (imageSetValid)
    {
        //std::replace(tmpImageSet.begin(), tmpImageSet.end(), '_', '/');
        tmpImageSet += "/*";
        tmpImageSet = tmpImagesRootPath + tmpImageSet;
    }

    // DoNothing logic only has one single frame which has to be assigned
    // We cant use general wildcard here
    // Example: We want "/LEVEL1/IMAGES/ARCHESFRONT/*1.PID"
    if (logic == "DoNothing")
    {
        // Unfortunately, this hack IS NOT ENOUGH, index "i" == -1 means "i" should be 1
        // I really have no clue how could Claw's level parsing code should be sane by any
        // means. Ok, enough of my crying.
        if (wwdObject->i == -1)
        {
            wwdObject->i = 1;
        }
        tmpImageSet += ToStr(wwdObject->i) + ".PID";
    }

    XML_ADD_TEXT_ELEMENT("ImagePath", tmpImageSet.c_str(), actorRenderComponent);

    std::string aniPath;
    // We probably should have valid animation frames in order to create an animation :d
    if (imageSetValid)
    {
        aniPath = tmpImageSet;
        aniPath.replace(aniPath.find("/IMAGES/"), strlen("/IMAGES/"), "/ANIS/");
    }

    //=========================================================================
    // Specific logics to XML
    //=========================================================================

    if (logic.find("AmbientSound") != std::string::npos)
    {
        //pActorElem->LinkEndChild(AmbientSoundToXml(wwdObject));
    }
    else if (logic.find("SoundTrigger") != std::string::npos)
    {
        //pActorElem->LinkEndChild(SoundTriggerToXml(wwdObject));
    }
    else if (logic.find("StackedCrates") != std::string::npos)
    {
        //pActorElem->LinkEndChild(StackedCratesToXml(wwdObject));
    }
    else if (logic.find("Crate") != std::string::npos)
    {
        std::vector<PickupType> loot;
        if (wwdObject->powerup > 0) { loot.push_back(PickupType(wwdObject->powerup)); }
        if (wwdObject->userRect1.left > 0) { loot.push_back(PickupType(wwdObject->userRect1.left)); }
        if (wwdObject->userRect1.right > 0) { loot.push_back(PickupType(wwdObject->userRect1.right)); }
        if (wwdObject->userRect1.bottom > 0) { loot.push_back(PickupType(wwdObject->userRect1.bottom)); }
        if (wwdObject->userRect1.top > 0) { loot.push_back(PickupType(wwdObject->userRect1.top)); }
        return ActorTemplates::CreateXmlData_CrateActor(tmpImageSet, Point(wwdObject->x, wwdObject->y), loot, 5, wwdObject->z);
    }
    else if (logic.find("Candy") != std::string::npos)
    {
        TiXmlElement* elem = EyeCandyToXml(wwdObject);
        if (elem)
        {
            pActorElem->LinkEndChild(elem);
        }
    }
    else if (logic.find("CrumblingPeg") != std::string::npos)
    {
        pActorElem->LinkEndChild(CrumblingPegToXml(wwdObject));

        TiXmlElement* animElem = new TiXmlElement("AnimationComponent");
        XML_ADD_1_PARAM_ELEMENT("Animation", "type", "cycle50", animElem);
        pActorElem->LinkEndChild(animElem);
    }
    else if (logic.find("Elevator") != std::string::npos)
    {
        pActorElem->LinkEndChild(ElevatorToXml(wwdObject));
    }
    else if (logic.find("TogglePeg") != std::string::npos)
    {
        pActorElem->LinkEndChild(TogglePegToXml(wwdObject));

        TiXmlElement* animElem = new TiXmlElement("AnimationComponent");
        XML_ADD_1_PARAM_ELEMENT("Animation", "type", "cycle75", animElem);
        pActorElem->LinkEndChild(animElem);
    }
    else if (logic.find("SuperCheckpoint") != std::string::npos)
    {
        //pActorElem->LinkEndChild(SuperCheckpointToXml(wwdObject));
    }
    else if (logic.find("Checkpoint") != std::string::npos)
    {
        //pActorElem->LinkEndChild(CheckpointToXml(wwdObject));
    }
    else if (logic.find("PowderKeg") != std::string::npos)
    {
        //pActorElem->LinkEndChild(PowderKegToXml(wwdObject));
    }
    else if (logic.find("Officer") != std::string::npos)
    {
        //pActorElem->LinkEndChild(OfficerToXml(wwdObject));
    }
    else if (logic.find("Soldier") != std::string::npos)
    {
        //pActorElem->LinkEndChild(SoldierToXml(wwdObject));
    }
    else if (logic.find("Rat") != std::string::npos)
    {
        //pActorElem->LinkEndChild(RatToXml(wwdObject));
    }
    else if (logic.find("AmmoPowerup") != std::string::npos)
    {
        //pActorElem->LinkEndChild(AmmoToXml(wwdObject));
    }
    else if (logic.find("SpecialPowerup") != std::string::npos ||
        imageSet.find("CATNIPS") != std::string::npos)
    {
        //pActorElem->LinkEndChild(SpecialPowerupToXml(wwdObject));

#define CREATE_POWERUP_COMPONENT(name, duration) \
    TiXmlElement* pPowerupPickupComponent = new TiXmlElement("PowerupPickupComponent"); \
    XML_ADD_TEXT_ELEMENT("Type", name, pPowerupPickupComponent); \
    XML_ADD_TEXT_ELEMENT("Duration", duration, pPowerupPickupComponent); \
    pActorElem->LinkEndChild(pPowerupPickupComponent); \

        // All powerups should have trigger
        pActorElem->LinkEndChild(CreateTriggerComponent(1, false, true));

        std::string imageSet = wwdObject->imageSet;
        if (imageSet == "GAME_WARP")
        {
            TiXmlElement* pTeleportPickupComponent = new TiXmlElement("TeleportPickupComponent");
            XML_ADD_2_PARAM_ELEMENT("Destination", "x", ToStr(wwdObject->speedX).c_str(), "y", ToStr(wwdObject->speedY).c_str(), pTeleportPickupComponent);
            pActorElem->LinkEndChild(pTeleportPickupComponent);

            TiXmlElement* pAnimCompElem = new TiXmlElement("AnimationComponent");
            pAnimCompElem->LinkEndChild(CreateCycleAnimation(125));
            pActorElem->LinkEndChild(pAnimCompElem);
        }
        else if (imageSet == "GAME_POWERUPS_INVULNERABLE")
        {
            CREATE_POWERUP_COMPONENT("Invulnerability", "30000");
        }
        else if (imageSet == "GAME_POWERUPS_GHOST")
        {
            CREATE_POWERUP_COMPONENT("Invisibility", "30000");
        }
        else if (imageSet == "GAME_CATNIPS_NIP1")
        {
            int duration = 15000;
            if (wwdObject->smarts > 0)
            {
                duration = wwdObject->smarts;
            }

            CREATE_POWERUP_COMPONENT("Catnip", ToStr(duration).c_str());
        }
        else if (imageSet == "GAME_CATNIPS_NIP2")
        {
            int duration = 30000;
            if (wwdObject->smarts > 0)
            {
                duration = wwdObject->smarts;
            }

            CREATE_POWERUP_COMPONENT("Catnip", ToStr(duration).c_str());
        }
        else if (imageSet == "GAME_POWERUPS_FIRESWORD")
        {
            CREATE_POWERUP_COMPONENT("FireSword", "30000");
        }
        else if (imageSet == "GAME_POWERUPS_ICESWORD")
        {
            CREATE_POWERUP_COMPONENT("IceSword", "30000");
        }
        else if (imageSet == "GAME_POWERUPS_LIGHTNINGSWORD")
        {
            CREATE_POWERUP_COMPONENT("LightningSword", "30000");
        }
        else
        {
            LOG_ERROR("Unknown special powerup: " + imageSet);
        }
    }
    else if (logic == "TreasurePowerup" ||
             logic == "GlitterlessPowerup")
    {
        pActorElem->LinkEndChild(CreateTriggerComponent(1, false, true));

        std::string imageSet = wwdObject->imageSet;
        int points = 0;
        if (imageSet == "GAME_TREASURE_COINS") points = 100;
        else if (imageSet == "GAME_TREASURE_GOLDBARS") points = 500;
        else if (imageSet == "GAME_TREASURE_NECKLACE") points = 10000;
        else if (imageSet.find("GAME_TREASURE_RINGS") != std::string::npos) points = 1500;
        else if (imageSet.find("GAME_TREASURE_CHALICES") != std::string::npos) points = 2500;
        else if (imageSet.find("GAME_TREASURE_CROSSES") != std::string::npos) points = 5000;
        else if (imageSet.find("GAME_TREASURE_SCEPTERS") != std::string::npos) points = 7500;
        else if (imageSet.find("GAME_TREASURE_GECKOS") != std::string::npos) points = 10000;
        else if (imageSet.find("GAME_TREASURE_CROWNS") != std::string::npos) points = 15000;
        else if (imageSet.find("GAME_TREASURE_JEWELEDSKULL") != std::string::npos) points = 25000;

        TiXmlElement* pScoreElement = new TiXmlElement("TreasurePickupComponent");
        XML_ADD_TEXT_ELEMENT("ScorePoints", ToStr(points).c_str(), pScoreElement);
        pActorElem->LinkEndChild(pScoreElement);



        TiXmlElement* elem = TreasureToXml(wwdObject);
        if (elem)
        {
            pActorElem->LinkEndChild(elem);
        }
    }
    else if (logic == "HealthPowerup" && imageSet.find("CATNIPS") == std::string::npos)
    {
        pActorElem->LinkEndChild(CreateTriggerComponent(1, false, true));

        TiXmlElement* healthPickupComponent = new TiXmlElement("HealthPickupComponent");
        pActorElem->LinkEndChild(healthPickupComponent);

        std::string imageSet = wwdObject->imageSet;
        if (imageSet == "LEVEL_HEALTH")
        {
            XML_ADD_TEXT_ELEMENT("Health", "5", healthPickupComponent);
        }
        else if (imageSet == "GAME_HEALTH_POTION1")
        {
            XML_ADD_TEXT_ELEMENT("Health", "10", healthPickupComponent);
        }
        else if (imageSet == "GAME_HEALTH_POTION2")
        {
            XML_ADD_TEXT_ELEMENT("Health", "20", healthPickupComponent);
        }
        else if (imageSet == "GAME_HEALTH_POTION3")
        {
            XML_ADD_TEXT_ELEMENT("Health", "35", healthPickupComponent);
        }
        else
        {
            LOG_WARNING("Trying to parse unknown HealthPickupComponent" + imageSet);
        }
    }
    else
    {
        //LOG_WARNING("Unknown logic: " + logic);
    }

    return pActorElem;
}

//=====================================================================================================================
// Claw to Xml
//=====================================================================================================================

TiXmlElement* CreateClawActor(WapWwd* pWapWwd)
{
    TiXmlElement* pClawActor = new TiXmlElement("Actor");
    pClawActor->SetAttribute("Type", "Claw");
    pClawActor->SetAttribute("resource", "created");

    //pClawActor->LinkEndChild(CreatePositionComponent(pWapWwd->properties.startX, pWapWwd->properties.startY));
    pClawActor->LinkEndChild(CreatePositionComponent(4850, 4100));
    pClawActor->LinkEndChild(CreateCollisionComponent(40, 110));
    pClawActor->LinkEndChild(CreatePhysicsComponent(true, false, true, 1500, 40, 110, 4.0, 0.0, 0.5));
    pClawActor->LinkEndChild(CreateControllableComponent(true));
    pClawActor->LinkEndChild(CreateAnimationComponent("/CLAW/ANIS/*"));
    pClawActor->LinkEndChild(CreateSoundComponent("/CLAW/SOUNDS/*"));
    pClawActor->LinkEndChild(CreateActorRenderComponent("/CLAW/IMAGES/*", 4000));

    TiXmlElement* pScoreComponent = new TiXmlElement("ScoreComponent");
    XML_ADD_TEXT_ELEMENT("Score", "0", pScoreComponent);
    pClawActor->LinkEndChild(pScoreComponent);

    TiXmlElement* pLifeComponent = new TiXmlElement("LifeComponent");
    XML_ADD_TEXT_ELEMENT("Lives", "1", pLifeComponent);
    pClawActor->LinkEndChild(pLifeComponent);

    TiXmlElement* pHealthComponent = new TiXmlElement("HealthComponent");
    XML_ADD_TEXT_ELEMENT("Health", "80", pHealthComponent);
    XML_ADD_TEXT_ELEMENT("MaxHealth", "150", pHealthComponent);
    pClawActor->LinkEndChild(pHealthComponent);

    TiXmlElement* pAmmoComponent = new TiXmlElement("AmmoComponent");
    XML_ADD_TEXT_ELEMENT("Pistol", "15", pAmmoComponent);
    XML_ADD_TEXT_ELEMENT("Magic", "10", pAmmoComponent);
    XML_ADD_TEXT_ELEMENT("Dynamite", "5", pAmmoComponent);
    pClawActor->LinkEndChild(pAmmoComponent);

    TiXmlElement* pPowerupComponent = new TiXmlElement("PowerupComponent");
    pClawActor->LinkEndChild(pPowerupComponent);

    return pClawActor;
}

//=====================================================================================================================
// HUD to Xml
//=====================================================================================================================

TiXmlElement* CreateHUDElement(std::string pathToImages, int animFrameDuration, std::string animPath, Point position, bool anchorRight, bool anchorBottom, std::string key, bool visible = true)
{
    TiXmlElement* pHUDElement = new TiXmlElement("Actor");
    pHUDElement->SetAttribute("Type", pathToImages.c_str());
    pHUDElement->SetAttribute("resource", "created");

    pHUDElement->LinkEndChild(CreatePositionComponent(position.x, position.y));

    if (!animPath.empty())
    {
        pHUDElement->LinkEndChild(CreateAnimationComponent(animPath));
    }
    else
    {
        TiXmlElement* pAnimCompElem = new TiXmlElement("AnimationComponent");
        pAnimCompElem->LinkEndChild(CreateCycleAnimation(animFrameDuration));
        pHUDElement->LinkEndChild(pAnimCompElem);
    }

    TiXmlElement* pHUDRenderComponentElem = new TiXmlElement("HUDRenderComponent");
    XML_ADD_TEXT_ELEMENT("ImagePath", pathToImages.c_str(), pHUDRenderComponentElem);
    if (anchorRight)
    {
        XML_ADD_TEXT_ELEMENT("AnchorRight", "true", pHUDRenderComponentElem);
    }
    if (anchorBottom)
    {
        XML_ADD_TEXT_ELEMENT("AnchorBottom", "true", pHUDRenderComponentElem);
    }
    if (visible)
    {
        XML_ADD_TEXT_ELEMENT("Visible", "true", pHUDRenderComponentElem);
    }
    else
    {
        XML_ADD_TEXT_ELEMENT("Visible", "false", pHUDRenderComponentElem);
    }
    if (!key.empty())
    {
        XML_ADD_TEXT_ELEMENT("HUDElementKey", key.c_str(), pHUDRenderComponentElem);
    }
    pHUDElement->LinkEndChild(pHUDRenderComponentElem);

    return pHUDElement;
}

void WwdToXml(WapWwd* wapWwd)
{
    PROFILE_CPU("WWD->XML");
    TiXmlDocument xmlDoc;

    //----- [Level]
    TiXmlElement* root = new TiXmlElement("Level");
    xmlDoc.LinkEndChild(root);

    //---- [Level::LevelProperties]
    TiXmlElement* levelProperties = new TiXmlElement("LevelProperties");
    root->LinkEndChild(levelProperties);

    XML_ADD_TEXT_ELEMENT("LevelName", wapWwd->properties.levelName, levelProperties);
    XML_ADD_TEXT_ELEMENT("Author", wapWwd->properties.author, levelProperties);
    XML_ADD_TEXT_ELEMENT("Created", wapWwd->properties.birth, levelProperties);
    XML_ADD_TEXT_ELEMENT("RezFile", wapWwd->properties.rezFile, levelProperties);
    XML_ADD_TEXT_ELEMENT("TileDirectory", wapWwd->properties.imageDirectoryPath, levelProperties);
    XML_ADD_TEXT_ELEMENT("Palette", wapWwd->properties.rezPalettePath, levelProperties);
    XML_ADD_TEXT_ELEMENT("LaunchApp", wapWwd->properties.launchApp, levelProperties);
    XML_ADD_TEXT_ELEMENT("ImageSet1", wapWwd->properties.imageSet1, levelProperties);
    XML_ADD_TEXT_ELEMENT("ImageSet2", wapWwd->properties.imageSet2, levelProperties);
    XML_ADD_TEXT_ELEMENT("ImageSet3", wapWwd->properties.imageSet3, levelProperties);
    XML_ADD_TEXT_ELEMENT("ImageSet4", wapWwd->properties.imageSet4, levelProperties);
    XML_ADD_TEXT_ELEMENT("Prefix1", wapWwd->properties.prefix1, levelProperties);
    XML_ADD_TEXT_ELEMENT("Prefix2", wapWwd->properties.prefix1, levelProperties);
    XML_ADD_TEXT_ELEMENT("Prefix3", wapWwd->properties.prefix1, levelProperties);
    XML_ADD_TEXT_ELEMENT("Prefix4", wapWwd->properties.prefix1, levelProperties);

    XML_ADD_TEXT_ELEMENT("UseZCoords", ToStr((wapWwd->properties.flags & 2) != 0).c_str(), levelProperties);

    TiXmlElement* spawnPos = new TiXmlElement("Spawn");
    spawnPos->SetAttribute("x", wapWwd->properties.startX);
    spawnPos->SetAttribute("y", wapWwd->properties.startY);
    levelProperties->LinkEndChild(spawnPos);

    XML_ADD_TEXT_ELEMENT("PlanesCount", ToStr(wapWwd->properties.numPlanes).c_str(), levelProperties);

    TiXmlElement* tileDescRootElem = new TiXmlElement("TileDescriptions");
    levelProperties->LinkEndChild(tileDescRootElem);
    int numTileDescriptions = wapWwd->tileDescriptionsCount;
    for (int tileDescIdx = 0; tileDescIdx < numTileDescriptions; tileDescIdx++)
    {
        WwdTileDescription wwdTileDesc = wapWwd->tileDescriptions[tileDescIdx];

        TiXmlElement* tileDescElem = new TiXmlElement("TileDescription");
        tileDescRootElem->LinkEndChild(tileDescElem);

        XML_ADD_TEXT_ELEMENT("TileId", ToStr(tileDescIdx).c_str(), tileDescElem);
        XML_ADD_2_PARAM_ELEMENT("Size", "width", wwdTileDesc.width, "height", wwdTileDesc.height, tileDescElem);
        
        if (wwdTileDesc.type == WAP_TILE_TYPE_SINGLE)
        {
            XML_ADD_TEXT_ELEMENT("Type", "single", tileDescElem);
            XML_ADD_TEXT_ELEMENT("InsideAttrib", ToStr(wwdTileDesc.insideAttrib).c_str(), tileDescElem);
            TiXmlElement* rectElem = new TiXmlElement("TileRect");
            rectElem->SetAttribute("left", 0);
            rectElem->SetAttribute("top", 0);
            rectElem->SetAttribute("right", wwdTileDesc.width);
            rectElem->SetAttribute("bottom", wwdTileDesc.height);
            tileDescElem->LinkEndChild(rectElem);
        }
        else
        {
            XML_ADD_TEXT_ELEMENT("Type", "double", tileDescElem);
            XML_ADD_TEXT_ELEMENT("InsideAttrib", ToStr(wwdTileDesc.insideAttrib).c_str(), tileDescElem);
            XML_ADD_TEXT_ELEMENT("OutsideAttrib", ToStr(wwdTileDesc.outsideAttrib).c_str(), tileDescElem);
            TiXmlElement* rectElem = new TiXmlElement("TileRect");
            rectElem->SetAttribute("left", wwdTileDesc.rect.left);
            rectElem->SetAttribute("top", wwdTileDesc.rect.top);
            rectElem->SetAttribute("right", wwdTileDesc.rect.right);
            rectElem->SetAttribute("bottom", wwdTileDesc.rect.bottom);
            tileDescElem->LinkEndChild(rectElem);
        }
    }

    // Defalt
    int mainPlaneIdx = -1;

    std::string tileRootDirPath = wapWwd->properties.imageDirectoryPath;
    std::replace(tileRootDirPath.begin(), tileRootDirPath.end(), '\\', '/');

    //---- [Level::Actor type=Plane]
    for (uint16 planeIdx = 0; planeIdx < wapWwd->properties.numPlanes; ++planeIdx)
    {
        WwdPlane wwdPlane = wapWwd->planes[planeIdx];

        TiXmlElement* plane = new TiXmlElement("Actor");
        plane->SetAttribute("Type", "Plane");
        root->LinkEndChild(plane);

        //---- [Level::Actor::PositionComponent::Images]
        INSERT_POSITION_COMPONENT(0, 0, plane);

        TiXmlElement* planeRenderComponentElem = new TiXmlElement("TilePlaneRenderComponent");
        plane->LinkEndChild(planeRenderComponentElem);

        //---- [Level::Actor::TilePlaneRenderComponent::Images]
        std::string tileDirName;
        if (std::string(wwdPlane.properties.name) == "Background") { tileDirName = "BACK"; }
        else if (std::string(wwdPlane.properties.name) == "Action") { tileDirName = "ACTION"; }
        else if (std::string(wwdPlane.properties.name) == "Front") { tileDirName = "FRONT"; }
        else { LOG_ERROR("Unknown tile plane name: " + std::string(wwdPlane.properties.name)); }
        std::string planeImageDirPath = tileRootDirPath + "/" + tileDirName + "/*";
        XML_ADD_TEXT_ELEMENT("ImagePath", planeImageDirPath.c_str(), planeRenderComponentElem);

        //[Level::Actor::TilePlaneRenderComponent::PlaneProperties]
        TiXmlElement* planeProperties = new TiXmlElement("PlaneProperties");
        planeRenderComponentElem->LinkEndChild(planeProperties);

        XML_ADD_TEXT_ELEMENT("PlaneName", wwdPlane.properties.name, planeProperties);

        XML_ADD_TEXT_ELEMENT("MainPlane", ToStr((wwdPlane.properties.flags & WAP_PLANE_FLAG_MAIN_PLANE) != 0).c_str(), planeProperties);
        XML_ADD_TEXT_ELEMENT("NoDraw", ToStr((wwdPlane.properties.flags & WAP_PLANE_FLAG_NO_DRAW) != 0).c_str(), planeProperties);
        XML_ADD_TEXT_ELEMENT("WrappedX", ToStr((wwdPlane.properties.flags & WAP_PLANE_FLAG_X_WRAPPING) != 0).c_str(), planeProperties);
        XML_ADD_TEXT_ELEMENT("WrappedY", ToStr((wwdPlane.properties.flags & WAP_PLANE_FLAG_Y_WRAPPING) != 0).c_str(), planeProperties);
        XML_ADD_TEXT_ELEMENT("TileAutoSized", ToStr((wwdPlane.properties.flags & WAP_PLANE_FLAG_AUTO_TILE_SIZE) != 0).c_str(), planeProperties);

        XML_ADD_TEXT_ELEMENT("TotalTileCount", ToStr(wwdPlane.tilesCount).c_str(), planeProperties);

        TiXmlElement* tileSize = new TiXmlElement("TilePixelSize");
        tileSize->SetAttribute("width", wwdPlane.properties.tilePixelWidth);
        tileSize->SetAttribute("height", wwdPlane.properties.tilePixelHeight);
        planeProperties->LinkEndChild(tileSize);

        TiXmlElement* planeSize = new TiXmlElement("PlanePixelSize");
        planeSize->SetAttribute("width", wwdPlane.properties.pixelWidth);
        planeSize->SetAttribute("height", wwdPlane.properties.pixelHeight);
        planeProperties->LinkEndChild(planeSize);


        TiXmlElement* moveSpeed = new TiXmlElement("MoveSpeedPercentage");
        moveSpeed->SetAttribute("x", wwdPlane.properties.movementPercentX);
        moveSpeed->SetAttribute("y", wwdPlane.properties.movementPercentY);
        planeProperties->LinkEndChild(moveSpeed);

        XML_ADD_TEXT_ELEMENT("FillColor", ToStr(wwdPlane.properties.fillColor).c_str(), planeProperties);
        XML_ADD_TEXT_ELEMENT("ImageSetsCount", ToStr(wwdPlane.properties.imageSetsCount).c_str(), planeProperties);
        XML_ADD_TEXT_ELEMENT("ZCoord", ToStr(wwdPlane.properties.coordZ).c_str(), planeProperties);

        //[Level::Actor::TilePlaneRenderComponent::Tiles]
        TiXmlElement* tiles = new TiXmlElement("Tiles");
        planeRenderComponentElem->LinkEndChild(tiles);

        uint32 tilesCount = wwdPlane.tilesCount;
        int32* pTiles = wwdPlane.tiles;
        for (uint32 tileIdx = 0; tileIdx < tilesCount; ++tileIdx)
        {
            XML_ADD_TEXT_ELEMENT("Tile", ToStr(pTiles[tileIdx]).c_str(), tiles);
        }

        if (wwdPlane.properties.flags & WAP_PLANE_FLAG_MAIN_PLANE)
        {
            mainPlaneIdx = planeIdx;
        }
    }

    // There has to be a main plane in the game
    assert(mainPlaneIdx != -1);

    //---- [Level::Actors]
    TiXmlElement* actorsElem = new TiXmlElement("Actors");
    root->LinkEndChild(actorsElem);

    WwdObject* wwdActors = wapWwd->planes[mainPlaneIdx].objects;
    uint32 actorsCount = wapWwd->planes[mainPlaneIdx].objectsCount;

    std::string imagesRootPath = wapWwd->properties.imageSet1;
    std::replace(imagesRootPath.begin(), imagesRootPath.end(), '\\', '/');
    imagesRootPath += '/';
    imagesRootPath.insert(0, 1, '/');

    for (uint32 actorIdx = 0; actorIdx < actorsCount; actorIdx++)
    {
        WwdObject actorProperties = wwdActors[actorIdx];

        std::string name = actorProperties.name;
        std::string logic = actorProperties.logic;
        std::string imageSet = actorProperties.imageSet;
        std::string sound = actorProperties.sound;

        root->LinkEndChild(WwdObjectToXml(&actorProperties, imagesRootPath));
        continue;

        //=============================================================================================================
        // Deprecated, not used
        //=============================================================================================================

        bool visible = true;
        bool hasCollisionComponent = false;


        //----- [Level::Actors::ActorProperties]
        TiXmlElement* actorPropertiesElem = new TiXmlElement("ActorProperties");
        actorsElem->LinkEndChild(actorPropertiesElem);

        XML_ADD_TEXT_ELEMENT("Name", actorProperties.name, actorPropertiesElem);
        XML_ADD_TEXT_ELEMENT("Logic", actorProperties.logic, actorPropertiesElem);
        XML_ADD_TEXT_ELEMENT("ImageSet", actorProperties.imageSet, actorPropertiesElem);
        XML_ADD_TEXT_ELEMENT("Sound", actorProperties.sound, actorPropertiesElem);

        //----- [Level::Actors::ActorProperties::PositionComponent]
        TiXmlElement* positionComponent = new TiXmlElement("PositionComponent");
        actorPropertiesElem->LinkEndChild(positionComponent);

        XML_ADD_2_PARAM_ELEMENT("Position", "x", actorProperties.x, "y", actorProperties.y, positionComponent);

        //----- [Level::Actors::ActorProperties::CollisionComponent]
        TiXmlElement* collisionComponent = new TiXmlElement("CollisionComponent");
        actorPropertiesElem->LinkEndChild(collisionComponent);

        //----- [Level::Actors::ActorProperties::PhysicsComponent]
        TiXmlElement* physicsComponent = new TiXmlElement("PhysicsComponent");
        actorPropertiesElem->LinkEndChild(physicsComponent);



        if (imageSet.find("GAME_HEALTH") != std::string::npos)
        {
            //----- [Level::Actors::ActorProperties::HealthPickupComponent]
            TiXmlElement* healthPickupComponent = new TiXmlElement("HealthPickupComponent");
            actorPropertiesElem->LinkEndChild(healthPickupComponent);

            if (imageSet == "LEVEL_HEALTH")
            {
                XML_ADD_TEXT_ELEMENT("Health", "5", healthPickupComponent);
            }
            else if (imageSet == "GAME_HEALTH_POTION1")
            {
                XML_ADD_TEXT_ELEMENT("Health", "10", healthPickupComponent);
            }
            else if (imageSet == "GAME_HEALTH_POTION2")
            {
                XML_ADD_TEXT_ELEMENT("Health", "20", healthPickupComponent);
            }
            else if (imageSet == "GAME_HEALTH_POTION3")
            {
                XML_ADD_TEXT_ELEMENT("Health", "35", healthPickupComponent);
            }
            else
            {
                LOG_WARNING("Trying to parse unknown HealthPickupComponent" + imageSet);
            }
        }

        if (imageSet.find("GAME_AMMO") != std::string::npos ||
            imageSet.find("GAME_DYNAMITE") != std::string::npos ||
            imageSet.find("GAME_MAGIC") != std::string::npos)
        {
            //----- [Level::Actors::ActorProperties::AmmoPickupComponent]
            TiXmlElement* ammoPickupComponent = new TiXmlElement("AmmoPickupComponent");
            actorPropertiesElem->LinkEndChild(ammoPickupComponent);

            if (imageSet == "GAME_AMMO_SHOT")
            {
                XML_ADD_TEXT_ELEMENT("Gunpowder", "5", ammoPickupComponent);
            }
            else if (imageSet == "GAME_AMMO_SHOTBAG")
            {
                XML_ADD_TEXT_ELEMENT("Gunpowder", "10", ammoPickupComponent);
            }
            else if (imageSet == "GAME_AMMO_DEATHBAG")
            {
                XML_ADD_TEXT_ELEMENT("Gunpowder", "25", ammoPickupComponent);
            }
            else if (imageSet == "GAME_DYNAMITE")
            {
                XML_ADD_TEXT_ELEMENT("Dynamite", "5", ammoPickupComponent);
            }
            else if (imageSet == "GAME_MAGIC_GLOW")
            {
                XML_ADD_TEXT_ELEMENT("Magic", "5", ammoPickupComponent);
            }
            else if (imageSet == "GAME_MAGIC_STARGLOW")
            {
                XML_ADD_TEXT_ELEMENT("Magic", "10", ammoPickupComponent);
            }
            else if (imageSet == "GAME_MAGICCLAW")
            {
                XML_ADD_TEXT_ELEMENT("Magic", "25", ammoPickupComponent);
            }
            else
            {
                LOG_WARNING("Trying to parse unknown AmmoPickupComponent: " + imageSet);
            }
        }

        if (logic == "TreasurePowerup" || logic == "GlitterlessPowerup")
        {
            //----- [Level::Actors::ActorProperties::TreasurePickupComponent]
            TiXmlElement* treasurePickupComponent = new TiXmlElement("TreasurePickupComponent");
            actorPropertiesElem->LinkEndChild(treasurePickupComponent);

            assert(imageSet.find("TREASURE") != std::string::npos);

            int32 points = 0;

            if (imageSet == "GAME_TREASURE_COINS") points = 100;
            else if (imageSet == "GAME_TREASURE_GOLDBARS") points = 500;
            else if (imageSet == "GAME_TREASURE_NECKLACE") points = 10000;
            else if (imageSet.find("GAME_TREASURE_RINGS") != std::string::npos) points = 1500;
            else if (imageSet.find("GAME_TREASURE_CHALICES") != std::string::npos) points = 2500;
            else if (imageSet.find("GAME_TREASURE_CROSSES") != std::string::npos) points = 5000;
            else if (imageSet.find("GAME_TREASURE_SCEPTERS") != std::string::npos) points = 7500;
            else if (imageSet.find("GAME_TREASURE_GECKOS") != std::string::npos) points = 10000;
            else if (imageSet.find("GAME_TREASURE_CROWNS") != std::string::npos) points = 15000;
            else if (imageSet.find("GAME_TREASURE_JEWELEDSKULL") != std::string::npos) points = 25000;

            XML_ADD_TEXT_ELEMENT("Points", ToStr(points).c_str(), treasurePickupComponent);
        }

        if (actorProperties.powerup != 0)
        {
            //----- [Level::Actors::ActorProperties::InventoryComponent]
            TiXmlElement* inventorypComponent = new TiXmlElement("InventoryComponent");
            actorPropertiesElem->LinkEndChild(inventorypComponent);

            // If anyone can come up with better solution, go ahead lol
            if (actorProperties.powerup != 0) XML_ADD_TEXT_ELEMENT("Item", ToStr(actorProperties.powerup).c_str(), inventorypComponent);
            if (actorProperties.userRect1.top != 0) XML_ADD_TEXT_ELEMENT("Item", ToStr(actorProperties.userRect1.top).c_str(), inventorypComponent);
            if (actorProperties.userRect1.left != 0) XML_ADD_TEXT_ELEMENT("Item", ToStr(actorProperties.userRect1.left).c_str(), inventorypComponent);
            if (actorProperties.userRect1.bottom != 0) XML_ADD_TEXT_ELEMENT("Item", ToStr(actorProperties.userRect1.bottom).c_str(), inventorypComponent);
            if (actorProperties.userRect1.right != 0) XML_ADD_TEXT_ELEMENT("Item", ToStr(actorProperties.userRect1.right).c_str(), inventorypComponent);
            if (actorProperties.userRect2.top != 0) XML_ADD_TEXT_ELEMENT("Item", ToStr(actorProperties.userRect2.top).c_str(), inventorypComponent);
            if (actorProperties.userRect2.left != 0) XML_ADD_TEXT_ELEMENT("Item", ToStr(actorProperties.userRect2.left).c_str(), inventorypComponent);
            if (actorProperties.userRect2.bottom != 0) XML_ADD_TEXT_ELEMENT("Item", ToStr(actorProperties.userRect2.bottom).c_str(), inventorypComponent);
            if (actorProperties.userRect2.right != 0) XML_ADD_TEXT_ELEMENT("Item", ToStr(actorProperties.userRect2.right).c_str(), inventorypComponent);
        }

        if (logic.find("Elevator") != std::string::npos)
        {
            //----- [Level::Actors::ActorProperties::AI_ElevatorComponent]
            TiXmlElement* aiElevatorComponent = new TiXmlElement("AI_ElevatorComponent");
            actorPropertiesElem->LinkEndChild(aiElevatorComponent);

            if (logic == "TriggerElevator")
            {
                XML_ADD_TEXT_ELEMENT("TriggeredBehaviour", "true", aiElevatorComponent);
            }
            else if (logic == "StartElevator")
            {
                XML_ADD_TEXT_ELEMENT("StartBehaviour", "true", aiElevatorComponent);
            }
            else if (logic == "StopElevator")
            {
                XML_ADD_TEXT_ELEMENT("StopBehaviour", "true", aiElevatorComponent);
            }
            else if (logic == "Elevator")
            {
                XML_ADD_TEXT_ELEMENT("StandardBehaviour", "true", aiElevatorComponent);
            }
            else
            {
                LOG_WARNING("Unknown Elevator: " + logic)

            }

            XML_ADD_2_PARAM_ELEMENT("MinPosition", "x", ToStr(actorProperties.minX).c_str(), "y", ToStr(actorProperties.minY).c_str(), aiElevatorComponent);
            XML_ADD_2_PARAM_ELEMENT("MaxPosition", "x", ToStr(actorProperties.maxX).c_str(), "y", ToStr(actorProperties.maxY).c_str(), aiElevatorComponent);
        }

        if (logic.find("TogglePeg") != std::string::npos)
        {
            //----- [Level::Actors::ActorProperties::AI_TogglePegComponent]
            TiXmlElement* aiTogglePegComponent = new TiXmlElement("AI_TogglePegComponent");
            actorPropertiesElem->LinkEndChild(aiTogglePegComponent);

            if (logic == "TogglePeg")
            {
                XML_ADD_TEXT_ELEMENT("Delay", "0", aiTogglePegComponent);
            }
            else if (logic == "TogglePeg2")
            {
                XML_ADD_TEXT_ELEMENT("Delay", "750", aiTogglePegComponent);
            }
            else if (logic == "TogglePeg3")
            {
                XML_ADD_TEXT_ELEMENT("Delay", "1500", aiTogglePegComponent);
            }
            else if (logic == "TogglePeg4")
            {
                XML_ADD_TEXT_ELEMENT("Delay", "2250", aiTogglePegComponent);
            }
            else
            {
                LOG_WARNING("Unknown TogglePeg: " + logic)
            }

            XML_ADD_TEXT_ELEMENT("TimeOn", "1500", aiTogglePegComponent);
            XML_ADD_TEXT_ELEMENT("TimeOff", "1500", aiTogglePegComponent);
        }

        if (logic.find("CrumblingPeg") != std::string::npos)
        {
            //----- [Level::Actors::ActorProperties::AI_CrumblingPegComponent]
            TiXmlElement* aiCrumblingPegComponent = new TiXmlElement("AI_CrumblingPegComponent");
            actorPropertiesElem->LinkEndChild(aiCrumblingPegComponent);

            XML_ADD_TEXT_ELEMENT("FlootOffset", "10", aiCrumblingPegComponent);
            XML_ADD_TEXT_ELEMENT("CrumbleFrameIdx", "10", aiCrumblingPegComponent);
        }

        if (logic.find("SoundTrigger") != std::string::npos)
        {
            //----- [Level::Actors::ActorProperties::AI_SoundTriggerComponent]
            TiXmlElement* aiSoundTriggerComponent = new TiXmlElement("AI_SoundTriggerComponent");
            actorPropertiesElem->LinkEndChild(aiSoundTriggerComponent);

            visible = false;

            int32 width = 0, height = 0;
            if (logic.find("Tiny") != std::string::npos)
            {
                width = height = 32;
            }
            else if (logic.find("Small") != std::string::npos)
            {
                width = height = 64;
            }
            else if (logic.find("Big") != std::string::npos)
            {
                width = height = 256;
            }
            else if (logic.find("Huge") != std::string::npos)
            {
                width = height = 512;
            }
            else if (logic.find("Wide") != std::string::npos)
            {
                width = 200;
                height = 64;
            }
            else if (logic.find("Tall") != std::string::npos)
            {
                width = 64;
                height = 200;
            }
            else // Standard 
            {
                width = height = 128;
            }

            XML_ADD_2_PARAM_ELEMENT("Size", "width", ToStr(width).c_str(), "height", ToStr(height).c_str(), aiSoundTriggerComponent);


            if (logic.find("ClawDialog") != std::string::npos)
            {
                XML_ADD_TEXT_ELEMENT("ShowClawDialog", "true", aiSoundTriggerComponent);
            }
            else
            {
                XML_ADD_TEXT_ELEMENT("ShowClawDialog", "false", aiSoundTriggerComponent);
            }

            int32 repeatTimes = actorProperties.smarts;
            /*if (repeatTimes == -1)
            {
            repeatTimes = INT32_MAX;
            }*/
            XML_ADD_TEXT_ELEMENT("RepeatTimes", ToStr(repeatTimes).c_str(), aiSoundTriggerComponent);

            XML_ADD_TEXT_ELEMENT("Sound", sound.c_str(), aiSoundTriggerComponent);
        }


        //----- [Level::Actors::ActorProperties::ActorRenderComponent]
        TiXmlElement* actorRenderComponent = new TiXmlElement("ActorRenderComponent");
        actorPropertiesElem->LinkEndChild(actorRenderComponent);

        XML_ADD_TEXT_ELEMENT("Visible", ToStr(visible).c_str(), actorRenderComponent);
        XML_ADD_TEXT_ELEMENT("Mirrored", ToStr((actorProperties.drawFlags & WAP_OBJECT_DRAW_FLAG_MIRROR) != 0).c_str(), actorRenderComponent);
        XML_ADD_TEXT_ELEMENT("Inverted", ToStr((actorProperties.drawFlags & WAP_OBJECT_DRAW_FLAG_INVERT) != 0).c_str(), actorRenderComponent);

        std::string tmpImagesRootPath = imagesRootPath;
        std::string tmpImageSet = imageSet;
        bool imageSetValid = false;
        if (imageSet.find("LEVEL_") == 0)
        {
            tmpImageSet.erase(0, strlen("LEVEL_"));
            imageSetValid = true;
        }
        else if (imageSet.find("GAME_") == 0)
        {
            tmpImageSet.erase(0, strlen("GAME_"));
            tmpImagesRootPath = std::string("/GAME/IMAGES/");
            imageSetValid = true;

        }
        else
        {
            LOG_WARNING("Unknown actor image path: " + imageSet);
        }

        if (imageSetValid)
        {
            std::replace(tmpImageSet.begin(), tmpImageSet.end(), '_', '/');
            tmpImageSet += "/*";
            tmpImageSet = tmpImagesRootPath + tmpImageSet;
            XML_ADD_TEXT_ELEMENT("ImagePath", tmpImageSet.c_str(), actorRenderComponent);
        }

        // We probably should have valid animation frames in order to create an animation :d
        if (imageSetValid)
        {
            //----- [Level::Actors::ActorProperties::AnimationComponent]
            TiXmlElement* animationComponent = new TiXmlElement("AnimationComponent");
            actorPropertiesElem->LinkEndChild(animationComponent);
            std::string aniPath = tmpImageSet;
            aniPath.replace(aniPath.find("/IMAGES/"), strlen("/IMAGES/"), "/ANIS/");
            XML_ADD_TEXT_ELEMENT("AnimationPath", aniPath.c_str(), animationComponent);
        }
    }

    root->LinkEndChild(CreateClawActor(wapWwd));

    // Create HUD
    root->LinkEndChild(CreateHUDElement("/GAME/IMAGES/INTERFACE/TREASURECHEST/*", 150, "/GAME/ANIS/INTERFACE/CHEST.ANI", Point(20, 20), false, false, "score"));
    root->LinkEndChild(CreateHUDElement("/GAME/IMAGES/INTERFACE/STOPWATCH/*", 125, "", Point(20, 60), false, false, "stopwatch", false));
    root->LinkEndChild(CreateHUDElement("/GAME/IMAGES/INTERFACE/HEALTHHEART/*", 125, "", Point(-33, 15), true, false, "health"));
    root->LinkEndChild(CreateHUDElement("/GAME/IMAGES/INTERFACE/WEAPONS/PISTOL/*", 0, "/GAME/ANIS/INTERFACE/PISTOL.ANI", Point(-26, 45), true, false, "pistol", true));
    root->LinkEndChild(CreateHUDElement("/GAME/IMAGES/INTERFACE/WEAPONS/MAGIC/*", 0, "/GAME/ANIS/INTERFACE/MAGIC.ANI", Point(-26, 45), true, false, "magic", false));
    root->LinkEndChild(CreateHUDElement("/GAME/IMAGES/INTERFACE/WEAPONS/DYNAMITE/*", 0, "/GAME/ANIS/INTERFACE/DYNAMITE.ANI", Point(-26, 45), true, false, "dynamite", false));
    root->LinkEndChild(CreateHUDElement("/GAME/IMAGES/INTERFACE/LIVESHEAD/*", 0, "/GAME/ANIS/INTERFACE/LIVES.ANI", Point(-18, 75), true, false, "lives"));

    //xmlDoc.Print();
    xmlDoc.SaveFile("LEVEL1.xml");
}

#endif