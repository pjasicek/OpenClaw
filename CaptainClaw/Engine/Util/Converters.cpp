#include "Converters.h"

TiXmlElement* WwdToXml(WapWwd* wapWwd)
{
    PROFILE_CPU("WWD->XML");
    TiXmlDocument xmlDoc;

    //----- [Level]
    TiXmlElement* root = new TiXmlElement("Level");
    //xmlDoc.LinkEndChild(root);

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
    // Level 2 does not have /LEVEL2/TILES but only LEVEL2/TILES
    if (tileRootDirPath[0] != '/')
    {
        tileRootDirPath.insert(0, "/");
    }

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

        // Here should be any objects that cant for some reason by created in that function
        if (logic.find("StackedCrates") != std::string::npos)
        {
            WwdObject* wwdObject = &actorProperties;
            if (wwdObject->height == 0)
            {
                wwdObject->height = 1;
            }
            assert(wwdObject->height > 0 && wwdObject->height <= 8 && "Invalid stacked crate height. Should be between 1 and 8");

            for (int crateIdx = 0; crateIdx < wwdObject->height; crateIdx++)
            {
                std::vector<PickupType> loot;
                // Is there a better way ?
                if (crateIdx == 0) { loot.push_back(PickupType(wwdObject->userRect1.left)); }
                else if (crateIdx == 1) { loot.push_back(PickupType(wwdObject->userRect1.top)); }
                else if (crateIdx == 2) { loot.push_back(PickupType(wwdObject->userRect1.right)); }
                else if (crateIdx == 3) { loot.push_back(PickupType(wwdObject->userRect1.bottom)); }
                else if (crateIdx == 4) { loot.push_back(PickupType(wwdObject->userRect1.left)); }
                else if (crateIdx == 5) { loot.push_back(PickupType(wwdObject->userRect1.top)); }
                else if (crateIdx == 6) { loot.push_back(PickupType(wwdObject->userRect1.right)); }
                else if (crateIdx == 7) { loot.push_back(PickupType(wwdObject->userRect1.bottom)); }

                int positionOffset = -(crateIdx * 60);

                root->LinkEndChild(ActorTemplates::CreateXmlData_CrateActor(
                    "/LEVEL1/IMAGES/CRATES/*",
                    Point(wwdObject->x, wwdObject->y + positionOffset),
                    loot,
                    5, wwdObject->z + crateIdx));
            }
        }
        else
        {
            root->LinkEndChild(WwdObjectToXml(&actorProperties, imagesRootPath));
        }

        continue;
#if 0
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

        // TODO: Use stuff from ActorTemplates.cpp
        if (logic == "TreasurePowerup" || logic == "GlitterlessPowerup")
        {
            //----- [Level::Actors::ActorProperties::TreasurePickupComponent]
            TiXmlElement* treasurePickupComponent = new TiXmlElement("TreasurePickupComponent");
            actorPropertiesElem->LinkEndChild(treasurePickupComponent);

            assert(imageSet.find("TREASURE") != std::string::npos);

            int32 points = 0;
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

            XML_ADD_TEXT_ELEMENT("Points", ToStr(points).c_str(), treasurePickupComponent);
            XML_ADD_TEXT_ELEMENT("PickupSound", pickupSound.c_str(), treasurePickupComponent);
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
#endif
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


    //xmlDoc.SaveFile("LEVEL1.xml");
    return root;
}