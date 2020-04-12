#ifndef RENDERCOMPONENT_H_
#define RENDERCOMPONENT_H_

#include "../../SharedDefines.h"
#include "../ActorComponent.h"

class Image;
typedef std::map<std::string, shared_ptr<Image>> ImageMap;

//=================================================================================================
// BaseRenderComponent Declaration
//

class PositionComponent;
class SceneNode;
class BaseRenderComponent : public ActorComponent
{
public:
    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;
    virtual void VPostInit() override;
    virtual void VOnChanged() override;

    weak_ptr<Image> GetImage(std::string imageName);
    weak_ptr<Image> GetImage(uint32 imageId);
    bool HasImage(std::string imageName);
    bool HasImage(int32 imageId);

    uint32 GetImagesCount() const { return m_ImageMap.size(); }

    // Gets actor's X-Y-W-H
    virtual SDL_Rect VGetPositionRect() = 0;

    shared_ptr<SceneNode> GetScneNodePublicTest() { return GetSceneNode(); }

protected:
    // loads the SceneNode specific data (represented in the <SceneNode> tag)
    virtual bool VDelegateInit(TiXmlElement* pData) { return true; }
    virtual shared_ptr<SceneNode> VCreateSceneNode(void) = 0;  // factory method to create the appropriate scene node

    // editor stuff
    virtual TiXmlElement* VCreateBaseElement(void) { return NULL; /*return new TiXmlElement(VGetName());*/ }
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement) = 0;

    ImageMap m_ImageMap;

    shared_ptr<SceneNode> m_pSceneNode;

    PositionComponent* m_pPositionComponent;

private:
    shared_ptr<SceneNode> GetSceneNode();
};

//=================================================================================================


//=================================================================================================
// ActorRenderComponent Declaration
//

class ActorRenderComponent : public BaseRenderComponent
{
public:
    ActorRenderComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pXmlData) override;

    virtual SDL_Rect VGetPositionRect() override;

    // It was very expensive set of functions. ALL WORLD ACTORS (even invisible) each time
    // try to find an image in the image map.
    // Now it is lazy functions. It does not update image until a GetCurrentImage call.
    weak_ptr<Image> GetCurrentImage();
    void SetImage(std::string imageName);

    void SetMirrored(bool mirrored) { m_IsMirrored = mirrored; }

    inline void SetVisible(bool visible) { m_IsVisible = visible; }
    inline bool IsVisible() { return m_IsVisible; }
    inline bool IsMirrored() { return m_IsMirrored; }
    inline bool IsInverted() { return m_IsInverted; }
    inline int GetAlpha() { return m_Alpha; }
    inline void SetAlpha(int alpha) { m_Alpha = alpha; }
    inline SDL_Color GetColorMod() { return m_ColorMod; }
    void SetColorMod(const SDL_Color& color) { m_ColorMod = color; }

protected:
    virtual shared_ptr<SceneNode> VCreateSceneNode() override;

    // Editor stuff
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement) override;

    void UpdateCurrentImage();

    shared_ptr<Image> m_CachedImage;
    std::string m_CurrentImageName;
    bool m_IsCachedImageExpired;

private:
    bool m_IsVisible;
    bool m_IsMirrored;
    bool m_IsInverted;
    int m_Alpha;
    SDL_Color m_ColorMod;
    int32 m_ZCoord;
};

//=================================================================================================


//=================================================================================================
// TilePlaneRenderComponent Declaration
//

enum TilePlaneRenderPosition
{
    TilePlaneRenderPosition_Background,
    TilePlaneRenderPosition_Action,
    TilePlaneRenderPosition_Foreground
};

typedef std::vector<Image*> TileImageList;

struct TilePlaneProperties
{
    std::string name;

    int tilePixelWidth;
    int tilePixelHeight;

    int tilesOnAxisX;
    int tilesOnAxisY;

    int planePixelWidth;
    int planePixelHeight;

    int movementPercentX;
    int movementPercentY;

    int fillColor;

    int zCoord;

    bool isMainPlane;
    bool isDrawable;
    bool isWrappedX;
    bool isWrappedY;
    bool isTileAutosized;
};

// Helper struct
struct TileInfo
{
    int tileId;
    int x;
    int y;
};

typedef std::vector<int> TileList;
class TilePlaneRenderComponent : public BaseRenderComponent
{
public:
    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pXmlData) override;

    virtual SDL_Rect VGetPositionRect() override;

    const TilePlaneProperties* const GetTilePlaneProperties() const { return &m_PlaneProperties; }
    const TileImageList* const GetTileImageList() const { return &m_TileImageList; }

protected:
    virtual shared_ptr<SceneNode> VCreateSceneNode() override;

    // Editor stuff
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement) override;

private:
    void ProcessMainPlaneTiles(const TileList& tileList);
    TileList GetAllContinuousTiles(const TileList& tileList, int fromTileIdx);
    TileInfo GetTileInfo(const TileList& tileList, int tileIdx);

    // Background, action, foreground
    TilePlaneRenderPosition m_RenderLocation;

    std::string m_PlaneName;

    TilePlaneProperties m_PlaneProperties;

    // This is 1D array representing all tiles from top left to bottom right corner
    TileImageList m_TileImageList;

    // Fill color
    shared_ptr<Image> m_pFillImage;

    // How far does this plane span
    SDL_Rect m_PositionRect;
};

//=================================================================================================

//=================================================================================================
// HUDRenderComponent Declaration
//

class HUDRenderComponent : public ActorRenderComponent
{
public:
    HUDRenderComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }
    virtual bool VDelegateInit(TiXmlElement* pXmlData) override;

    virtual SDL_Rect VGetPositionRect() override;

    bool IsAnchoredRight() { return m_IsAnchoredRight; }
    bool IsAnchoredBottom() { return m_IsAnchoredBottom; }

protected:
    virtual shared_ptr<SceneNode> VCreateSceneNode() override;

    // Editor stuff
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement) override;

private:
    bool m_IsAnchoredRight;
    bool m_IsAnchoredBottom;
    std::string m_HUDElementKey;
};

//=================================================================================================

#endif
