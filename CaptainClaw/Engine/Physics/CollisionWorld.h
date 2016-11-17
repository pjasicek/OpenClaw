#ifndef COLLISIONWORLD_H_
#define COLLISIONWORLD_H_

#include <SDL.h>
#include <stdint.h>
#include <vector>
#include <map>

class Level;
class Plane;
class Camera;

struct TileCollisionPrototype;



enum MovementInfoEvent
{
    MOVEMENT_INFO_NONE,
    MOVEMENT_INFO_SIDEHIT_SOLID,
    MOVEMENT_INFO_HIT_FLOOR_SOLID,
    MOVEMENT_INFO_HIT_CEILING_SOLID,
    MOVEMENT_INFO_LANDED_LADDER,
    MOVEMENT_INFO_HIT_DEATH
};

struct MovementInfo
{
    int32_t movementX;
    int32_t movementY;
    MovementInfoEvent moveEvent;
};

enum Direction
{
    DIRECTION_X,
    DIRECTION_Y
};

class CollisionWorld
{
public:
    CollisionWorld(Level* level);
    ~CollisionWorld();

    void GetIntersectingCollisionRegions(SDL_Rect* worldRect, std::vector<CollisionRegion*>* collisionRegions);

    void DebugRender(SDL_Renderer* renderer, Camera* camera);

    MovementInfo ResolveMovement(SDL_Rect* body, int32_t deltaX, int32_t deltaY);

    static inline int32_t GetRegionKey(int32_t row, int32_t col) { return row * 10000 + col; }

private:
    bool IsColliding(SDL_Rect* body, CollisionRegion* colRegion, CollisionRect* outColRect);
    int32_t CalculateAvailableMove(SDL_Rect body, int32_t moveDelta, std::vector<CollisionRegion*>& colRegions, Direction direction);

    uint32_t _collisionWorldWidth;
    uint32_t _collisionWorldHeight;

    uint32_t _collisionRegionWidth;
    uint32_t _collisionRegionHeight;

    std::map<int32_t, CollisionRegion*> _collisionRegionsMap;
};

#endif