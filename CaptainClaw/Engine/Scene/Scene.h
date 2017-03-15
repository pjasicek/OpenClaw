#ifndef __SCENE_H__
#define __SCENE_H__

#include "../SharedDefines.h"
#include "SceneNodes.h"

class Scene
{
public:
    Scene(SDL_Renderer* renderer);
    virtual ~Scene();

    void OnRender();
    bool OnLostDevice() { return true; }
    void OnUpdate(uint32 msDiff);

    shared_ptr<ISceneNode> FindActor(uint32 actorId);
    bool AddChild(uint32 actorId, shared_ptr<ISceneNode> kid);
    bool RemoveChild(uint32 actorId);

    inline void SetCamera(shared_ptr<CameraNode> camera) { m_pCamera = camera; }
    inline const shared_ptr<CameraNode> GetCamera() const { return m_pCamera; }

    inline SDL_Renderer* GetRenderer() { return m_pRenderer; }

    void SortSceneNodesByZCoord();

    // Event delegates
    void NewRenderComponentDelegate(IEventDataPtr pEventData);
    void ModifiedRenderComponentDelegate(IEventDataPtr pEventData);
    void DestroyActorDelegate(IEventDataPtr pEventData);
    void MoveActorDelegate(IEventDataPtr pEventData);

protected:
    shared_ptr<SceneNode>   m_pRoot;
    shared_ptr<CameraNode>  m_pCamera;
    SDL_Renderer*           m_pRenderer;

    SceneActorMap           m_ActorMap;

private:
};

#endif //__SCENE_H__

