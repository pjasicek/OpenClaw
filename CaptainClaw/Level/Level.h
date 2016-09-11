#ifndef LEVEL_H_
#define LEVEL_H_

#include <SDL.h>
#include <string>
#include <stdint.h>
#include <map>
#include <vector>
#include "Camera.h"

class Game;
class Plane;
class Claw;
struct LoadLevelInfo;

struct LevelResourcePaths
{
    std::string wwdPath;
    std::string palettePath;
    std::string levelImagesPath;
    std::string levelAnimationsPath;
    std::string levelSoundsPath;
    std::string levelMusicPath;
    std::string levelTilesPath;
};

struct TileCollisionRectangle
{
    uint32_t attributeFlag; // WwdAttributeFlags
    SDL_Rect collisionRect;
};

struct TilePrototype
{
    int32_t id;
    uint32_t planeIdx;
    uint32_t width;
    uint32_t height;
    std::vector<TileCollisionRectangle> collisionRectangles;
    SDL_Texture* texture;
};

class Level
{
public:
    Level(LoadLevelInfo* loadLevelInfo, Game* game);
    ~Level();

    void Update(uint32_t msDiff);

    inline TilePrototype* GetTilePrototype(int32_t tileId) { return _tilePrototypeMap[tileId]; }

    inline Camera* GetCamera() { return _camera; }

    static LevelResourcePaths GetResourcePaths(uint32_t levelNumber);

private:
    RezFile* FindTileFile(RezDirectory* tileDirectory, std::string fileName);
    TilePrototype* CreateTilePrototype(WwdTileDescription* tileDescription, uint32_t tileId, WapPid* pid, SDL_Renderer* renderer);

    Game* _game;
    Camera* _camera;
    uint32_t _levelNumber;
    std::string _levelName;

    std::map<int32_t, TilePrototype*> _tilePrototypeMap;
    std::vector<Plane*> _planesVector;

    int32_t _worldPixelWidth;
    int32_t _worldPixelHeight;
};

#endif