#ifndef __CONVERTERS_H__
#define __CONVERTERS_H__

#include <algorithm>
#include <libwap.h>
#include "../SharedDefines.h"
#include "../Actor/ActorTemplates.h"
#include "../GameApp/BaseGameApp.h"

//
//
// TODO TODO TODO: Somehow refactor this, delete unneeded ones and usse everything from Actor/ActorTemplates.cpp
//
//

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

inline TiXmlElement* CreatePlayAreaElement(int x, int y, int w, int h)
{
    TiXmlElement* pPlayAreaElement = new TiXmlElement("PlayArea");
    pPlayAreaElement->SetAttribute("x", x);
    pPlayAreaElement->SetAttribute("y", y);
    pPlayAreaElement->SetAttribute("width", w);
    pPlayAreaElement->SetAttribute("height", h);
    return pPlayAreaElement;
}


//=====================================================================================================================

inline TiXmlElement* CreateAnimationComponent(std::string aniPath)
{
    TiXmlElement* animationComponent = new TiXmlElement("AnimationComponent");
    XML_ADD_TEXT_ELEMENT("AnimationPath", aniPath.c_str(), animationComponent);
    return animationComponent;
}

inline TiXmlElement* CreateCycleAnimation(int animFrameTime)
{
    TiXmlElement* cycleElem = new TiXmlElement("Animation");
    std::string cycleStr = "cycle" + ToStr(animFrameTime);
    cycleElem->SetAttribute("type", cycleStr.c_str());
    //XML_ADD_1_PARAM_ELEMENT("Animation", "type", cycleStr.c_str(), cycleElem);

    return cycleElem;
}

inline TiXmlElement* CreateTriggerComponent(int enterCount, bool onceALife, bool isStatic)
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
inline TiXmlElement* AmbientSoundToXml(WwdObject* pWwdObject)
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

inline TiXmlElement* SoundTriggerToXml(WwdObject* pWwdObject)
{
    TiXmlElement* pSoundTriggerElem = new TiXmlElement("SoundTrigger");


    return pSoundTriggerElem;
}

inline TiXmlElement* StackedCratesToXml(WwdObject* pWwdObject)
{
    return NULL;
}

inline TiXmlElement* CrateToXml(WwdObject* pWwdObject)
{
    return NULL;
}

inline TiXmlElement* EyeCandyToXml(WwdObject* pWwdObject)
{
    if (std::string(pWwdObject->logic).find("AniCandy") != std::string::npos)
    {
        TiXmlElement* animElem = new TiXmlElement("AnimationComponent");
        XML_ADD_1_PARAM_ELEMENT("Animation", "type", "cycle150", animElem);

        return animElem;
    }

    return NULL;
}

inline TiXmlElement* CrumblingPegToXml(WwdObject* pWwdObject)
{
    TiXmlElement* pCrumblingPegAIComponent = new TiXmlElement("CrumblingPegAIComponent");

    XML_ADD_TEXT_ELEMENT("FloorOffset", "10", pCrumblingPegAIComponent);
    XML_ADD_TEXT_ELEMENT("CrumbleFrameIdx", "10", pCrumblingPegAIComponent);

    return pCrumblingPegAIComponent;
}

inline TiXmlElement* SuperCheckpointToXml(WwdObject* pWwdObject)
{
    return NULL;
}

inline TiXmlElement* CheckpointToXml(WwdObject* pWwdObject)
{
    return NULL;
}

inline TiXmlElement* PowderKegToXml(WwdObject* pWwdObject)
{
    return NULL;
}

inline TiXmlElement* OfficerToXml(WwdObject* pWwdObject)
{
    return NULL;
}

inline TiXmlElement* SoldierToXml(WwdObject* pWwdObject)
{
    return NULL;
}

inline TiXmlElement* RatToXml(WwdObject* pWwdObject)
{
    return NULL;
}

inline TiXmlElement* AmmoToXml(WwdObject* pWwdObject)
{
    return NULL;
}

inline TiXmlElement* TreasureToXml(WwdObject* pWwdObject)
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

inline TiXmlElement* SpecialPowerupToXml(WwdObject* pWwdObject)
{
    return NULL;
}

inline TiXmlElement* TogglePegToXml(WwdObject* pWwdObject)
{
    TiXmlElement* pTogglePegAIElem = new TiXmlElement("TogglePegAIComponent");
    pTogglePegAIElem->SetAttribute("Type", pWwdObject->logic);
    pTogglePegAIElem->SetAttribute("resource", "created");

    std::string logic = pWwdObject->logic;
    std::string imageSet = pWwdObject->imageSet;

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

    // Different image sets have different anim frames at which they toggle

    int toggleFrameIdx = -1;
    if (imageSet == "LEVEL_PEG") // LEVEL 1
    {
        toggleFrameIdx = 9;
    }
    else if (imageSet == "LEVEL_PEGSLIDER") // LEVEL 2
    {
        toggleFrameIdx = 2;
    }

    assert(toggleFrameIdx != -1);

    XML_ADD_TEXT_ELEMENT("ToggleFrameIdx", ToStr(toggleFrameIdx).c_str(), pTogglePegAIElem);

    return pTogglePegAIElem;
}

inline TiXmlElement* WwdObjectToXml(WwdObject* wwdObject, std::string& imagesRootPath, int levelNumber)
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
        std::string sound = wwdObject->sound;
        int soundVolume = wwdObject->damage;
        if (soundVolume == 0)
        {
            soundVolume = 100;
        }

        if (logic == "GlobalAmbientSound")
        {
            int minTimeOff = wwdObject->moveRect.right;
            int maxTimeOff = wwdObject->moveRect.bottom;
            int minTimeOn = wwdObject->moveRect.left;
            int maxTimeOn = wwdObject->moveRect.top;
            bool isLooping = minTimeOn == 0;

            // Level 2 global sounds are SO DAMN LOUD
            if ((levelNumber == 2 ) && isLooping)
            {
                soundVolume /= 3;
            }

            SAFE_DELETE(pActorElem);
            return ActorTemplates::CreateXmlData_GlobalAmbientSoundActor(
                sound,
                soundVolume,
                minTimeOff,
                maxTimeOff,
                minTimeOn,
                maxTimeOn,
                isLooping);
        }
        else if (logic == "SpotAmbientSound" || 
                 logic == "AmbientSound")
        {
            /*if (pWwdObject->maxX != 0 && pWwdObject->maxY != 0)
            {
                pAmbientSoundElem->LinkEndChild(CreatePlayAreaElement(pWwdObject->minX, pWwdObject->minY,
                    pWwdObject->maxX - pWwdObject->minX, pWwdObject->maxY - pWwdObject->minY));
            }*/

            assert(wwdObject->maxX != 0);
            assert(wwdObject->maxY != 0);

            // Claw guys seem to have some typos in image set names...
            if (sound == "LEVEL_AMBIENT_ANGVIL")
            {
                sound = "LEVEL_AMBIENT_ANVIL";
                // This had to be some kind of a mistake on their part
                soundVolume = 0;
            }

            /*Point center(
                (wwdObject->minX + wwdObject->maxX) / 2,
                (wwdObject->minY + wwdObject->maxY) / 2);*/
            Point center(wwdObject->x, wwdObject->y);
            Point size(
                wwdObject->maxX - wwdObject->minX,
                wwdObject->maxY - wwdObject->minY);
            if (size.y > size.x)
            {
                size.y = size.x;
            }

            LocalAmbientSoundDef soundDef;
            soundDef.sound = ActorTemplates::GetSoundPathFromClawPath(sound);
            soundDef.volume = soundVolume;
            soundDef.soundAreaSize = size;

            LOG("Size: " + soundDef.soundAreaSize.ToString());
            SAFE_DELETE(pActorElem);
            return ActorTemplates::CreateXmlData_LocalAmbientSound(
                ActorPrototype_LocalAmbientSound,
                center,
                soundDef);
        }
        else
        {
            LOG("Not created ambient sound logic: " + logic);
            Point pos(wwdObject->x, wwdObject->y);
            LOG("Position: " + pos.ToString());
        }

        //pActorElem->LinkEndChild(AmbientSoundToXml(wwdObject));
    }
    else if (logic.find("SoundTrigger") != std::string::npos)
    {
        std::string sound = wwdObject->sound;
        if (true)//sound == "LEVEL_TRIGGER_PUDDLE2")
        {
            SDL_Rect predefinedPosition;
            predefinedPosition.x = wwdObject->minX;
            predefinedPosition.y = wwdObject->minY;
            predefinedPosition.w = wwdObject->maxX;
            predefinedPosition.h = wwdObject->maxY;

            int enterCount = wwdObject->smarts;
            // TODO: ...
            if (enterCount == 0)
            {
                enterCount = 1;
            }

            SAFE_DELETE(pActorElem);
            return ActorTemplates::CreateXmlData_SoundTriggerActor(sound, logic, Point(wwdObject->x, wwdObject->y), predefinedPosition, enterCount);
        }
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

        SAFE_DELETE(pActorElem);
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
        SAFE_DELETE(pActorElem);
        return ActorTemplates::CreateXmlData_CrumblingPeg(tmpImageSet, Point(wwdObject->x, wwdObject->y), wwdObject->z);

        pActorElem->LinkEndChild(CrumblingPegToXml(wwdObject));

        TiXmlElement* animElem = new TiXmlElement("AnimationComponent");
        XML_ADD_1_PARAM_ELEMENT("Animation", "type", "cycle50", animElem);
        pActorElem->LinkEndChild(animElem);
    }
    else if (logic.find("Elevator") != std::string::npos)
    {
        SAFE_DELETE(pActorElem);

        std::string logic = wwdObject->logic;

        Point speed(0, 0);
        bool hasHorizontalMovement = false;
        bool hasVerticalMovement = false;
        if (wwdObject->minX > 0 && wwdObject->maxX > 0)
        {
            hasHorizontalMovement = true;
        }
        if (wwdObject->minY > 0 && wwdObject->maxY > 0)
        {
            hasVerticalMovement = true;
        }

        if (hasHorizontalMovement)
        {
            if (wwdObject->speedX > 0)
            {
                speed.x = wwdObject->speedX;
            }
            else
            {
                speed.x = 125;
            }
        }
        if (hasVerticalMovement)
        {
            if (wwdObject->speedY > 0)
            {
                speed.y = wwdObject->speedY;
            }
            else if (wwdObject->speedX > 0)
            {
                speed.y = wwdObject->speedX;
            }
            else
            {
                speed.y = 125;
            }
        }

        ElevatorDef elevatorDef;
        elevatorDef.speed = speed;
        elevatorDef.minPosition.Set(wwdObject->minX, wwdObject->minY);
        elevatorDef.maxPosition.Set(wwdObject->maxX, wwdObject->maxY);
        elevatorDef.hasTriggerBehaviour = logic.find("Trigger") != std::string::npos;
        elevatorDef.hasStartBehaviour = logic.find("Start") != std::string::npos;
        elevatorDef.hasStopBehaviour = logic.find("Stop") != std::string::npos;
        elevatorDef.hasOneWayBehaviour = logic.find("OneWay") != std::string::npos;

        Point position(wwdObject->x, wwdObject->y);

        ActorPrototype elevatorProto = ActorPrototype_Start;
        if (levelNumber == 1)
        {
            elevatorProto = ActorPrototype_Level1_Elevator;
        }
        else if (levelNumber == 2)
        {
            elevatorProto = ActorPrototype_Level2_Elevator;
        }
        assert(elevatorProto != ActorPrototype_Start && "Unsupported level ?");

        return ActorTemplates::CreateXmlData_ElevatorActor(elevatorProto, position, elevatorDef);
    }
    else if (logic.find("TogglePeg") != std::string::npos)
    {
        SAFE_DELETE(pActorElem);

        std::string logic = wwdObject->logic;
        std::string imageSet = wwdObject->imageSet;

        TogglePegDef togglePegDef;

        uint32 delay = 0;
        if (logic == "TogglePeg")
        {
            togglePegDef.delay = 0;
        }
        else if (logic == "TogglePeg2")
        {
            togglePegDef.delay = 750;
        }
        else if (logic == "TogglePeg3")
        {
            togglePegDef.delay = 1500;
        }
        else if (logic == "TogglePeg4")
        {
            togglePegDef.delay = 2250;
        }
        else
        {
            LOG_WARNING("Unknown TogglePeg: " + logic)
        }

        if (wwdObject->speed > 0)
        {
            togglePegDef.delay = wwdObject->speed;
        }

        uint32 timeOn = 0;
        uint32 timeOff = 0;
        if (wwdObject->speedX > 0)
        {
            timeOn = wwdObject->speedX;
        }
        else
        {
            timeOn = 1500;
        }
        if (wwdObject->speedY > 0)
        {
            timeOff = wwdObject->speedY;
        }
        else
        {
            timeOff = 1500;
        }

        if (wwdObject->smarts & 0x1)
        {
            togglePegDef.isAlwaysOn = true;
        }
        else
        {
            togglePegDef.timeOn = timeOn;
            togglePegDef.timeOff = timeOff;
        }

        Point position(wwdObject->x, wwdObject->y);

        ActorPrototype proto = ActorPrototype_Start;
        if (levelNumber == 1) proto = ActorPrototype_Level1_TogglePeg;
        if (levelNumber == 2) proto = ActorPrototype_Level2_TogglePeg;

        assert(proto != ActorPrototype_Start);

        return ActorTemplates::CreateXmlData_TogglePegActor(
            proto,
            position,
            togglePegDef);

        /*pActorElem->LinkEndChild(TogglePegToXml(wwdObject));

        TiXmlElement* animElem = new TiXmlElement("AnimationComponent");
        XML_ADD_1_PARAM_ELEMENT("Animation", "type", "cycle75", animElem);
        pActorElem->LinkEndChild(animElem);*/
    }
    else if (logic.find("Checkpoint") != std::string::npos)
    {
        bool isSaveCheckpoint = false;
        uint32 saveCheckpointNumber = 0;
        if (logic == "FirstSuperCheckpoint")
        {
            isSaveCheckpoint = true;
            saveCheckpointNumber = 1;
        }
        else if (logic == "SecondSuperCheckpoint")
        {
            isSaveCheckpoint = true;
            saveCheckpointNumber = 2;
        }

        SAFE_DELETE(pActorElem);
        return ActorTemplates::CreateXmlData_CheckpointActor(
            wwdObject->imageSet, 
            Point(wwdObject->x, wwdObject->y), 
            wwdObject->z, 
            Point(wwdObject->x, wwdObject->y), 
            isSaveCheckpoint, 
            saveCheckpointNumber);
    }
    else if (logic.find("PowderKeg") != std::string::npos)
    {
        //pActorElem->LinkEndChild(PowderKegToXml(wwdObject));
        SAFE_DELETE(pActorElem);
        return ActorTemplates::CreateXmlData_PowderKegActor(tmpImageSet, Point(wwdObject->x, wwdObject->y), 50, wwdObject->z);
    }
    else if (logic == "Officer" ||
             logic == "Soldier" ||
             logic == "Rat")
    {
        std::vector<PickupType> loot;
        if (wwdObject->powerup > 0) { loot.push_back(PickupType(wwdObject->powerup)); }
        if (wwdObject->userRect1.left > 0) { loot.push_back(PickupType(wwdObject->userRect1.left)); }
        if (wwdObject->userRect1.right > 0) { loot.push_back(PickupType(wwdObject->userRect1.right)); }
        if (wwdObject->userRect1.bottom > 0) { loot.push_back(PickupType(wwdObject->userRect1.bottom)); }
        if (wwdObject->userRect1.top > 0) { loot.push_back(PickupType(wwdObject->userRect1.top)); }

        SAFE_DELETE(pActorElem);

        ActorPrototype actorProto = ActorPrototype_Start;
        if (levelNumber == 1)
        {
            if (logic == "Rat")
            {
                actorProto = ActorPrototype_Level1_Rat;
            }
            else if (logic == "Soldier")
            {
                actorProto = ActorPrototype_Level1_Soldier;
            }
            else if (logic == "Officer")
            {
                actorProto = ActorPrototype_Level1_Officer;
            }
        }
        else if (levelNumber == 2)
        {
            if (logic == "Soldier")
            {
                actorProto = ActorPrototype_Level2_Soldier;
            }
            else if (logic == "Officer")
            {
                actorProto = ActorPrototype_Level2_Officer;
            }
        }
        assert(actorProto != ActorPrototype_Start && "Unsupported level ?");


        return ActorTemplates::CreateXmlData_EnemyAIActor(
            actorProto,
            Point(wwdObject->x, wwdObject->y), 
            loot, 
            wwdObject->minX, 
            wwdObject->maxX,
            wwdObject->userValue1);
    }
    else if (logic.find("Rat") != std::string::npos)
    {
        //pActorElem->LinkEndChild(RatToXml(wwdObject));
    }
    else if (logic.find("AmmoPowerup") != std::string::npos)
    {
        std::string pickupSound;
        if (std::string(wwdObject->imageSet).find("MAGIC") != std::string::npos)
        {
            pickupSound = SOUND_GAME_PICKUP_MAGIC;
        }
        else
        {
            pickupSound = SOUND_GAME_PICKUP_AMMUNITION;
        }

        SAFE_DELETE(pActorElem);
        return ActorTemplates::CreateXmlData_AmmoPickupActor(wwdObject->imageSet, pickupSound, Point(wwdObject->x, wwdObject->y), true);
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

        if (imageSet != "GAME_WARP" || imageSet == "GAME_VERTWARP")
        {
            pActorElem->LinkEndChild(ActorTemplates::CreateXmlData_GlitterComponent("Glitter_Yellow", true, false));
        }

        std::string imageSet = wwdObject->imageSet;
        if (imageSet == "GAME_WARP" || imageSet == "GAME_VERTWARP")
        {
            TiXmlElement* pTeleportPickupComponent = new TiXmlElement("TeleportPickupComponent");
            XML_ADD_2_PARAM_ELEMENT("Destination", "x", ToStr(wwdObject->speedX).c_str(), "y", ToStr(wwdObject->speedY).c_str(), pTeleportPickupComponent);
            pActorElem->LinkEndChild(pTeleportPickupComponent);

            TiXmlElement* pAnimCompElem = new TiXmlElement("AnimationComponent");
            pAnimCompElem->LinkEndChild(CreateCycleAnimation(125));
            pActorElem->LinkEndChild(pAnimCompElem);
        }
        else if (imageSet == "GAME_POWERUPS_EXTRALIFE")
        {
            SAFE_DELETE(pActorElem);
            return ActorTemplates::CreateXmlData_LifePickupActor(
                imageSet, SOUND_GAME_EXTRA_LIFE, Point(wwdObject->x, wwdObject->y), true);
        }
        else if (imageSet == "GAME_POWERUPS_INVULNERABLE")
        {
            CREATE_POWERUP_COMPONENT("Invulnerability", "30000");
            XML_ADD_TEXT_ELEMENT("PickupSound", SOUND_GAME_PICKUP_MAGIC, pPowerupPickupComponent);
        }
        else if (imageSet == "GAME_POWERUPS_GHOST")
        {
            CREATE_POWERUP_COMPONENT("Invisibility", "30000");
            XML_ADD_TEXT_ELEMENT("PickupSound", SOUND_GAME_PICKUP_MAGIC, pPowerupPickupComponent);
        }
        else if (imageSet == "GAME_CATNIPS_NIP1")
        {
            int duration = 15000;
            if (wwdObject->smarts > 0)
            {
                duration = wwdObject->smarts;
            }

            CREATE_POWERUP_COMPONENT("Catnip", ToStr(duration).c_str());
            XML_ADD_TEXT_ELEMENT("PickupSound", SOUND_GAME_PICKUP_CATNIP, pPowerupPickupComponent);
        }
        else if (imageSet == "GAME_CATNIPS_NIP2")
        {
            int duration = 30000;
            if (wwdObject->smarts > 0)
            {
                duration = wwdObject->smarts;
            }

            CREATE_POWERUP_COMPONENT("Catnip", ToStr(duration).c_str());
            XML_ADD_TEXT_ELEMENT("PickupSound", SOUND_GAME_PICKUP_CATNIP, pPowerupPickupComponent);
        }
        else if (imageSet == "GAME_POWERUPS_FIRESWORD")
        {
            CREATE_POWERUP_COMPONENT("FireSword", "30000");
            XML_ADD_TEXT_ELEMENT("PickupSound", SOUND_CLAW_PICKUP_FIRE_SWORD, pPowerupPickupComponent);
        }
        else if (imageSet == "GAME_POWERUPS_ICESWORD")
        {
            CREATE_POWERUP_COMPONENT("IceSword", "30000");
            XML_ADD_TEXT_ELEMENT("PickupSound", SOUND_CLAW_PICKUP_FROST_SWORD, pPowerupPickupComponent);
        }
        else if (imageSet == "GAME_POWERUPS_LIGHTNINGSWORD")
        {
            CREATE_POWERUP_COMPONENT("LightningSword", "30000");
            XML_ADD_TEXT_ELEMENT("PickupSound", SOUND_CLAW_PICKUP_LIGHTNING_SWORD, pPowerupPickupComponent);
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
        std::string pickupSound;

        if (imageSet == "GAME_TREASURE_COINS")
        {
            points = 100;
            pickupSound = SOUND_GAME_TREASURE_COIN;
        }
        else if (imageSet == "GAME_TREASURE_GOLDBARS")
        {
            points = 500;
            pickupSound = SOUND_GAME_TREASURE_GOLDBAR;
        }
        else if (imageSet == "GAME_TREASURE_NECKLACE")
        {
            points = 10000;
            pickupSound = SOUND_GAME_TREASURE_GECKO;
        }
        else if (imageSet.find("GAME_TREASURE_RINGS") != std::string::npos)
        {
            points = 1500;
            pickupSound = SOUND_GAME_TREASURE_RING;
        }
        else if (imageSet.find("GAME_TREASURE_CHALICES") != std::string::npos)
        {
            points = 2500;
            pickupSound = SOUND_GAME_TREASURE_CHALICE;
        }
        else if (imageSet.find("GAME_TREASURE_CROSSES") != std::string::npos)
        {
            points = 5000;
            pickupSound = SOUND_GAME_TREASURE_CROSS;
        }
        else if (imageSet.find("GAME_TREASURE_SCEPTERS") != std::string::npos)
        {
            points = 7500;
            pickupSound = SOUND_GAME_TREASURE_SCEPTER;
        }
        else if (imageSet.find("GAME_TREASURE_GECKOS") != std::string::npos)
        {
            points = 10000;
            pickupSound = SOUND_GAME_TREASURE_GECKO;
        }
        else if (imageSet.find("GAME_TREASURE_CROWNS") != std::string::npos)
        {
            points = 15000;
            pickupSound = SOUND_GAME_TREASURE_CROWN;
        }
        else if (imageSet.find("GAME_TREASURE_JEWELEDSKULL") != std::string::npos)
        {
            points = 25000;
            pickupSound = SOUND_GAME_TREASURE_SKULL;
        }
        else
        {
            assert(false && "Unknown treasure");
        }

        TiXmlElement* pScoreElement = new TiXmlElement("TreasurePickupComponent");
        XML_ADD_TEXT_ELEMENT("ScorePoints", ToStr(points).c_str(), pScoreElement);
        XML_ADD_TEXT_ELEMENT("PickupSound", pickupSound.c_str(), pScoreElement);
        pActorElem->LinkEndChild(pScoreElement);

        if (imageSet != "GAME_TREASURE_COINS")
        {
            pActorElem->LinkEndChild(ActorTemplates::CreateXmlData_GlitterComponent("Glitter_Yellow", true, false));
        }

        TiXmlElement* elem = TreasureToXml(wwdObject);
        if (elem)
        {
            pActorElem->LinkEndChild(elem);
        }
    }
    else if (logic == "HealthPowerup" && imageSet.find("CATNIPS") == std::string::npos)
    {
        std::string pickupSound = SOUND_GAME_PICKUP_FOODITEM;
        if (std::string(wwdObject->imageSet).find("POTION") != std::string::npos ||
            std::string(wwdObject->imageSet).find("MILK") != std::string::npos)
        {
            pickupSound = SOUND_GAME_PICKUP_POTION;
        }


        // TODO: Use this instead of the stuff below
        /*delete pActorElem;
        return ActorTemplates::CreateXmlData_HealthPickupActor(tmpImageSet, pickupSound, Point(wwdObject->x, wwdObject->y), true);*/

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
            assert(false);
        }

        XML_ADD_TEXT_ELEMENT("PickupSound", pickupSound.c_str(), healthPickupComponent);
    }
    else if (logic == "TowerCannonLeft")
    {
        SAFE_DELETE(pActorElem);

        assert(levelNumber == 2 && "Expected only level 2");

        Point position(wwdObject->x, wwdObject->y);
        return ActorTemplates::CreateXmlData_Actor(ActorPrototype_Level2_TowerCannonLeft, position);
    }
    else if (logic == "TowerCannonRight")
    {
        SAFE_DELETE(pActorElem);

        assert(levelNumber == 2 && "Expected only level 2");

        Point position(wwdObject->x, wwdObject->y);
        return ActorTemplates::CreateXmlData_Actor(ActorPrototype_Level2_TowerCannonRight, position);
    }
    else
    {
        static std::vector<std::string> s_ReportedUnknownLogicsList;

        bool isAlreadyReported = false;
        for (std::string unkLogic : s_ReportedUnknownLogicsList)
        {
            if (unkLogic == logic)
            {
                isAlreadyReported = true;
                break;
            }
        }

        if (!isAlreadyReported)
        {
            s_ReportedUnknownLogicsList.push_back(logic);
            LOG_WARNING("Unknown logic: " + logic);
        }
    }

    return pActorElem;
}

//=====================================================================================================================
// Claw to Xml
//=====================================================================================================================

inline TiXmlElement* CreateClawActor(WapWwd* pWapWwd)
{
    TiXmlElement* pClawActor = new TiXmlElement("Actor");
    pClawActor->SetAttribute("Type", "Claw");
    pClawActor->SetAttribute("resource", "created");

    /*ActorBodyDef clawBodyDef;
    clawBodyDef.addFootSensor = true;
    clawBodyDef.bodyType = b2_dynamicBody;
    clawBodyDef.collisionFlag = CollisionFlag_Controller;
    clawBodyDef.collisionMask = 0x1;
    clawBodyDef.makeSensor = false;
    clawBodyDef.makeCapsule = true;
    clawBodyDef.size = Point(40, 130);
    clawBodyDef.gravityScale = 4.0;
    clawBodyDef.fixtureType = FixtureType_Controller;
    pClawActor->LinkEndChild(ActorTemplates::CreatePhysicsComponent(&clawBodyDef));*/

    pClawActor->LinkEndChild(CreatePositionComponent(pWapWwd->properties.startX, pWapWwd->properties.startY));
    //pClawActor->LinkEndChild(CreatePositionComponent(6250, 4350));
    pClawActor->LinkEndChild(CreateCollisionComponent(40, 110));
    pClawActor->LinkEndChild(CreatePhysicsComponent(true, false, true, g_pApp->GetGlobalOptions()->maxJumpHeight, 40, 110, 4.0, 0.0, 0.5));
    pClawActor->LinkEndChild(CreateControllableComponent(true));
    pClawActor->LinkEndChild(CreateAnimationComponent("/CLAW/ANIS/*"));
    pClawActor->LinkEndChild(CreateSoundComponent("/CLAW/SOUNDS/*"));
    pClawActor->LinkEndChild(CreateActorRenderComponent("/CLAW/IMAGES/*", 4000));
    pClawActor->LinkEndChild(ActorTemplates::CreateFollowableComponent(Point(-5, -80), "/GAME/IMAGES/EXCLAMATION/*", ""));

    TiXmlElement* pScoreComponent = new TiXmlElement("ScoreComponent");
    XML_ADD_TEXT_ELEMENT("Score", "0", pScoreComponent);
    pClawActor->LinkEndChild(pScoreComponent);

    TiXmlElement* pLifeComponent = new TiXmlElement("LifeComponent");
    XML_ADD_TEXT_ELEMENT("Lives", "0", pLifeComponent);
    pClawActor->LinkEndChild(pLifeComponent);

    TiXmlElement* pHealthComponent = new TiXmlElement("HealthComponent");
    XML_ADD_TEXT_ELEMENT("Health", "0", pHealthComponent);
    XML_ADD_TEXT_ELEMENT("MaxHealth", "100", pHealthComponent);
    pClawActor->LinkEndChild(pHealthComponent);

    TiXmlElement* pAmmoComponent = new TiXmlElement("AmmoComponent");
    XML_ADD_TEXT_ELEMENT("Pistol", "0", pAmmoComponent);
    XML_ADD_TEXT_ELEMENT("Magic", "0", pAmmoComponent);
    XML_ADD_TEXT_ELEMENT("Dynamite", "0", pAmmoComponent);
    pClawActor->LinkEndChild(pAmmoComponent);

    TiXmlElement* pPowerupComponent = new TiXmlElement("PowerupComponent");
    pClawActor->LinkEndChild(pPowerupComponent);

    return pClawActor;
}

//=====================================================================================================================
// HUD to Xml
//=====================================================================================================================

inline TiXmlElement* CreateHUDElement(std::string pathToImages, int animFrameDuration, std::string animPath, Point position, bool anchorRight, bool anchorBottom, std::string key, bool visible = true)
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

TiXmlElement* WwdToXml(WapWwd* wapWwd, int levelNumber);


#endif
