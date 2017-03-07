#ifndef __PHYSICSDEBUGDRAWER_H__
#define __PHYSICSDEBUGDRAWER_H__

#include <SDL2/SDL.h>
#include "ClawPhysics.h"

class CameraNode;
class PhysicsDebugDrawer : public b2Draw
{
public:
    PhysicsDebugDrawer();

    void PrepareForDraw(SDL_Renderer* pRenderer, std::shared_ptr<CameraNode> pCamera);

    virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
    virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
    virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
    virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
    virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
    virtual void DrawTransform(const b2Transform& xf);
    virtual void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);

private:
    SDL_Renderer* m_pRenderer;
    std::shared_ptr<CameraNode> m_pCamera;
};

#endif