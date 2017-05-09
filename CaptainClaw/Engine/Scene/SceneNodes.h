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

    virtual void VSetPosition(Point& position) = 0;
    
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

    virtual void VSetPosition(Point& position) { m_Properties.m_Position = position; }
    Point GetPosition() { return m_Properties.m_Position; }

    int32 GetOrientation() const { return m_Properties.m_Orientation; }

    virtual int32 GetZCoord() const { return m_Properties.m_ZCoord; }

    virtual void SortChildrenByZCoord();

protected:
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
    virtual bool VIsVisible(Scene* pScene) { return true; }
};

class CameraNode : public SceneNode
{
public:
    CameraNode(Point position, uint32 width, uint32 height);

    virtual void VRender(Scene* pScene);
    virtual bool IsVisible(Scene* pScene) const { return m_Active; }

    Point GetPosition() { return m_Properties.GetPosition() + m_CameraOffset; }

    void SetViewPosition(Scene* pScene);
    void SetTarget(shared_ptr<SceneNode> pTarget) { m_pTarget = pTarget; }
    shared_ptr<SceneNode> GetTarget() { return m_pTarget; }
    void ClearTarget() { m_pTarget = shared_ptr<SceneNode>(); }

    void SetSize(uint32 width, uint32 height) { m_Width = width; m_Height = height; }

    uint32 GetWidth() { return m_Width; }
    uint32 GetHeight() { return m_Height; }

    // TODO: Calc this only when changes are performed, do not recalc it every single time
    // since this function is called A LOT
    inline SDL_Rect GetCameraRect() const
    {
        return{ (int)(m_Properties.GetPosition().x + m_CameraOffset.x),
            (int)(m_Properties.GetPosition().y + m_CameraOffset.y),
            (int)(m_Width / m_ScaleX),
            (int)(m_Height / m_ScaleY) };
    }

    inline double GetCameraOffsetX() { return m_CameraOffset.x; }
    inline double GetCameraOffsetY() { return m_CameraOffset.y; }

    inline void SetCameraOffsetX(double offX) { m_CameraOffset.x = offX; }
    inline void SetCameraOffsetY(double offY) { m_CameraOffset.y = offY; }

    inline void AddCameraOffsetX(double offset) { m_CameraOffset.x += offset; }
    inline void AddCameraOffsetY(double offset) { m_CameraOffset.y += offset; }

    inline void SetCameraOffset(double offX, double offY) { m_CameraOffset.Set(offX, offY); }

    bool IntersectsWithPoint(const Point& point, float cameraScale = 1.0f);

    inline Point GetCenterPosition() 
    {
        return Point(
            m_Properties.GetPosition().x + m_CameraOffset.x + m_Width / 2, 
            m_Properties.GetPosition().y + m_CameraOffset.y + m_Height / 2);
    }

protected:
    uint32      m_Width;
    uint32      m_Height;
    Point       m_CameraOffset;
    bool        m_Active;
    bool        m_DebugCamera;

    float m_ScaleX;
    float m_ScaleY;

    shared_ptr<SceneNode> m_pTarget;
};

#endif //__SCENENODE_H__