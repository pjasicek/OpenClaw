#include "../Resource/ResourceCache.h"
#include "../Audio/Audio.h"
#include "../Events/EventMgr.h"
#include "../Events/EventMgrImpl.h"
#include "../Events/Events.h"
#include "BaseGameLogic.h"
#include "../UserInterface/HumanView.h"
#include "../Resource/ResourceMgr.h"
#include "../Graphics2D/Image.h"

// Resource loaders
#include "../Resource/Loaders/DefaultLoader.h"
#include "../Resource/Loaders/XmlLoader.h"
#include "../Resource/Loaders/WwdLoader.h"
#include "../Resource/Loaders/PalLoader.h"
#include "../Resource/Loaders/PidLoader.h"
#include "../Resource/Loaders/AniLoader.h"
#include "../Resource/Loaders/WavLoader.h"
#include "../Resource/Loaders/MidiLoader.h"
#include "../Resource/Loaders/PcxLoader.h"
#include "../Resource/Loaders/PngLoader.h"

#include "BaseGameApp.h"

#include <cctype>

TiXmlElement* CreateDefaultDisplayConfig();
TiXmlElement* CreateDefaultAudioConfig();
TiXmlElement* CreateDefaultFontConfig();
TiXmlElement* CreateDefaultAssetsConfig();
TiXmlDocument CreateDefaultConfig();

BaseGameApp* g_pApp = NULL;

BaseGameApp::BaseGameApp()
{
    g_pApp = this;

    m_pGame = NULL;
    m_pResourceCache = NULL;
    m_pEventMgr = NULL;
    m_pWindow = NULL;
    m_pRenderer = NULL;
    m_pPalette = NULL;
    m_pAudio = NULL;
    m_pConsoleFont = NULL;
    m_IsRunning = false;
    m_QuitRequested = false;
    m_IsQuitting = false;
}

bool BaseGameApp::Initialize(int argc, char** argv)
{
    RegisterEngineEvents();
    VRegisterGameEvents();

    // Initialization sequence
    if (!InitializeEventMgr()) return false;
    if (!InitializeDisplay(m_GameOptions)) return false;
    if (!InitializeAudio(m_GameOptions)) return false;
    if (!InitializeFont(m_GameOptions)) return false;
    if (!InitializeResources(m_GameOptions)) return false;
    if (!InitializeLocalization(m_GameOptions)) return false;
    if (!ReadActorXmlPrototypes(m_GameOptions)) return false;

    RegisterAllDelegates();

    m_pGame = VCreateGameAndView();
    if (!m_pGame)
    {
        LOG_ERROR("Failed to initialize game logic.");
        return false;
    }

    m_pResourceCache->Preload("/CLAW/*", NULL);
    m_pResourceCache->Preload("/GAME/*", NULL);
    m_pResourceCache->Preload("/STATES/*", NULL);

    m_pResourceMgr->VPreload("*", NULL, CUSTOM_RESOURCE);

    if (!VPerformStartupTests())
    {
        LOG_ERROR("Failed to pass certain startup tests.");
        return false;
    }

    m_IsRunning = true;

    return true;
}

void BaseGameApp::Terminate()
{
    LOG("Terminating...");

    RemoveAllDelegates();

    SAFE_DELETE(m_pGame);
    SDL_DestroyRenderer(m_pRenderer);
    SDL_DestroyWindow(m_pWindow);
    SAFE_DELETE(m_pAudio);
    // TODO - this causes crashes
    //SAFE_DELETE(m_pEventMgr);
    //SAFE_DELETE(m_pResourceCache);

    SaveGameOptions();
}

#define STARTUP_TEST(condition, error) \
{ \
    if (!(condition)) \
    { \
       LOG_ERROR((error)); \
       bTestsOk = false; \
    } \
} \

#define STARTUP_TEST_FILE_PRESENCE_IN_RESCACHE(filePath, resCacheName, error) \
{ \
    std::vector<std::string> matchedFiles = m_pResourceMgr->VMatch((filePath), (resCacheName)); \
    STARTUP_TEST(matchedFiles.size() > 0, error); \
    if (bTestsOk) \
    { \
        std::string filePathCopy = (filePath); \
        std::transform(filePathCopy.begin(), filePathCopy.end(), filePathCopy.begin(), (int(*)(int)) std::tolower); \
        STARTUP_TEST(matchedFiles.size() == 1, "More than 1 file found"); \
        STARTUP_TEST(matchedFiles[0] == (filePathCopy), (error)); \
    } \
} \


bool BaseGameApp::VPerformStartupTests()
{
    bool bTestsOk = true;

    // Base SDL video, audio and events
    STARTUP_TEST(SDL_WasInit(SDL_INIT_VIDEO), "SDL Video subsystem is unitialized");
    STARTUP_TEST(SDL_WasInit(SDL_INIT_AUDIO), "SDL Audio subsystem is unitialized");
    STARTUP_TEST(SDL_WasInit(SDL_INIT_EVENTS), "SDL Event subsystem is unitialized");
    STARTUP_TEST(m_pWindow != NULL, "SDL Window is NULL");
    STARTUP_TEST(m_pRenderer != NULL, "SDL Renderer is NULL");
    
    // Game logic
    STARTUP_TEST(m_pGame != NULL, "Game Logic is NULL");

    // Game view
    STARTUP_TEST(GetHumanView() != NULL, "Human View is NULL");

    // Event manager
    STARTUP_TEST(IEventMgr::Get() != NULL, "Event manager is unitialized");

    // Audio manager
    STARTUP_TEST(m_pAudio != NULL, "Audio manager is unitialized");

    // Resources
    STARTUP_TEST(m_pResourceMgr->VHasResourceCache(ORIGINAL_RESOURCE), std::string(ORIGINAL_RESOURCE) + " is not part of ResourceMgr");
    STARTUP_TEST(m_pResourceMgr->VHasResourceCache(CUSTOM_RESOURCE), std::string(CUSTOM_RESOURCE) + " is not part of ResourceMgr");

    // Files located in my custom ASSETS.ZIP
    STARTUP_TEST_FILE_PRESENCE_IN_RESCACHE(
        "/ActorPrototypes/LEVEL1/LEVEL1_SOLDIER.XML", 
        CUSTOM_RESOURCE, 
        "/ActorPrototypes/LEVEL1/LEVEL1_SOLDIER.XML not found in: " + std::string(CUSTOM_RESOURCE));

    STARTUP_TEST_FILE_PRESENCE_IN_RESCACHE(
        "/ActorPrototypes/LEVEL1/LEVEL1_OFFICER.XML",
        CUSTOM_RESOURCE,
        "/ActorPrototypes/LEVEL1/LEVEL1_OFFICER.XML not found in: " + std::string(CUSTOM_RESOURCE));

    return bTestsOk;
}

//=====================================================================================================================
// BaseGameApp::Run - Main game loop
//
//    Handle events -> update game -> render views
//=====================================================================================================================

int32 BaseGameApp::Run()
{
    static uint32 lastTime = SDL_GetTicks();
    SDL_Event event;
    int consecutiveLagSpikes = 0;

    while (m_IsRunning)
    {
        //PROFILE_CPU("MAINLOOP");

        uint32 now = SDL_GetTicks();
        uint32 elapsedTime = now - lastTime;
        lastTime = now;

        // This occurs when recovering program from background or after load
        // We want to ignore these situations
        if (elapsedTime > 1000)
        {
            consecutiveLagSpikes++;
            if (consecutiveLagSpikes > 10)
            {
                LOG_ERROR("Experiencing lag spikes, " + ToStr(consecutiveLagSpikes) + "high latency frames in a row");
            }
            continue;
        }
        consecutiveLagSpikes = 0;

        // Handle all input events
        while (SDL_PollEvent(&event))
        {
            OnEvent(event);
        }

        if (m_pGame)
        {
            // Update game
            {
                //PROFILE_CPU("ONLY GAME UPDATE");
                IEventMgr::Get()->VUpdate(20); // Allow event queue to process for up to 20 ms
                m_pGame->VOnUpdate(elapsedTime);
            }

            // Render game
            for (auto pGameView : m_pGame->m_GameViews)
            {
                //PROFILE_CPU("ONLY RENDER");
                pGameView->VOnRender(elapsedTime);
            }
            
            //m_pGame->VRenderDiagnostics();
        }

        // Artificially decrease fps. Configurable from console
        SDL_Delay(m_GlobalOptions.cpuDelayMs);
    }

    Terminate();

    return 0;
}

void BaseGameApp::OnEvent(SDL_Event& event)
{
    switch (event.type)
    {
        case SDL_QUIT:
        case SDL_APP_TERMINATING:
        {
            m_IsRunning = false;
            break;
        }

        case SDL_WINDOWEVENT:
        {
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED /*||
                event.window.event == SDL_WINDOWEVENT_RESIZED*/)
            {
                OnDisplayChange(event.window.data1, event.window.data2);
            }
            else if (event.window.event == SDL_WINDOWEVENT_RESTORED)
            {
                VOnRestore();
            }
            else if (event.window.event == SDL_WINDOWEVENT_MINIMIZED)
            {
                void VOnMinimized();
            }
            break;
        }

        case SDL_APP_LOWMEMORY:
        {
            LOG_WARNING("Running low on memory");
            break;
        }

        case SDL_APP_DIDENTERBACKGROUND:
        {
            LOG("Entered background");
            break;
        }

        case SDL_APP_DIDENTERFOREGROUND:
        {
            LOG("Entered foreground");
            break;
        }

        case SDL_KEYDOWN:
        case SDL_KEYUP:
        case SDL_TEXTEDITING:
        case SDL_TEXTINPUT:
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEWHEEL:
        case SDL_FINGERUP:
        case SDL_FINGERDOWN:
        case SDL_FINGERMOTION:
        {
            if (m_pGame)
            {
                for (GameViewList::reverse_iterator iter = m_pGame->m_GameViews.rbegin();
                    iter != m_pGame->m_GameViews.rend(); ++iter)
                {
                    (*iter)->VOnEvent(event);
                }
            }
            break;
        }
    }
}

void BaseGameApp::OnDisplayChange(int newWidth, int newHeight)
{
    LOG("Display changed. New Width-Height: " + ToStr(newWidth) + "-" + ToStr(newHeight));
}

void BaseGameApp::VOnRestore()
{
    LOG("Window restored.");
}

void BaseGameApp::VOnMinimized()
{
    LOG("Window minimized");
}

bool BaseGameApp::LoadStrings(std::string language)
{
    return true;
}

std::string BaseGameApp::GetString(std::string stringId)
{
    return "";
}

HumanView* BaseGameApp::GetHumanView() const
{
    HumanView *pView = NULL;
    for (GameViewList::iterator i = m_pGame->m_GameViews.begin(); i != m_pGame->m_GameViews.end(); ++i)
    {
        if ((*i)->VGetType() == GameView_Human)
        {
            shared_ptr<IGameView> pIGameView(*i);
            pView = static_cast<HumanView *>(&*pIGameView);
            break;
        }
    }

    return pView;
}

bool BaseGameApp::LoadGameOptions(const char* inConfigFile)
{
    if (!m_XmlConfiguration.LoadFile(inConfigFile))
    {
        LOG_WARNING("Configuration file: " + std::string(inConfigFile)
            + " not found - creating default configuration");
        m_XmlConfiguration = CreateAndReturnDefaultConfig(inConfigFile);
    }

    TiXmlElement* configRoot = m_XmlConfiguration.RootElement();
    if (configRoot == NULL)
    {
        LOG_ERROR("Could not load root element for config file");
        return false;
    }

    //-------------------------------------------------------------------------
    // Display
    //-------------------------------------------------------------------------
    TiXmlElement* displayElem = configRoot->FirstChildElement("Display");
    if (displayElem)
    {
        TiXmlElement* windowSizeElem = displayElem->FirstChildElement("Size");
        if (windowSizeElem)
        {
            windowSizeElem->Attribute("width", &m_GameOptions.windowWidth);
            windowSizeElem->Attribute("height", &m_GameOptions.windowHeight);
        }

        ParseValueFromXmlElem(&m_GameOptions.scale,
            displayElem->FirstChildElement("Scale"));
        ParseValueFromXmlElem(&m_GameOptions.useVerticalSync,
            displayElem->FirstChildElement("UseVerticalSync"));
        ParseValueFromXmlElem(&m_GameOptions.isFullscreen,
            displayElem->FirstChildElement("IsFullscreen"));
        ParseValueFromXmlElem(&m_GameOptions.isFullscreenDesktop,
            displayElem->FirstChildElement("IsFullscreenDesktop"));
    }

    //-------------------------------------------------------------------------
    // Audio
    //-------------------------------------------------------------------------
    TiXmlElement* audioElem = configRoot->FirstChildElement("Audio");
    if (audioElem)
    {
        ParseValueFromXmlElem(&m_GameOptions.frequency,
            audioElem->FirstChildElement("Frequency"));
        ParseValueFromXmlElem(&m_GameOptions.soundChannels,
            audioElem->FirstChildElement("SoundChannels"));
        ParseValueFromXmlElem(&m_GameOptions.mixingChannels,
            audioElem->FirstChildElement("MixingChannels"));
        ParseValueFromXmlElem(&m_GameOptions.chunkSize,
            audioElem->FirstChildElement("ChunkSize"));
        ParseValueFromXmlElem(&m_GameOptions.midiRpcServerPath,
            audioElem->FirstChildElement("MusiscRpcServerPath"));
        ParseValueFromXmlElem(&m_GameOptions.soundVolume,
            audioElem->FirstChildElement("SoundVolume"));
        ParseValueFromXmlElem(&m_GameOptions.musicVolume,
            audioElem->FirstChildElement("MusicVolume"));
        ParseValueFromXmlElem(&m_GameOptions.soundOn,
            audioElem->FirstChildElement("SoundOn"));
        ParseValueFromXmlElem(&m_GameOptions.musicOn,
            audioElem->FirstChildElement("MusicOn"));
    }

    //-------------------------------------------------------------------------
    // Assets
    //-------------------------------------------------------------------------
    TiXmlElement* assetsElem = configRoot->FirstChildElement("Assets");
    if (assetsElem)
    {
        ParseValueFromXmlElem(&m_GameOptions.rezArchivePath,
            assetsElem->FirstChildElement("RezArchive"));
        ParseValueFromXmlElem(&m_GameOptions.customArchivePath,
            assetsElem->FirstChildElement("CustomArchive"));
        ParseValueFromXmlElem(&m_GameOptions.resourceCacheSize,
            assetsElem->FirstChildElement("ResourceCacheSize"));
        ParseValueFromXmlElem(&m_GameOptions.tempDir,
            assetsElem->FirstChildElement("TempDir"));
        ParseValueFromXmlElem(&m_GameOptions.savesFile,
            assetsElem->FirstChildElement("SavesFile"));
    }

    //-------------------------------------------------------------------------
    // Font
    //-------------------------------------------------------------------------
    TiXmlElement* fontRootElem = configRoot->FirstChildElement("Font");
    if (fontRootElem)
    {
        for (TiXmlElement* fontElem = fontRootElem->FirstChildElement("Font");
            fontElem != NULL;
            fontElem = fontElem->NextSiblingElement("Font"))
        {
            if (fontElem->GetText())
            {
                m_GameOptions.fontNames.push_back(fontElem->GetText());
            }
        }

        TiXmlElement* consoleFontElem = fontRootElem->FirstChildElement("ConsoleFont");
        if (consoleFontElem)
        {
            consoleFontElem->Attribute("size", (int*)&m_GameOptions.consoleFontSize);
            if (const char* fontName = consoleFontElem->Attribute("font"))
            {
                m_GameOptions.consoleFontName = fontName;
            }
        }
    }

    //-------------------------------------------------------------------------
    // Console
    //-------------------------------------------------------------------------
    if (TiXmlElement* pConsoleRootElem = configRoot->FirstChildElement("Console"))
    {
        ParseValueFromXmlElem(&m_GameOptions.consoleConfig.backgroundImagePath,
            pConsoleRootElem->FirstChildElement("BackgroundImagePath"));
        ParseValueFromXmlElem(&m_GameOptions.consoleConfig.stretchBackgroundImage,
            pConsoleRootElem->FirstChildElement("StretchBackgroundImage"));
        ParseValueFromXmlElem(&m_GameOptions.consoleConfig.widthRatio,
            pConsoleRootElem->FirstChildElement("WidthRatio"));
        ParseValueFromXmlElem(&m_GameOptions.consoleConfig.heightRatio,
            pConsoleRootElem->FirstChildElement("HeightRatio"));
        ParseValueFromXmlElem(&m_GameOptions.consoleConfig.lineSeparatorHeight,
            pConsoleRootElem->FirstChildElement("LineSeparatorHeight"));
        ParseValueFromXmlElem(&m_GameOptions.consoleConfig.commandPromptOffsetY,
            pConsoleRootElem->FirstChildElement("CommandPromptOffsetY"));
        ParseValueFromXmlElem(&m_GameOptions.consoleConfig.consoleAnimationSpeed,
            pConsoleRootElem->FirstChildElement("ConsoleAnimationSpeed"));
        if (TiXmlElement* pElem = pConsoleRootElem->FirstChildElement("FontColor"))
        {
            int r, g, b;
            pElem->Attribute("r", &r);
            pElem->Attribute("g", &g);
            pElem->Attribute("b", &b);
            m_GameOptions.consoleConfig.fontColor.r = r;
            m_GameOptions.consoleConfig.fontColor.g = g;
            m_GameOptions.consoleConfig.fontColor.b = b;
        }
        ParseValueFromXmlElem(&m_GameOptions.consoleConfig.fontHeight,
            pConsoleRootElem->FirstChildElement("FontHeight"));
        ParseValueFromXmlElem(&m_GameOptions.consoleConfig.leftOffset,
            pConsoleRootElem->FirstChildElement("LeftOffset"));
        ParseValueFromXmlElem(&m_GameOptions.consoleConfig.commandPrompt,
            pConsoleRootElem->FirstChildElement("CommandPrompt"));
        ParseValueFromXmlElem(&m_GameOptions.consoleConfig.fontPath,
            pConsoleRootElem->FirstChildElement("FontPath"));
    }
    else
    {
        LOG_ERROR("Console configuration is missing.");
        return false;
    }
    //-------------------------------------------------------------------------
    // Global options
    //-------------------------------------------------------------------------
    if (TiXmlElement* pGlobalOptionsRootElem = configRoot->FirstChildElement("GlobalOptions"))
    {
        ParseValueFromXmlElem(&m_GlobalOptions.cpuDelayMs, 
            pGlobalOptionsRootElem->FirstChildElement("CpuDelay"));
        ParseValueFromXmlElem(&m_GlobalOptions.maxJumpSpeed,
            pGlobalOptionsRootElem->FirstChildElement("MaxJumpSpeed"));
        ParseValueFromXmlElem(&m_GlobalOptions.maxFallSpeed,
            pGlobalOptionsRootElem->FirstChildElement("MaxFallSpeed"));
        ParseValueFromXmlElem(&m_GlobalOptions.idleSoundQuoteIntervalMs,
            pGlobalOptionsRootElem->FirstChildElement("IdleSoundQuoteInterval"));
        ParseValueFromXmlElem(&m_GlobalOptions.platformSpeedModifier,
            pGlobalOptionsRootElem->FirstChildElement("PlatformSpeedModifier"));
        ParseValueFromXmlElem(&m_GlobalOptions.maxJumpHeight,
            pGlobalOptionsRootElem->FirstChildElement("MaxJumpHeight"));
        ParseValueFromXmlElem(&m_GlobalOptions.powerupMaxJumpHeight,
            pGlobalOptionsRootElem->FirstChildElement("PowerupMaxJumpHeight"));
        ParseValueFromXmlElem(&m_GlobalOptions.skipMenu,
            pGlobalOptionsRootElem->FirstChildElement("SkipMenu"));
        ParseValueFromXmlElem(&m_GlobalOptions.startLookUpOrDownTime,
            pGlobalOptionsRootElem->FirstChildElement("StartLookUpOrDownTime"));
        ParseValueFromXmlElem(&m_GlobalOptions.maxLookUpOrDownDistance,
            pGlobalOptionsRootElem->FirstChildElement("MaxLookUpOrDownDistance"));
        ParseValueFromXmlElem(&m_GlobalOptions.lookUpOrDownSpeed,
            pGlobalOptionsRootElem->FirstChildElement("LookUpOrDownSpeed"));
        ParseValueFromXmlElem(&m_GlobalOptions.scoreScreenPalPath,
            pGlobalOptionsRootElem->FirstChildElement("ScoreScreenPalPath"));
        ParseValueFromXmlElem(&m_GlobalOptions.clawRunningSpeed,
            pGlobalOptionsRootElem->FirstChildElement("ClawRunningSpeed"));
        /*ParseValueFromXmlElem(&m_GlobalOptions.springBoardSpringHeight,
            pGlobalOptionsRootElem->FirstChildElement("SpringBoardSpringHeight"));*/
        ParseValueFromXmlElem(&m_GlobalOptions.springBoardSpringSpeed,
            pGlobalOptionsRootElem->FirstChildElement("SpringBoardSpringSpeed"));
    }

    return true;
}

void BaseGameApp::SaveGameOptions(const char* outConfigFile)
{
    LOG_ERROR("Not implemented yet!");
    return;
}

//=====================================================================================================================
// Private implementations
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
// BaseGameApp::RegisterEngineEvents
//---------------------------------------------------------------------------------------------------------------------
void BaseGameApp::RegisterEngineEvents()
{
    /*REGISTER_EVENT(EventData_Environment_Loaded);
    REGISTER_EVENT(EventData_New_Actor);
    REGISTER_EVENT(EventData_Move_Actor);
    REGISTER_EVENT(EventData_Destroy_Actor);
    REGISTER_EVENT(EventData_Request_New_Actor);
    REGISTER_EVENT(EventData_Network_Player_Actor_Assignment);
    REGISTER_EVENT(EventData_Attach_Actor);
    REGISTER_EVENT(EventData_Collideable_Tile_Created);
    REGISTER_EVENT(EventData_Start_Climb);
    REGISTER_EVENT(EventData_Actor_Fire);
    REGISTER_EVENT(EventData_Actor_Attack);
    REGISTER_EVENT(EventData_New_HUD_Element);
    REGISTER_EVENT(EventData_New_Life);
    REGISTER_EVENT(EventData_Updated_Score);
    REGISTER_EVENT(EventData_Updated_Lives);
    REGISTER_EVENT(EventData_Updated_Health);
    REGISTER_EVENT(EventData_Updated_Ammo);
    REGISTER_EVENT(EventData_Updated_Ammo_Type);
    REGISTER_EVENT(EventData_Request_Change_Ammo_Type);
    REGISTER_EVENT(EventData_Teleport_Actor);*/
}

//---------------------------------------------------------------------------------------------------------------------
// BaseGameApp::InitializeDisplay
//
// Initializes SDL2 main game window and creates SDL2 renderer
//---------------------------------------------------------------------------------------------------------------------
bool BaseGameApp::InitializeDisplay(GameOptions& gameOptions)
{
    LOG(">>>>> Initializing display...");

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        LOG_ERROR("Failed to initialize SDL2 library");
        return false;
    }

    m_pWindow = SDL_CreateWindow(VGetGameTitle(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        gameOptions.windowWidth, gameOptions.windowHeight, SDL_WINDOW_SHOWN);
    if (m_pWindow == NULL)
    {
        LOG_ERROR("Failed to create main window");
        return false;
    }

    if (gameOptions.isFullscreen)
    {
        SDL_SetWindowFullscreen(m_pWindow, SDL_WINDOW_FULLSCREEN);
        SDL_GetWindowSize(m_pWindow, &m_GameOptions.windowWidth, &m_GameOptions.windowHeight);
    }
    else if (gameOptions.isFullscreenDesktop)
    {
        SDL_SetWindowFullscreen(m_pWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
        SDL_GetWindowSize(m_pWindow, &m_GameOptions.windowWidth, &m_GameOptions.windowHeight);
    }

    m_WindowSize.Set(gameOptions.windowWidth, gameOptions.windowHeight);

    uint32 rendererFlags = SDL_RENDERER_ACCELERATED;
    if (gameOptions.useVerticalSync)
    {
        rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
    }

    m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, rendererFlags);
    if (m_pRenderer == NULL)
    {
        LOG_ERROR("Failed to create SDL2 Renderer. Error: %s" + std::string(SDL_GetError()));
        return false;
    }

    SDL_RenderSetScale(m_pRenderer, (float)gameOptions.scale, (float)gameOptions.scale);

    LOG("Display successfully initialized.");

    return true;
}

//---------------------------------------------------------------------------------------------------------------------
// BaseGameApp::InitializeAudio
//
// Initializes SDL Mixer as audio device
//---------------------------------------------------------------------------------------------------------------------
bool BaseGameApp::InitializeAudio(GameOptions& gameOptions)
{
    LOG(">>>>> Initializing audio...");

    m_pAudio = new Audio();
    if (!m_pAudio->Initialize(gameOptions))
    {
        LOG_ERROR("Failed to initialize SDL Mixer audio subsystem");
        return false;
    }

    LOG("Audio successfully initialized.");

    return true;
}

//---------------------------------------------------------------------------------------------------------------------
// BaseGameApp::InitializeResources
//
// Register CLAW.REZ resource file as resource cache for assets the game is going to use
//---------------------------------------------------------------------------------------------------------------------
bool BaseGameApp::InitializeResources(GameOptions& gameOptions)
{
    LOG(">>>>> Initializing resource cache...");

    if (gameOptions.rezArchivePath.empty())
    {
        LOG_ERROR("No specified assets resource files in configuration.");
        return false;
    }

    IResourceFile* rezArchive = new ResourceRezArchive(gameOptions.rezArchivePath);

    m_pResourceCache = new ResourceCache(gameOptions.resourceCacheSize, rezArchive, ORIGINAL_RESOURCE);
    if (!m_pResourceCache->Init())
    {
        LOG_ERROR("Failed to initialize resource cachce from resource file: " + std::string(gameOptions.rezArchivePath));
        return false;
    }

    m_pResourceCache->RegisterLoader(DefaultResourceLoader::Create());
    m_pResourceCache->RegisterLoader(XmlResourceLoader::Create());
    m_pResourceCache->RegisterLoader(WwdResourceLoader::Create());
    m_pResourceCache->RegisterLoader(PalResourceLoader::Create());
    m_pResourceCache->RegisterLoader(PidResourceLoader::Create());
    m_pResourceCache->RegisterLoader(AniResourceLoader::Create());
    m_pResourceCache->RegisterLoader(WavResourceLoader::Create());
    m_pResourceCache->RegisterLoader(MidiResourceLoader::Create());
    m_pResourceCache->RegisterLoader(PcxResourceLoader::Create());

    IResourceFile* pCustomArchive = new ResourceZipArchive(gameOptions.customArchivePath);
    ResourceCache* pCustomCache = new ResourceCache(50, pCustomArchive, CUSTOM_RESOURCE);
    if (!pCustomCache->Init())
    {
        LOG_ERROR("Failed to initialize resource cachce from resource file: " + gameOptions.customArchivePath);
        return false;
    }

    pCustomCache->RegisterLoader(DefaultResourceLoader::Create());
    pCustomCache->RegisterLoader(XmlResourceLoader::Create());
    pCustomCache->RegisterLoader(WavResourceLoader::Create());
    pCustomCache->RegisterLoader(PcxResourceLoader::Create());
    pCustomCache->RegisterLoader(PngResourceLoader::Create());

    m_pResourceMgr = new ResourceMgrImpl();
    m_pResourceMgr->VAddResourceCache(m_pResourceCache);
    m_pResourceMgr->VAddResourceCache(pCustomCache);

    LOG("Resource cache successfully initialized");

    return true;
}

//---------------------------------------------------------------------------------------------------------------------
// BaseGameApp::InitializeFont
//---------------------------------------------------------------------------------------------------------------------
bool BaseGameApp::InitializeFont(GameOptions& gameOptions)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, ">>>>> Initializing font...");

    if (TTF_Init() < 0)
    {
        LOG_ERROR("Failed to initialize SDL TTF font subsystem");
        return false;
    }

    m_pConsoleFont = TTF_OpenFont(gameOptions.consoleFontName.c_str(), gameOptions.consoleFontSize);
    if (m_pConsoleFont == NULL)
    {
        LOG_ERROR("Failed to load TTF font");
        return false;
    }

    LOG("Font successfully initialized...");

    return true;
}

//---------------------------------------------------------------------------------------------------------------------
// BaseGameApp::InitializeLocalization
//---------------------------------------------------------------------------------------------------------------------
bool BaseGameApp::InitializeLocalization(GameOptions& gameOptions)
{
    return true;
}

//---------------------------------------------------------------------------------------------------------------------
// BaseGameApp::ReadActorXmlPrototypes
// 
//     Reads XML documents containing various actor prototypes which are then used to instantiate
//     concrete actors
//---------------------------------------------------------------------------------------------------------------------
bool BaseGameApp::ReadActorXmlPrototypes(GameOptions& gameOptions)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, ">>>>> Loading actor prototypes...");

    std::vector<std::string> xmlActorPrototypeFiles = m_pResourceMgr->VMatch("/ActorPrototypes/*.XML");
    for (std::string protoFile : xmlActorPrototypeFiles)
    {
        //LOG("Actor proto: " + protoFile);

        TiXmlElement* pActorProtoElem = XmlResourceLoader::LoadAndReturnRootXmlElement(protoFile.c_str());
        std::string protoName;
        if (!ParseAttributeFromXmlElem(&protoName, "ActorPrototypeName", pActorProtoElem))
        {
            LOG_ERROR(protoFile + " is missing ActorPrototypeName attribute in its root node !");
        }
        else
        {
            //LOG(protoFile + ": " + protoName);
            ActorPrototype actorProto = StringToEnum_ActorPrototype(protoName);

            // Create our own pointer
            TiXmlNode* pDuplicateNode = pActorProtoElem->Clone();
            assert(pDuplicateNode);
            TiXmlElement* pActorProtoElemDuplicate = pDuplicateNode->ToElement();
            assert(pActorProtoElemDuplicate);

            m_ActorXmlPrototypeMap.insert(std::make_pair(actorProto, pActorProtoElemDuplicate));
        }
    }

    bool loadedAllRequired = true;

    // When I provide specific purpose API, I should be very dilligent
    for (int actorPrototypeIdx = ActorPrototype_Start + 1;
        actorPrototypeIdx < ActorPrototype_Max;
        actorPrototypeIdx++)
    {
        auto findIt = m_ActorXmlPrototypeMap.find(ActorPrototype(actorPrototypeIdx));
        if (findIt == m_ActorXmlPrototypeMap.end())
        {
            LOG_ERROR("Actor prototype: \"" + 
                EnumToString_ActorPrototype(ActorPrototype(actorPrototypeIdx)) +
                std::string("\" was not found !"));
            loadedAllRequired = false;
        }
    }

    if (loadedAllRequired)
    {
        LOG("Actor prototypes loaded successfully.");
    }
    else
    {
        LOG_ERROR("Some of the actor prototypes were not loaded.");
    }

    return loadedAllRequired;
}

//---------------------------------------------------------------------------------------------------------------------
// BaseGameApp::InitializeEventMgr
//---------------------------------------------------------------------------------------------------------------------
bool BaseGameApp::InitializeEventMgr()
{
    m_pEventMgr = new EventMgr("BaseGameApp Event Mgr", true);
    if (!m_pEventMgr)
    {
        LOG_ERROR("Failed to create EventMgr.");
        return false;
    }

    return true;
}

Point BaseGameApp::GetScale()
{
    float scaleX, scaleY;
    uint32 windowFlags = GetWindowFlags();
    Point scale(1.0, 1.0);

    SDL_RenderGetScale(m_pRenderer, &scaleX, &scaleY);
    
    scale.Set((double)scaleX, (double)scaleY);

    return scale;
}

void BaseGameApp::SetScale(Point scale)
{
    SDL_RenderSetScale(m_pRenderer, (float)scale.x, (float)scale.y);
}

uint32 BaseGameApp::GetWindowFlags()
{
    return SDL_GetWindowFlags(m_pWindow);
}

class TiXmlMergeVisitor : public TiXmlVisitor
{
public:

    TiXmlMergeVisitor(TiXmlElement* pParentRootElem)
        :
        m_pParentRootElem(pParentRootElem)
    {

    }

    virtual bool VisitEnter(const TiXmlElement& elem, const TiXmlAttribute* pAttribute) override
    {
        std::string elemPath = GeTiXmlElementElementPath(&elem);
        /*LOG("Visiting element: " + std::string(elem.Value()) + " with path: " + elemPath);
        if (pAttribute != NULL)
        {
            LOG("Attribute value: " + std::string(pAttribute->Name()));
        }*/

        if (TiXmlElement* pParentElemToBeModified = GetTiXmlElementFromPath(m_pParentRootElem, elemPath))
        {
            // Check for attributes and text to be changed
            UpdateTiXmlElementAttributes(pParentElemToBeModified, &elem);
            UpdateTiXmlElementText(pParentElemToBeModified, &elem);
        }
        else
        {
            // Parent does not contain this element, so add it with all its descendants

            // First get parented node to which we will add the new one
            elemPath = elemPath.substr(0, elemPath.find_last_of("."));
            TiXmlElement* pParentElemToWhichAdd = GetTiXmlElementFromPath(m_pParentRootElem, elemPath);
            assert(pParentElemToWhichAdd != NULL);

            // Clone and add
            TiXmlElement* pChildElemCopy = elem.Clone()->ToElement();
            pParentElemToWhichAdd->LinkEndChild(pChildElemCopy);

            // We just added the whole subtree
            return false;
        }

        return true;
    }

    virtual bool VisitExit(const TiXmlElement& elem) override
    {
        /*if (elem.Parent() == NULL)
        {
            m_pParentRootElem->Print(stdout, -1);
            LOG("Visiting DONE");
        }*/

        return true;
    }

private:
    std::string GeTiXmlElementElementPath(const TiXmlElement* pElem)
    {
        assert(pElem != NULL);
        std::string path = pElem->Value();

        while (pElem->Parent() && pElem->Parent()->ToElement())
        {
            pElem = pElem->Parent()->ToElement();
            path.insert(0, std::string(pElem->Value()) + ".");
        }

        return path;
    }

    int UpdateTiXmlElementAttributes(TiXmlElement* updateThis, const TiXmlElement* withThis)
    {
        int numModifiedAttributes = 0;
        for (const TiXmlAttribute* pNewAttr = withThis->FirstAttribute();
            pNewAttr != NULL;
            pNewAttr = pNewAttr->Next())
        {
            updateThis->SetAttribute(pNewAttr->Name(), pNewAttr->Value());
            //LOG("Updated parent's [" + std::string(pNewAttr->Name()) + "] attribute with value [" + std::string(pNewAttr->Value()) + "]");
            numModifiedAttributes++;
        }

        return numModifiedAttributes;
    }

    bool UpdateTiXmlElementText(TiXmlElement* updateThis, const TiXmlElement* withThis)
    {
        if (withThis->GetText() == NULL)
        {
            return false;
        }

        return SetTiXmlElementText(withThis->GetText(), updateThis);
    }

    TiXmlElement* m_pParentRootElem;
};

// Remark: Caller is getting a NEW copy of the prototype -> caller is responsible for freeing this copy !
TiXmlElement* BaseGameApp::GetActorPrototypeElem(ActorPrototype proto)
{
    auto findIt = m_ActorXmlPrototypeMap.find(proto);
    assert(findIt != m_ActorXmlPrototypeMap.end());

    TiXmlNode* pCopy = findIt->second->Clone()->ToElement();
    assert(pCopy != NULL);

    TiXmlElement* pRootElem = pCopy->ToElement();
    assert(pRootElem != NULL);

    // If this is derived XML, load its parent and apply its changes
    if (pRootElem->Attribute("Parent") != NULL)
    {
        ActorPrototype parentProto = StringToEnum_ActorPrototype(pRootElem->Attribute("Parent"));
        TiXmlElement* pParentRootElem = GetActorPrototypeElem(parentProto);
        assert(pParentRootElem != NULL);

        // Merge changes from child to parent (child contains only delta changes)
        // IE. Child applies changes to Parent
        // * New attributes are added
        // * Existing attributes are overwritten
        // * New nodes are added
        // * Existing node text is overwritten
        TiXmlMergeVisitor mergeVisitor(pParentRootElem);
        pRootElem->Accept(&mergeVisitor);

        SAFE_DELETE(pRootElem);

        //pParentRootElem->Print(stdout, -1);

        return pParentRootElem;
    }

    return pCopy->ToElement();
}

//=====================================================================================================================
// Events
//=====================================================================================================================


void BaseGameApp::RegisterAllDelegates()
{
    IEventMgr::Get()->VAddListener(MakeDelegate(
        this, &BaseGameApp::QuitGameDelegate), EventData_Quit_Game::sk_EventType);
}

void BaseGameApp::RemoveAllDelegates()
{
    IEventMgr::Get()->VRemoveListener(MakeDelegate(
        this, &BaseGameApp::QuitGameDelegate), EventData_Quit_Game::sk_EventType);
}

void BaseGameApp::QuitGameDelegate(IEventDataPtr pEventData)
{
    Terminate();
    exit(0);
}

//=====================================================================================================================
// XML config management
//=====================================================================================================================

TiXmlElement* CreateDefaultDisplayConfig()
{
    TiXmlElement* display = new TiXmlElement("Display");

    XML_ADD_2_PARAM_ELEMENT("Size", "width", ToStr(1280).c_str(), "height", ToStr(768).c_str(), display);
    XML_ADD_TEXT_ELEMENT("Scale", "1", display);
    XML_ADD_TEXT_ELEMENT("UseVerticalSync", "true", display);
    XML_ADD_TEXT_ELEMENT("IsFullscreen", "false", display);
    XML_ADD_TEXT_ELEMENT("IsFullscreenDesktop", "false", display);

    return display;
}

TiXmlElement* CreateDefaultAudioConfig()
{
    TiXmlElement* audio = new TiXmlElement("Audio");

    XML_ADD_TEXT_ELEMENT("Frequency", "44100", audio);
    XML_ADD_TEXT_ELEMENT("SoundChannels", "1", audio);
    XML_ADD_TEXT_ELEMENT("MixingChannels", "24", audio);
    XML_ADD_TEXT_ELEMENT("ChunkSize", "2048", audio);
    XML_ADD_TEXT_ELEMENT("SoundVolume", "50", audio);
    XML_ADD_TEXT_ELEMENT("MusicVolume", "50", audio);
    XML_ADD_TEXT_ELEMENT("MusicRpcServerPath", "MidiProc.exe", audio);

    return audio;
}

TiXmlElement* CreateDefaultFontConfig()
{
    TiXmlElement* font = new TiXmlElement("Font");

    XML_ADD_TEXT_ELEMENT("Font", "clacon.ttf", font);
    XML_ADD_2_PARAM_ELEMENT("ConsoleFont", "font", "clacon.ttf", "size", "20", font);

    return font;
}

TiXmlElement* CreateDefaultAssetsConfig()
{
    TiXmlElement* assets = new TiXmlElement("Assets");

    XML_ADD_TEXT_ELEMENT("RezArchive", "CLAW.REZ", assets);
    XML_ADD_TEXT_ELEMENT("ResourceCacheSize", "50", assets);
    XML_ADD_TEXT_ELEMENT("TempDir", ".", assets);
    XML_ADD_TEXT_ELEMENT("SavesFile", "SAVES.XML", assets);

    return assets;
}

TiXmlElement* CreateDefaultConsoleConfig()
{
TiXmlElement* pConsoleConfig = new TiXmlElement("Console");

    // Assume that the default constructor has default values set
    ConsoleConfig defaultConfig;

    XML_ADD_TEXT_ELEMENT("BackgroundImagePath",
        defaultConfig.backgroundImagePath.c_str(), pConsoleConfig);
    XML_ADD_TEXT_ELEMENT("StretchBackgroundImage",
        ToStr(defaultConfig.stretchBackgroundImage).c_str(), pConsoleConfig);
    XML_ADD_TEXT_ELEMENT("WidthRatio",
        ToStr(defaultConfig.widthRatio).c_str(), pConsoleConfig);
    XML_ADD_TEXT_ELEMENT("HeightRatio",
        ToStr(defaultConfig.heightRatio).c_str(), pConsoleConfig);
    XML_ADD_TEXT_ELEMENT("LineSeparatorHeight",
        ToStr(defaultConfig.lineSeparatorHeight).c_str(), pConsoleConfig);
    XML_ADD_TEXT_ELEMENT("CommandPromptOffsetY",
        ToStr(defaultConfig.commandPromptOffsetY).c_str(), pConsoleConfig);
    XML_ADD_TEXT_ELEMENT("ConsoleAnimationSpeed",
        ToStr(defaultConfig.consoleAnimationSpeed).c_str(), pConsoleConfig);
    XML_ADD_TEXT_ELEMENT("FontPath",
        defaultConfig.fontPath.c_str(), pConsoleConfig);

    TiXmlElement* pColorElem = new TiXmlElement("FontColor");
    pColorElem->SetAttribute("r", defaultConfig.fontColor.r);
    pColorElem->SetAttribute("g", defaultConfig.fontColor.g);
    pColorElem->SetAttribute("b", defaultConfig.fontColor.b);
    pConsoleConfig->LinkEndChild(pColorElem);

    XML_ADD_TEXT_ELEMENT("FontHeight",
        ToStr(defaultConfig.fontHeight).c_str(), pConsoleConfig);
    XML_ADD_TEXT_ELEMENT("LeftOffset",
        ToStr(defaultConfig.leftOffset).c_str(), pConsoleConfig);
    XML_ADD_TEXT_ELEMENT("CommandPrompt",
        defaultConfig.commandPrompt.c_str(), pConsoleConfig);

    return pConsoleConfig;
}

TiXmlDocument BaseGameApp::CreateAndReturnDefaultConfig(const char* inConfigFile)
{
    TiXmlDocument xmlConfig;

    //----- [Configuration]
    TiXmlElement* root = new TiXmlElement("Configuration");
    xmlConfig.LinkEndChild(root);

    root->LinkEndChild(CreateDefaultDisplayConfig());
    root->LinkEndChild(CreateDefaultAudioConfig());
    root->LinkEndChild(CreateDefaultFontConfig());
    root->LinkEndChild(CreateDefaultAssetsConfig());
    root->LinkEndChild(CreateDefaultConsoleConfig());

    xmlConfig.SaveFile(inConfigFile);

    return xmlConfig;
}
