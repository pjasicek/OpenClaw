#include <Tinyxml/tinystr.h>

#include "../Resource/ResourceCache.h"
#include "../Audio/Audio.h"
#include "../Events/EventMgr.h"
#include "../Events/EventMgrImpl.h"
#include "../Events/Events.h"
#include "BaseGameLogic.h"
#include "../UserInterface/HumanView.h"

// Resource loaders
#include "..\Resource\Loaders\DefaultLoader.h"
#include "..\Resource\Loaders\XmlLoader.h"
#include "..\Resource\Loaders\WwdLoader.h"
#include "..\Resource\Loaders\PalLoader.h"
#include "..\Resource\Loaders\PidLoader.h"
#include "..\Resource\Loaders\AniLoader.h"
#include "..\Resource\Loaders\WavLoader.h"

#include "BaseGameApp.h"

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
    if (!InitializeDisplay(m_GameOptions)) return false;
    if (!InitializeAudio(m_GameOptions)) return false;
    if (!InitializeFont(m_GameOptions)) return false;
    if (!InitializeResources(m_GameOptions)) return false;
    if (!InitializeLocalization(m_GameOptions)) return false;
    if (!InitializeEventMgr()) return false;

    m_pGame = VCreateGameAndView();
    if (!m_pGame)
    {
        LOG_ERROR("Failed to initialize game logic.");
        return false;
    }

    m_pResourceCache->Preload("/LEVEL1/*", NULL);

    m_pResourceCache->Preload("/CLAW/*", NULL);
    m_pResourceCache->Preload("/GAME/*", NULL);
    m_pResourceCache->Preload("/STATES/*", NULL);

    m_IsRunning = true;

    return true;
}

void BaseGameApp::Terminate()
{
    LOG("Terminating...");

    SAFE_DELETE(m_pGame);
    SDL_DestroyRenderer(m_pRenderer);
    SDL_DestroyWindow(m_pWindow);
    SAFE_DELETE(m_pAudio);
    // TODO - this causes crashes
    //SAFE_DELETE(m_pEventMgr);
    //SAFE_DELETE(m_pResourceCache);

    SaveGameOptions();
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

    while (m_IsRunning)
    {
        //PROFILE_CPU("MAINLOOP");

        uint32 now = SDL_GetTicks();
        uint32 elapsedTime = now - lastTime;
        lastTime = now;

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
                IEventMgr::Get()->VUpdate(15); // Allow event queue to process for up to 15 ms
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
    return NULL;
}

bool BaseGameApp::LoadGameOptions(const char* inConfigFile)
{
    if (!m_XmlConfiguration.LoadFile(inConfigFile))
    {
        LOG_WARNING("Configuration file: " + std::string(inConfigFile) + " not found - creating default configuration");
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
    if (!displayElem) // Create default if it does not exist in config
    {
        displayElem = CreateDefaultDisplayConfig();
        configRoot->LinkEndChild(displayElem);
    }

    // Program deffensively here, missing element could cause disaster
    TiXmlElement* windowSizeElem = displayElem->FirstChildElement("Size");
    if (!windowSizeElem)
    {
        windowSizeElem = new TiXmlElement("Size");
        windowSizeElem->SetAttribute("width", 1280);
        windowSizeElem->SetAttribute("height", 768);
        displayElem->LinkEndChild(windowSizeElem);
    }

    windowSizeElem->Attribute("width", &m_GameOptions.windowWidth);
    windowSizeElem->Attribute("height", &m_GameOptions.windowHeight);

    TiXmlElement* scaleElem = displayElem->FirstChildElement("Scale");
    if (!scaleElem)
    {
        scaleElem = new TiXmlElement("Scale");
        displayElem->LinkEndChild(scaleElem->LinkEndChild(new TiXmlText("1.0")));
    }

    m_GameOptions.scale = std::stod(scaleElem->GetText());

    TiXmlElement* useVsyncElem = displayElem->FirstChildElement("UseVerticalSync");
    if (useVsyncElem && (std::string(useVsyncElem->GetText()) == "true"))
    {
        m_GameOptions.useVerticalSync = true;
    }

    //-------------------------------------------------------------------------
    // Audio
    //-------------------------------------------------------------------------

    TiXmlElement* audioElem = configRoot->FirstChildElement("Audio");
    if (!audioElem) // Create default if it does not exist in config
    {
        audioElem = CreateDefaultAudioConfig();
        configRoot->LinkEndChild(audioElem);
    }

    // Program deffensively here, missing element could cause disaster
    TiXmlElement* frequencyElem = audioElem->FirstChildElement("Frequency");
    if (!frequencyElem)
    {
        frequencyElem = new TiXmlElement("Frequency");
        frequencyElem->LinkEndChild(new TiXmlText("44100"));
        audioElem->LinkEndChild(frequencyElem);
    }

    TiXmlElement* channelsElem = audioElem->FirstChildElement("Channels");
    if (!channelsElem)
    {
        channelsElem = new TiXmlElement("Channels");
        channelsElem->LinkEndChild(new TiXmlText("2"));
        audioElem->LinkEndChild(channelsElem);
    }

    TiXmlElement* chunkSizeElem = audioElem->FirstChildElement("ChunkSize");
    if (!chunkSizeElem)
    {
        chunkSizeElem = new TiXmlElement("ChunkSize");
        chunkSizeElem->LinkEndChild(new TiXmlText("2048"));
        audioElem->LinkEndChild(chunkSizeElem);
    }

    TiXmlElement* midiRpcPathElem = audioElem->FirstChildElement("MusicRpcServerPath");
    if (!midiRpcPathElem)
    {
        midiRpcPathElem = new TiXmlElement("MusicRpcServerPath");
        midiRpcPathElem->LinkEndChild(new TiXmlText("MidiProc.exe"));
        audioElem->LinkEndChild(midiRpcPathElem);
    }

    m_GameOptions.frequency = std::stoi(frequencyElem->GetText());
    m_GameOptions.channels = std::stoi(channelsElem->GetText());
    m_GameOptions.chunkSize = std::stoi(chunkSizeElem->GetText());
    m_GameOptions.midiRpcServerPath = midiRpcPathElem->GetText();

    //-------------------------------------------------------------------------
    // Assets
    //-------------------------------------------------------------------------

    TiXmlElement* assetsElem = configRoot->FirstChildElement("Assets");
    if (!assetsElem) // Create default if it does not exist in config
    {
        assetsElem = CreateDefaultAssetsConfig();
        configRoot->LinkEndChild(assetsElem);
    }

    // Program deffensively here, missing element could cause disaster
    TiXmlElement* rezArchivePathElem = assetsElem->FirstChildElement("RezArchive");
    if (!rezArchivePathElem)
    {
        rezArchivePathElem = new TiXmlElement("RezArchive");
        rezArchivePathElem->LinkEndChild(new TiXmlText("CLAW.REZ"));
        assetsElem->LinkEndChild(rezArchivePathElem);
    }

    TiXmlElement* resourceCacheSizeElem = assetsElem->FirstChildElement("ResourceCacheSize");
    if (!rezArchivePathElem)
    {
        resourceCacheSizeElem = new TiXmlElement("ResourceCacheSize");
        resourceCacheSizeElem->LinkEndChild(new TiXmlText("50"));
        assetsElem->LinkEndChild(resourceCacheSizeElem);
    }

    m_GameOptions.resourceFileNames.push_back(rezArchivePathElem->GetText());
    m_GameOptions.resourceCacheSize = std::stoi(resourceCacheSizeElem->GetText());

    //-------------------------------------------------------------------------
    // Font
    //-------------------------------------------------------------------------

    TiXmlElement* fontRootElem = configRoot->FirstChildElement("Font");
    if (!fontRootElem) // Create default if it does not exist in config
    {
        fontRootElem = CreateDefaultFontConfig();
        configRoot->LinkEndChild(fontRootElem);
    }

    for (TiXmlElement* fontElem = fontRootElem->FirstChildElement("Font");
        fontElem != NULL; fontElem = fontElem->NextSiblingElement("Font"))
    {
        m_GameOptions.fontNames.push_back(fontElem->GetText());
    }

    if (m_GameOptions.fontNames.empty())
    {
        TiXmlElement* fontChildElem = new TiXmlElement("Font");
        fontChildElem->LinkEndChild(new TiXmlText("clacon.ttf"));
        fontRootElem->LinkEndChild(fontChildElem);
    }

    TiXmlElement* consoleFontElem = fontRootElem->FirstChildElement("ConsoleFont");
    if (!consoleFontElem)
    {
        consoleFontElem = new TiXmlElement("ConsoleFont");
        consoleFontElem->SetAttribute("font", "clacon.ttf");
        consoleFontElem->SetAttribute("size", 20);
        fontRootElem->LinkEndChild(consoleFontElem);
    }

    m_GameOptions.consoleFontName = consoleFontElem->Attribute("font");
    consoleFontElem->Attribute("size", &m_GameOptions.consoleFontSize);

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
    REGISTER_EVENT(EventData_Environment_Loaded);
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

    SDL_RenderSetScale(m_pRenderer, gameOptions.scale, gameOptions.scale);

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
    if (!m_pAudio->Initialize(gameOptions.frequency, gameOptions.channels, gameOptions.chunkSize, gameOptions.midiRpcServerPath))
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

    if (gameOptions.resourceFileNames.empty())
    {
        LOG_ERROR("No specified assets resource files in configuration.");
        return false;
    }

    IResourceFile* rezArchive = new ResourceRezArchive(gameOptions.resourceFileNames[0]);
    m_pResourceCache = new ResourceCache(gameOptions.resourceCacheSize, rezArchive);
    if (!m_pResourceCache->Init())
    {
        LOG_ERROR("Failed to initialize resource cachce from resource file: " + std::string(gameOptions.resourceFileNames[0]));
        return false;
    }

    m_pResourceCache->RegisterLoader(DefaultResourceLoader::Create());
    m_pResourceCache->RegisterLoader(XmlResourceLoader::Create());
    m_pResourceCache->RegisterLoader(WwdResourceLoader::Create());
    m_pResourceCache->RegisterLoader(PalResourceLoader::Create());
    m_pResourceCache->RegisterLoader(PidResourceLoader::Create());
    m_pResourceCache->RegisterLoader(AniResourceLoader::Create());
    m_pResourceCache->RegisterLoader(WavResourceLoader::Create());

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

    m_pConsoleFont = TTF_OpenFont(gameOptions.consoleFontName, gameOptions.consoleFontSize);
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

//=====================================================================================================================
// XML config management
//=====================================================================================================================

TiXmlElement* CreateDefaultDisplayConfig()
{
    TiXmlElement* display = new TiXmlElement("Display");

    XML_ADD_2_PARAM_ELEMENT("Size", "width", ToStr(1280).c_str(), "height", ToStr(768).c_str(), display);
    XML_ADD_TEXT_ELEMENT("Scale", "1", display);
    XML_ADD_TEXT_ELEMENT("UseVerticalSync", "true", display);

    return display;
}

TiXmlElement* CreateDefaultAudioConfig()
{
    TiXmlElement* audio = new TiXmlElement("Audio");

    XML_ADD_TEXT_ELEMENT("Frequency", "44100", audio);
    XML_ADD_TEXT_ELEMENT("Channels", "2", audio);
    XML_ADD_TEXT_ELEMENT("ChunkSize", "2048", audio);
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

    return assets;
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

    xmlConfig.SaveFile(inConfigFile);

    return xmlConfig;
}