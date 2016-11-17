#include <assert.h>
#include <algorithm>

#include "Plane.h"
#include "../Util/Util.h"
#include "../Image.h"
#include "../GameApplication.h"

#include "../Engine/Resource/Loaders/PidLoader.h"

#include <iostream>
using namespace std;

Plane::Plane(Level* level, WwdPlane* wwdPlane, uint32_t planeId, std::string& tileBaseDirectoryPath, SDL_Renderer* renderer)
{
    assert(level != NULL);
    assert(wwdPlane != NULL);
    assert(renderer != NULL);

    //------ Plane properties

    _planeId = planeId;
    
    _flags = wwdPlane->properties.flags;
    _name = std::string(wwdPlane->properties.name);

    _tilePixelWidth = wwdPlane->properties.tilePixelWidth;
    _tilePixelHeight = wwdPlane->properties.tilePixelHeight;

    _planePixelWidth = wwdPlane->properties.pixelWidth;
    _planePixelHeight = wwdPlane->properties.pixelHeight;

    _tilesOnAxisX = wwdPlane->properties.tilesOnAxisX;
    _tilesOnAxisY = wwdPlane->properties.tilesOnAxisY;
    _totalTiles = _tilesOnAxisX * _tilesOnAxisY;

    _movementPercentX = wwdPlane->properties.movementPercentX;
    _movementPercentY = wwdPlane->properties.movementPercentY;

    _fillColor = wwdPlane->properties.fillColor;

    _coordZ = wwdPlane->properties.coordZ;

    //------- Tiles
    
    uint32_t tilesCount = wwdPlane->tilesCount;
    _tiles.reserve(tilesCount);
    _tileTextures.reserve(tilesCount);

    // Load tile textures
    for (uint32_t tileIdx = 0; tileIdx < tilesCount; tileIdx++)
    {
        _tiles.push_back(wwdPlane->tiles[tileIdx]);
    }

    //-------- Load tile textures

    std::vector<int32_t> uniqueTilesVector = _tiles;
    std::sort(uniqueTilesVector.begin(), uniqueTilesVector.end());
    uniqueTilesVector.erase(std::unique(uniqueTilesVector.begin(), uniqueTilesVector.end()), uniqueTilesVector.end());

    // Plane's tile subfolder name is expected to be first entry in wwdPlane->imageSets[0]
    // So for highest flexibility use that
    assert(wwdPlane->imageSetsCount > 0);
    std::string pathToTileDirectory = tileBaseDirectoryPath + "/" + std::string(wwdPlane->imageSets[0]);

    WapPal* palette = level->GetPalette();
    assert(palette != NULL);

    for (int32_t tileId : uniqueTilesVector)
    {
        // Tiles with id == -1 have no textures, these are empty clear spaces
        if (tileId < 0)
        {
            continue;
        }

        std::string tileIdStr = Util::ConvertToThreeDigitsString(tileId);
        std::string tileFileName = tileIdStr + ".PID";
        // This is hack, monolith made some mistake in lvl1
        if ((level->GetLevelNumber() == 1) && (tileId == 74))
        {
            tileFileName.erase(0, 1);
        }

        
        std::string tileFullFilePath = pathToTileDirectory + "/" + tileFileName;
        shared_ptr<Image> tileImage = PidResourceLoader::LoadAndReturnImage(tileFullFilePath.c_str(), palette);
        assert(tileImage != nullptr);

        _tileImageMap.insert(std::pair<int32_t, shared_ptr<Image>>(tileId, tileImage));
    }

    //--------- Make texture map
    // We dont need any other information about general tile but its texture

    for (int32_t tileId : _tiles)
    {
        // Not all tiles need to have a texture - they can be empty
        SDL_Texture* tileTexture = NULL;
        if (_tileImageMap.count(tileId) == 0)
        {
            tileTexture = NULL;
        }
        else
        {
            tileTexture = _tileImageMap[tileId]->GetTexture();
        }

        _tileTextures.push_back(tileTexture);
    }

    //-------- Objects
}

Plane::~Plane()
{

}

void Plane::Render(SDL_Renderer* renderer, Camera* camera)
{
    RenderTiles(renderer, camera);
    RenderObjects(renderer, camera);
}

void Plane::RenderTiles(SDL_Renderer* renderer, Camera* camera)
{
    float scaleX, scaleY;
    camera->GetScale(&scaleX, &scaleY); 

    int32_t renderPadding = camera->GetPadding();

    float movementRatioX = _movementPercentX / 100.0f;
    float movementRatioY = _movementPercentY / 100.0f;

    int32_t parallaxCameraPosX = (int32_t)(camera->GetX() * movementRatioX);
    int32_t parallaxCameraPosY = (int32_t)(camera->GetY() * movementRatioY);

    int32_t startCol = parallaxCameraPosX / (int32_t)_tilePixelWidth;
    int32_t startRow = parallaxCameraPosY / (int32_t)_tilePixelHeight;

    int32_t colTilesToRender = (uint32_t)((camera->GetCameraWidth() / _tilePixelWidth) / scaleX) +
                               (uint32_t)((renderPadding * 2) / _tilePixelWidth);

    int32_t rowTilesToRender = (uint32_t)((camera->GetCameraHeight() / _tilePixelHeight) / scaleY) +
                               (uint32_t)((renderPadding * 2) / _tilePixelHeight);

    // Some planes (Back, Front) repeat themselves, which means they can be rendered
    // even when out of bounds
    int32_t maxTileIdxX = _tilesOnAxisX;
    int32_t maxTileIdxY = _tilesOnAxisY;
    int32_t minTileIdxX = 0;
    int32_t minTileIdxY = 0;
    if (_flags & WAP_PLANE_FLAG_X_WRAPPING)
    {
        maxTileIdxX = INT32_MAX;
        minTileIdxX = INT32_MIN;
    }
    if (_flags & WAP_PLANE_FLAG_Y_WRAPPING)
    {
        maxTileIdxY = INT32_MAX;
        minTileIdxY = INT32_MIN;
    }

    int32_t row, col;
    for (row = startRow; row < (startRow + rowTilesToRender); row++)
    {
        for (col = startCol; col < (startCol + colTilesToRender); col++)
        {
            // Dont render anything out of bounds
            if ((col < minTileIdxX) || (col > maxTileIdxX) ||
                (row < minTileIdxY) || (row > maxTileIdxY))

            {
                continue;
            }

            int32_t x = (col - startCol) * _tilePixelWidth;
            int32_t y = (row - startRow) * _tilePixelHeight;

            SDL_Texture* texture = _tileTextures[(row % _tilesOnAxisY) * _tilesOnAxisX + (col % _tilesOnAxisX)];
            if (texture != NULL)
            {
                SDL_Rect tileRect = { x - (parallaxCameraPosX % _tilePixelWidth),
                                      y - (parallaxCameraPosY % _tilePixelHeight),
                                      _tilePixelWidth,
                                      _tilePixelHeight };

                SDL_RenderCopy(renderer, texture, NULL, &tileRect);
            }
        }
    }
}

void Plane::RenderObjects(SDL_Renderer* renderer, Camera* camera)
{

}