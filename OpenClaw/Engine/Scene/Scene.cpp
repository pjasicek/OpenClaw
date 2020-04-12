#include "Scene.h"
#include "../Events/EventMgr.h"
#include "../Events/Events.h"

//=================================================================================================
// Scene Implementation
//

Scene::Scene(SDL_Renderer* renderer)
{
    m_pRoot.reset(new RootNode());
    m_pRenderer = renderer;

    // Register event delegates here
    IEventMgr* pEventMgr = IEventMgr::Get();
    pEventMgr->VAddListener(MakeDelegate(this, &Scene::NewRenderComponentDelegate), EventData_New_Render_Component::sk_EventType);
    pEventMgr->VAddListener(MakeDelegate(this, &Scene::MoveActorDelegate), EventData_Move_Actor::sk_EventType);
    pEventMgr->VAddListener(MakeDelegate(this, &Scene::DestroyActorDelegate), EventData_Destroy_Actor::sk_EventType);
}

Scene::~Scene()
{
    IEventMgr* pEventMgr = IEventMgr::Get();
    pEventMgr->VRemoveListener(MakeDelegate(this, &Scene::NewRenderComponentDelegate), EventData_New_Render_Component::sk_EventType);
    pEventMgr->VRemoveListener(MakeDelegate(this, &Scene::DestroyActorDelegate), EventData_Destroy_Actor::sk_EventType);
    pEventMgr->VRemoveListener(MakeDelegate(this, &Scene::MoveActorDelegate), EventData_Move_Actor::sk_EventType);
}

void Scene::OnUpdate(uint32 msDiff)
{
    return m_pRoot->VOnUpdate(this, msDiff);
}

void Scene::OnRender()
{
    if (m_pRoot && m_pCamera)
    {
        m_pCamera->SetViewPosition(this);

        m_pRoot->VPreRender(this);
        m_pRoot->VRender(this);
        m_pRoot->VRenderChildren(this);
        m_pRoot->VPostRender(this);
    }
}

shared_ptr<ISceneNode> Scene::FindActor(uint32 actorId)
{
    SceneActorMap::iterator iter = m_ActorMap.find(actorId);
    if (iter == m_ActorMap.end())
    {
        return shared_ptr<ISceneNode>();
    }

    return iter->second;
}

bool Scene::AddChild(uint32 actorId, shared_ptr<ISceneNode> kid)
{
    if (actorId != INVALID_ACTOR_ID)
    {
        auto result = m_ActorMap.insert(std::make_pair(actorId, kid));
        if (!result.second)
        {
            LOG_WARNING("Overwriting existing actor in scene. ActorId: " + ToStr(actorId));
            m_ActorMap[actorId] = kid;
        }
    }
    

    return m_pRoot->VAddChild(kid);
}

bool Scene::RemoveChild(uint32 actorId)
{
    if (actorId == INVALID_ACTOR_ID)
    {
        return false;
    }

    shared_ptr<ISceneNode> kid = FindActor(actorId);
    if (!kid)
    {
        //LOG_WARNING("Attempting to remove nonexisting actor. ActorId: " + ToStr(actorId));
        return false;
    }

    m_ActorMap.erase(actorId);
    return m_pRoot->VRemoveChild(actorId);
}

void Scene::SortSceneNodesByZCoord()
{
    m_pRoot->SortChildrenByZCoord();
}

//---------------------------------------------------------------------------------------------------------------------
// Event delegates
//---------------------------------------------------------------------------------------------------------------------

void Scene::NewRenderComponentDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_New_Render_Component> pCastEventData = static_pointer_cast<EventData_New_Render_Component>(pEventData);

    uint32 actorId = pCastEventData->GetActorId();
    shared_ptr<SceneNode> pSceneNode = pCastEventData->GetSceneNode();

    AddChild(actorId, pSceneNode);
}

void Scene::ModifiedRenderComponentDelegate(IEventDataPtr pEventData)
{

}

void Scene::DestroyActorDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Destroy_Actor> pCastEventData = static_pointer_cast<EventData_Destroy_Actor>(pEventData);

    RemoveChild(pCastEventData->GetActorId());
}

void Scene::MoveActorDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Move_Actor> pCastEventData = static_pointer_cast<EventData_Move_Actor>(pEventData);

    shared_ptr<ISceneNode> pNode = FindActor(pCastEventData->GetActorId());
    if (pNode)
    {
        Point moveDestination = pCastEventData->GetMove();
        pNode->VSetPosition(moveDestination);
    }
}