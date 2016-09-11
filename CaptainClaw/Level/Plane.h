#ifndef PLANE_H_
#define PLANE_H_

#include <SDL.h>
#include <libwap.h>
#include "Level.h"

class Plane
{
public:
    Plane(Level* level, WwdPlane* wwdPlane, uint32_t planeId, SDL_Renderer* renderer);
    ~Plane();

    void Render(SDL_Renderer* renderer, Camera* camera);

    inline uint32_t GetId() { return _planeId; }

    inline uint32_t GetFlags() { return _flags; }
    inline std::string GetName() { return _name; }

    inline uint32_t GetTilePixelWidth() { return _tilePixelWidth; }
    inline uint32_t GetTilePixelHeight() { return _tilePixelHeight; }

    inline uint32_t GetPlanePixelWidth() { return _planePixelWidth; }
    inline uint32_t GetPlanePixelHeight() { return _planePixelHeight; }

    inline uint32_t GetCountTilesX() { return _tilesOnAxisX; }
    inline uint32_t GetCountTilesY() { return _tilesOnAxisY; }

    inline uint32_t GetMovementPercentX() { return _movementPercentX; }
    inline uint32_t GetMovementPercentY() { return _movementPercentY; }

    inline uint32_t GetFillColor() { return _fillColor; }

    inline uint32_t GetCoordZ() { return _coordZ; }

    inline  std::vector<int32_t>& GetTiles() { return _tiles; }

    inline bool IsMainPlane() { return _flags & WAP_PLANE_FLAG_MAIN_PLANE; }

    inline int32_t GetTile(uint32_t x, uint32_t y) { return _tiles[y * _tilesOnAxisX + x]; }
    inline SDL_Texture* GetTileTexture(uint32_t x, uint32_t y) { return _tileTextures[y * _tilesOnAxisX + x]; }

private:
    void RenderTiles(SDL_Renderer* renderer, Camera* camera);
    void RenderObjects(SDL_Renderer*, Camera* camera);

    uint32_t _planeId;

    uint32_t _flags; // WwdPlaneFlags
    std::string _name;

    uint32_t _tilePixelWidth;
    uint32_t _tilePixelHeight;

    uint32_t _planePixelWidth;
    uint32_t _planePixelHeight;

    uint32_t _tilesOnAxisX;
    uint32_t _tilesOnAxisY;
    uint32_t _totalTiles;

    uint32_t _movementPercentX;
    uint32_t _movementPercentY;

    uint32_t _fillColor;
    
    uint32_t _coordZ;

    std::vector<int32_t> _tiles;
    std::vector<SDL_Texture*> _tileTextures;
};



#endif