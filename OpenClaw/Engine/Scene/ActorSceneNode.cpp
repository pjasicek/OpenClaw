#include "Scene.h"
#include "ActorSceneNode.h"
#include "../Actor/Components/RenderComponent.h"
#include "../Graphics2D/Image.h"

SDL2ActorSceneNode::SDL2ActorSceneNode(const uint32 actorId,
    BaseRenderComponent* pRenderComponent,
    RenderPass renderPass,
    Point position,
    int32 zCoord)
    : SceneNode(actorId, pRenderComponent, renderPass, position, zCoord)
{
    
}

SDL2ActorSceneNode::~SDL2ActorSceneNode()
{

}

void SDL2ActorSceneNode::VRender(Scene* pScene)
{
    // In Claw every actor is a rectangle which simplifies stuff A LOT. Keep this is mind
    // in case of engine transition to another project.
    ActorRenderComponent* arc = static_cast<ActorRenderComponent*>(m_pRenderComponent);
    
    shared_ptr<Image> actorImage = MakeStrongPtr(arc->GetCurrentImage());
    if (!actorImage)
    {
        LOG_WARNING("Trying to render actor without active image. ActorId: " + ToStr(m_Properties.GetActorId()));
        return;
    }

    if (!arc->IsVisible())
    {
        return;
    }

    shared_ptr<CameraNode> pCamera = pScene->GetCamera();
    if (!pCamera)
    {
        LOG_ERROR("Trying to render without active camera. ActorId: " + ToStr(m_Properties.GetActorId()));
        return;
    }

    //LOG("Rendering actorId: " + ToStr(m_Properties.GetActorId()));

    // Actor rect: Top-Left coordinates of actors rectangle and its width-height. It is up to
    // render component to provide us correct data. This just renders stuff without assuming anything.
    // Also image offset is taken into account when calculating position rect.
    // For more info go to VGetPositionRect()'s definition.
    // const SDL_Rect actorWorldRect = arc->VGetPositionRect();
    const SDL_Rect cameraRect = pCamera->GetCameraRect();
    int32 offsetX = arc->IsMirrored() ? -actorImage->GetOffsetX() : actorImage->GetOffsetX();
    int32 offsetY = arc->IsInverted() ? -actorImage->GetOffsetY() : actorImage->GetOffsetY();
    SDL_Rect renderRect =
    {
        (int)m_Properties.GetPosition().x - actorImage->GetWidth() / 2  + offsetX - cameraRect.x,
        (int)m_Properties.GetPosition().y - actorImage->GetHeight() / 2 + offsetY - cameraRect.y,
        actorImage->GetWidth(),
        actorImage->GetHeight()
    };

    // Do not render anything which is not seen
    if (renderRect.x > cameraRect.w ||
        renderRect.y > cameraRect.h ||
        renderRect.x + renderRect.w < 0 ||
        renderRect.y + renderRect.h < 0) {
        return;
    }

    SDL_SetTextureAlphaMod(actorImage->GetTexture(), arc->GetAlpha());

    SDL_Color colorMod = arc->GetColorMod();
    SDL_SetTextureColorMod(actorImage->GetTexture(), colorMod.r, colorMod.g, colorMod.b);

    SDL_Renderer* renderer = pScene->GetRenderer();
    SDL_RenderCopyEx(renderer, actorImage->GetTexture(), NULL, &renderRect, 0, NULL, 
        arc->IsMirrored() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}
