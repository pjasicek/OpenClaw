#ifndef __SCENENODE_H__
#define __SCENENODE_H__

#include "../SharedDefines.h"

enum RenderPass
{
    RenderPass_0,
    RenderPass_Background = RenderPass_0,
    RenderPass_Action,
    RenderPass_Actor,
    RenderPass_Foreground,
    RenderPass_HUD,
    RenderPass_NotRendered,
    RenderPass_Last
};

// Forward declarations
class SceneNode;
class Scene;
class MovementController;
class BaseRenderComponent;
class SceneNodeProperties;

class ISceneNode
{
public:
    virtual const SceneNodeProperties* const VGetProperties() const = 0;

    virtual void VOnUpdate(Scene* pScene, uint32 msDiff) = 0;
    
    virtual void VPreRender(Scene* pScene) = 0;
    virtual void VRender(Scene* pScene) = 0;
    virtual void VRenderChildren(Scene* pScene) = 0;
    virtual void VPostRender(Scene* pScene) = 0;
    
    virtual bool VIsVisible(Scene* pScene) const = 0;

    virtual void VSetPosition(const Point& position) = 0;
    
    virtual bool VAddChild(shared_ptr<ISceneNode> kid) = 0;
    virtual bool VRemoveChild(uint32 actorId) = 0;
    virtual bool VOnLostDevice(Scene* pScene) = 0;

    virtual void SortChildrenByZCoord() = 0;
    virtual int32 GetZCoord() const = 0;
};

class SceneNodeProperties
{
    friend class SceneNode;

public:
    SceneNodeProperties();
    const uint32& GetActorId() const { return m_ActorId; }
    const char* GetName() const { return m_Name.c_str(); }
    const Point& GetPosition() const { return m_Position; }
    const int32 GetOrientation() const { return m_Orientation; }
    const int32 GetZCoord() const { return m_ZCoord; }

    RenderPass GetRenderPass() const { return m_RenderPass; }

protected:
    uint32          m_ActorId;
    std::string     m_Name;
    Point           m_Position;
    RenderPass      m_RenderPass;
    int32           m_Orientation;
    int32           m_ZCoord;
};

typedef std::vector <shared_ptr<ISceneNode>> SceneNodeList;

class SceneNode : public ISceneNode
{
public:
    SceneNode(uint32 actorId, BaseRenderComponent* renderComponent, RenderPass renderPass, Point position, int32 zCoord = 0);
    virtual ~SceneNode();

    virtual const SceneNodeProperties* const VGetProperties() const { return &m_Properties; }

    virtual void VOnUpdate(Scene* pScene, uint32 msDiff);

    virtual void VPreRender(Scene* pScene) { }
    virtual void VRender(Scene* pScene) { }
    virtual void VPostRender(Scene* pScene) { }
    virtual void VRenderChildren(Scene* pScene);

    virtual bool VIsVisible(Scene* pScene) const;

    virtual bool VAddChild(shared_ptr<ISceneNode> kid);
    virtual bool VRemoveChild(uint32 actorId);
    virtual bool VOnLostDevice(Scene* pScene);

    void VSetPosition(const Point& position) override;
    Point GetPosition() { return m_Properties.m_Position; }

    void SetParent(SceneNode *node) { m_pParent = node; }
    SceneNode* GetParent() { return m_pParent; }

    int32 GetOrientation() const { return m_Properties.m_Orientation; }

    virtual int32 GetZCoord() const { return m_Properties.m_ZCoord; }

    virtual void SortChildrenByZCoord();

protected:
    virtual void VOnBeforeChildrenModifyPosition(SceneNode *children, const Point &position) {}

    void RenderNode(Scene* pScene, std::shared_ptr<ISceneNode> &node);

    static bool NodeCompare(const shared_ptr<ISceneNode> &lhs, const shared_ptr<ISceneNode> &rhs) {
        return lhs->GetZCoord() < rhs->GetZCoord();
    }

    SceneNodeList           m_ChildrenList;
    SceneNode*              m_pParent;
    SceneNodeProperties     m_Properties;
    BaseRenderComponent*    m_pRenderComponent;
};

typedef std::map<uint32, shared_ptr<ISceneNode>> SceneActorMap;

class RootNode : public SceneNode
{
public:
    RootNode();
    virtual bool VAddChild(shared_ptr<ISceneNode> kid);
    virtual void VRenderChildren(Scene* pScene);
    virtual bool VRemoveChild(uint32 actorId);
    virtual bool VIsVisible(Scene* pScene) const override { return true; }
};

class GridNode : public SceneNode
{
public:
    GridNode(RenderPass renderPass);
    bool VAddChild(shared_ptr<ISceneNode> kid) override;
    void VRenderChildren(Scene* pScene) override;
    bool VRemoveChild(uint32 actorId) override;
    bool VIsVisible(Scene* pScene) const override { return true; }

    void VOnUpdate(Scene* pScene, uint32 msDiff) override;

    void SortChildrenByZCoord() override;

protected:
    void VOnBeforeChildrenModifyPosition(SceneNode *children, const Point &position) override;
    SDL_Point WorldToGridPosition(int x, int y) const;
    SDL_Rect GridIntersection(const SDL_Rect &cameraRect) const;
    void AddToGrid(int x, int y, shared_ptr<ISceneNode> &kid);
    shared_ptr<ISceneNode> RemoveFromGrid(int x, int y, uint32 actorId);

    std::vector<std::vector<SceneNodeList>> m_Grid; // m_Grid[x][y]
    const int m_CellWidth, m_CellHeight;
    const int m_MaxRowCount, m_MaxColumnCount;
};

class CameraNode : public SceneNode
{
public:
    CameraNode(Point position, uint32 width, uint32 height);

    void VRender(Scene* pScene) override;
    bool VIsVisible(Scene* pScene) const override { return m_Active; }

    void VSetPosition(const Point &position) override {
        SceneNode::VSetPosition(position);
        CalcCameraRect();
    }
    // Overridden but non-virtual. Be careful with it
    Point GetPosition() { return m_Properties.GetPosition() + m_CameraOffset; }

    void SetViewPosition(Scene* pScene);
    void SetTarget(shared_ptr<SceneNode> pTarget) { m_pTarget = pTarget; }
    shared_ptr<SceneNode> GetTarget() { return m_pTarget; }
    void ClearTarget() { m_pTarget = shared_ptr<SceneNode>(); }

    void SetSize(uint32 width, uint32 height) { m_Width = width; m_Height = height; CalcCameraRect(); }
    void SetScale(float scaleX, float scaleY) { m_ScaleX = scaleX; m_ScaleY = scaleY; }

    uint32 GetWidth() { return m_Width; }
    uint32 GetHeight() { return m_Height; }

    // Calc this only when changes are performed, and do not recalc it every single time
    // since this function is called A LOT
    inline SDL_Rect GetCameraRect() const
    {
        return m_CachedCameraRect;
    }

    inline double GetCameraOffsetX() const { return m_CameraOffset.x; }
    inline double GetCameraOffsetY() const { return m_CameraOffset.y; }

    inline void SetCameraOffsetX(double offX) { SetCameraOffset(offX, m_CameraOffset.y); }
    inline void SetCameraOffsetY(double offY) { SetCameraOffset(m_CameraOffset.x, offY); }

    inline void AddCameraOffsetX(double offset) { SetCameraOffset(m_CameraOffset.x + offset, m_CameraOffset.y); }
    inline void AddCameraOffsetY(double offset) { SetCameraOffset(m_CameraOffset.x, m_CameraOffset.y + offset); }

    inline void SetCameraOffset(double offX, double offY) { m_CameraOffset.Set(offX, offY); CalcCameraRect(); }

    bool IntersectsWithPoint(const Point& point, float cameraScale = 1.0f) const;

    inline Point GetCenterPosition() const
    {
        return Point(
            m_Properties.GetPosition().x + m_CameraOffset.x + m_Width / 2, 
            m_Properties.GetPosition().y + m_CameraOffset.y + m_Height / 2);
    }

protected:
    void CalcCameraRect() {
        m_CachedCameraRect = {(int) (m_Properties.GetPosition().x + m_CameraOffset.x),
                              (int) (m_Properties.GetPosition().y + m_CameraOffset.y),
                              (int) (m_Width / m_ScaleX),
                              (int) (m_Height / m_ScaleY)};
    }

    uint32      m_Width;
    uint32      m_Height;
    Point       m_CameraOffset;
    bool        m_Active;
    bool        m_DebugCamera;

    float m_ScaleX;
    float m_ScaleY;

    shared_ptr<SceneNode> m_pTarget;
    SDL_Rect m_CachedCameraRect;
};

#endif //__SCENENODE_H__
