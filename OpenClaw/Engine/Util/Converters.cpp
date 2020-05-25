#include "Converters.h"
#include "../Events/Events.h"

void FixupWwdObject(WwdObject* pObj, int levelNumber);

TiXmlElement* WwdToXml(WapWwd* wapWwd, int levelNumber)
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
        WwdTileDescription& wwdTileDesc = wapWwd->tileDescriptions[tileDescIdx];

        TiXmlElement* tileDescElem = new TiXmlElement("TileDescription");
        tileDescRootElem->LinkEndChild(tileDescElem);

        XML_ADD_TEXT_ELEMENT("TileId", ToStr(tileDescIdx).c_str(), tileDescElem);
        XML_ADD_2_PARAM_ELEMENT("Size", "width", wwdTileDesc.width, "height", wwdTileDesc.height, tileDescElem);

        // This is Monolith's hack which I dont get
        if (levelNumber == 5 && tileDescIdx == 509)
        {
            wwdTileDesc.insideAttrib = WAP_TILE_ATTRIBUTE_CLEAR;
            wwdTileDesc.outsideAttrib = WAP_TILE_ATTRIBUTE_CLEAR;
        }
        // ???????????
        if (levelNumber == 11 && tileDescIdx == 39)
        {
            wwdTileDesc.insideAttrib = WAP_TILE_ATTRIBUTE_CLEAR;
            wwdTileDesc.outsideAttrib = WAP_TILE_ATTRIBUTE_CLEAR;
        }

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

    std::vector<std::string> notLoadedActorList;

    for (uint32 actorIdx = 0; actorIdx < actorsCount; actorIdx++)
    {
        WwdObject actorProperties = wwdActors[actorIdx];

        WwdObject* wwdObject = &actorProperties;
        FixupWwdObject(wwdObject, levelNumber);

        std::string name = actorProperties.name;
        std::string logic = actorProperties.logic;
        std::string imageSet = actorProperties.imageSet;
        std::string sound = actorProperties.sound;

        // Get image set of actor, e.g. /LEVEL1/IMAGES/SOLDIER/*.PID
        // TODO: This is code duplication. Fix it
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

        if (logic == "CursePowerup" || logic == "JumpSwitch")
        {
            continue;
        }

        // Here should be any objects that cant for some reason by created in that function
        if (logic.find("StackedCrates") != std::string::npos)
        {
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

                std::string crateImageSet = "/LEVEL" + ToStr(levelNumber) + "/IMAGES/CRATES/*";
                root->LinkEndChild(ActorTemplates::CreateXmlData_CrateActor(
                    crateImageSet,
                    Point(wwdObject->x, wwdObject->y + positionOffset),
                    loot,
                    5, wwdObject->z + crateIdx));
            }
        }
        else if (levelNumber == 3 && sound == "LEVEL_AMBIENT_ANVIL")
        {
            continue;
        }
        else if (logic == "BreakPlank")
        {
            Point position = Point(wwdObject->x, wwdObject->y);
            ActorPrototype proto = ActorPrototype_None;

            switch (levelNumber)
            {
                case 5: proto = ActorPrototype_Level5_CrumblingPeg; break;
                case 11: proto = ActorPrototype_Level11_BreakPlank; break;
                case 12: proto = ActorPrototype_Level12_CrumblingPeg; break;
                default: notLoadedActorList.push_back(actorProperties.logic); continue;
            }

            // Temporary hack
            assert(
                proto == ActorPrototype_Level5_CrumblingPeg 
                || proto == ActorPrototype_Level11_BreakPlank
                || proto == ActorPrototype_Level12_CrumblingPeg
            );

            int crumbleDelay = wwdObject->counter;
            int width = wwdObject->width;

            for (int pegIdx = 0; pegIdx < width; pegIdx++)
            {
                root->LinkEndChild(
                    ActorTemplates::CreateXmlData_CrumblingPeg(proto, position, tmpImageSet, crumbleDelay));

                position.x += 64;
            }
        }
        else if (logic == "Laser")
        {
            // TODO: missing logic
            continue;
        }
        else if (logic == "AquatisCrack")
        {
            // TODO: missing logic
            continue;
        }
        else if (levelNumber == 12 && (logic == "AquatisDynamite" || logic == "Tentacle"))
        {
            continue;
        }
        else
        {
            if (TiXmlElement* pActorElem = WwdObjectToXml(&actorProperties, imagesRootPath, levelNumber))
            {
                root->LinkEndChild(pActorElem);
            }
            else
            {
                notLoadedActorList.push_back(actorProperties.logic);
            }
        }
    }

    if (notLoadedActorList.size() > 0)
    {
        // sort | uniq
        std::sort(notLoadedActorList.begin(), notLoadedActorList.end());
        notLoadedActorList.erase(unique(notLoadedActorList.begin(), notLoadedActorList.end()), notLoadedActorList.end());

        LOG_ERROR("Failed to load certain actors:");
        for (const std::string& actorLogic : notLoadedActorList)
        {
            LOG_ERROR("NOT LOADED: " + actorLogic);
        }

        FAIL("Failed to load level " + ToStr(levelNumber) + " due to unimplemented actor prototypes");
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

    // Boss Bar
    HUDElementDef def;
    def.imageSet = "/GAME/IMAGES/BOSSBAR/*";
    def.isPositionProtortional = true;
    def.positionProportion = Point(0.5, 0.8);
    def.isAnchoredRight = false;
    def.isAnchoredBottom = false;
    def.HUDElemKey = "bossbar";
    def.isVisible = false;

    root->LinkEndChild(CreateHUDElement(def));

    //xmlDoc.Print();


    //xmlDoc.SaveFile("LEVEL1.xml");
    return root;
}

void FixupWwdObject(WwdObject* pObj, int levelNumber)
{
    std::string logic = pObj->logic;
    std::string image = pObj->imageSet;
}
