#ifndef __CONVERTERS_H__
#define __CONVERTERS_H__

#include <algorithm>
#include <libwap.h>
#include "../SharedDefines.h"
#include "../Actor/ActorTemplates.h"
#include "../GameApp/BaseGameApp.h"
#include "ClawLevelUtil.h"

//
//
// TODO TODO TODO: Somehow refactor this, delete unneeded ones and use everything from Actor/ActorTemplates.cpp
//
//

ActorPrototype ActorLogicToActorPrototype(const std::string& logic, int levelNumber);
TiXmlElement* WwdToXml(WapWwd* wapWwd, int levelNumber);

#define INSERT_POSITION_COMPONENT(x, y, rootElem) \
{ \
   rootElem->LinkEndChild(CreatePositionComponent(x, y)); \
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

inline TiXmlElement* CreateActorRenderComponent(const char* imagesPath, int32 zCoord, bool visible = true)
{
    TiXmlElement* elem = new TiXmlElement("ActorRenderComponent");

    XML_ADD_TEXT_ELEMENT("ImagePath", imagesPath, elem);
    XML_ADD_TEXT_ELEMENT("ZCoord", ToStr(zCoord).c_str(), elem);
    XML_ADD_TEXT_ELEMENT("Visible", ToStr(visible).c_str(), elem);

    return elem;
}

//=====================================================================================================================

inline TiXmlElement* CreateAnimationComponent(const std::string &aniPath)
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

//=====================================================================================================================

inline TiXmlElement* EyeCandyToXml(WwdObject* pWwdObject)
{
    if (std::string(pWwdObject->logic).find("Ani") != std::string::npos)
    {
        TiXmlElement* animElem = new TiXmlElement("AnimationComponent");
        XML_ADD_1_PARAM_ELEMENT("Animation", "type", "cycle150", animElem);

        return animElem;
    }

    return NULL;
}

// @moveType: Left/Right/LeftTop/... or stop for a certain time only 1-9 are valid
// @value: pixel distance or milisecond stop
inline ElevatorStepDef GetElevatorStepDef(int moveType, int value)
{
    assert(moveType >= 1 && moveType <= 9);
    assert(value > 0);

    Direction dir = Direction_None;

    switch (moveType)
    {
        case 1: dir = Direction_Down_Left; break;
        case 2: dir = Direction_Down; break;
        case 3: dir = Direction_Down_Right; break;
        case 4: dir = Direction_Left; break;
        case 5: dir = Direction_None; break;
        case 6: dir = Direction_Right; break;
        case 7: dir = Direction_Up_Left; break;
        case 8: dir = Direction_Up; break;
        case 9: dir = Direction_Up_Right; break;
        default: assert(false);
    }

    ElevatorStepDef def;

    // Stop for given amount of milis
    if (dir == Direction_None)
    {
        def.isWaiting = true;
        def.waitMsTime = value;
    }
    else
    {
        def.direction = dir;
        def.stepDeltaDistance = value;
    }
    
    return def; 
}

inline TiXmlElement* WwdObjectToXml(WwdObject* wwdObject, std::string& imagesRootPath, int levelNumber)
{
    TiXmlElement* pActorElem = new TiXmlElement("Actor");
    pActorElem->SetAttribute("Type", wwdObject->imageSet);

    std::string logic = wwdObject->logic;
    std::string imageSet = wwdObject->imageSet;

    bool bIsMirrored = wwdObject->drawFlags & WAP_OBJECT_DRAW_FLAG_MIRROR;
    bool bIsInverted = wwdObject->drawFlags & WAP_OBJECT_DRAW_FLAG_INVERT;
    bool bIsVisible = !(wwdObject->drawFlags & WAP_OBJECT_DRAW_FLAG_NO_DRAW);

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
        tmpImagesRootPath = "/LEVEL" + ToStr(levelNumber) + "/IMAGES/";
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
    if (logic == "DoNothing" || logic == "DoNothingNormal")
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

    // Level 9 and 10 SHOOTERS_PUFFDARTLEFT is screwed
    if ((levelNumber == 9 || levelNumber == 10 || levelNumber == 13) &&
            tmpImageSet.find("SHOOTERS") != std::string::npos)
    {
        std::replace(tmpImageSet.begin(), tmpImageSet.end(), '_', '/');
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

    ActorPrototype actorProto = ClawLevelUtil::ActorLogicToActorPrototype(levelNumber, logic);
    const Point actorPosition = Point(wwdObject->x, wwdObject->y);

    std::vector<XmlNodeOverride> xmlOverrideList;

    // Defaults
    xmlOverrideList.push_back(XmlNodeOverride("Actor.ActorRenderComponent.ImagePath", tmpImageSet));
    xmlOverrideList.push_back(XmlNodeOverride("Actor.ActorRenderComponent.Visible", bIsVisible));
    xmlOverrideList.push_back(XmlNodeOverride("Actor.ActorRenderComponent.Mirrored", bIsMirrored));
    xmlOverrideList.push_back(XmlNodeOverride("Actor.ActorRenderComponent.Inverted", bIsInverted));
    xmlOverrideList.push_back(XmlNodeOverride("Actor.ActorRenderComponent.ZCoord", wwdObject->z));
    xmlOverrideList.push_back(XmlNodeOverride("Actor.PositionComponent.Position", "x", actorPosition.x, "y", actorPosition.y));

    if (actorProto == ActorPrototype_Null)
    {
        xmlOverrideList.push_back(XmlNodeOverride("Actor.ActorRenderComponent.Visible", false));
        return ActorTemplates::CreateXmlData_Actor(actorProto, xmlOverrideList);
    }

    //LOG("Logic: " + logic);
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

            // Level 2/3/4 global sounds are SO DAMN LOUD
            if ((levelNumber == 2 || levelNumber == 3 || levelNumber == 4) && isLooping)
            {
                soundVolume /= 5;
            }
            // Also non-looping sounds require tweaks
            if ((levelNumber == 1 || levelNumber == 2 || levelNumber == 3 || levelNumber == 4) && !isLooping)
            {
                //soundVolume = (int)((float)soundVolume / 1.5f);
            }
            if (levelNumber == 13)
            {
                if (sound == "LEVEL_TRIGGER_BIRDSCALL5")
                {
                    sound = "/LEVEL13/SOUNDS/TRIGGER/BIRDCALL5.WAV";
                }
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

            /*assert(wwdObject->maxX != 0);
            assert(wwdObject->maxY != 0);*/

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
            Point size;

            if (wwdObject->maxX == 0 && wwdObject->maxY == 0)
            {
                assert(wwdObject->hitRect.left != 0);
                assert(wwdObject->hitRect.right != 0);
                assert(wwdObject->hitRect.top != 0);
                assert(wwdObject->hitRect.bottom != 0);

                center.Set(
                    (wwdObject->hitRect.right + wwdObject->hitRect.left) / 2,
                    (wwdObject->hitRect.bottom + wwdObject->hitRect.top) / 2);
                size.Set(
                    (wwdObject->hitRect.right - wwdObject->hitRect.left),
                    (wwdObject->hitRect.bottom - wwdObject->hitRect.top));
            }
            else
            {
                size.Set(
                    wwdObject->maxX - wwdObject->minX,
                    wwdObject->maxY - wwdObject->minY);
            }

            assert(!center.IsZeroXY());
            assert(!size.IsZeroXY());
            
            if (size.y > size.x)
            {
                size.y = size.x;
            }

            LocalAmbientSoundDef soundDef;
            soundDef.sound = ActorTemplates::GetSoundPathFromClawPath(sound);
            soundDef.volume = soundVolume;
            soundDef.soundAreaSize = size;

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

            if ((levelNumber == 3) && (sound == "LEVEL_TRIGGER_BIRDCALL2"))
            {
                sound = "LEVEL_AMBIENT_BIRDCALL2";
            }
            if ((levelNumber == 3) && (sound == "LEVEL_TRIGGER_1013"))
            {
                sound = "/LEVEL10/SOUNDS/TRIGGER/1013.WAV";
            }
            if ((levelNumber == 12))
            {
                if (sound == "LEVEL_TRIGGER_1044")
                {
                    sound = "/LEVEL11/SOUNDS/TRIGGER/1044.WAV";
                } 
                else if (sound == "LEVEL_TRIGGER_1005")
                {
                    sound = "/LEVEL11/SOUNDS/TRIGGER/1005.WAV";
                }
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
        if (wwdObject->userRect2.left > 0) { loot.push_back(PickupType(wwdObject->userRect2.left)); }
        if (wwdObject->userRect2.right > 0) { loot.push_back(PickupType(wwdObject->userRect2.right)); }
        if (wwdObject->userRect2.bottom > 0) { loot.push_back(PickupType(wwdObject->userRect2.bottom)); }
        if (wwdObject->userRect2.top > 0) { loot.push_back(PickupType(wwdObject->userRect2.top)); }

        SAFE_DELETE(pActorElem);
        return ActorTemplates::CreateXmlData_CrateActor(tmpImageSet, Point(wwdObject->x, wwdObject->y), loot, 5, wwdObject->z);
    }
    else if (logic.find("Statue") != std::string::npos)
    {
        SAFE_DELETE(pActorElem);
        if (actorProto == ActorPrototype_None)
        {
            return NULL;
        }

        std::vector<PickupType> loot;
        if (wwdObject->powerup > 0) { loot.push_back(PickupType(wwdObject->powerup)); }
        if (wwdObject->userRect1.left > 0) { loot.push_back(PickupType(wwdObject->userRect1.left)); }
        if (wwdObject->userRect1.right > 0) { loot.push_back(PickupType(wwdObject->userRect1.right)); }
        if (wwdObject->userRect1.bottom > 0) { loot.push_back(PickupType(wwdObject->userRect1.bottom)); }
        if (wwdObject->userRect1.top > 0) { loot.push_back(PickupType(wwdObject->userRect1.top)); }
        if (wwdObject->userRect2.left > 0) { loot.push_back(PickupType(wwdObject->userRect2.left)); }
        if (wwdObject->userRect2.right > 0) { loot.push_back(PickupType(wwdObject->userRect2.right)); }
        if (wwdObject->userRect2.bottom > 0) { loot.push_back(PickupType(wwdObject->userRect2.bottom)); }
        if (wwdObject->userRect2.top > 0) { loot.push_back(PickupType(wwdObject->userRect2.top)); }

        return ActorTemplates::CreateXmlData_LootContainer(
            ActorPrototype_Level5_LootStatue, 
            Point(wwdObject->x, wwdObject->y), 
            loot, 
            wwdObject->z);
    }
    else if (logic.find("Candy") != std::string::npos ||
             logic == "AniCycle")
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
        if (actorProto == ActorPrototype_None)
        {
            return NULL;
        }

        return ActorTemplates::CreateXmlData_CrumblingPeg(actorProto, actorPosition, tmpImageSet, 0);
    }
    else if (logic.find("Elevator") != std::string::npos &&
             logic != "PathElevator")
    {
        SAFE_DELETE(pActorElem);
        if (actorProto == ActorPrototype_None)
        {
            return NULL;
        }

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

        return ActorTemplates::CreateXmlData_ElevatorActor(actorProto, position, tmpImageSet, elevatorDef);
    }
    else if (logic.find("TogglePeg") != std::string::npos)
    {
        SAFE_DELETE(pActorElem);
        if (actorProto == ActorPrototype_None)
        {
            return NULL;
        }

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

        // 2 different TogglePegs 
        if (levelNumber == 8)
        {
            if (imageSet == "LEVEL_PEG")
            {
                actorProto = ActorPrototype_Level8_TogglePeg_2;
            }
            else
            {
                actorProto = ActorPrototype_Level8_TogglePeg;
            }
        }

        return ActorTemplates::CreateXmlData_TogglePegActor(
            actorProto,
            actorPosition,
            togglePegDef);
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
    else if (logic == "CrabNest")
    {
        SAFE_DELETE(pActorElem);
        if (actorProto == ActorPrototype_None)
        {
            return NULL;
        }
        
        Point spawnAreaPosition((wwdObject->minX + wwdObject->maxX) / 2, (wwdObject->minY + wwdObject->maxY) / 2);
        Point spawnAreaSize(wwdObject->maxX - wwdObject->minX, wwdObject->maxY - wwdObject->minY);
        Point spawnAreaOffset((spawnAreaPosition.x - actorPosition.x) / 2, (spawnAreaPosition.y - actorPosition.y));

        int numCrabs = wwdObject->userValue1;
        std::vector<ActorSpawnInfo> spawnedActorInfoList;
        spawnedActorInfoList.reserve(numCrabs);
        for (int i = 0; i < numCrabs; i++)
        {
            ActorSpawnInfo actorSpawnInfo;
            actorSpawnInfo.actorProto = ActorPrototype_Level7_HermitCrab;
            actorSpawnInfo.spawnPositionOffset = Point(0, -10);

            double randVelocityX = (double)Util::GetRandomNumber(-1000, 1000) / 1000.0;
            actorSpawnInfo.initialVelocity = Point(randVelocityX, -5);

            spawnedActorInfoList.push_back(actorSpawnInfo);
        }

        return ActorTemplates::CreateXmlData_ActorSpawner(
            actorProto,
            actorPosition,
            spawnAreaOffset,
            spawnAreaSize,
            spawnedActorInfoList);
    }
    else if (logic == "Officer" ||
             logic == "Soldier" ||
             logic == "Rat" ||
             logic == "PunkRat" ||
             logic == "CutThroat" ||
             logic == "RobberThief" ||
             logic == "TownGuard1" ||
             logic == "TownGuard2" ||
             logic == "Seagull" ||
             logic == "RedTailPirate" ||
             logic == "BearSailor" ||
             logic == "HermitCrab" ||
             logic == "Raux" ||
             logic == "Katherine" ||
             logic == "Wolvington" ||
             logic == "CrazyHook" ||
             logic == "PegLeg" ||
             logic == "Marrow" ||
             logic == "Mercat" ||
             logic == "Siren" ||
             logic == "Fish" ||
             logic == "Aquatis" ||
             logic == "RedTail")
    {
        SAFE_DELETE(pActorElem);
        if (actorProto == ActorPrototype_None)
        {
            return NULL;
        }

        std::vector<PickupType> loot;
        if (wwdObject->powerup > 0) { loot.push_back(PickupType(wwdObject->powerup)); }
        if (wwdObject->userRect1.left > 0) { loot.push_back(PickupType(wwdObject->userRect1.left)); }
        if (wwdObject->userRect1.right > 0) { loot.push_back(PickupType(wwdObject->userRect1.right)); }
        if (wwdObject->userRect1.bottom > 0) { loot.push_back(PickupType(wwdObject->userRect1.bottom)); }
        if (wwdObject->userRect1.top > 0) { loot.push_back(PickupType(wwdObject->userRect1.top)); }

        return ActorTemplates::CreateXmlData_EnemyAIActor(
            actorProto,
            Point(wwdObject->x, wwdObject->y), 
            loot, 
            wwdObject->minX, 
            wwdObject->maxX,
            wwdObject->userValue1,
            wwdObject->drawFlags & WAP_OBJECT_DRAW_FLAG_MIRROR);
    }
    else if (logic == "MarrowFloor")
    {
        double floorSpeed = wwdObject->speedX;
        Direction floorActiveDirection = wwdObject->direction == 0 ? Direction_Left : Direction_Right;

        // todo
        SAFE_DELETE(pActorElem);
        if (actorProto == ActorPrototype_None)
        {
            return NULL;
        }

        return ActorTemplates::CreateXmlData_Actor(actorProto, xmlOverrideList);
    }
    else if (logic == "Parrot")
    {
        SAFE_DELETE(pActorElem);
        if (actorProto == ActorPrototype_None)
        {
            return NULL;
        }

        return ActorTemplates::CreateXmlData_Actor(actorProto, xmlOverrideList);
    }
    else if (logic.find("CannonButton") != std::string::npos)
    {
        SAFE_DELETE(pActorElem);
        return ActorTemplates::CreateXmlData_Actor(ActorPrototype_Level8_GabrielButton, actorPosition);
    }
    else if (logic.find("GabrielCannon") != std::string::npos)
    {
        SAFE_DELETE(pActorElem);
        return ActorTemplates::CreateXmlData_Actor(ActorPrototype_Level8_GabrielCannon, actorPosition);
    }
    else if (logic.find("Laser") != std::string::npos)
    {
        SAFE_DELETE(pActorElem);
        return NULL; // TODO
    }
    else if (logic.find("Gabriel") != std::string::npos)
    {
        SAFE_DELETE(pActorElem);
        return ActorTemplates::CreateXmlData_Actor(ActorPrototype_Level8_Gabriel, actorPosition);
    }
    else if (logic.find("AquatisCrack") != std::string::npos)
    {
        SAFE_DELETE(pActorElem);
        return NULL; // TODO
    }
    else if (logic.find("Rat") != std::string::npos)
    {
        //pActorElem->LinkEndChild(RatToXml(wwdObject));
    }
    //=============================================================================================
    // PICKUPS
    //=============================================================================================
    else if (logic == "AmmoPowerup" ||
             logic == "SpecialPowerup" ||
             logic == "TreasurePowerup" ||
             logic == "GlitterlessPowerup" ||
             logic == "HealthPowerup" ||
             logic == "BossWarp" ||
             logic == "EndOfLevelPowerup" ||
             logic == "MagicPowerup")
    {
        SAFE_DELETE(pActorElem);

        ParamMap paramMap;
        paramMap["IsMirrored"] = ToStr((wwdObject->drawFlags & WAP_OBJECT_DRAW_FLAG_MIRROR) != 0);
        paramMap["IsInverted"] = ToStr((wwdObject->drawFlags & WAP_OBJECT_DRAW_FLAG_INVERT) != 0);

        if (imageSet == "GAME_WARP" || imageSet == "GAME_VERTWARP" || imageSet == "GAME_BOSSWARP")
        {
            paramMap["IsGlitter"] = "false";
            paramMap["DestinationX"] = ToStr(wwdObject->speedX);
            paramMap["DestinationY"] = ToStr(wwdObject->speedY);
        }

        if (imageSet == "GAME_TREASURE_COINS")
        {
            paramMap["IsGlitter"] = "false";
        }

        if ((imageSet.find("_CATNIP") != std::string::npos) ||
            imageSet.find("_POWERUPS") != std::string::npos)
        {
            if (wwdObject->smarts > 0)
            {
                paramMap["PowerupDuration"] = ToStr(wwdObject->smarts);
            }
        }

        PickupType pickupType = ActorTemplates::StringToEnum_ImageSetToPickupType(wwdObject->imageSet);

        return ActorTemplates::CreateXmlData_PickupActor(pickupType, Point(wwdObject->x, wwdObject->y), true, paramMap);
    }
    else if (logic == "TowerCannonLeft")
    {
        SAFE_DELETE(pActorElem);
        if (actorProto == ActorPrototype_None)
        {
            return NULL;
        }

        Point position(wwdObject->x, wwdObject->y);
        return ActorTemplates::CreateXmlData_Actor(actorProto, position);
    }
    else if (logic == "TowerCannonRight")
    {
        SAFE_DELETE(pActorElem);
        if (actorProto == ActorPrototype_None)
        {
            return NULL;
        }

        Point position(wwdObject->x, wwdObject->y);
        return ActorTemplates::CreateXmlData_Actor(actorProto, position);
    }
    else if (logic == "TProjectile")
    {
        SAFE_DELETE(pActorElem);
        if (actorProto == ActorPrototype_None)
        {
            return NULL;
        }

        ActorPrototype projectileProto = ActorPrototype_None;
        std::string shootAnim;
        Point projectileSpawnOffset;

        if (imageSet.find("UP") != std::string::npos)
        {
            shootAnim = "puffdartup";
            projectileProto = ActorPrototype_Level9_DartProjectile_Up;
            projectileSpawnOffset.Set(0, 60);
        }
        else if (imageSet.find("DOWN") != std::string::npos)
        {
            shootAnim = "puffdartdown";
            projectileProto = ActorPrototype_Level9_DartProjectile_Down;
            projectileSpawnOffset.Set(0, -60);
        }
        else if (imageSet.find("RIGHT") != std::string::npos)
        {
            shootAnim = "puffdartright";
            projectileProto = ActorPrototype_Level9_DartProjectile_Right;
            projectileSpawnOffset.Set(-60, 0); 
        }
        else if (imageSet.find("LEFT") != std::string::npos)
        {
            shootAnim = "puffdartleft";
            projectileProto = ActorPrototype_Level9_DartProjectile_Left;
            projectileSpawnOffset.Set(60, 0);
        }

        assert(!shootAnim.empty());
        assert(projectileProto != ActorPrototype_None);

        Point triggerSize(wwdObject->maxX - wwdObject->minX, wwdObject->maxY - wwdObject->minY);
        Point triggerAreaOffset(
            ((wwdObject->maxX + wwdObject->minX) / 2) - actorPosition.x, 
            ((wwdObject->maxY + wwdObject->minY) / 2) - actorPosition.y);

        xmlOverrideList.push_back(XmlNodeOverride("Actor.ProjectileSpawnerComponent.FireAnim", shootAnim));
        xmlOverrideList.push_back(XmlNodeOverride("Actor.ProjectileSpawnerComponent.ProjectilePrototype", EnumToString_ActorPrototype(projectileProto)));
        xmlOverrideList.push_back(XmlNodeOverride("Actor.ProjectileSpawnerComponent.ProjectileSpawnOffset", "x", projectileSpawnOffset.x, "y", projectileSpawnOffset.y));
        xmlOverrideList.push_back(XmlNodeOverride("Actor.ProjectileSpawnerComponent.TriggerAreaSize", "width", triggerSize.x, "height", triggerSize.y));
        xmlOverrideList.push_back(XmlNodeOverride("Actor.ProjectileSpawnerComponent.TriggerAreaOffset", "x", triggerAreaOffset.x, "y", triggerAreaOffset.y));
        
        return ActorTemplates::CreateXmlData_Actor(actorProto, xmlOverrideList);
    }
    else if (logic == "BossStager")
    {
        SAFE_DELETE(pActorElem);
        if (actorProto == ActorPrototype_None)
        {
            return NULL;
        }

        Point position(wwdObject->x, wwdObject->y);
        return ActorTemplates::CreateXmlData_Actor(actorProto, position);
    }
    else if (logic == "PathElevator")
    {
        SAFE_DELETE(pActorElem);
        if (actorProto == ActorPrototype_None)
        {
            return NULL;
        }

        PathElevatorDef pathElevatorDef;

        pathElevatorDef.speed = wwdObject->speed;
        if (wwdObject->moveRect.left != 0)
        {
            pathElevatorDef.elevatorPath.push_back(GetElevatorStepDef(wwdObject->moveRect.left, wwdObject->moveRect.top));
        }
        if (wwdObject->moveRect.right != 0)
        {
            pathElevatorDef.elevatorPath.push_back(GetElevatorStepDef(wwdObject->moveRect.right, wwdObject->moveRect.bottom));
        }

        if (wwdObject->hitRect.left != 0)
        {
            pathElevatorDef.elevatorPath.push_back(GetElevatorStepDef(wwdObject->hitRect.left, wwdObject->hitRect.top));
        }
        if (wwdObject->hitRect.right != 0)
        {
            pathElevatorDef.elevatorPath.push_back(GetElevatorStepDef(wwdObject->hitRect.right, wwdObject->hitRect.bottom));
        }

        if (wwdObject->attackRect.left != 0)
        {
            pathElevatorDef.elevatorPath.push_back(GetElevatorStepDef(wwdObject->attackRect.left, wwdObject->attackRect.top));
        }
        if (wwdObject->attackRect.right != 0)
        {
            pathElevatorDef.elevatorPath.push_back(GetElevatorStepDef(wwdObject->attackRect.right, wwdObject->attackRect.bottom));
        }

        if (wwdObject->clipRect.left != 0)
        {
            pathElevatorDef.elevatorPath.push_back(GetElevatorStepDef(wwdObject->clipRect.left, wwdObject->clipRect.top));
        }
        if (wwdObject->clipRect.right != 0)
        {
            pathElevatorDef.elevatorPath.push_back(GetElevatorStepDef(wwdObject->clipRect.right, wwdObject->clipRect.bottom));
        }

        // Default value. First occurance in level 7
        if (pathElevatorDef.speed == 0)
        {
            pathElevatorDef.speed = 125;
        }

        assert(pathElevatorDef.elevatorPath.size() >= 1);

        Point position(wwdObject->x, wwdObject->y);

        return ActorTemplates::CreateXmlData_PathElevator(
            actorProto,
            position,
            tmpImageSet,
            pathElevatorDef);
    }
    else if (logic.find("FloorSpike") != std::string::npos)
    {
        SAFE_DELETE(pActorElem);
        if (actorProto == ActorPrototype_None)
        {
            return NULL;
        }

        int logicNumber = 0;
        if (logic == "FloorSpike")
        {
            logicNumber = 0;
        }
        else if (logic == "FloorSpike2")
        {
            logicNumber = 1;
        }
        else if (logic == "FloorSpike3")
        {
            logicNumber = 2;
        }
        else if (logic == "FloorSpike4")
        {
            logicNumber = 3;
        }
        else
        {
            LOG("Offending floor spike: " + logic);
            assert(false && "Unknown floor spike");
        }

        const int defaultFullCycleTime = 3000; // TimeOn + TimeOff from configs
        const int defaultDelay = defaultFullCycleTime / 4 * logicNumber;

        const int startDelay = wwdObject->speed > 0 ? wwdObject->speed : defaultDelay;
        const int timeOn = wwdObject->speedX > 0 ? wwdObject->speedX : (defaultFullCycleTime / 2);
        const int timeOff = wwdObject->speedY > 0 ? wwdObject->speedY : (defaultFullCycleTime / 2);

        xmlOverrideList.push_back(XmlNodeOverride("Actor.FloorSpikeComponent.StartDelay", startDelay));
        xmlOverrideList.push_back(XmlNodeOverride("Actor.FloorSpikeComponent.TimeOn", timeOn));
        xmlOverrideList.push_back(XmlNodeOverride("Actor.FloorSpikeComponent.TimeOff", timeOff));

        return ActorTemplates::CreateXmlData_Actor(actorProto, xmlOverrideList);
    }
    else if (logic.find("SawBlade") != std::string::npos)
    {
        SAFE_DELETE(pActorElem);
        if (actorProto == ActorPrototype_None)
        {
            return NULL;
        }

        int delay = 0;
        if (logic == "SawBlade")
        {
            delay = 0;
        }
        else if (logic == "SawBlade2")
        {
            delay = 750;
        }
        else if (logic == "SawBlade3")
        {
            delay = 1500;
        }
        else if (logic == "SawBlade4")
        {
            delay = 2250;
        }
        else
        {
            LOG("Offending SawBlade: " + logic);
            assert(false && "Unknown floor SawBlade");
        }

        xmlOverrideList.push_back(XmlNodeOverride("Actor.SawBladeComponent.StartDelay", delay));

        return ActorTemplates::CreateXmlData_Actor(actorProto, xmlOverrideList);
    }
    else if (logic == "ConveyorBelt")
    {
        SAFE_DELETE(pActorElem);
        if (actorProto == ActorPrototype_None)
        {
            return NULL;
        }

        double speed = (double)wwdObject->speed * g_pApp->GetGlobalOptions()->platformSpeedModifier;

        xmlOverrideList.push_back(XmlNodeOverride("Actor.ConveyorBeltComponent.Speed", speed));

        return ActorTemplates::CreateXmlData_Actor(actorProto, xmlOverrideList);
    }
    else if (logic == "SkullCannon")
    {
        SAFE_DELETE(pActorElem);
        if (actorProto == ActorPrototype_None)
        {
            return NULL;
        }


        int projectileSpawnOffsetX = 32;
        if (bIsMirrored)
        {
            xmlOverrideList.push_back(XmlNodeOverride("Actor.ProjectileSpawnerComponent.ProjectileSpawnOffset", "x", projectileSpawnOffsetX * -1, "y", 10));
            xmlOverrideList.push_back(XmlNodeOverride("Actor.ProjectileSpawnerComponent.ProjectileDirection", EnumToString_Direction(Direction_Left)));
        }
        else
        {
            xmlOverrideList.push_back(XmlNodeOverride("Actor.ProjectileSpawnerComponent.ProjectileSpawnOffset", "x", projectileSpawnOffsetX, "y", 10));
            xmlOverrideList.push_back(XmlNodeOverride("Actor.ProjectileSpawnerComponent.ProjectileDirection", EnumToString_Direction(Direction_Right)));
        }

        return ActorTemplates::CreateXmlData_Actor(actorProto, xmlOverrideList);
    }
    else if (logic == "GooVent")
    {
        SAFE_DELETE(pActorElem);
        if (actorProto == ActorPrototype_None)
        {
            return NULL;
        }

        Point position(wwdObject->x, wwdObject->y);

        return ActorTemplates::CreateXmlData_Actor(actorProto, position);
    }
    else if (logic == "AniRope")
    {
        ActorPrototype proto = ActorPrototype_BaseRope;
        Point position(wwdObject->x, wwdObject->y);

        RopeDef def;
        def.timeToFlayBackAndForth = wwdObject->speedX;
        if (def.timeToFlayBackAndForth == 0)
        {
            def.timeToFlayBackAndForth = 1500;
        }
        assert(def.timeToFlayBackAndForth != 0);

        // Specified value in WWD is only the time to flay in 1 direction
        def.timeToFlayBackAndForth *= 2;

        SAFE_DELETE(pActorElem);

        return ActorTemplates::CreateXmlData_Rope(
            proto,
            position,
            tmpImageSet,
            def);
    }
    else if (logic.find("SteppingStone") != std::string::npos)
    {
        SAFE_DELETE(pActorElem);
        if (actorProto == ActorPrototype_None)
        {
            return NULL;
        }

        Point position(wwdObject->x, wwdObject->y);

        // I dont know where this is defined
        SteppingGroundDef def;
        def.timeOn = 150;
        def.timeOff = 500;

        return ActorTemplates::CreateXmlData_SteppingGround(
            actorProto,
            position,
            def);
    }
    else if (logic == "SpringBoard" ||
             logic == "GroundBlower" ||
             logic == "WaterRock")
    {
        SAFE_DELETE(pActorElem);
        if (actorProto == ActorPrototype_None)
        {
            return NULL;
        }

        Point position(wwdObject->x, wwdObject->y);

        SpringBoardDef def;
        def.springHeight = wwdObject->maxY;
        LOG("Position: " + position.ToString() + ", SpringHeight: " + ToStr(def.springHeight));
        if (def.springHeight == 0)
        {
            def.springHeight = 450;
        }

        // Everything should be in XML here
        return ActorTemplates::CreateXmlData_SpringBoard(
            actorProto,
            position,
            def);
    }
    else
    {
        static std::vector<std::string> s_ReportedUnknownLogicsList;

        bool isAlreadyReported = false;
        for (std::string &unkLogic : s_ReportedUnknownLogicsList)
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

    // If we have actor prototype defined and not processed, it is a failure on our side
    if (actorProto != ActorPrototype_None)
    {
        SAFE_DELETE(pActorElem);
        return NULL;
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
    //pClawActor->LinkEndChild(CreateCollisionComponent(40, 100));
    pClawActor->LinkEndChild(CreatePhysicsComponent(true, false, true, g_pApp->GetGlobalOptions()->maxJumpHeight, 40, 90, 4.0, 0.0, 0.5));
    pClawActor->LinkEndChild(CreateControllableComponent(true));
    pClawActor->LinkEndChild(CreateAnimationComponent("/CLAW/ANIS/*"));
    pClawActor->LinkEndChild(CreateActorRenderComponent("/CLAW/IMAGES/*", (int32) zIndexes::ClawActor));
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

inline TiXmlElement* CreateHUDElement(const std::string &pathToImages, int animFrameDuration, const std::string &animPath,
                 const Point &position, bool anchorRight, bool anchorBottom,
                 const std::string &key, bool visible = true)
{
    TiXmlElement* pHUDElement = new TiXmlElement("Actor");
    pHUDElement->SetAttribute("Type", pathToImages.c_str());
    pHUDElement->SetAttribute("resource", "created");

    pHUDElement->LinkEndChild(CreatePositionComponent(position.x, position.y));

    if (!animPath.empty())
    {
        pHUDElement->LinkEndChild(CreateAnimationComponent(animPath));
    }
    else if (animFrameDuration > 0)
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

struct HUDElementDef
{
    HUDElementDef()
    {
        isPositionProtortional = false;
        isAnchoredRight = false;
        isAnchoredBottom = false;
        isVisible = true;
    }

    std::string imageSet;
    AnimationDef animDef;
    bool isPositionProtortional;
    Point positionProportion;
    Point position;
    bool isAnchoredRight;
    bool isAnchoredBottom;
    bool isVisible;
    std::string HUDElemKey;
};

inline TiXmlElement* CreateHUDElement(const HUDElementDef& def)
{
    Point pos = def.position;
    if (def.isPositionProtortional)
    {
        Point windowSize = g_pApp->GetWindowSize();
        Point windowScale = g_pApp->GetScale();

        pos.Set(
            (windowSize.x * def.positionProportion.x) / windowScale.x, 
            (windowSize.y * def.positionProportion.y) / windowScale.y);
    }

    return CreateHUDElement(
        def.imageSet,
        def.animDef.cycleAnimationDuration,
        def.animDef.animationPath,
        pos,
        def.isAnchoredRight,
        def.isAnchoredBottom,
        def.HUDElemKey,
        def.isVisible);
}

#endif
