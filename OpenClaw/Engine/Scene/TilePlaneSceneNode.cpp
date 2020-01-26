#include <SDL2/SDL.h>
#include "Scene.h"
#include "TilePlaneSceneNode.h"
#include "../Actor/Components/RenderComponent.h"
#include "../Graphics2D/Image.h"
#include "../GameApp/BaseGameApp.h"

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

    const int32_t numTilesPadding = 0;

    const SDL_Rect cameraRect = camera->GetCameraRect();

    float movementRatioX = pProperties->movementPercentX / 100.0f;
    float movementRatioY = pProperties->movementPercentY / 100.0f;

    float parallaxCameraPosX = (float) cameraRect.x * movementRatioX;
    float parallaxCameraPosY = (float) cameraRect.y * movementRatioY;

    int32_t startCol = (int32_t)(parallaxCameraPosX / tilePixelWidth) - numTilesPadding;
    int32_t startRow = (int32_t)(parallaxCameraPosY / tilePixelHeight) - numTilesPadding;

    // We need to add 2 due to startCol/startRow + colTilesToRender/rowTilesToRender float->int casting
    int32_t colTilesToRender = (uint32_t)(cameraRect.w / tilePixelWidth) + 2 + numTilesPadding;
    int32_t rowTilesToRender = (uint32_t)(cameraRect.h / tilePixelHeight) + 2 + numTilesPadding;

    // Some planes (Back, Front) repeat themselves, which means they can be rendered
    // even when out of bounds
    int32_t maxTileIdxX = pProperties->tilesOnAxisX;
    int32_t maxTileIdxY = pProperties->tilesOnAxisY;
    int32_t minTileIdxX = 0;
    int32_t minTileIdxY = 0;
    // TODO: Wrap even when when out of bounds on the negative side
    if (pProperties->isWrappedX)
    {
        maxTileIdxX = INT32_MAX;
        minTileIdxX = 0;
    }
    if (pProperties->isWrappedY)
    {
        maxTileIdxY = INT32_MAX;
        minTileIdxY = 0;
    }

    int32_t row, col;
    for (row = startRow; row < (startRow + rowTilesToRender) && row <= maxTileIdxY; row++)
    {
        if (row < minTileIdxY)
        {
            continue;
        }
        const int rowTileIndex = row % pProperties->tilesOnAxisY;

        for (col = startCol; col < (startCol + colTilesToRender) && col <= maxTileIdxX; col++)
        {
            // Dont render anything out of bounds
            if (col < minTileIdxX)
            {
                continue;
            }
            const int colTileIndex = col % pProperties->tilesOnAxisX;

            Image* image = (*pImageList)[rowTileIndex * pProperties->tilesOnAxisX + colTileIndex];

            if (image && image->GetTexture() != NULL)
            {
                int32_t x = col * tilePixelWidth - parallaxCameraPosX;
                int32_t y = row * tilePixelHeight - parallaxCameraPosY;
                SDL_Rect tileRect = { x,
                    y,
                    tilePixelWidth,
                    tilePixelHeight };

                SDL_RenderCopy(renderer, image->GetTexture(), NULL, &tileRect);
            }
        }
    }
}
