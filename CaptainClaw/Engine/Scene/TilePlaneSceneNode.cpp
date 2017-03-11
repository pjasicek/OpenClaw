#include <SDL2/SDL.h>
#include "Scene.h"
#include "TilePlaneSceneNode.h"
#include "../Actor/Components/RenderComponent.h"
#include "../Graphics2D/Image.h"

SDL2TilePlaneSceneNode::SDL2TilePlaneSceneNode(const uint32 actorId,
    BaseRenderComponent* pRenderComponent,
    RenderPass renderPass,
    Point position)
    : SceneNode(actorId, pRenderComponent, renderPass, position)
{

}

SDL2TilePlaneSceneNode::~SDL2TilePlaneSceneNode()
{

}

void SDL2TilePlaneSceneNode::VRender(Scene* pScene)
{
    TilePlaneRenderComponent* pRenderComponent = static_cast<TilePlaneRenderComponent*>(m_pRenderComponent);

    const TilePlaneProperties* pProperties = pRenderComponent->GetTilePlaneProperties();
    const TileImageList* pImageList = pRenderComponent->GetTileImageList();

    shared_ptr<CameraNode> camera = pScene->GetCamera();
    SDL_Renderer* renderer = pScene->GetRenderer();

    // Multiple times user variables
    int32 tilePixelWidth = pProperties->tilePixelWidth;
    int32 tilePixelHeight = pProperties->tilePixelHeight;

    int32_t renderPadding = 100;

    float movementRatioX = pProperties->movementPercentX / 100.0f;
    float movementRatioY = pProperties->movementPercentY / 100.0f;

    int32_t parallaxCameraPosX = (int32_t)(camera->GetPosition().x * movementRatioX);
    int32_t parallaxCameraPosY = (int32_t)(camera->GetPosition().y * movementRatioY);

    int32_t startCol = parallaxCameraPosX / tilePixelWidth;
    int32_t startRow = parallaxCameraPosY / tilePixelHeight;

    int32_t colTilesToRender = (uint32_t)((camera->GetWidth() / tilePixelWidth)) +
        (uint32_t)((renderPadding * 2) / pProperties->tilePixelWidth);

    int32_t rowTilesToRender = (uint32_t)((camera->GetHeight() / tilePixelHeight)) +
        (uint32_t)((renderPadding * 2) / tilePixelHeight);

    // Some planes (Back, Front) repeat themselves, which means they can be rendered
    // even when out of bounds
    int32_t maxTileIdxX = pProperties->tilesOnAxisX;
    int32_t maxTileIdxY = pProperties->tilesOnAxisY;
    int32_t minTileIdxX = 0;
    int32_t minTileIdxY = 0;
    if (pProperties->isWrappedX)
    {
        maxTileIdxX = INT32_MAX;
        minTileIdxX = INT32_MIN;
    }
    if (pProperties->isWrappedY)
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

            int32_t x = (col - startCol) * tilePixelWidth;
            int32_t y = (row - startRow) * tilePixelHeight;
            Image* image = (*pImageList)[(row % pProperties->tilesOnAxisY) * pProperties->tilesOnAxisX + (col % pProperties->tilesOnAxisX)];

            if (image && image->GetTexture() != NULL)
            {
                SDL_Rect tileRect = { x - (parallaxCameraPosX % tilePixelWidth),
                    y - (parallaxCameraPosY % tilePixelHeight),
                    tilePixelWidth,
                    tilePixelHeight };

                SDL_RenderCopy(renderer, image->GetTexture(), NULL, &tileRect);
            }
        }
    }
}
