#include <assert.h>

#include "CollisionWorld.h"
#include "../Level/Level.h"
#include "../Level/Plane.h"
#include "../Util/Util.h"

#include <iostream>
using namespace std;

static const uint32_t COLLISION_RECT_SIZE = 192;

CollisionWorld::CollisionWorld(Level* level)
{
    Plane* plane = level->GetMainPlane();
    assert(plane != NULL);

    _collisionWorldWidth = plane->GetPlanePixelHeight();
    _collisionWorldHeight = plane->GetPlanePixelWidth();

    cout << "w = " << _collisionWorldWidth << endl;
    cout << "h = " << _collisionWorldHeight << endl;

    _collisionRegionWidth = COLLISION_RECT_SIZE;
    _collisionRegionHeight = COLLISION_RECT_SIZE;

    uint32_t tileWidth = plane->GetTilePixelWidth();
    uint32_t tileHeight = plane->GetTilePixelHeight();

    cout << "tileWidth = " << tileWidth << endl;
    cout << "tileHeight = " << tileHeight << endl;

    assert(_collisionRegionWidth > tileWidth);
    assert(_collisionRegionHeight > tileHeight);

    assert(_collisionRegionWidth % tileWidth == 0);
    assert(_collisionRegionHeight % tileHeight == 0);

    uint32_t tileColsCount = plane->GetCountTilesX();
    uint32_t tileRowsCount = plane->GetCountTilesY();
    std::vector<int32_t>& tiles = plane->GetTiles();

    cout << "tileColsCount = " << tileColsCount << endl;
    cout << "tileRowsCount = " << tileRowsCount << endl;
    cout << "tiles = " << tiles.size() << endl;

    for (uint32_t tileRow = 0; tileRow < tileRowsCount; tileRow++)
    {
        for (uint32_t tileCol = 0; tileCol < tileColsCount; tileCol++)
        {
            int32_t tile = tiles[tileRow * tileColsCount + tileCol];
            if (tile == -1)
            {
                continue;
            }

            TileCollisionPrototype* tileProto = level->GetTilePrototype(tile);
            assert(tileProto != NULL);

            int32_t x = tileCol * tileWidth;
            int32_t y = tileRow * tileHeight;

            int32_t regionCol = x / _collisionRegionWidth;
            int32_t regionRow = y / _collisionRegionHeight;
            int32_t key = GetRegionKey(regionRow, regionCol);
            //cout << "col row " << regionCol << " " << regionRow << endl;

            for (TileCollisionRectangle tileCollisionRect : tileProto->collisionRectangles)
            {
                if (tileCollisionRect.attributeFlag == WAP_TILE_ATTRIBUTE_CLEAR)
                {
                    continue;
                }

                if (_collisionRegionsMap.count(key) == 0)
                {
                    CollisionRegion* region = new CollisionRegion;
                    _collisionRegionsMap.insert(std::make_pair(key, region));
                }
                else
                {
                    /*cout << "Found region: " << regionRow << " " << regionCol << endl;
                    cout << x << "x" << y << endl;*/
                }

                CollisionRegion* region = _collisionRegionsMap[key];
                assert(region != NULL);

                //cout << "regionCol = " << regionCol << endl;
                //cout << "regionRow = " << regionRow << endl;


                SDL_Rect r = tileCollisionRect.collisionRect;
               // Util::PrintRect(r, to_string(tile));
                SDL_Rect worldRect = { x + r.x, y + r.y, r.w, r.h };
                uint32_t collisionType = tileCollisionRect.attributeFlag;
                region->staticRectsVec.push_back({ worldRect, collisionType });

                //cout << "tile = " << tile << endl;
                //Util::PrintRect(worldRect, to_string(tileCollisionRect.attributeFlag));
            }
        }
    }

    /*for (auto x : _collisionRegionsMap)
    {
        cout << x.first << endl;
        Util::PrintRect(x.second->staticRectsVec[0].worldRect, "");
    }
    exit(0);*/
    /*cout << "map size = " << _collisionRegionsMap.size() << endl;
    for (auto a : _collisionRegionsMap)
    {
        if (a.second == NULL)
        {
            cout << "NULL" << endl;
            continue;
        }
        cout << "s = " << a.second->staticRectsVec.size() << endl;
    }*/
    //exit(0);
}

void CollisionWorld::GetIntersectingCollisionRegions(SDL_Rect* worldRect, std::vector<CollisionRegion*>* collisionRegions)
{
    /*assert(worldRect->w < _collisionRegionWidth);
    assert(worldRect->h < _collisionRegionHeight);*/

    int32_t startCol = (worldRect->x)  / _collisionRegionWidth;
    int32_t startRow = (worldRect->y) / _collisionRegionHeight;

    int32_t regionColsCount = (worldRect->w / _collisionRegionWidth) + 2;
    int32_t regionRowsCount = (worldRect->h / _collisionRegionHeight) + 2;

    int32_t row, col;
    for (row = startRow; row < (startRow + regionRowsCount); row++)
    {
        for (col = startCol; col < (startCol + regionColsCount); col++)
        {
            int32_t key = GetRegionKey(row, col);
            CollisionRegion* region = _collisionRegionsMap[key];
            if (region != NULL)
            {
                collisionRegions->push_back(region);
            }
        }
    }
}

MovementInfo CollisionWorld::ResolveMovement(SDL_Rect* body, int32_t deltaX, int32_t deltaY)
{
    START_QUERY_PERFORMANCE_TIMER;
    MovementInfo moveInfo = { 0 };

    if (deltaX == 0 && deltaY == 0)
    {
        return moveInfo;
    }

    // Get all required collision regions
    SDL_Rect grownBody = (*body);
    
    if (deltaX < 0) { grownBody.x += 2*deltaX; }
    else { grownBody.w += deltaX; }
    if (deltaY < 0) { grownBody.y += 2*deltaY; }
    else { grownBody.y += deltaY; }

    std::vector<CollisionRegion*> colRegions;
    GetIntersectingCollisionRegions(&grownBody, &colRegions);

    moveInfo.movementX = CalculateAvailableMove((*body), deltaX, colRegions, DIRECTION_X);
    moveInfo.movementY = CalculateAvailableMove((*body), deltaY, colRegions, DIRECTION_Y);
    //END_QUERY_PERFORMANCE_TIMER

    return moveInfo;
}

void CollisionWorld::DebugRender(SDL_Renderer* renderer, Camera* camera)
{
    int32_t cameraX = camera->GetX();
    int32_t cameraY = camera->GetY();
    float scaleX, scaleY;
    camera->GetScale(&scaleX, &scaleY);

    uint32_t cameraWidth, cameraHeight;
    camera->GetCameraSize(&cameraWidth, &cameraHeight);

    int32_t centerX = cameraX + int32_t((cameraWidth / 2) / scaleX);
    int32_t centerY = cameraY + int32_t((cameraHeight / 2) / scaleY);

    SDL_Rect rect = { centerX - 50/2, centerY - 110/2, 50, 110 };
    
    SDL_Rect centerRect = { int32_t((cameraWidth / 2 - rect.w / 2) / scaleX) ,
        int32_t((cameraHeight / 2 - rect.h / 2) / scaleY), 
        rect.w, 
        rect.h };

    std::vector<CollisionRegion*> colRegions;
    
    GetIntersectingCollisionRegions(&rect, &colRegions);
    
    for (CollisionRegion* colRegion : colRegions)
    {
        for (CollisionRect colRect : colRegion->staticRectsVec)
        {
            SDL_Rect rect = colRect.worldRect;
            SDL_Rect renderRect = { rect.x - cameraX, rect.y - cameraY, rect.w, rect.h };

            if (colRect.collisionType == WAP_TILE_ATTRIBUTE_CLIMB)
            {
                SDL_SetRenderDrawColor(renderer, 0, 250, 0, 0);
            }
            else if (colRect.collisionType == WAP_TILE_ATTRIBUTE_DEATH)
            {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
            }
            else if (colRect.collisionType == WAP_TILE_ATTRIBUTE_GROUND)
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 0);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 0, 250, 250, 0);
            }

            SDL_RenderDrawRect(renderer, &renderRect);
        }
    }

    SDL_SetRenderDrawColor(renderer, 250, 0, 250, 0);
    SDL_RenderDrawRect(renderer, &centerRect);

    int32_t startRegionCol = cameraX / _collisionRegionWidth;
    int32_t startRegionRow = cameraY / _collisionRegionHeight;

    int32_t regionCols = cameraWidth / _collisionRegionWidth + 2;
    int32_t regionsRows = cameraHeight / _collisionRegionHeight + 2;

    int32_t regionRow, regionCol;
    for (regionRow = startRegionRow; regionRow < (startRegionRow + regionsRows); regionRow++)
    {
        for (regionCol = startRegionCol; regionCol < (startRegionCol + regionCols); regionCol++)
        {
            int32_t key = GetRegionKey(regionRow, regionCol);

            CollisionRegion* region = _collisionRegionsMap[key];
            if (region == NULL)
            {
                continue;
            }

            /*cout << "found key" << endl;
            cout << "col = " << regionCol << ", row = " << regionRow << endl;*/

            SDL_Rect r = { (regionCol * COLLISION_RECT_SIZE) - cameraX, (regionRow * COLLISION_RECT_SIZE) - cameraY, COLLISION_RECT_SIZE, COLLISION_RECT_SIZE };
            SDL_SetRenderDrawColor(renderer, 200, 200, 0, 0);
            SDL_RenderDrawRect(renderer, &r);
            //Util::PrintRect(r, "asfd");
            /*SDL_SetRenderDrawColor(renderer, 0, 250, 0, 0);
            for (CollisionRect& colRect : region->staticRectsVec)
            {
                SDL_Rect rect = colRect.worldRect;
                SDL_Rect renderRect = { rect.x - cameraX, rect.y - cameraY, rect.w, rect.h };
                //Util::PrintRect(renderRect, "CollisionRect");

                if (colRect.collisionType == WAP_TILE_ATTRIBUTE_CLIMB)
                {
                    SDL_SetRenderDrawColor(renderer, 0, 250, 0, 0);
                }
                else if (colRect.collisionType == WAP_TILE_ATTRIBUTE_DEATH)
                {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
                }
                else if (colRect.collisionType == WAP_TILE_ATTRIBUTE_GROUND)
                {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 0);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, 0, 250, 250, 0);
                }

                SDL_RenderDrawRect(renderer, &renderRect);
            }*/
        }
    }
}

//-----------------------------------------------------------------------------------------------------------
// Private implementations
//-----------------------------------------------------------------------------------------------------------

bool CollisionWorld::IsColliding(SDL_Rect* body, CollisionRegion* colRegion, CollisionRect* outColRect)
{
    SDL_Rect dummy;
    for (CollisionRect colRect : colRegion->staticRectsVec)
    {
        if (SDL_IntersectRect(&colRect.worldRect, body, &dummy))
        {
            if (outColRect != NULL)
            {
                (*outColRect) = colRect;
            }
            return true;
        }
    }

    return false;
}

int32_t CollisionWorld::CalculateAvailableMove(SDL_Rect body, int32_t moveDelta, std::vector<CollisionRegion*>& colRegions, Direction direction)
{
    int32_t step;
    int32_t prevDistance = 0;
    CollisionRect colRect;
    for (step = 0; step != moveDelta; moveDelta > 0 ? ++step : --step)
    {

        if (direction == DIRECTION_X)
        {
            moveDelta > 0 ? body.x++ : body.x--;
        }
        else
        {
            moveDelta > 0 ? body.y++ : body.y--;
        }

        for (CollisionRegion* colRegion : colRegions)
        {
            if (IsColliding(&body, colRegion, &colRect))
            {
                if (colRect.collisionType == WAP_TILE_ATTRIBUTE_CLIMB)
                {
                    // Check if body is sufficently inside the latter 

                    // On the left side of the ladder
                    if ((body.x + body.w) < (colRect.worldRect.x + colRect.worldRect.w))
                    {
                        if ((body.x + body.w) > (colRect.worldRect.x + colRect.worldRect.w / 2))
                        {
                            // Can hang on to the ladder
                        }
                    }
                    else // On the right side of the ladder
                    {
                        if (body.x < (colRect.worldRect.x + colRect.worldRect.w / 2))
                        {
                            // Can hang on to the ladder
                        }
                    }

                    continue;
                }

                // Only falling onto ground tile is classified as collision
                // moving left / right / jumping through it is possible
                if ((colRect.collisionType == WAP_TILE_ATTRIBUTE_GROUND ||
                     colRect.collisionType == WAP_TILE_ATTRIBUTE_CLIMB) &&
                    ((direction != DIRECTION_Y || moveDelta < 0) ||
                    (body.y + body.h) > (colRect.worldRect.y + 1)))
                {
                    continue;
                }

                if (colRect.collisionType == WAP_TILE_ATTRIBUTE_DEATH)
                {
                    // Collided with death tile
                }

                if (abs(prevDistance) > 0 && direction == DIRECTION_X)
                {
                    // Ran into a wall
                }
                else if (prevDistance != 0 && direction == DIRECTION_Y)
                {
                    if (prevDistance > 0)
                    {
                        // Collided with floor
                    }
                    else if (prevDistance < 0)
                    {
                        // Collided with ceiling
                    }
                }

                return prevDistance;
            }
        }
        prevDistance = step;
    }

    return moveDelta;
}