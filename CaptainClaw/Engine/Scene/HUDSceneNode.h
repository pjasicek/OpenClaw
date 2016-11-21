#ifndef __HUDSCENENODE_H__
#define __HUDSCENENODE_H__

#include "../SharedDefines.h"
#include "../Scene/SceneNodes.h"

class SDL2HUDSceneNode : public SceneNode
{
public:
    SDL2HUDSceneNode(const uint32 actorId,
        BaseRenderComponent* pRenderComponent,
        RenderPass renderPass,
        Point position,
        bool visible);

    virtual ~SDL2HUDSceneNode();

    // Interface overrides
    virtual void VRender(Scene* pScene);
    virtual bool IsVisible(Scene* pScene) const { return m_IsActive; }
    virtual void SetVisible(bool visible) { m_IsActive = visible; }

protected:
    bool m_IsActive;
};

#endif