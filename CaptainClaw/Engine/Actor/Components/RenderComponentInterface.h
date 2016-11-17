#ifndef __RENDERCOMPONENTINTERFACE_H_
#define __RENDERCOMPONENTINTERFACE_H_

#include "../ActorComponent.h"

class SceneNode;
class RenderComponentInterface : public ActorComponent
{
    virtual shared_ptr<SceneNode> VGetSceneNode() = 0;
};

#endif