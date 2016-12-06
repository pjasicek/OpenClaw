#include "SceneNodes.h"
#include "Scene.h"
#include "../Actor/ActorComponent.h"
#include "../Actor/Components/RenderComponent.h"

//=================================================================================================
// SceneNodeProperties Implementation
//

SceneNodeProperties::SceneNodeProperties()
{
    m_ActorId = INVALID_ACTOR_ID;
    m_RenderPass = RenderPass_0;
    m_ZCoord = 0;
}

//=================================================================================================
// SceneNode Implementation
//

SceneNode::SceneNode(uint32 actorId, BaseRenderComponent* renderComponent, RenderPass renderPass, Point position, int32 zCoord)
{
    m_pParent = NULL;
    m_Properties.m_ActorId = actorId;
    m_Properties.m_Orientation = 0;
    m_Properties.m_Name = ""; // TODO
    m_Properties.m_Position = position;
    m_Properties.m_RenderPass = renderPass;
    m_Properties.m_ZCoord = zCoord;
    m_pRenderComponent = renderComponent;

    /*if (m_Properties.m_Width == 0 || m_Properties.m_Height == 0)
    {
        LOG_WARNING("Created scene note attached to actor: " + ToStr(actorId) + " has invalid dimensions");
    }*/
}

SceneNode::~SceneNode()
{

}

// Idk now, SDL2 should handle everything on its own
bool SceneNode::VOnLostDevice(Scene* pScene)
{
    return true;
}

void SceneNode::VOnUpdate(Scene* pScene, uint32 msDiff)
{
    // This is meant to be called from any class
    // that inherits from SceneNode and overloads
    // VOnUpdate()

    /*SceneNodeList::iterator i = m_ChildrenList.begin();
    SceneNodeList::iterator end = m_ChildrenList.end();

    while (i != end)
    {
    (*i)->VOnUpdate(pScene, msDiff);
    ++i;
    }*/

    for (auto child : m_ChildrenList)
    {
        child->VOnUpdate(pScene, msDiff);
    }
}

void SceneNode::VRenderChildren(Scene* pScene)
{
    for (auto childNode : m_ChildrenList)
    {
        // TODO: Huge overhead from testing visibility of every single actor each frame
        // Possible solution: Use Box2D Broadphase to retrieve all actors within AABB
        if (childNode->VIsVisible(pScene))
        {
            childNode->VPreRender(pScene);
            childNode->VRender(pScene);
            childNode->VRenderChildren(pScene);
            childNode->VPostRender(pScene);
        }
    }
}

bool SceneNode::VIsVisible(Scene* pScene) const
{
    //LOG("Checking visibility");

    if (!m_pRenderComponent)
    {
        return false;
    }

    shared_ptr<CameraNode> pCamera = pScene->GetCamera();
    if (!pCamera)
    {
        LOG_ERROR("Checking visibility without available scene camera");
        return false;
    }

    const SDL_Rect cameraRect = pCamera->GetCameraRect();
    SDL_Rect actorRect = m_pRenderComponent->VGetPositionRect();

    SDL_Rect result;
    if (!SDL_IntersectRect(&cameraRect, &(m_pRenderComponent->VGetPositionRect()), &result))
    {
        return false;
    }

    return true;
}

bool SceneNode::VAddChild(shared_ptr<ISceneNode> ikid)
{
    m_ChildrenList.push_back(ikid);

    shared_ptr<SceneNode> kid = static_pointer_cast<SceneNode>(ikid);
    kid->m_pParent = this;

    return true;
}

bool SceneNode::VRemoveChild(uint32 actorId)
{
    for (SceneNodeList::iterator iter = m_ChildrenList.begin(); iter != m_ChildrenList.end(); ++iter)
    {
        const SceneNodeProperties* pProperties = (*iter)->VGetProperties();
        if (pProperties->GetActorId() != INVALID_ACTOR_ID && actorId == pProperties->GetActorId())
        {
            iter = m_ChildrenList.erase(iter);
            return true;
        }
    }

    return false;
}

void SceneNode::SortChildrenByZCoord()
{
    for (auto pChildNode : m_ChildrenList)
    {
        pChildNode->SortChildrenByZCoord();
    }

    std::sort(m_ChildrenList.begin(), m_ChildrenList.end(), 
        [](const shared_ptr<ISceneNode>& lhs, const shared_ptr<ISceneNode>& rhs)
    {
        return lhs->GetZCoord() < rhs->GetZCoord();
    });
}

//=================================================================================================
// RootNode Implementation
//

RootNode::RootNode() : SceneNode(INVALID_ACTOR_ID, NULL, RenderPass_0, { 0, 0 })
{
    m_ChildrenList.reserve(RenderPass_Last);

    shared_ptr<SceneNode> backgroundGroup(new SceneNode(INVALID_ACTOR_ID, NULL, RenderPass_Background, { 0, 0 }));
    m_ChildrenList.push_back(backgroundGroup);

    shared_ptr<SceneNode> actionGroup(new SceneNode(INVALID_ACTOR_ID, NULL, RenderPass_Action, { 0, 0 }));
    m_ChildrenList.push_back(actionGroup);

    shared_ptr<SceneNode> actorGroup(new SceneNode(INVALID_ACTOR_ID, NULL, RenderPass_Actor, { 0, 0 }));
    m_ChildrenList.push_back(actorGroup);

    shared_ptr<SceneNode> foregroundGroup(new SceneNode(INVALID_ACTOR_ID, NULL, RenderPass_Foreground, { 0, 0 }));
    m_ChildrenList.push_back(foregroundGroup);

    shared_ptr<SceneNode> HUDGroup(new SceneNode(INVALID_ACTOR_ID, NULL, RenderPass_HUD, { 0, 0 }));
    m_ChildrenList.push_back(HUDGroup); 

    shared_ptr<SceneNode> invisibleGroup(new SceneNode(INVALID_ACTOR_ID, NULL, RenderPass_NotRendered, { 0, 0 }));
    m_ChildrenList.push_back(invisibleGroup);
}

bool RootNode::VAddChild(shared_ptr<ISceneNode> kid)
{
    RenderPass pass = kid->VGetProperties()->GetRenderPass();
    if ((uint16)pass >= m_ChildrenList.size() || !m_ChildrenList[pass])
    {
        assert(0 && "There is not such render pass");
        return false;
    }

    //LOG("Adding child to render pass: " + ToStr(pass));
    m_ChildrenList[pass]->VAddChild(kid);
    return true;
}

bool RootNode::VRemoveChild(uint32 actorId)
{
    bool anythingRemoved = false;
    for (uint16 pass = RenderPass_0; pass < RenderPass_Last; ++pass)
    {
        if (m_ChildrenList[pass]->VRemoveChild(actorId))
        {
            anythingRemoved = true;
        }
    }

    return anythingRemoved;
}


void RootNode::VRenderChildren(Scene* pScene)
{
    for (uint16 pass = RenderPass_0; pass < RenderPass_Last; ++pass)
    {
        switch (pass)
        {
            case RenderPass_Background:
            case RenderPass_Action:
            case RenderPass_Foreground:
                m_ChildrenList[pass]->VRenderChildren(pScene);
                break;

            case RenderPass_Actor:
                m_ChildrenList[pass]->VRenderChildren(pScene);
                break;

            case RenderPass_HUD:
                m_ChildrenList[pass]->VRenderChildren(pScene);
                break;
        }
    }
}

//=================================================================================================
// CameraNode Implementation
//

CameraNode::CameraNode(Point position, uint32 width, uint32 height)
    : SceneNode(INVALID_ACTOR_ID, NULL, RenderPass_0, position),
    m_OffsetX(0),
    m_OffsetY(0),
    m_Active(true), 
    m_DebugCamera(false)
{
    m_Width = width;
    m_Height = height;
}

void CameraNode::VRender(Scene* pScene)
{
    if (m_DebugCamera)
    {
        // Some debug maybe
    }
}

SDL_Rect CameraNode::GetCameraRect() const
{
    return { m_Properties.GetPosition().x,
             m_Properties.GetPosition().y,
             m_Width,
             m_Height };
}

void CameraNode::SetViewPosition(Scene* pScene)
{
    // If there is a target, make sure target is in the center of the camera
    if (m_pTarget)
    {
        Point targetPos = m_pTarget->VGetProperties()->GetPosition();
        // Center camera
        Point cameraPos = targetPos - Point(m_Width / 2, m_Height / 2);
        VSetPosition(cameraPos);
    }
}
