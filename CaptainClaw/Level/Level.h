#ifndef LEVEL_H_
#define LEVEL_H_

#include <SDL.h>
#include <string>
#include <stdint.h>
#include <map>
#include <vector>
#include "Camera.h"
#include <libwap.h>

#include "../SharedDefines.h"

class Plane;
class CollisionWorld;
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

struct TileCollisionPrototype
{
    int32_t id;
    uint32_t planeIdx;
    uint32_t width;
    uint32_t height;
    std::vector<TileCollisionRectangle> collisionRectangles;
};

class Scene;
class CameraNode;
class Level
{
public:
    Level(LoadLevelInfo* loadLevelInfo);
    ~Level();

    inline uint32_t GetLevelNumber() { return _levelNumber; }

    void Update(uint32_t msDiff);

    inline TileCollisionPrototype* GetTilePrototype(int32_t tileId) { return _tilePrototypeMap[tileId]; }

    inline WapPal* GetPalette() { return _palette; }

    inline Camera* GetCamera() { return _camera; }

    inline Plane* GetMainPlane() { return _mainPlane; }

    static LevelResourcePaths GetResourcePaths(uint32_t levelNumber);

    inline CollisionWorld* GetCollisionWorld() const { return _collisionWorld; }

    WeakActorPtr GetActor(uint32 actorId);

private:
    TileCollisionPrototype* CreateTilePrototype(WwdTileDescription* tileDescription, uint32_t tileId);

    std::map<uint32, StrongActorPtr> _actorMap;

    Camera* _camera;
    uint32_t _levelNumber;
    std::string _levelName;

    WapPal* _palette;

    std::map<int32_t, TileCollisionPrototype*> _tilePrototypeMap;
    std::vector<Plane*> _planesVector;

    CollisionWorld* _collisionWorld;

    Plane* _mainPlane;

    shared_ptr<Scene> m_pScene;
    shared_ptr<CameraNode> m_pCamera;
};

#endif