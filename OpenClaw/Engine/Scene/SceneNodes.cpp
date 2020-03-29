#include <cmath>
#include "SceneNodes.h"
#include "Scene.h"
#include "../Actor/ActorComponent.h"
#include "../Actor/Components/RenderComponent.h"
#include "../GameApp/BaseGameApp.h"

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
    //LOG("Destroyed SceneNode: " + ToStr(m_Properties.GetActorId()));
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
    for (auto &child : m_ChildrenList)
    {
        child->VOnUpdate(pScene, msDiff);
    }
}

void SceneNode::VRenderChildren(Scene* pScene)
{
    for (auto &childNode : m_ChildrenList)
    {
        RenderNode(pScene, childNode);
    }
}

void SceneNode::RenderNode(Scene *pScene, std::shared_ptr<ISceneNode> &node) {
    if (node->VIsVisible(pScene))
    {
        node->VPreRender(pScene);
        node->VRender(pScene);
        node->VRenderChildren(pScene);
        node->VPostRender(pScene);
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
    const SDL_Rect position = m_pRenderComponent->VGetPositionRect();
    return SDL_HasIntersection(&cameraRect, &position);
}

bool SceneNode::VAddChild(shared_ptr<ISceneNode> ikid)
{
    m_ChildrenList.push_back(ikid);

    shared_ptr<SceneNode> kid = static_pointer_cast<SceneNode>(ikid);
    kid->SetParent(this);

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
    for (auto &pChildNode : m_ChildrenList)
    {
        pChildNode->SortChildrenByZCoord();
    }

    std::sort(m_ChildrenList.begin(), m_ChildrenList.end(), NodeCompare);
}

void SceneNode::VSetPosition(const Point &position) {
    if (m_pParent) {
        m_pParent->VOnBeforeChildrenModifyPosition(this, position);
    }
    m_Properties.m_Position = position;
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

//    shared_ptr<SceneNode> actorGroup(new SceneNode(INVALID_ACTOR_ID, NULL, RenderPass_Actor, { 0, 0 }));
    shared_ptr<SceneNode> actorGroup(new GridNode(RenderPass_Actor));
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
// GridNode Implementation
// This implementation uses actor positions to store nodes and reduce visibility checks
// It's useless for UI or another nodes storing without actor position.
// Each cell stores sorted collection of nodes.
// VOnUpdate() UPDATES ONLY VISIBLE NODES IN RANDOM ORDER!!! BE CAREFUL

// I hope map won't be greater than 1000 tiles. So it's about 1000 tiles * 64 px / 640 px ~ 100 cells for the worst cases
GridNode::GridNode(RenderPass renderPass) : m_CellWidth((int) (g_pApp->GetWindowSizeScaled().x * 1.3)),
                                            m_CellHeight((int) (g_pApp->GetWindowSizeScaled().y * 1.3)),
                                            m_MaxRowCount(100),
                                            m_MaxColumnCount(100),
                                            SceneNode(INVALID_ACTOR_ID, nullptr, renderPass, {0, 0}) {

}

bool GridNode::VAddChild(shared_ptr<ISceneNode> kid) {
    const uint32 actorId = kid->VGetProperties()->GetActorId();
    if (actorId == INVALID_ACTOR_ID) {
        LOG_WARNING("SceneNode without actor id will be added to GridNode");
        return SceneNode::VAddChild(kid);
    }
    const Point actorPos = kid->VGetProperties()->GetPosition();
    const SDL_Point cellPos = WorldToGridPosition((int) actorPos.x, (int) actorPos.y);
    AddToGrid(cellPos.x, cellPos.y, kid);
    return true;
}

SDL_Point GridNode::WorldToGridPosition(int x, int y) const {
    // Max column/row count prevents infinite memory growing if an actor falls out of bound
    return SDL_Point{
            std::min(max(x, 0) / m_CellWidth, m_MaxColumnCount),
            std::min(max(y, 0) / m_CellHeight, m_MaxRowCount)
    };
}

void GridNode::AddToGrid(int x, int y, shared_ptr<ISceneNode> &ikid) {
    if (m_Grid.size() <= x) {
        m_Grid.resize(x + 1);
    }
    auto &col = m_Grid[x];
    if (col.size() <= y) {
        col.resize(y + 1);
    }
    auto &cell = col[y];

    // Nodes in grid cells are always ordered
    cell.insert(
            std::lower_bound(cell.begin(), cell.end(), ikid, NodeCompare),
            ikid
    );

    shared_ptr<SceneNode> kid = static_pointer_cast<SceneNode>(ikid);
    kid->SetParent(this);
}

bool GridNode::VRemoveChild(uint32 actorId) {
    // TODO: Expensive
    for (auto &column : m_Grid) {
        for (auto &nodes : column) {
            for (auto it = nodes.begin(); it != nodes.end(); ++it) {
                if ((*it)->VGetProperties()->GetActorId() == actorId) {
                    nodes.erase(it);
                    return true;
                }
            }
        }
    }
    return SceneNode::VRemoveChild(actorId);
}


void GridNode::VRenderChildren(Scene* pScene)
{
    shared_ptr<CameraNode> pCamera = pScene->GetCamera();
    if (!pCamera)
    {
        LOG_ERROR("Checking visibility without available scene camera");
        return;
    }

    // Render grid elements. We need to find visible cells, collect nodes, sort it and render
    const SDL_Rect intersect = GridIntersection(pCamera->GetCameraRect());

    int nodeCount = 0;
    for (int x = intersect.x; x < intersect.x + intersect.w && x < m_Grid.size(); ++x) {
        auto &column = m_Grid[x];
        for (int y = intersect.y; y < intersect.y + intersect.h && y < column.size(); ++y) {
            nodeCount += column[y].size();
        }
    }

    std::vector<shared_ptr<ISceneNode>> intersectedNodes;
    intersectedNodes.reserve(nodeCount);
    for (int x = intersect.x; x < intersect.x + intersect.w && x < m_Grid.size(); ++x) {
        auto &column = m_Grid[x];
        for (int y = intersect.y; y < intersect.y + intersect.h && y < column.size(); ++y) {
            auto &nodes = column[y];
            if (!nodes.empty()) {
                // Insert sorted vector to the end of sorted vector
                int oldSize = intersectedNodes.size();
                intersectedNodes.insert(intersectedNodes.end(), nodes.begin(), nodes.end());
                // Sort resulted vector
                std::inplace_merge(intersectedNodes.begin(), intersectedNodes.begin() + oldSize, intersectedNodes.end(), NodeCompare);
            }
        }
    }
    for (auto &node : intersectedNodes) {
        RenderNode(pScene, node);
    }

    // Render another
    SceneNode::VRenderChildren(pScene);
}

SDL_Rect GridNode::GridIntersection(const SDL_Rect &cameraRect) const {
    const SDL_Point start = WorldToGridPosition(cameraRect.x, cameraRect.y);
    const SDL_Point end = WorldToGridPosition(cameraRect.x + cameraRect.w, cameraRect.y + cameraRect.h);
    return SDL_Rect{max(start.x - 1, 0), max(start.y - 1, 0), end.x - start.x + 3, end.y - start.y + 3};
}

void GridNode::VOnUpdate(Scene *pScene, uint32 msDiff) {
    shared_ptr<CameraNode> pCamera = pScene->GetCamera();
    if (!pCamera)
    {
        LOG_ERROR("Update node without available scene camera");
        return;
    }

    // NOW UPDATE ONLY VISIBLE CELLS IN RANDOM ORDER!!!
    const SDL_Rect intersect = GridIntersection(pCamera->GetCameraRect());
    for (int x = intersect.x; x < intersect.x + intersect.w && x < m_Grid.size(); ++x) {
        auto &column = m_Grid[x];
        for (int y = intersect.y; y < intersect.y + intersect.h && y < column.size(); ++y) {
            auto &nodes = column[y];
            for (auto &node : nodes) {
                node->VOnUpdate(pScene, msDiff);
            }
        }
    }

    SceneNode::VOnUpdate(pScene, msDiff);
}

void GridNode::SortChildrenByZCoord() {
    // Grid is always sorted
    SceneNode::SortChildrenByZCoord();
}

// Move node to another cell if it's needed
void GridNode::VOnBeforeChildrenModifyPosition(SceneNode *children, const Point &position) {
    uint32 actorId = children->VGetProperties()->GetActorId();
    if (actorId != INVALID_ACTOR_ID) {
        const Point oldPos = children->VGetProperties()->GetPosition();
        const SDL_Point oldGridPos = WorldToGridPosition((int) oldPos.x, (int) oldPos.y);

        const SDL_Point newGridPos = WorldToGridPosition((int) position.x, (int) position.y);
        if (oldGridPos.x != newGridPos.x || oldGridPos.y != newGridPos.y) {
            shared_ptr<ISceneNode> node = RemoveFromGrid(oldGridPos.x, oldGridPos.y, actorId);
            if (node) {
                AddToGrid(newGridPos.x, newGridPos.y, node);
            } else {
                LOG_ERROR("Scene node was not found in GridNode!")
                SceneNode::VOnBeforeChildrenModifyPosition(children, position);
            }
        }
    }
}

shared_ptr<ISceneNode> GridNode::RemoveFromGrid(int x, int y, uint32 actorId) {
    if (m_Grid.size() > x) {
        auto &col = m_Grid[x];
        if (col.size() > y) {
            auto &cell = col[y];
            for (auto it = cell.begin(); it != cell.end(); ++it) {
                if ((*it)->VGetProperties()->GetActorId() == actorId) {
                    shared_ptr<ISceneNode> node = *it;
                    it = cell.erase(it);
                    return node;
                }
            }
        }
    }
    return shared_ptr<ISceneNode>();
}

//=================================================================================================
// CameraNode Implementation
//

CameraNode::CameraNode(Point position, uint32 width, uint32 height)
    : SceneNode(INVALID_ACTOR_ID, NULL, RenderPass_0, position),
    m_Active(true),
    m_DebugCamera(false)
{
    m_Width = width;
    m_Height = height;

    Point scale = g_pApp->GetScale();
    m_ScaleX = (float)scale.x;
    m_ScaleY = (float)scale.y;
}

void CameraNode::VRender(Scene* pScene)
{
    if (m_DebugCamera)
    {
        // Some debug maybe
    }
}

void CameraNode::SetViewPosition(Scene* pScene)
{
    // If there is a target, make sure target is in the center of the camera
    if (m_pTarget)
    {
        Point targetPos = m_pTarget->VGetProperties()->GetPosition();
        // Center camera
        Point scale = g_pApp->GetScale();
        Point cameraPos = targetPos - Point((m_Width / 2) / scale.x, (m_Height / 2) / scale.y);
        VSetPosition(cameraPos);
    }
}

bool CameraNode::IntersectsWithPoint(const Point& point, float cameraScale) const
{
    SDL_Rect cameraRect = GetCameraRect();

    // If the projectile is out of camera bounds, destroy it
    SDL_Rect scaledCameraRect;
    scaledCameraRect.x = cameraRect.x - cameraRect.w * (cameraScale - 1.0f);
    scaledCameraRect.y = cameraRect.y - cameraRect.h * (cameraScale - 1.0f);
    scaledCameraRect.w = cameraRect.w + cameraRect.w * ((cameraScale - 1.0f) * 2);
    scaledCameraRect.h = cameraRect.h + cameraRect.h * ((cameraScale - 1.0f) * 2);

    if ((point.x < scaledCameraRect.x) ||
        (point.x >(scaledCameraRect.x + scaledCameraRect.w)) ||
        (point.y < scaledCameraRect.y) ||
        (point.y >(scaledCameraRect.y + scaledCameraRect.h)))
    {
        return false;
    }

    return true;
}