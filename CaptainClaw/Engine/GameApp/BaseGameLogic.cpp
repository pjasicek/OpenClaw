#include "../Actor/ActorFactory.h"
#include "../UserInterface/HumanView.h"
#include "../Events/Events.h"
#include "../Resource/Loaders/XmlLoader.h"
#include "../Resource/Loaders/PalLoader.h"
#include "../Events/EventMgr.h"

#include "GameSaves.h"
#include "BaseGameLogic.h"

#include <algorithm>

//=================================================================================================
//
// BaseGameLogic implementation
//
//=================================================================================================

BaseGameLogic::BaseGameLogic()
{
    m_Lifetime = 0;
    m_pProcessMgr = NULL;
    m_LastActorId = 0;
    m_GameState = GameState_Initializing;
    m_HumanPlayersAttached = 0;
    m_AIPlayersAttached = 0;
    m_HumanGamesLoaded = 0;
    m_pActorFactory = NULL;
    m_Proxy = false;
    m_RenderDiagnostics = true;

    m_pGameSaveMgr.reset(new GameSaveMgr());

    //RegisterEngineScriptEvents();
    RegisterAllDelegates();
}

BaseGameLogic::~BaseGameLogic()
{
    while (!m_GameViews.empty())
    {
        m_GameViews.pop_front();
    }

    SAFE_DELETE(m_pProcessMgr);
    SAFE_DELETE(m_pActorFactory);

    // Destroy all actors
    for (auto actorIter : m_ActorMap)
    {
        actorIter.second->Destroy();
    }
    m_ActorMap.clear();

    RemoveAllDelegates();

    // TODO: Remove this after its tested
    TiXmlDocument saveGamesDoc;
    saveGamesDoc.LinkEndChild(m_pGameSaveMgr->ToXml());
    saveGamesDoc.SaveFile("SAVES_test.XML");
}

bool BaseGameLogic::Initialize()
{
    m_pActorFactory = VCreateActorFactory();

    TiXmlDocument gameSaves("SAVES.XML");
    gameSaves.LoadFile();
    if (gameSaves.Error())
    {
        LOG_ERROR("Error while loading SAVES.XML: " + std::string(gameSaves.ErrorDesc()));
        return false;
    }

    if (!m_pGameSaveMgr->Initialize(gameSaves.RootElement()))
    {
        return false;
    }

    return true;
}

std::string BaseGameLogic::GetActorXml(uint32 actorId)
{
    StrongActorPtr pActor = MakeStrongPtr(VGetActor(actorId));
    if (pActor)
    {
        return pActor->ToXML();
    }
    else
    {
        LOG_ERROR("Could not find actor: " + ToStr(actorId));
    }

    return "";
}

bool BaseGameLogic::VLoadGame(const char* xmlLevelResource)
{
    PROFILE_CPU("GAME LOADING");
    PROFILE_MEMORY("GAME LOADING");

    m_pCurrentLevel.reset(new LevelData);

    // TODO: This should not be here. This will be set when we select level from GUI
    m_pCurrentLevel->m_LeveNumber = 1;
    m_pCurrentLevel->m_LoadedCheckpoint = 0;

    // Level is going to be loaded from XML WWD
    TiXmlElement* pXmlLevelRoot = XmlResourceLoader::LoadAndReturnRootXmlElement(xmlLevelResource, true);
    if (pXmlLevelRoot == NULL)
    {
        LOG_ERROR("Could not load level resource file: " + std::string(xmlLevelResource));
        return false;
    }

    // Get level palette
    TiXmlElement* pLevelProperties = pXmlLevelRoot->FirstChildElement("LevelProperties");
    if (!pLevelProperties)
    {
        LOG_ERROR("Level: " + std::string(xmlLevelResource) + " does not have level properties node.");
        return false;
    }

    if (TiXmlElement* pLevelNameElem = pLevelProperties->FirstChildElement("LevelName"))
    {
        m_pCurrentLevel->m_LevelName = pLevelNameElem->GetText();
    }
    if (TiXmlElement* pLevelAuthorElem = pLevelProperties->FirstChildElement("Author"))
    {
        m_pCurrentLevel->m_LevelAuthor = pLevelAuthorElem->GetText();
    }
    if (TiXmlElement* pLevelCreatedDateElem = pLevelProperties->FirstChildElement("Created"))
    {
        m_pCurrentLevel->m_LevelCreatedDate = pLevelCreatedDateElem->GetText();
    }

    // Tile descriptions
    if (TiXmlElement* pTileDescRootElem = pLevelProperties->FirstChildElement("TileDescriptions"))
    {
        for (TiXmlElement* pTileDescElem = pTileDescRootElem->FirstChildElement("TileDescription");
            pTileDescElem; pTileDescElem = pTileDescElem->NextSiblingElement("TileDescription"))
        {
            
            // Maybe code more deffensively here, revisit in future probably.. right now I dont want to
            // add 10000 conditions to assert correct xml format

            // TileDescription will maybe be used by editor, it is not used directly by game
            //    but only to parse TileCollisionPrototype from it which is used by physics subsystem
            TileDescription tileDesc;
            tileDesc.tileId = std::stoi(pTileDescElem->FirstChildElement("TileId")->GetText());

            TiXmlElement* pTileSizeElem = pTileDescElem->FirstChildElement("Size");
            tileDesc.width = std::stoi(pTileSizeElem->Attribute("width"));
            tileDesc.height = std::stoi(pTileSizeElem->Attribute("height"));

            if (std::string(pTileDescElem->FirstChildElement("Type")->GetText()) == "single")
            {
                tileDesc.type = WAP_TILE_TYPE_SINGLE;
            }
            else
            {
                tileDesc.type = WAP_TILE_TYPE_DOUBLE;
                tileDesc.outsideAttrib = std::stoi(pTileDescElem->FirstChildElement("OutsideAttrib")->GetText());
            }
            tileDesc.insideAttrib = std::stoi(pTileDescElem->FirstChildElement("InsideAttrib")->GetText());
            
            TiXmlElement* pTileRectElem = pTileDescElem->FirstChildElement("TileRect");
            tileDesc.rect.left = std::stoi(pTileRectElem->Attribute("left"));
            tileDesc.rect.top = std::stoi(pTileRectElem->Attribute("top"));
            tileDesc.rect.right = std::stoi(pTileRectElem->Attribute("right"));
            tileDesc.rect.bottom = std::stoi(pTileRectElem->Attribute("bottom"));

            m_pCurrentLevel->m_TileDescriptionMap.insert(std::make_pair(tileDesc.tileId, tileDesc));

            // This structure is actually used in game in order to prevent recalculating the collision rects
            //    over and over again
            TileCollisionPrototype tileProto;
            tileProto.id = tileDesc.tileId;
            tileProto.width = tileDesc.width;
            tileProto.height = tileDesc.height;
            Util::ParseCollisionRectanglesFromTile(&tileProto, &tileDesc);

            m_pCurrentLevel->m_TileCollisionPrototypeMap.insert(std::make_pair(tileProto.id, tileProto));
        }
    }
    else
    {
        assert(false && "Tile descriptions element not found.");
    }

    std::string palettePath = pLevelProperties->FirstChildElement("Palette")->GetText();
    std::replace(palettePath.begin(), palettePath.end(), '\\', '/');
    g_pApp->SetCurrentPalette(PalResourceLoader::LoadAndReturnPal(palettePath.c_str()));
    
    uint32 clawId = -1;
    for (TiXmlElement* pActorElem = pXmlLevelRoot->FirstChildElement("Actor"); pActorElem; 
        pActorElem = pActorElem->NextSiblingElement("Actor"))
    {
        //LOG("Creating actor: " + std::string(pActorElem->Attribute("Type")));
        //if (std::string(pActorElem->Attribute("Type")) != "Plane") break;
        StrongActorPtr pActor = VCreateActor(pActorElem, NULL);
        if (pActor)
        {
            shared_ptr<EventData_New_Actor> pNewActorEvent(new EventData_New_Actor(pActor->GetGUID()));
            IEventMgr::Get()->VQueueEvent(pNewActorEvent);

            // Get Claw's GUID
            if (pActor->GetName() == "Claw")
            {
                assert(clawId == -1 && "Multiple Captain Claws in this level - not supported at this time !");
                clawId = pActor->GetGUID();
            }
        }
        else
        {
            return false;
        }
    }

    // Notify all human views
    for (auto pGameView : m_GameViews)
    {
        if (pGameView->VGetType() == GameView_Human)
        {
            shared_ptr<HumanView> pHumanView = static_pointer_cast<HumanView>(pGameView);
            pHumanView->LoadGame(pXmlLevelRoot);
        }
    }

    // Load game save data
    const CheckpointSave* pCheckpointSave = m_pGameSaveMgr->GetCheckpointSave(
        m_pCurrentLevel->m_LeveNumber, m_pCurrentLevel->m_LoadedCheckpoint);
    assert(pCheckpointSave != NULL);

    // Load claw stats: Score, Health, Lives, Ammo: Bullets, Magic, Dynamite
    IEventMgr* pEventMgr = IEventMgr::Get();
    pEventMgr->VQueueEvent(IEventDataPtr(new EventData_Modify_Player_Stat(clawId, PlayerStat_Score, pCheckpointSave->score, false)));
    pEventMgr->VQueueEvent(IEventDataPtr(new EventData_Modify_Player_Stat(clawId, PlayerStat_Health, pCheckpointSave->health, false)));
    pEventMgr->VQueueEvent(IEventDataPtr(new EventData_Modify_Player_Stat(clawId, PlayerStat_Lives, pCheckpointSave->lives, false)));
    pEventMgr->VQueueEvent(IEventDataPtr(new EventData_Modify_Player_Stat(clawId, PlayerStat_Bullets, pCheckpointSave->bulletCount, false)));
    pEventMgr->VQueueEvent(IEventDataPtr(new EventData_Modify_Player_Stat(clawId, PlayerStat_Magic, pCheckpointSave->magicCount, false)));
    pEventMgr->VQueueEvent(IEventDataPtr(new EventData_Modify_Player_Stat(clawId, PlayerStat_Dynamite, pCheckpointSave->dynamiteCount, false)));

    // Set claw to spawn location
    m_CurrentSpawnPosition = GetSpawnPosition(m_pCurrentLevel->m_LeveNumber, m_pCurrentLevel->m_LoadedCheckpoint);
    //pEventMgr->VQueueEvent(IEventDataPtr(new EventData_Teleport_Actor(clawId, m_CurrentSpawnPosition)));

    LOG("Level loaded !");
    LOG("Level name: " + m_pCurrentLevel->m_LevelName);
    LOG("Level author: " + m_pCurrentLevel->m_LevelAuthor);
    LOG("Level created date: " + m_pCurrentLevel->m_LevelCreatedDate);

    SAFE_DELETE(pXmlLevelRoot);

    return true;
}

void BaseGameLogic::VSetProxy()
{
    m_Proxy = true;
}

StrongActorPtr BaseGameLogic::VCreateActor(const std::string& xmlActorResource, TiXmlElement* overrides)
{
    assert(m_pActorFactory);
    
    StrongActorPtr pActor = m_pActorFactory->CreateActor(xmlActorResource.c_str(), overrides);
    if (pActor)
    {
        m_ActorMap.insert(std::make_pair(pActor->GetGUID(), pActor));
        if ((m_GameState == GameState_LoadSaveMenu) || (m_GameState == GameState_IngameRunning))
        {
            // Create event that actor was created
        }
        return pActor;
    }
    else
    {
        return StrongActorPtr();
    }
}

StrongActorPtr BaseGameLogic::VCreateActor(TiXmlElement* pActorRoot, TiXmlElement* overrides)
{
    assert(m_pActorFactory);

    StrongActorPtr pActor = m_pActorFactory->CreateActor(pActorRoot, overrides);
    if (pActor)
    {
        m_ActorMap.insert(std::make_pair(pActor->GetGUID(), pActor));
        if ((m_GameState == GameState_LoadSaveMenu) || (m_GameState == GameState_IngameRunning))
        {
            // Create event that actor was created
        }
        return pActor;
    }
    else
    {
        return StrongActorPtr();
    }
}

void BaseGameLogic::VDestroyActor(const uint32 actorId)
{
    // Trigger actor destroyed event prior removing it here

    auto findIter = m_ActorMap.find(actorId);
    if (findIter != m_ActorMap.end())
    {
        findIter->second->Destroy();
        m_ActorMap.erase(findIter);
    }
}

WeakActorPtr BaseGameLogic::VGetActor(const uint32 actorId)
{
    auto findIter = m_ActorMap.find(actorId);
    if (findIter != m_ActorMap.end())
    {
        return findIter->second;
    }

    return WeakActorPtr();
}

void BaseGameLogic::VModifyActor(const uint32 actorId, TiXmlElement* overrides)
{
    assert(m_pActorFactory);

    auto findIter = m_ActorMap.begin();
    if (findIter != m_ActorMap.end())
    {
        m_pActorFactory->ModifyActor(findIter->second, overrides);
    }
}

void BaseGameLogic::VOnUpdate(uint32 msDiff)
{
    m_Lifetime += msDiff;

    switch (m_GameState)
    {
        case GameState_Initializing:
        {
            VChangeState(GameState_LoadingLevel);
            break;
        }

        case GameState_LoadingLevel:
        {
            VChangeState(GameState_IngameRunning);
            break;
        }

        case GameState_IngameRunning:
        {
            if (m_pProcessMgr)
            {
                m_pProcessMgr->UpdateProcesses(msDiff);
            }
            
            if (m_pPhysics)
            {
                //PROFILE_CPU("PHYSICS");
                m_pPhysics->VOnUpdate(msDiff);
                m_pPhysics->VSyncVisibleScene();
                break;

                static uint32 timeSinceLastUpdate = 16;
                const uint32 updateInterval = 0;

                timeSinceLastUpdate += msDiff;
                if (timeSinceLastUpdate >= updateInterval)
                {
                    //PROFILE_CPU("PHYSICS");
                    //LOG(ToStr(timeSinceLastUpdate));
                    m_pPhysics->VOnUpdate(timeSinceLastUpdate);
                    m_pPhysics->VSyncVisibleScene();

                    timeSinceLastUpdate = 0;
                }
            }

            break;
        }

        default:
        {
            LOG_ERROR("Unknown state: " + ToStr(m_GameState));
            break;
        }
    }

    // Update all game views
    for (auto pGameView : m_GameViews)
    {
        pGameView->VOnUpdate(msDiff);
    }

    // Update all game actors
    for (auto actorIter : m_ActorMap)
    {
        actorIter.second->Update(msDiff);
    }
}

void BaseGameLogic::VChangeState(GameState newState)
{
    if (newState == GameState_LoadingLevel)
    {
        if (!VLoadGame("LEVEL1.xml"))
        {
            LOG_ERROR("Could not load level");
            exit(1);
        }
    }

    LOG("Changing to: " + ToStr(newState));
    m_GameState = newState;
}

void BaseGameLogic::VRenderDiagnostics(SDL_Renderer* pRenderer, shared_ptr<CameraNode> pCamera)
{
    if (m_RenderDiagnostics && m_pPhysics)
    {
        m_pPhysics->VRenderDiagnostics(pRenderer, pCamera);
    }
}

void BaseGameLogic::VAddView(shared_ptr<IGameView> pView, uint32 actorId)
{
    int viewId = static_cast<int>(m_GameViews.size());
    m_GameViews.push_back(pView);
    pView->VOnAttach(viewId, actorId);
}

void BaseGameLogic::VRemoveView(shared_ptr<IGameView> pView)
{
    m_GameViews.remove(pView);
}

ActorFactory* BaseGameLogic::VCreateActorFactory()
{
    return new ActorFactory();
}

void BaseGameLogic::RequestDestroyActorDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Destroy_Actor> pCastEventData = static_pointer_cast<EventData_Destroy_Actor>(pEventData);
    VDestroyActor(pCastEventData->GetActorId());
}

void BaseGameLogic::MoveActorDelegate(IEventDataPtr pEventData)
{

}

void BaseGameLogic::RequestNewActorDelegate(IEventDataPtr pEventData)
{

}

void BaseGameLogic::CollideableTileCreatedDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Collideable_Tile_Created> pCastEventData = static_pointer_cast<EventData_Collideable_Tile_Created>(pEventData);

    auto findIt = m_pCurrentLevel->m_TileCollisionPrototypeMap.find(pCastEventData->GetTileId());
    if (findIt != m_pCurrentLevel->m_TileCollisionPrototypeMap.end())
    {
        TileCollisionPrototype& tileProto = findIt->second;

        for (auto tileCollisionRect : tileProto.collisionRectangles)
        {
            Point position = Point(pCastEventData->GetPositionX() + tileCollisionRect.collisionRect.x,
                                   pCastEventData->GetPositionY() + tileCollisionRect.collisionRect.y);
            Point size = Point(tileCollisionRect.collisionRect.w, tileCollisionRect.collisionRect.h);

            m_pPhysics->VAddStaticGeometry(position, size, tileCollisionRect.collisionType);
        }
    }
    else
    {
        LOG_WARNING("Unknown tile! Id = " + ToStr(pCastEventData->GetTileId()));
    }
}

//=====================================================================================================================
// Private
//=====================================================================================================================

void BaseGameLogic::RegisterAllDelegates()
{
    IEventMgr::Get()->VAddListener(MakeDelegate(this, &BaseGameLogic::CollideableTileCreatedDelegate), EventData_Collideable_Tile_Created::sk_EventType);
    IEventMgr::Get()->VAddListener(MakeDelegate(this, &BaseGameLogic::RequestDestroyActorDelegate), EventData_Destroy_Actor::sk_EventType);
}

void BaseGameLogic::RemoveAllDelegates()
{
    IEventMgr::Get()->VRemoveListener(MakeDelegate(this, &BaseGameLogic::CollideableTileCreatedDelegate), EventData_Collideable_Tile_Created::sk_EventType);
    IEventMgr::Get()->VRemoveListener(MakeDelegate(this, &BaseGameLogic::RequestDestroyActorDelegate), EventData_Destroy_Actor::sk_EventType);
}