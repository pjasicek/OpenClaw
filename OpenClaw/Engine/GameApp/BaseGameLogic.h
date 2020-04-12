#ifndef __BASE_GAME_LOGIC__
#define __BASE_GAME_LOGIC__

#include "../SharedDefines.h"
#include "../Process/ProcessMgr.h"
#include "../Actor/Actor.h"
#include "CommandHandler.h"

typedef std::map<uint32, StrongActorPtr> ActorMap;

class GameSaveMgr;
class LevelData;
class ActorFactory;
class BaseGameApp;
class BaseGameLogic : public IGameLogic
{
    // This is just to give game app access to game views
    friend class BaseGameApp;

    // Command handler should have unlimited access
    friend class CommandHandler;

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
    virtual bool VLoadScoreScreen(const char* xmlScoreScreenResource);
    virtual bool VEnterMenu(const char* xmlMenuResource);
    virtual void VSetProxy();

    // Logic update
    virtual void VOnUpdate(uint32 msDiff);

    // Changing game logic state
    virtual void VChangeState(GameState newState);
    const GameState GetGameState() const { return m_GameState; }

    // ???
    virtual void VResetLevel();

    // Render diagnostics
    void ToggleRenderDiagnostics() { m_RenderDiagnostics = !m_RenderDiagnostics; }
    virtual void VRenderDiagnostics(SDL_Renderer* pRenderer, shared_ptr<CameraNode> pCamera);
    virtual shared_ptr<IGamePhysics> VGetGamePhysics() { return m_pPhysics; }

    void AttachProcess(StrongProcessPtr pProcess) { if (m_pProcessMgr) { m_pProcessMgr->AttachProcess(pProcess); } }

    shared_ptr<LevelData> GetCurrentLevelData() { return m_pCurrentLevel; }
    shared_ptr<GameSaveMgr> GetGameSaveMgr() { return m_pGameSaveMgr; }

    void UnloadLevel();
    void SetLevelData(shared_ptr<LevelData> pLevelData) { m_pCurrentLevel = pLevelData; }

    void SetRunning(bool running) { m_bRunning = running; }
    bool IsRunning() { return m_bRunning; }

    StrongActorPtr GetClawActor();

    StrongActorPtr FindActorByName(const std::string& name, bool bIsUnique);
    ActorList FindActorByName(const std::string& name);

protected:
    virtual ActorFactory* VCreateActorFactory();

    virtual bool VLoadGameDelegate(TiXmlElement* pLevelData) { return true; }

    void MoveActorDelegate(IEventDataPtr pEventData);
    void RequestNewActorDelegate(IEventDataPtr pEventData);
    void CollideableTileCreatedDelegate(IEventDataPtr pEventData);
    void CreateStaticGeometryDelegate(IEventDataPtr pEventData);
    void RequestDestroyActorDelegate(IEventDataPtr pEventData);
    void ItemPickedUpDelegate(IEventDataPtr pEventData);
    void FinishedLevelDelegate(IEventDataPtr pEventData);
    void ActorEnteredBossAreaDelegate(IEventDataPtr pEventData);
    void BossFightStartedDelegate(IEventDataPtr pEventData);
    void IngameMenuEndLifeDelegate(IEventDataPtr pEventData);
    void WorldFinishedLoadingDelegate(IEventDataPtr pEventData);

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
    bool m_bRunning;

    bool m_RenderDiagnostics;
    shared_ptr<IGamePhysics> m_pPhysics;
    shared_ptr<LevelData> m_pCurrentLevel;
    shared_ptr<GameSaveMgr> m_pGameSaveMgr;

    int m_SelectedLevel;

    Point m_CurrentSpawnPosition;

    WeakActorPtr m_pClawActor;

private:
    void ExecuteStartupCommands(const std::string& startupCommandsFile);
    void CreateSinglePhysicsTile(int x, int y, const TileCollisionPrototype& proto);
    //void LoadGameWorkerThread(const char* pXmlLevelPath, float* pProgress, bool* pRet);

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
    int tileId;
    int type;
    int width;
    int height;
    int insideAttrib;
    int outsideAttrib;
    TileRect rect;
};

typedef std::map<int32, TileDescription> TileDescriptionMap;
typedef std::map<int32, TileCollisionPrototype> TileCollisionPrototypeMap;
typedef std::map<PickupType, int> PickupMap;

// Class containing level (meta)data
class LevelData
{
    friend class BaseGameLogic;

public:
    LevelData(int levelNumber, bool isNewGame, int loadedCheckpoint)
    {
        m_bIsNewGame = isNewGame;
        m_LeveNumber = levelNumber;
        m_LoadedCheckpoint = loadedCheckpoint;
    }

    LevelData()
    {
        m_LevelName = "Unknown";
        m_LevelAuthor = "Unknown";
        m_LevelCreatedDate = "Unknown";

        m_bIsNewGame = true;
        m_LeveNumber = -1;
        m_LoadedCheckpoint = -1;
    }

    std::string GetLevelName() const { return m_LevelName; }
    std::string GetLevelAuthor() const { return m_LevelAuthor; }
    std::string GetLevelCreatedDate() const { return m_LevelCreatedDate; }
    uint32 GetLevelNumber() const { return m_LeveNumber; }
    uint32 GetLoadedCheckpointNumber() const { return m_LoadedCheckpoint; }

    const PickupMap* GetLootedItems() { return &m_LootedPickupsMap; }

private:
    std::string m_LevelName;
    std::string m_LevelAuthor;
    std::string m_LevelCreatedDate;

    bool m_bIsNewGame;
    uint32 m_LeveNumber;
    uint32 m_LoadedCheckpoint;

    TileDescriptionMap m_TileDescriptionMap;
    TileCollisionPrototypeMap m_TileCollisionPrototypeMap;

    // How many times were certain pickups picked up
    PickupMap m_LootedPickupsMap;
    PickupMap m_TotalPickupsMap;
};

#endif
