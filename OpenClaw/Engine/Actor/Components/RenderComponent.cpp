#include "RenderComponent.h"
#include "../../GameApp/BaseGameApp.h"
#include "../../GameApp/BaseGameLogic.h"
#include "../../Graphics2D/Image.h"

#include "../../Resource/Loaders/PidLoader.h"

#include "PositionComponent.h"

#include "../../Scene/ActorSceneNode.h"
#include "../../Scene/TilePlaneSceneNode.h"
#include "../../Scene/HUDSceneNode.h"

#include "../../Events/Events.h"
#include "../Actor.h"

#include <algorithm>
#include <cctype>

const char* ActorRenderComponent::g_Name = "ActorRenderComponent";
const char* TilePlaneRenderComponent::g_Name = "TilePlaneRenderComponent";
const char* HUDRenderComponent::g_Name = "HUDRenderComponent";

//=================================================================================================
// BaseRenderComponent Implementation
//
//=================================================================================================

bool BaseRenderComponent::VInit(TiXmlElement* pXmlData)
{
    assert(pXmlData != NULL);

    WapPal* palette = g_pApp->GetCurrentPalette();

    for (TiXmlElement* pImagePathElem = pXmlData->FirstChildElement("ImagePath");
        pImagePathElem; pImagePathElem = pImagePathElem->NextSiblingElement("ImagePath"))
    {
        if (palette == NULL)
        {
            LOG_ERROR("Attempting to create BaseRenderComponent without existing palette");
            return false;
        }

        const char* imagesPath = pImagePathElem->GetText();
        assert(imagesPath != NULL);

        // Get all files residing in given directory
        // !!! THIS ASSUMES THAT WE ONLY WANT IMAGES FROM THIS DIRECTORY. IT IGNORES ALL NESTED DIRECTORIES !!!
        // Maybe add recursive algo to libwap
        std::string imageDir = std::string(imagesPath);
        //imageDir = imageDir.substr(0, imageDir.find("*")); // Get rid of everything after '*' including '*'
        imageDir = imageDir.substr(0, imageDir.find_last_of("/")); // Get rid of filenames - get just path to the final directory
        std::vector<std::string> matchingPathNames =
            g_pApp->GetResourceCache()->GetAllFilesInDirectory(imageDir.c_str());

        // Remove all images which dont conform to the given pattern
        // This affects probably only object with "DoNothing" logic
        // Compute everything in lowercase to assure compatibility with everything in the engine
        std::string imageDirLowercase(imagesPath);
        std::transform(imageDirLowercase.begin(), imageDirLowercase.end(), imageDirLowercase.begin(), (int(*)(int)) std::tolower);
        //LOG("ImageDir: " + imageDir);
        for (auto iter = matchingPathNames.begin(); iter != matchingPathNames.end(); /*++iter*/)
        {
            if (!WildcardMatch(imageDirLowercase.c_str(), (*iter).c_str()))
            {
                iter = matchingPathNames.erase(iter);
            }
            else
            {
                iter++;
            }
        }

        for (std::string& imagePath : matchingPathNames)
        {
            // Only load known image formats
            if (!WildcardMatch("*.pid", imagePath.c_str()))
            {
                continue;
            }

            shared_ptr<Image> image = PidResourceLoader::LoadAndReturnImage(imagePath.c_str(), palette);
            if (!image)
            {
                LOG_WARNING("Failed to load image: " + imagePath);
                return false;
            }

            std::string imageNameKey = StripPathAndExtension(imagePath);

            // Check if we dont already have the image loaded
            if (m_ImageMap.count(imageNameKey) > 0)
            {
                LOG_WARNING("Trying to load existing image: " + imagePath);
                continue;
            }

            // HACK: all animation frames should be in format frameXXX
            /*if (imageNameKey.find("chest") != std::string::npos)
            {
                imageNameKey.replace(0, 5, "frame");
            }
            // HACK: all animation frames should be in format frameXXX (length = 8)
            if (imageNameKey.find("frame") != std::string::npos && imageNameKey.length() != 8)
            {
                int imageNameNumStr = std::stoi(std::string(imageNameKey).erase(0, 5));
                imageNameKey = "frame" + Util::ConvertToThreeDigitsString(imageNameNumStr);
            }*/
            // Just reconstruct it...
            if (imageNameKey.length() > 3 /* Hack for checkpointflag */ || 
                std::string(pXmlData->Parent()->ToElement()->Attribute("Type")) == "GAME_CHECKPOINTFLAG")
            {
                std::string tmp = imageNameKey;
                tmp.erase(std::remove_if(tmp.begin(), tmp.end(), (int(*)(int))std::isalpha), tmp.end());
                if (!tmp.empty())
                {
                    int imageNum = std::stoi(tmp);
                    imageNameKey = "frame" + Util::ConvertToThreeDigitsString(imageNum);
                }
                else
                {
                    //LOG(imagePath);
                }
            }

            m_ImageMap.insert(std::make_pair(imageNameKey, image));
        }
    }

    if (m_ImageMap.empty())
    {
        LOG_WARNING("Image map for render component is empty. Actor type: " + std::string(pXmlData->Parent()->ToElement()->Attribute("Type")));
    }

    /*for (auto it : m_ImageMap)
    {
        LOG(it.first);
    }*/

    return VDelegateInit(pXmlData);
}

TiXmlElement* BaseRenderComponent::VGenerateXml()
{
    return NULL;
}

void BaseRenderComponent::VPostInit()
{
    shared_ptr<SceneNode> pNode = GetSceneNode();
    if (pNode)
    {
        shared_ptr<EventData_New_Render_Component> pEvent(new EventData_New_Render_Component(m_pOwner->GetGUID(), pNode));
        IEventMgr::Get()->VTriggerEvent(pEvent);
    }

    m_pPositionComponent = m_pOwner->GetPositionComponent().get();
    assert(m_pPositionComponent != NULL);
}

void BaseRenderComponent::VOnChanged()
{

}

shared_ptr<SceneNode> BaseRenderComponent::GetSceneNode()
{
    if (!m_pSceneNode)
    {
        m_pSceneNode = VCreateSceneNode();
    }

    return m_pSceneNode;
}

weak_ptr<Image> BaseRenderComponent::GetImage(std::string imageName)
{
    if (m_ImageMap.count(imageName) > 0)
    {
        return m_ImageMap[imageName];
    }

    return weak_ptr<Image>();
}

weak_ptr<Image> BaseRenderComponent::GetImage(uint32 imageId)
{
    return weak_ptr<Image>();
}

bool BaseRenderComponent::HasImage(std::string imageName)
{
    if (m_ImageMap.count(imageName) > 0)
    {
        return true;
    }

    return false;
}

bool BaseRenderComponent::HasImage(int32 imageId)
{
    return false;
}

//=================================================================================================
// [ActorComponent::BaseRenderComponent::ActorRenderComponent]
// 
//      ActorRenderComponent Implementation
//
//=================================================================================================

ActorRenderComponent::ActorRenderComponent()
    :
    m_Alpha(255)
{
    // Everything is visible by default, should be explicitly stated that its not visible
    m_IsVisible = true;
    m_IsMirrored = false;
    m_IsInverted = false;
    m_ZCoord = 0;
    m_ColorMod.r = m_ColorMod.g = m_ColorMod.b = m_ColorMod.a = 255;
}

bool ActorRenderComponent::VDelegateInit(TiXmlElement* pXmlData)
{
    if (TiXmlElement* pElem = pXmlData->FirstChildElement("Visible"))
    {
        if (std::string(pElem->GetText()) == "true")
        {
            m_IsVisible = true;
        }
        else
        {
            m_IsVisible = false;
        }
    }
    if (pXmlData->FirstChildElement("Mirrored") &&
        std::string(pXmlData->FirstChildElement("Mirrored")->GetText()) == "true")
    {
        m_IsMirrored = true;
    }
    if (pXmlData->FirstChildElement("Inverted") &&
        std::string(pXmlData->FirstChildElement("Inverted")->GetText()) == "true")
    {
        m_IsInverted = true;
    }
    if (TiXmlElement* pElem = pXmlData->FirstChildElement("ZCoord"))
    {
        m_ZCoord = std::stoi(pElem->GetText());
    }

    if (!m_IsVisible)
    {
        if (!m_ImageMap.empty())
        {
            m_CachedImage = m_ImageMap.begin()->second;
            m_IsCachedImageExpired = false;
        }
        else
        {
            LOG_WARNING("Invisible actor has no dummy image !");
        }
    }

    if (m_IsVisible)
    {
        if (m_ImageMap.empty())
        {
            LOG_WARNING("Creating actor render component without valid image.");
            return true;
        }
        m_CachedImage = m_ImageMap.begin()->second;
        m_IsCachedImageExpired = false;
    }

    // Hacky. Rebuild image names which dont have "frame" in it after initialization
    if (m_ImageMap.begin()->first.find("frame") == std::string::npos)
    {
        ImageMap normalizedImageMap;
        int currImageIdx = 1;
        for (auto& it : m_ImageMap)
        {
            std::string imageKey = "frame" + Util::ConvertToThreeDigitsString(currImageIdx);
            normalizedImageMap.insert(std::make_pair(imageKey, it.second));
            currImageIdx++;
        }
        m_ImageMap.clear();
        m_ImageMap = normalizedImageMap;
    }

    //LOG("---------------------");

    return true;
}

SDL_Rect ActorRenderComponent::VGetPositionRect()
{
    SDL_Rect positionRect = { 0 };

    if (!m_IsVisible)
    {
        return positionRect;
    }

    shared_ptr<Image> currentImage = GetCurrentImage().lock();
    if (!currentImage)
    {
        return positionRect;
    }

    Point position = m_pPositionComponent->GetPosition();

    positionRect = {
            (int)position.GetX() - currentImage->GetWidth() / 2 + currentImage->GetOffsetX(),
            (int)position.GetY() - currentImage->GetHeight() / 2 + currentImage->GetOffsetY(),
            currentImage->GetWidth(),
            currentImage->GetHeight()
    };

    return positionRect;
}

shared_ptr<SceneNode> ActorRenderComponent::VCreateSceneNode()
{
    shared_ptr<PositionComponent> pPositionComponent = m_pOwner->GetPositionComponent();
    if (!pPositionComponent)
    {
        // can't render without a transform
        return shared_ptr<SceneNode>();
    }

    Point pos(pPositionComponent->GetX(), pPositionComponent->GetY());
    shared_ptr<SceneNode> pActorNode(new SDL2ActorSceneNode(m_pOwner->GetGUID(), this, RenderPass_Actor, pos, m_ZCoord));

    return pActorNode;
}

void ActorRenderComponent::VCreateInheritedXmlElements(TiXmlElement* pBaseElement)
{

}

void ActorRenderComponent::SetImage(std::string imageName) {
    m_CurrentImageName.assign(std::move(imageName));
    m_IsCachedImageExpired = true;
}

void ActorRenderComponent::UpdateCurrentImage() {
    // Hack.. only 2, 3, 4
    /*if (m_pOwner->GetName() == "LEVEL_TORCH2")
    {
        imageName = imageName.substr(7);
        int num = std::stoi(imageName) + 1;
        imageName = ToStr(num);
    }
    else if (m_pOwner->GetName() == "Level4_SpringBoard")
    {
        imageName = imageName.substr(5);
    }*/

    // Hacks
    /*if (m_pOwner->GetName() == "StaticImage")
    {
        LOG("Map:");
        for (auto& it : m_ImageMap)
        {
            LOG("Image: " + it.first);
        }
    }
    else */if (m_pOwner->GetName() == "Level6_GroundBlower")
    {
        SetAlpha(255);
    }

    m_IsCachedImageExpired = false;
    const auto image = m_ImageMap.find(m_CurrentImageName);
    if (m_ImageMap.end() != image)
    {
        m_CachedImage = image->second;
    }
    else
    {
        // Known... Treasure chest HUD
        if (m_CurrentImageName == "frame000")
        {
            return;
        }
        else if (m_pOwner->GetName() == "Level6_GroundBlower")
        {
            // This actor is missing a frame with empty image
            SetAlpha(0);
            return;
        }

        LOG("Actor: " + m_pOwner->GetName() + " ImageName: " + m_CurrentImageName);
        /*LOG("Images: ");
        for (auto im : m_ImageMap)
        {
            LOG("Image: " + im.first);
        }*/
        return;
        assert(false);

        // Try 01, 02, 03...
        std::string newImageName = m_CurrentImageName.substr(6);
        if (m_ImageMap.count(newImageName) > 0)
        {
            LOG("Setting: " + newImageName);
            m_CachedImage = m_ImageMap[newImageName];
            return;
        }

        newImageName = m_CurrentImageName.substr(5);
        if (m_ImageMap.count(newImageName) > 0)
        {
            LOG("Setting: " + newImageName);
            m_CachedImage = m_ImageMap[newImageName];
            return;
        }

        LOG("NewImageName: " + newImageName);
        LOG_ERROR("Trying to set nonexistant image: " + m_CurrentImageName + " to render component of actor: " +
            m_pOwner->GetName());

        /*LOG("Actor has following images: ");
        for (auto iter : m_ImageMap)
        {
            LOG("Image: " + iter.first);
        }*/
    }
}

weak_ptr<Image> ActorRenderComponent::GetCurrentImage() {
    if (m_IsCachedImageExpired) {
        UpdateCurrentImage();
    }
    return m_CachedImage;
}

//=================================================================================================
// [ActorComponent::BaseRenderComponent::TilePlaneRenderComponent]
// 
//      TilePlaneRenderComponent Implementation
//
//=================================================================================================

void TilePlaneRenderComponent::ProcessMainPlaneTiles(const TileList& tileList)
{
    const int tilesCount = tileList.size();
    int currentTileIdx = 0;
    
    while (currentTileIdx < tilesCount)
    {
        TileList continuousTiles = GetAllContinuousTiles(tileList, currentTileIdx);
        assert(continuousTiles.size() >= 1);

        // Discard all empty tiles
        if (continuousTiles[0] > -1)
        {
            int tileWidth = m_PlaneProperties.tilePixelWidth;
            TileInfo firstTileInfo = GetTileInfo(tileList, currentTileIdx);
            int currentX = firstTileInfo.x;
            int currentY = firstTileInfo.y;

            shared_ptr<EventData_Collideable_Tile_Created> pEvent(
                new EventData_Collideable_Tile_Created(firstTileInfo.tileId, currentX, currentY, continuousTiles.size()));
            IEventMgr::Get()->VTriggerEvent(pEvent);

            /*for (int tileId : continuousTiles)
            {
                shared_ptr<EventData_Collideable_Tile_Created> pEvent(
                    new EventData_Collideable_Tile_Created(tileId, currentX, currentY, 1));
                IEventMgr::Get()->VTriggerEvent(pEvent);

                currentX += tileWidth;
            }*/
        }

        currentTileIdx += continuousTiles.size();
    }
}

TileList TilePlaneRenderComponent::GetAllContinuousTiles(const TileList& tileList, int fromTileIdx)
{
    const int tilesCount = tileList.size();

    TileList continuousTileList;

    // First tile always belongs in the list
    TileInfo currentTile = GetTileInfo(tileList, fromTileIdx);
    continuousTileList.push_back(currentTile.tileId);

    // We already have the first tile, check next one
    int currentTileIdx = fromTileIdx + 1;

    // Continue adding tiles as long as its ID and Y position matches the first tile
    while (true)
    {
        // Keep track of the idx so we do not step out of bounds
        if (currentTileIdx >= tilesCount)
        {
            break;
        }

        TileInfo nextTile = GetTileInfo(tileList, currentTileIdx);
        if ((nextTile.tileId == currentTile.tileId) && (nextTile.y == currentTile.y))
        {
            continuousTileList.push_back(nextTile.tileId);
            currentTile = nextTile;
            currentTileIdx++;
        }
        else
        {
            break;
        }
    }

    return continuousTileList;
}

TileInfo TilePlaneRenderComponent::GetTileInfo(const TileList& tileList, int tileIdx)
{
    assert(tileIdx < (int)tileList.size());

    TileInfo tileInfo;

    tileInfo.tileId = tileList[tileIdx];
    tileInfo.x = (tileIdx * m_PlaneProperties.tilePixelWidth) % m_PlaneProperties.planePixelWidth;
    tileInfo.y = ((tileIdx / m_PlaneProperties.tilesOnAxisX) * m_PlaneProperties.tilePixelHeight) % m_PlaneProperties.planePixelHeight;

    return tileInfo;
}

bool TilePlaneRenderComponent::VDelegateInit(TiXmlElement* pXmlData)
{
    TiXmlElement* pPlaneProperties = pXmlData->FirstChildElement("PlaneProperties");
    if (!pPlaneProperties)
    {
        LOG_ERROR("Could not locate PlaneProperties.");
        return false;
    }

    // If anyone knows a better way, tell me
    //-------------------------------------------------------------------------
    // Fill plane properties
    //-------------------------------------------------------------------------
    if (TiXmlElement* node = pPlaneProperties->FirstChildElement("PlaneName"))
    {
        m_PlaneProperties.name = node->GetText();
    }
    if (TiXmlElement* node = pPlaneProperties->FirstChildElement("MainPlane"))
    {
        m_PlaneProperties.isMainPlane = std::string(node->GetText()) == "true";
    }
    if (TiXmlElement* node = pPlaneProperties->FirstChildElement("NoDraw"))
    {
        m_PlaneProperties.isDrawable = std::string(node->GetText()) == "true";
    }
    if (TiXmlElement* node = pPlaneProperties->FirstChildElement("WrappedX"))
    {
        m_PlaneProperties.isWrappedX = std::string(node->GetText()) == "true";
    }
    if (TiXmlElement* node = pPlaneProperties->FirstChildElement("WrappedY"))
    {
        m_PlaneProperties.isWrappedY = std::string(node->GetText()) == "true";
    }
    if (TiXmlElement* node = pPlaneProperties->FirstChildElement("TileAutoSized"))
    {
        m_PlaneProperties.isTileAutosized = std::string(node->GetText()) == "true";
    }
    if (TiXmlElement* node = pPlaneProperties->FirstChildElement("TilePixelSize"))
    {
        node->Attribute("width", &m_PlaneProperties.tilePixelWidth);
        node->Attribute("height", &m_PlaneProperties.tilePixelHeight);
    }
    if (TiXmlElement* node = pPlaneProperties->FirstChildElement("PlanePixelSize"))
    {
        node->Attribute("width", &m_PlaneProperties.planePixelWidth);
        node->Attribute("height", &m_PlaneProperties.planePixelHeight);
    }
    if (TiXmlElement* node = pPlaneProperties->FirstChildElement("MoveSpeedPercentage"))
    {
        node->Attribute("x", &m_PlaneProperties.movementPercentX);
        node->Attribute("y", &m_PlaneProperties.movementPercentY);
    }
    if (TiXmlElement* node = pPlaneProperties->FirstChildElement("FillColor"))
    {
        m_PlaneProperties.fillColor = std::stoi(node->GetText());
    }
    if (TiXmlElement* node = pPlaneProperties->FirstChildElement("ZCoord"))
    {
        m_PlaneProperties.zCoord = std::stoi(node->GetText());
    }
    
    m_PlaneProperties.tilesOnAxisX = m_PlaneProperties.planePixelWidth / m_PlaneProperties.tilePixelWidth;
    m_PlaneProperties.tilesOnAxisY = m_PlaneProperties.planePixelHeight / m_PlaneProperties.tilePixelHeight;

    // Filler tile - for the background plane

    WapPal* pPallette = g_pApp->GetCurrentPalette();
    assert(m_PlaneProperties.fillColor >= 0 && m_PlaneProperties.fillColor <= 255);
    WAP_ColorRGBA wapColor = pPallette->colors[m_PlaneProperties.fillColor];
    SDL_Color fillColor = { wapColor.r, wapColor.g, wapColor.b, wapColor.a };

    m_pFillImage.reset(Image::CreateImageFromColor(
        fillColor,
        m_PlaneProperties.tilePixelWidth,
        m_PlaneProperties.tilePixelHeight,
        g_pApp->GetRenderer()));
    assert(m_pFillImage != nullptr);

    //-------------------------------------------------------------------------
    // Fill plane tiles
    //-------------------------------------------------------------------------

    TiXmlElement* pTileElements = pXmlData->FirstChildElement("Tiles");
    if (!pTileElements)
    {
        LOG_ERROR("Tiles are missing.");
        return false;
    }
    PROFILE_CPU("PLANE CREATION");
    int32 tileIdx = 0;

    TileList tileList;
    for (TiXmlElement* pTileNode = pTileElements->FirstChildElement(); 
        pTileNode != NULL; 
        pTileNode = pTileNode->NextSiblingElement())
    {
        std::string tileFileName(pTileNode->GetText());

        // Temporarily keep track of the tiles stored
        int32 tileId = std::stoi(tileFileName);
        tileList.push_back(tileId);

        // Convert to three digits, e.g. "2" -> "002" or "15" -> "015"
        if (tileFileName.length() == 1) 
        { 
            tileFileName = "00" + tileFileName; 
        }
        else if (tileFileName.length() == 2 &&
            !(g_pApp->GetGameLogic()->GetCurrentLevelData()->GetLevelNumber() == 1 && tileFileName == "74")) 
        { 
            tileFileName = "0" + tileFileName; 
        }

        auto findIt = m_ImageMap.find(tileFileName);
        if (findIt != m_ImageMap.end())
        {
            m_TileImageList.push_back(findIt->second.get());
        }
        else if (tileFileName == "0-1" || tileFileName == "-1")
        {
            m_TileImageList.push_back(NULL);
        }
        else if (m_PlaneProperties.name == "Background") // Use fill color, only aplicable to background
        {
            assert(m_pFillImage != nullptr);

            m_TileImageList.push_back(m_pFillImage.get());
        }
        else if (m_PlaneProperties.name == "Front") // Empty image on front plane most likely. First occurance on level 7
        {
            m_TileImageList.push_back(NULL);
        }
        else if (m_PlaneProperties.name == "Action") // Fill image ?. First occurance on level 8
        {
            m_TileImageList.push_back(m_pFillImage.get());
        }
        else
        {
            LOG_ERROR("Could not find plane tile: " + tileFileName + " on plane: " + m_PlaneProperties.name);
            return false;
        }

        tileIdx++;
    }

    if (m_PlaneProperties.isMainPlane)
    {
        ProcessMainPlaneTiles(tileList);
    }

    if (m_TileImageList.empty())
    {
        LOG_ERROR("No tiles on plane were loaded.");
        return false;
    }

    int planeWidth = m_PlaneProperties.planePixelWidth;
    int planeHeight = m_PlaneProperties.planePixelHeight * 2;
    if (m_PlaneProperties.isWrappedX)
    {
        planeWidth = INT32_MAX;
    }
    if (m_PlaneProperties.isWrappedY)
    {
        planeHeight = INT32_MAX;
    }
    
    m_PositionRect = { 0, 0, planeWidth, planeHeight };

    return true;
}

SDL_Rect TilePlaneRenderComponent::VGetPositionRect()
{
    return m_PositionRect;
}

shared_ptr<SceneNode> TilePlaneRenderComponent::VCreateSceneNode()
{
    shared_ptr<PositionComponent> pPositionComponent = m_pOwner->GetPositionComponent();
    if (!pPositionComponent)
    {
        // can't render without a transform
        return shared_ptr<SceneNode>();
    }

    RenderPass renderPass;
    if (m_PlaneProperties.name == "Background")
    {
        renderPass = RenderPass_Background;
    }
    else if (m_PlaneProperties.name == "Action")
    {
        renderPass = RenderPass_Action;
    }
    else if (m_PlaneProperties.name == "Front")
    {
        renderPass = RenderPass_Foreground;
    }
    else
    {
        LOG_ERROR("Unknown plane name: " + m_PlaneProperties.name + " - cannot assign corrent render pass");
        return shared_ptr<SceneNode>();
    }

    Point pos(pPositionComponent->GetX(), pPositionComponent->GetY());
    shared_ptr<SceneNode> pTilePlaneNode(new SDL2TilePlaneSceneNode(m_pOwner->GetGUID(), this, renderPass, pos));

    return pTilePlaneNode;
}

void TilePlaneRenderComponent::VCreateInheritedXmlElements(TiXmlElement* pBaseElement)
{

}

//=================================================================================================
// [ActorComponent::BaseRenderComponent::ActorRenderComponent::HUDRenderComponent]
// 
//      HUDRenderComponent Implementation
//
//=================================================================================================

HUDRenderComponent::HUDRenderComponent()
    :
    m_IsAnchoredRight(false),
    m_IsAnchoredBottom(false)
{ }

bool HUDRenderComponent::VDelegateInit(TiXmlElement* pXmlData)
{
    if (!ActorRenderComponent::VDelegateInit(pXmlData))
    {
        return false;
    }

    if (TiXmlElement* pElem = pXmlData->FirstChildElement("AnchorRight"))
    {
        m_IsAnchoredRight = std::string(pElem->GetText()) == "true";
    }
    if (TiXmlElement* pElem = pXmlData->FirstChildElement("AnchorBottom"))
    {
        m_IsAnchoredBottom = std::string(pElem->GetText()) == "true";
    }
    if (TiXmlElement* pElem = pXmlData->FirstChildElement("HUDElementKey"))
    {
        m_HUDElementKey = pElem->GetText();
    }

    return true;
}

SDL_Rect HUDRenderComponent::VGetPositionRect()
{
    // HACK: Always visible
    return { 0, 0, 1000000, 1000000 };
}

shared_ptr<SceneNode> HUDRenderComponent::VCreateSceneNode()
{
    shared_ptr<PositionComponent> pPositionComponent = m_pOwner->GetPositionComponent();
    if (!pPositionComponent)
    {
        // can't render without a transform
        return shared_ptr<SceneNode>();
    }

    Point pos(pPositionComponent->GetX(), pPositionComponent->GetY());
    shared_ptr<SDL2HUDSceneNode> pHUDNode(new SDL2HUDSceneNode(m_pOwner->GetGUID(), this, RenderPass_HUD, pos, IsVisible()));

    shared_ptr<EventData_New_HUD_Element> pEvent(new EventData_New_HUD_Element(m_pOwner->GetGUID(), m_HUDElementKey, pHUDNode));
    IEventMgr::Get()->VTriggerEvent(pEvent);

    return pHUDNode;
}

void HUDRenderComponent::VCreateInheritedXmlElements(TiXmlElement* pBaseElement)
{

}
