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

    RenderPass GetRenderPass() const { return m_RenderPass; }

protected:
    uint32          m_ActorId;
    std::string     m_Name;
    Point           m_Position;
    RenderPass      m_RenderPass;
    int32           m_Orientation;
};

typedef std::vector <shared_ptr<ISceneNode>> SceneNodeList;

class SceneNode : public ISceneNode
{
public:
    SceneNode(uint32 actorId, BaseRenderComponent* renderComponent, RenderPass renderPass, Point position);
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

    void SetViewPosition(Scene* pScene);
    void SetTarget(shared_ptr<SceneNode> pTarget) { m_pTarget = pTarget; }
    shared_ptr<SceneNode> GetTarget() { return m_pTarget; }
    void ClearTarget() { m_pTarget = shared_ptr<SceneNode>(); }

    void SetSize(uint32 width, uint32 height) { m_Width = width; m_Height = height; }

    uint32 GetWidth() { return m_Width; }
    uint32 GetHeight() { return m_Height; }

    SDL_Rect GetCameraRect() const;

    int32 GetCameraOffsetX() { return m_OffsetX; }
    int32 GetCameraOffsetY() { return m_OffsetY; }
    void SetCameraOffsetX(int32 offX) { m_OffsetX = offX; }
    void SetCameraOffsetY(int32 offY) { m_OffsetY = offY; }
    void SetCameraOffset(int32 offX, int32 offY) { m_OffsetX = offX; m_OffsetY = offY; }

protected:
    uint32      m_Width;
    uint32      m_Height;
    uint32      m_OffsetX;
    uint32      m_OffsetY;
    bool        m_Active;
    bool        m_DebugCamera;

    shared_ptr<SceneNode> m_pTarget;
};

#endif //__SCENENODE_H__