#include <SDL.h>
#include "Scene.h"
#include "HUDSceneNode.h"
#include "../Actor/Components/RenderComponent.h"
#include "../Graphics2D/Image.h"

SDL2HUDSceneNode::SDL2HUDSceneNode(const uint32 actorId,
    BaseRenderComponent* pRenderComponent,
    RenderPass renderPass,
    Point position,
    bool visible)
    : SceneNode(actorId, pRenderComponent, renderPass, position)
{
    m_IsActive = visible;
}

SDL2HUDSceneNode::~SDL2HUDSceneNode()
{

}

void SDL2HUDSceneNode::VRender(Scene* pScene)
{
    if (!IsVisible(pScene))
    {
        return;
    }

    HUDRenderComponent* hrc = static_cast<HUDRenderComponent*>(m_pRenderComponent);

    shared_ptr<Image> actorImage = MakeStrongPtr(hrc->GetCurrentImage());
    if (!actorImage)
    {
        LOG_WARNING("Trying to render actor without active image. ActorId: " + ToStr(m_Properties.GetActorId()));
        return;
    }

    int32 offsetX = hrc->IsMirrored() ? -actorImage->GetOffsetX() : actorImage->GetOffsetX();
    int32 offsetY = hrc->IsInverted() ? -actorImage->GetOffsetY() : actorImage->GetOffsetY();

    int32 x = m_Properties.GetPosition().x - actorImage->GetWidth() / 2 + offsetX;
    if (hrc->IsAnchoredRight())
    {
        x += pScene->GetCamera()->GetWidth();
    }
    int32 y = m_Properties.GetPosition().y - actorImage->GetHeight() / 2 + offsetY;
    if (hrc->IsAnchoredBottom())
    {
        y += pScene->GetCamera()->GetHeight();
    }

    // HACK: Pistol first frame has incorrect offset...
    if (hrc->IsAnchoredRight() && actorImage->GetWidth() == 52 && actorImage->GetHeight() == 14 && actorImage->GetOffsetY() == 1)
    {
        y += 2;
    }

    SDL_Rect renderRect =
    {
        x,
        y,
        actorImage->GetWidth(),
        actorImage->GetHeight()
    };

    SDL_Renderer* renderer = pScene->GetRenderer();
    SDL_RenderCopyEx(renderer, actorImage->GetTexture(), NULL, &renderRect, 0, NULL,
        hrc->IsMirrored() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}