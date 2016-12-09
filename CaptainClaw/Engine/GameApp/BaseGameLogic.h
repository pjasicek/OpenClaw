#ifndef __BASE_GAME_LOGIC__
#define __BASE_GAME_LOGIC__

#include "../SharedDefines.h"
#include "../Process/ProcessMgr.h"
#include "../Actor/Actor.h"

enum GameState
{
    GameState_Invalid,
    GameState_Initializing,
    GameState_MainMenu,
    GameState_OptionsMenu,
    GameState_SelectLevelMenu,
    GameState_LoadSaveMenu,
    GameState_LoadingLevel,
    GameState_IngameRunning,
    GameState_IngamePaused,
    GameState_FinishedLevel,
    GameState_Cutscene
};

typedef std::map<uint32, StrongActorPtr> ActorMap;

class GameSaveMgr;
class LevelData;
class ActorFactory;
class BaseGameApp;
class BaseGameLogic : public IGameLogic
{
    // This is just to give game app access to game views
    friend class BaseGameApp;

public:
    BaseGameLogic();
    virtual ~BaseGameLogic();

    bool Initialize();

    /*// IGameLogic interface
    virtual WeakActorPtr VGetActorPtr(const uint32 actorId);
    virtual StrongActorPtr VCreateActor(const std::string& xmlActorResource, TiXmlElement* overrides);
    virtual void VDestroyActor(const uint32 actorId);
    virtual bool VLoadGame(const char* xmlLevelResource);
    virtual void VSetProxy();
    virtual void VOnUpdate(uint32 msDiff);
    virtual void VChangeState(enum GameState newState);
    virtual void VMoveActor(const uint32 actorId, Point newPosition);
    virtual shared_ptr<IGamePhysics> VGetGamePhysics();

    // BaseGameLogic*/

    // View management
    virtual void VAddView(shared_ptr<IGameView> pView, uint32 actorId = INVALID_ACTOR_ID);
    virtual void VRemoveView(shared_ptr<IGameView> pView);

    // Actor management
    virtual StrongActorPtr VCreateActor(const std::string& xmlActorResource, TiXmlElement* overrides);
    virtual StrongActorPtr VCreateActor(TiXmlElement* pActorRoot, TiXmlElement* overrides);
    virtual void VDestroyActor(const uint32 actorId);
    virtual WeakActorPtr VGetActor(const uint32 actorId);
    virtual void VModifyActor(const uint32 actorId, TiXmlElement* overrides);

    virtual void VMoveActor(const uint32_t actorId, Point newPosition) { }

    std::string GetActorXml(uint32 actorId);

    // Level management

    // Subclasses can't override this function, they have to use VLoadGameDelegate() instead
    virtual bool VLoadGame(const char* xmlLevelResource);
    virtual void VSetProxy();

    // Logic update
    virtual void VOnUpdate(uint32 msDiff);

    // Changing game logic state
    virtual void VChangeState(GameState newState);
    const GameState GetGameState() const { return m_GameState; }

    // Render diagnostics
    void ToggleRenderDiagnostics() { m_RenderDiagnostics = !m_RenderDiagnostics; }
    virtual void VRenderDiagnostics(SDL_Renderer* pRenderer, shared_ptr<CameraNode> pCamera);
    virtual shared_ptr<IGamePhysics> VGetGamePhysics() { return m_pPhysics; }

    void AttachProcess(StrongProcessPtr pProcess) { if (m_pProcessMgr) { m_pProcessMgr->AttachProcess(pProcess); } }

    shared_ptr<GameSaveMgr> GetGameSaveMgr() { return m_pGameSaveMgr; }

protected:
    virtual ActorFactory* VCreateActorFactory();

    virtual bool VLoadGameDelegate(TiXmlElement* pLevelData) { return true; }

    void MoveActorDelegate(IEventDataPtr pEventData);
    void RequestNewActorDelegate(IEventDataPtr pEventData);
    void CollideableTileCreatedDelegate(IEventDataPtr pEventData);
    void RequestDestroyActorDelegate(IEventDataPtr pEventData);

    uint32 m_Lifetime;
    ProcessMgr* m_pProcessMgr;
    ActorMap m_ActorMap;
    uint32 m_LastActorId;
    GameState m_GameState;

    int m_HumanPlayersAttached;
    int m_HumanGamesLoaded;
    int m_AIPlayersAttached;

    GameViewList m_GameViews;
    ActorFactory* m_pActorFactory;

    bool m_Proxy;

    bool m_RenderDiagnostics;
    shared_ptr<IGamePhysics> m_pPhysics;

    shared_ptr<LevelData> m_pCurrentLevel;

    shared_ptr<GameSaveMgr> m_pGameSaveMgr;

private:
    void RegisterAllDelegates();
    void RemoveAllDelegates();
};

//=====================================================================================================================
// class LeveData
//=====================================================================================================================

struct TileCollisionRectangle
{
    CollisionType collisionType;
    SDL_Rect collisionRect;
};

struct TileCollisionPrototype
{
    int32 id;
    uint32 width;
    uint32 height;
    std::vector<TileCollisionRectangle> collisionRectangles;
};

struct TileRect
{
    int32 left;
    int32 top;
    int32 right;
    int32 bottom;
};

struct TileDescription
{
    int32 tileId;
    int32 type;
    uint32 width;
    uint32 height;
    int32 insideAttrib;
    int32 outsideAttrib;
    TileRect rect;
};

typedef std::map<int32, TileDescription> TileDescriptionMap;
typedef std::map<int32, TileCollisionPrototype> TileCollisionPrototypeMap;

// Class containing level (meta)data
class LevelData
{
    friend class BaseGameLogic;

public:
    LevelData()
    {
        m_LevelName = "Unknown";
        m_LevelAuthor = "Unknown";
        m_LevelCreatedDate = "Unknown";
    }

    std::string GetLevelName() const { return m_LevelName; }
    std::string GetLevelAuthor() const { return m_LevelAuthor; }
    std::string GetLevelCreatedDate() const { return m_LevelCreatedDate; }

private:
    std::string m_LevelName;
    std::string m_LevelAuthor;
    std::string m_LevelCreatedDate;

    TileDescriptionMap m_TileDescriptionMap;
    TileCollisionPrototypeMap m_TileCollisionPrototypeMap;
};

#endif