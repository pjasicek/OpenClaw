#ifndef __TILEPLANESCENENODE_H__
#define __TILEPLANESCENENODE_H__

#include "../SharedDefines.h"
#include "../Scene/SceneNodes.h"

class SDL2TilePlaneSceneNode : public SceneNode
{
public:
    SDL2TilePlaneSceneNode(const uint32 actorId,
        BaseRenderComponent* pRenderComponent,
        RenderPass renderPass,
        Point position);

    virtual ~SDL2TilePlaneSceneNode();

    // Interface overrides
    virtual void VRender(Scene* pScene);

protected:
};

#endif