#include "PhysicsDebugDrawer.h"
#include "../SharedDefines.h"
#include "../Scene/SceneNodes.h"

#include <SDL2/SDL2_gfxPrimitives.h>

// Conversion warnings from Box2D -> SDL2_gfx
#pragma warning(disable: 4244)

PhysicsDebugDrawer::PhysicsDebugDrawer()
{
    SetFlags(b2Draw::e_shapeBit);
}

void PhysicsDebugDrawer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    Point cameraPos = m_pCamera->GetPosition();

    std::vector<int16> vaX;
    std::vector<int16> vaY;

    vaX.reserve(vertexCount);
    vaY.reserve(vertexCount);
    for (int32 vertexIdx = 0; vertexIdx < vertexCount; vertexIdx++)
    {
        b2Vec2 worldPos = MetersToPixels(vertices[vertexIdx]);

        vaX.push_back(worldPos.x - cameraPos.x);
        vaY.push_back(worldPos.y - cameraPos.y);
    }

    polygonRGBA(m_pRenderer, vaX.data(), vaY.data(), vertexCount, color.r * 255, color.g * 255, color.b * 255, color.a * 255);
}

// This is ~20 times more cpu taxing than DrawPolygon
void PhysicsDebugDrawer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    // For now I guess
    DrawPolygon(vertices, vertexCount, color);

    /*Point cameraPos = m_pCamera->GetPosition();

    std::vector<int16> vaX;
    std::vector<int16> vaY;

    for (int32 vertexIdx = 0; vertexIdx < vertexCount; vertexIdx++)
    {
        vaX.push_back(vertices[vertexIdx].x - cameraPos.x);
        vaY.push_back(vertices[vertexIdx].y - cameraPos.y);
    }

    filledPolygonRGBA(m_pRenderer, vaX.data(), vaY.data(), vertexCount, color.r * 255, color.g * 255, color.b * 255, (color.a * 255) * 0.5);*/
}

void PhysicsDebugDrawer::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
    Point cameraPos = m_pCamera->GetPosition();
    b2Vec2 worldPos = MetersToPixels(center);
    float worldRadius = MetersToPixels(radius);

    circleRGBA(m_pRenderer, worldPos.x - cameraPos.x, worldPos.y - cameraPos.y, worldRadius, color.r * 255, color.g * 255, color.b * 255, color.a * 255);
}

void PhysicsDebugDrawer::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
    DrawCircle(center, radius, color);
}

void PhysicsDebugDrawer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{

}

void PhysicsDebugDrawer::DrawTransform(const b2Transform& xf)
{

}

void PhysicsDebugDrawer::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{
    SDL_SetRenderDrawColor(m_pRenderer, color.r * 255, color.g, color.b * 255, color.a * 255);

    SDL_RenderDrawPoint(m_pRenderer, p.x, p.y);
}

void PhysicsDebugDrawer::PrepareForDraw(SDL_Renderer* pRenderer, std::shared_ptr<CameraNode> pCamera)
{
    assert(pRenderer);
    assert(pCamera);

    m_pRenderer = pRenderer;
    m_pCamera = pCamera;
}