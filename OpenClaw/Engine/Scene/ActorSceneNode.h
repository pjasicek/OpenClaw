#ifndef __ACTORSCENENODE_H__
#define __ACTORSCENENODE_H__

#include "../SharedDefines.h"
#include "../Scene/SceneNodes.h"

class SDL2ActorSceneNode : public SceneNode
{
public:
    SDL2ActorSceneNode(const uint32 actorId,
        BaseRenderComponent* pRenderComponent,
        RenderPass renderPass,
        Point position,
        int32 zCoord);

    virtual ~SDL2ActorSceneNode();

    // Interface overrides
    virtual void VRender(Scene* pScene);

protected:
};

#endif