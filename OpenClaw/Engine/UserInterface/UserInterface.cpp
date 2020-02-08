#include "UserInterface.h"

#include "../Graphics2D/Image.h"
#include "../GameApp/BaseGameApp.h"
#include "../GameApp/BaseGameLogic.h"
#include "../GameApp/GameSaves.h"
#include "../Resource/Loaders/PcxLoader.h"
#include "../Resource/Loaders/PngLoader.h"
#include "../Resource/Loaders/PidLoader.h"
#include "../Resource/ResourceMgr.h"
#include "../Audio/Audio.h"

#include <cctype>

std::map<std::string, MenuPage> g_StringToMenuPageEnumMap =
{
    { "MenuPage_Main",                          MenuPage_Main },
    { "MenuPage_SinglePlayer",                  MenuPage_SinglePlayer },
    { "MenuPage_Multiplayer",                   MenuPage_Multiplayer },
    { "MenuPage_ReplayMovies",                  MenuPage_ReplayMovies },
    { "MenuPage_Options",                       MenuPage_Options },
    { "MenuPage_Credits",                       MenuPage_Credits },
    { "MenuPage_Help",                          MenuPage_Help },
    { "MenuPage_QuitGame",                      MenuPage_QuitGame },
    { "MenuPage_Help_TouchScreen",              MenuPage_Help_TouchScreen },
    { "MenuPage_SinglePlayer_NewGame",          MenuPage_SinglePlayer_NewGame },
    { "MenuPage_SinglePlayer_LoadGame",         MenuPage_SinglePlayer_LoadGame },
    { "MenuPage_SinglePlayer_LoadCustomLevel",  MenuPage_SinglePlayer_LoadCustomLevel },
    { "MenuPage_SinglePlayer_SaveGame",         MenuPage_SinglePlayer_SaveGame },
    { "MenuPage_SinglePlayer_UploadScores",     MenuPage_SinglePlayer_UploadScores },
    { "MenuPage_SinglePlayer_LoadGame_Level1",  MenuPage_SinglePlayer_LoadGame_Level1 },
    { "MenuPage_SinglePlayer_LoadGame_Level2",  MenuPage_SinglePlayer_LoadGame_Level2 },
    { "MenuPage_SinglePlayer_LoadGame_Level3",  MenuPage_SinglePlayer_LoadGame_Level3 },
    { "MenuPage_SinglePlayer_LoadGame_Level4",  MenuPage_SinglePlayer_LoadGame_Level4 },
    { "MenuPage_SinglePlayer_LoadGame_Level5",  MenuPage_SinglePlayer_LoadGame_Level5 },
    { "MenuPage_SinglePlayer_LoadGame_Level6",  MenuPage_SinglePlayer_LoadGame_Level6 },
    { "MenuPage_SinglePlayer_LoadGame_Level7",  MenuPage_SinglePlayer_LoadGame_Level7 },
    { "MenuPage_SinglePlayer_LoadGame_Level8",  MenuPage_SinglePlayer_LoadGame_Level8 },
    { "MenuPage_SinglePlayer_LoadGame_Level9",  MenuPage_SinglePlayer_LoadGame_Level9 },
    { "MenuPage_SinglePlayer_LoadGame_Level10", MenuPage_SinglePlayer_LoadGame_Level10 },
    { "MenuPage_SinglePlayer_LoadGame_Level11", MenuPage_SinglePlayer_LoadGame_Level11 },
    { "MenuPage_SinglePlayer_LoadGame_Level12", MenuPage_SinglePlayer_LoadGame_Level12 },
    { "MenuPage_SinglePlayer_LoadGame_Level13", MenuPage_SinglePlayer_LoadGame_Level13 },
    { "MenuPage_SinglePlayer_LoadGame_Level14", MenuPage_SinglePlayer_LoadGame_Level14 },
    { "MenuPage_Options_EditPlayers",           MenuPage_Options_EditPlayers },
    { "MenuPage_Options_Difficulty",            MenuPage_Options_Difficulty },
    { "MenuPage_Options_Controls",              MenuPage_Options_Controls },
    { "MenuPage_Options_Display",               MenuPage_Options_Display },
    { "MenuPage_Options_Audio",                 MenuPage_Options_Audio },
    { "MenuPage_Multiplayer_LevelRacing",       MenuPage_Multiplayer_LevelRacing },
    { "MenuPage_Multiplayer_EditMacros",        MenuPage_Multiplayer_EditMacros },

    // Ingame menu
    { "MenuPage_EndGame",                       MenuPage_EndGame },
    { "MenuPage_EndLife",                       MenuPage_EndLife }
};

std::map<std::string, SDL_Scancode> g_StringToSDLKeyCodeMap =
{
    { "Escape", SDL_SCANCODE_ESCAPE },
    { "Space",  SDL_SCANCODE_SPACE },
    { "Enter",  SDL_SCANCODE_RETURN },
    { "A", SDL_SCANCODE_A },
    { "B", SDL_SCANCODE_B },
    { "C", SDL_SCANCODE_C },
    { "D", SDL_SCANCODE_D },
    { "E", SDL_SCANCODE_E },
    { "F", SDL_SCANCODE_F },
    { "G", SDL_SCANCODE_G },
    { "H", SDL_SCANCODE_H },
    { "I", SDL_SCANCODE_I },
    { "J", SDL_SCANCODE_J },
    { "K", SDL_SCANCODE_K },
    { "L", SDL_SCANCODE_L },
    { "M", SDL_SCANCODE_M },
    { "N", SDL_SCANCODE_N },
    { "O", SDL_SCANCODE_O },
    { "P", SDL_SCANCODE_P },
    { "Q", SDL_SCANCODE_Q },
    { "R", SDL_SCANCODE_R },
    { "S", SDL_SCANCODE_S },
    { "T", SDL_SCANCODE_T },
    { "U", SDL_SCANCODE_U },
    { "V", SDL_SCANCODE_V },
    { "W", SDL_SCANCODE_W },
    { "X", SDL_SCANCODE_X },
    { "Y", SDL_SCANCODE_Y },
    { "Z", SDL_SCANCODE_Z },
    { "0", SDL_SCANCODE_0 },
    { "1", SDL_SCANCODE_1 },
    { "2", SDL_SCANCODE_2 },
    { "3", SDL_SCANCODE_3 },
    { "4", SDL_SCANCODE_4 },
    { "5", SDL_SCANCODE_5 },
    { "6", SDL_SCANCODE_6 },
    { "7", SDL_SCANCODE_7 },
    { "8", SDL_SCANCODE_8 },
    { "LeftArrow", SDL_SCANCODE_LEFT },
    { "RightArrow", SDL_SCANCODE_RIGHT },
};

std::map<std::string, MenuItemType> g_StringToMenuItemType =
{
    { "Text",       MenuItemType_Text },
    { "Button",     MenuItemType_Button },
    { "Slider",     MenuItemType_Slider },
    { "Image",      MenuItemType_Image },
};

static SDL_Rect GetScreenRect()
{
    Point windowSize = g_pApp->GetWindowSize();
    Point scale = g_pApp->GetScale();
    int targetWidth = (int)(windowSize.x / scale.x);
    int targetHeight = (int)(windowSize.y / scale.y);

    return { 0, 0, targetWidth, targetHeight };
}

static shared_ptr<Image> TryLoadPcxImageFromXmlElement(TiXmlElement* pElem)
{
    if (pElem == NULL)
    {
        return nullptr;
    }

    std::string imagePath = pElem->GetText();
    // This should not happen
    assert(!imagePath.empty());

    return PcxResourceLoader::LoadAndReturnImage(imagePath.c_str(), true, { 0, 0, 0, 0 });
}

static shared_ptr<Image> LoadImageFromXmlElement(TiXmlElement* pElem)
{
    if (pElem == NULL)
    {
        return nullptr;
    }

    std::string imagePath = pElem->GetText();
    // Length has to be atleast 5: A.BCD - one char for name, dot and 3 chars extension
    if (imagePath.length() <= 5)
    {
        return nullptr;
    }

    // Transform to lowercase to support all naming conventions
    std::transform(imagePath.begin(), imagePath.end(), imagePath.begin(), (int(*)(int)) std::tolower);

    // We are ASSUMING here that all PCXs are from original CLAW.REZ archive
    // For our own purpose we will use PNGs or JPEGs
    std::string extension = imagePath.substr(imagePath.length() - 4);
    shared_ptr<Image> pImage;
    if (extension == ".pcx")
    {
        pImage = PcxResourceLoader::LoadAndReturnImage(imagePath.c_str(), true, { 0, 0, 0, 0 });
    }
    else if (extension == ".png")
    {
        pImage = PngResourceLoader::LoadAndReturnImage(imagePath.c_str());
    }
    else if (extension == ".pid")
    {
        pImage = PidResourceLoader::LoadAndReturnImage(imagePath.c_str(), g_pApp->GetCurrentPalette());
    }
    else if (extension == ".jpg")
    {
        assert(false && "Unsupported");
    }
    else if (extension == ".jpeg")
    {
        assert(false && "Unsupported");
    }

    return pImage;
}

static MenuItemType StringToMenuItemTypeEnum(const std::string& str)
{
    if (str == "Text")
    {
        return MenuItemType_Text;
    }
    else if (str == "Button")
    {
        return MenuItemType_Button;
    }
    else if (str == "Slider")
    {
        return MenuItemType_Slider;
    }
    
    LOG_ERROR("Conflicting string: " + str);
    assert(false && "Unknown menu item type string");

    return MenuItemType_None;
}

static MenuPage StringToMenuPageEnum(const std::string& str)
{
    auto findIt = g_StringToMenuPageEnumMap.find(str);
    if (findIt == g_StringToMenuPageEnumMap.end())
    {
        LOG_ERROR("Conflicting string: " + str);
        assert(false && "Unknown menu page string");
    }

    return findIt->second;
}

static SDL_Scancode StringToSDLKeycode(const std::string& str)
{
    auto findIt = g_StringToSDLKeyCodeMap.find(str);
    if (findIt == g_StringToSDLKeyCodeMap.end())
    {
        LOG_ERROR("Conflicting string: " + str);
        assert(false && "Unknown hotkey string");
    }

    return findIt->second;
}

static MenuItemState StringToMenuItemStateEnum(const std::string& str)
{
    if (str == "Active")
    {
        return MenuItemState_Active;
    }
    else if (str == "Disabled")
    {
        return MenuItemState_Disabled;
    }
    else if (str == "Inactive")
    {
        return MenuItemState_Inactive;
    }

    LOG_ERROR("Conflicting string: " + str);
    assert(false && "Unknown menu item state string");

    return MenuItemState_None;
}

static MenuItemType StringToMenuItemType(const std::string& str)
{
    auto findIt = g_StringToMenuItemType.find(str);
    if (findIt == g_StringToMenuItemType.end())
    {
        LOG_ERROR("Conflicting string: " + str);
        assert(false && "Unknown menu item type");
    }

    return findIt->second;
}

static IEventDataPtr XmlElemToGeneratedEvent(TiXmlElement* pElem)
{
    if (pElem == NULL)
    {
        return nullptr;
    }

    IEventDataPtr pEventData;
    std::string eventType;
    ParseValueFromXmlElem(&eventType, pElem->FirstChildElement("Type"));
    if (eventType == "SwitchPage")
    {
        std::string pageName;
        ParseValueFromXmlElem(&pageName, pElem->FirstChildElement("PageName"));

        pEventData.reset(new EventData_Menu_SwitchPage(pageName));
    }
    else if (eventType == "LoadGame")
    {
        bool isNewGame;
        int levelNumber, checkpointNumber;
        ParseValueFromXmlElem(&isNewGame, pElem->FirstChildElement("IsNewGame"));
        ParseValueFromXmlElem(&levelNumber, pElem->FirstChildElement("LevelNumber"));
        ParseValueFromXmlElem(&checkpointNumber, pElem->FirstChildElement("CheckpointNumber"));

        pEventData.reset(new EventData_Menu_LoadGame(levelNumber, isNewGame, checkpointNumber));
        //pEventData.reset(new EventData_Finished_Level());
    }
    else if (eventType == "QuitGame")
    {
        pEventData.reset(new EventData_Quit_Game());
    }
    else if (eventType == "ChangeSoundEnabled")
    {
        bool isEnabled;
        bool isMusic;
        ParseValueFromXmlElem(&isEnabled, pElem->FirstChildElement("IsEnabled"));
        ParseValueFromXmlElem(&isMusic, pElem->FirstChildElement("IsMusic"));

        pEventData.reset(new EventData_Sound_Enabled_Changed(isEnabled, isMusic));
    }
    else if (eventType == "ModifyMenuItemVisibility")
    {
        std::string menuItemName;
        bool isVisible;
        ParseValueFromXmlElem(&menuItemName, pElem->FirstChildElement("MenuItemName"));
        ParseValueFromXmlElem(&isVisible, pElem->FirstChildElement("IsVisible"));

        pEventData.reset(new EventData_Menu_Modifiy_Item_Visibility(menuItemName, isVisible));
    }
    else if (eventType == "PlaySound")
    {
        std::string soundName;
        int volume = 100;
        bool isMusic;
        int loops = 0;
        ParseValueFromXmlElem(&soundName, pElem->FirstChildElement("SoundName"));
        ParseValueFromXmlElem(&volume, pElem->FirstChildElement("Volume"));
        ParseValueFromXmlElem(&isMusic, pElem->FirstChildElement("IsMusic"));
        ParseValueFromXmlElem(&loops, pElem->FirstChildElement("Loops"));

        SoundInfo soundInfo(soundName);
        soundInfo.soundVolume = volume;
        soundInfo.isMusic = isMusic;
        soundInfo.loops = loops;
        pEventData.reset(new EventData_Request_Play_Sound(soundInfo));
    }
    else if (eventType == "ModifyMenuItemState")
    {
        std::string menuItemName;
        std::string menuItemState;
        ParseValueFromXmlElem(&menuItemName, pElem->FirstChildElement("MenuItemName"));
        ParseValueFromXmlElem(&menuItemState, pElem->FirstChildElement("State"));

        pEventData.reset(new EventData_Menu_Modify_Item_State(menuItemName, menuItemState));
    }
    else if (eventType == "ModifyVolume")
    {
        int deltaVolume;
        if (!ParseValueFromXmlElem(&deltaVolume, pElem->FirstChildElement("DeltaVolume")))
        {
            LOG_ERROR("No delta volume defined in ModifyVolume event !");
            return nullptr;
        }
        bool isMusic;
        if (!ParseValueFromXmlElem(&isMusic, pElem->FirstChildElement("IsMusic")))
        {
            LOG_ERROR("No sound specification (sound X music) defined in ModifyVolume event !");
            return nullptr;
        }

        pEventData.reset(new EventData_Set_Volume(deltaVolume, true, isMusic));
    }
    else if (eventType == "ResumeGame")
    {
        pEventData.reset(new EventData_IngameMenu_Resume_Game());
    }
    else if (eventType == "EndLife")
    {
        pEventData.reset(new EventData_IngameMenu_End_Life());
    }
    else if (eventType == "EndGame")
    {
        pEventData.reset(new EventData_IngameMenu_End_Game());
    }
    else
    {
        return nullptr;
    }

    return pEventData;
}

Point g_MenuScale = Point(1.0, 1.0);

//-----------------------------------------------------------------------------
//
// ScreenElementMenu implementation
//
//-----------------------------------------------------------------------------

ScreenElementMenu::ScreenElementMenu(SDL_Renderer* pRenderer)
    :
    m_pRenderer(pRenderer),
    m_bIsVisible(true),
    m_MenuType(MenuType_None)
{
    IEventMgr::Get()->VAddListener(MakeDelegate(
        this, &ScreenElementMenu::SwitchPageDelegate), EventData_Menu_SwitchPage::sk_EventType);
    IEventMgr::Get()->VAddListener(MakeDelegate(
        this, &ScreenElementMenu::ModifyMenuItemVisibilityDelegate), EventData_Menu_Modifiy_Item_Visibility::sk_EventType);
    IEventMgr::Get()->VAddListener(MakeDelegate(
        this, &ScreenElementMenu::ModifyMenuItemStateDelegate), EventData_Menu_Modify_Item_State::sk_EventType);
}

ScreenElementMenu::~ScreenElementMenu()
{
    m_MenuPageMap.clear();

    IEventMgr::Get()->VRemoveListener(MakeDelegate(
        this, &ScreenElementMenu::SwitchPageDelegate), EventData_Menu_SwitchPage::sk_EventType);
    IEventMgr::Get()->VRemoveListener(MakeDelegate(
        this, &ScreenElementMenu::ModifyMenuItemVisibilityDelegate), EventData_Menu_Modifiy_Item_Visibility::sk_EventType);
    IEventMgr::Get()->VRemoveListener(MakeDelegate(
        this, &ScreenElementMenu::ModifyMenuItemStateDelegate), EventData_Menu_Modify_Item_State::sk_EventType);

    if (m_MenuType == MenuType_IngameMenu)
    {
        IEventMgr::Get()->VRemoveListener(MakeDelegate(
            this, &ScreenElementMenu::IngameMenuResumeGameDelegate), EventData_IngameMenu_Resume_Game::sk_EventType);
        IEventMgr::Get()->VRemoveListener(MakeDelegate(
            this, &ScreenElementMenu::IngameMenuEndLifeDelegate), EventData_IngameMenu_End_Life::sk_EventType);
        IEventMgr::Get()->VRemoveListener(MakeDelegate(
            this, &ScreenElementMenu::IngameMenuEndGameDelegate), EventData_IngameMenu_End_Game::sk_EventType);
    }
}

void ScreenElementMenu::VOnUpdate(uint32 msDiff)
{
    if (m_bIsVisible && m_MenuType == MenuType_IngameMenu)
    {
        if (g_pApp && g_pApp->GetGameLogic())
        {
            g_pApp->GetGameLogic()->SetRunning(false);
        }
    }

    assert(m_pActiveMenuPage);
    m_pActiveMenuPage->VOnUpdate(msDiff);
}

void ScreenElementMenu::VOnRender(uint32 msDiff)
{
    // Menu DOES NOT use renderer scaling
    SDL_RenderSetScale(m_pRenderer, 1.0f, 1.0f);

    assert(m_pBackground != nullptr);
    assert(m_pBackground->GetTexture() != NULL);

    SDL_Rect backgroundRect = GetScreenRect();
    SDL_RenderCopy(m_pRenderer, m_pBackground->GetTexture(), &backgroundRect, NULL);

    assert(m_pActiveMenuPage);
    m_pActiveMenuPage->VOnRender(msDiff);

    // Restore scale
    Point scale = Point(g_pApp->GetGameConfig()->scale, g_pApp->GetGameConfig()->scale);
    SDL_RenderSetScale(m_pRenderer, (float)scale.x, (float)scale.y);
}

bool ScreenElementMenu::VOnEvent(SDL_Event& evt)
{
    return m_pActiveMenuPage->VOnEvent(evt);
}

void ScreenElementMenu::VSetVisible(bool visible)
{
    if (visible == m_bIsVisible)
    {
        return;
    }

    if (visible)
    {
        if (g_pApp && g_pApp->GetGameLogic())
        {
            g_pApp->GetGameLogic()->SetRunning(false);
        }

        if (!m_MenuEnterSound.empty())
        {
            // Play some music
            SoundInfo soundInfo(m_MenuEnterSound);
            IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
                new EventData_Request_Play_Sound(soundInfo)));
        }

        // Use SwitchPage event ?
        m_pActiveMenuPage = m_MenuPageMap[m_DefaultMenuPage];
        m_pActiveMenuPage->OnPageLoaded();
    }
    else
    {
        if (g_pApp && g_pApp->GetGameLogic())
        {
            g_pApp->GetGameLogic()->SetRunning(true);
        }
    }

    m_bIsVisible = visible;
}

bool ScreenElementMenu::Initialize(TiXmlElement* pElem)
{
    assert(m_pRenderer != NULL);

    std::string menuTypeStr;
    DO_AND_CHECK(ParseValueFromXmlElem(&menuTypeStr, pElem->FirstChildElement("MenuType")));
    if (menuTypeStr == "MenuType_MainMenu")
    {
        m_MenuType = MenuType_MainMenu;
    }
    else if (menuTypeStr == "MenuType_IngameMenu")
    {
        m_MenuType = MenuType_IngameMenu;
    }
    
    assert(m_MenuType != MenuType_None);

    if (m_MenuType == MenuType_IngameMenu)
    {
        IEventMgr::Get()->VAddListener(MakeDelegate(
            this, &ScreenElementMenu::IngameMenuResumeGameDelegate), EventData_IngameMenu_Resume_Game::sk_EventType);
        IEventMgr::Get()->VAddListener(MakeDelegate(
            this, &ScreenElementMenu::IngameMenuEndLifeDelegate), EventData_IngameMenu_End_Life::sk_EventType);
        IEventMgr::Get()->VAddListener(MakeDelegate(
            this, &ScreenElementMenu::IngameMenuEndGameDelegate), EventData_IngameMenu_End_Game::sk_EventType);
    }

    std::string defaultMenuPageName;
    ParseValueFromXmlElem(&defaultMenuPageName, pElem->FirstChildElement("StartingPage"));
    m_DefaultMenuPage = StringToMenuPageEnum(defaultMenuPageName);

    //
    // ---------- Background Image ----------
    //
    std::string backgroundImagePath;
    ParseValueFromXmlElem(&backgroundImagePath, pElem->FirstChildElement("BackgroundImage"));

    // This is hack for now
    if (backgroundImagePath == "IngameMenuBackground")
    {
        m_pBackground = shared_ptr<Image>(new Image(Util::CreateSDLTextureRect(640, 480, COLOR_BLACK, m_pRenderer, 127)));
    }
    else
    {
        m_pBackground = PcxResourceLoader::LoadAndReturnImage(backgroundImagePath.c_str());
    }
    assert(m_pBackground != nullptr);

    //
    // ---------- Background Music ----------
    //
    std::string backgroundMusicPath;
    ParseValueFromXmlElem(&backgroundMusicPath, pElem->FirstChildElement("BackgroundMusic"));

    if (!backgroundMusicPath.empty())
    {
        // Play some music
        SoundInfo soundInfo(backgroundMusicPath);
        soundInfo.loops = -1;
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));
    }

    //
    // ---------- Enter Menu Sound ----------
    //
    ParseValueFromXmlElem(&m_MenuEnterSound, pElem->FirstChildElement("MenuEnterSound"));

    // Load all menu pages
    for (TiXmlElement* pMenuPageElem = pElem->FirstChildElement("MenuPage");
        pMenuPageElem != NULL;
        pMenuPageElem = pMenuPageElem->NextSiblingElement("MenuPage"))
    {
        std::string pageName;
        ParseValueFromXmlElem(&pageName, pMenuPageElem->FirstChildElement("PageName"));
        assert(!pageName.empty());
        MenuPage pageType = StringToMenuPageEnum(pageName);

        shared_ptr<ScreenElementMenuPage> pPage(new ScreenElementMenuPage(m_pRenderer));
        if (!pPage->Initialize(pMenuPageElem))
        {
            LOG_ERROR("Could not initialize menu page: " + pageName);
            return false;
        }

        m_MenuPageMap.insert(std::make_pair(pageType, pPage));
    }

    m_pActiveMenuPage = m_MenuPageMap[m_DefaultMenuPage];
    assert(m_pActiveMenuPage != nullptr);

    // Lets just assume that the background will take the whole screen
    // From that we can calculate current scale (can't use predefined scale here)
    Point windowSize = g_pApp->GetWindowSize();
    g_MenuScale.Set(windowSize.x / m_pBackground->GetWidth(), windowSize.y / m_pBackground->GetHeight());

    return true;
}

void ScreenElementMenu::SwitchPageDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Menu_SwitchPage> pCastEventData = 
        static_pointer_cast<EventData_Menu_SwitchPage>(pEventData);

    MenuPage pageType = StringToMenuPageEnum(pCastEventData->GetNewPageName());
    auto findIt = m_MenuPageMap.find(pageType);
    if (findIt != m_MenuPageMap.end())
    {
        m_pActiveMenuPage = findIt->second;
        m_pActiveMenuPage->OnPageLoaded();
    }
    else
    {
        LOG_ERROR("Could not switch to page: " + pCastEventData->GetNewPageName() + ", Probably not implemented yet");
    }
}

void ScreenElementMenu::ModifyMenuItemVisibilityDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Menu_Modifiy_Item_Visibility> pCastEventData =
        static_pointer_cast<EventData_Menu_Modifiy_Item_Visibility>(pEventData);

    std::string menuItemName = pCastEventData->GetMenuItemName();
    bool isVisible = pCastEventData->GetIsVisible();

    shared_ptr<ScreenElementMenuItem> pMenuItem = m_pActiveMenuPage->FindMenuItemByName(menuItemName);
    if (pMenuItem)
    {
        pMenuItem->VSetVisible(isVisible);
    }
    else
    {
        LOG_ERROR("Could not find menu item: " + menuItemName);
    }
}

void ScreenElementMenu::ModifyMenuItemStateDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Menu_Modify_Item_State> pCastEventData =
        static_pointer_cast<EventData_Menu_Modify_Item_State>(pEventData);

    MenuItemState state = StringToMenuItemStateEnum(pCastEventData->GetMenuItemState());
    shared_ptr<ScreenElementMenuItem> pMenuItem =
        m_pActiveMenuPage->FindMenuItemByName(pCastEventData->GetMenuItemName());
    if (pMenuItem)
    {
        pMenuItem->SetState(state);
    }
    else
    {
        LOG_ERROR("Could not find menu item: " + pCastEventData->GetMenuItemName());
    }
}

void ScreenElementMenu::IngameMenuResumeGameDelegate(IEventDataPtr pEventData)
{
    VSetVisible(false);
}

void ScreenElementMenu::IngameMenuEndLifeDelegate(IEventDataPtr pEventData)
{
    VSetVisible(false);
}

void ScreenElementMenu::IngameMenuEndGameDelegate(IEventDataPtr pEventData)
{
    VSetVisible(false);
}

//-----------------------------------------------------------------------------
//
// ScreenElementMenuPage implementation
//
//-----------------------------------------------------------------------------

ScreenElementMenuPage::ScreenElementMenuPage(SDL_Renderer* pRenderer)
    :
    m_pBackground(NULL),
    m_pRenderer(pRenderer)
{

}

ScreenElementMenuPage::~ScreenElementMenuPage()
{
    m_MenuItems.clear();
}

void ScreenElementMenuPage::VOnUpdate(uint32 msDiff)
{
    for (shared_ptr<ScreenElementMenuItem> pMenuItem : m_MenuItems)
    {
        pMenuItem->VOnUpdate(msDiff);
    }
}

void ScreenElementMenuPage::VOnRender(uint32 msDiff)
{
    if (m_pBackground)
    {
        assert(m_pBackground->GetTexture() != NULL);

        SDL_Rect backgroundRect = GetScreenRect();
        SDL_RenderCopy(m_pRenderer, m_pBackground->GetTexture(), &backgroundRect, NULL);
    }

    for (shared_ptr<ScreenElementMenuItem> pMenuItem : m_MenuItems)
    {
        if (pMenuItem->VIsVisible())
        {
            pMenuItem->VOnRender(msDiff);
        }
    }
}

bool ScreenElementMenuPage::VOnEvent(SDL_Event& evt)
{
    int activeMenuItemIdx = GetActiveMenuItemIdx();
    if (m_MenuItems.size() > 0)
    {
        assert(activeMenuItemIdx >= 0);
    }

    // Cannot be switch-case since I have to check the "repeat" aswell
    if (evt.type == SDL_KEYDOWN)
    {
        if (evt.key.repeat != 0)
        {
            return false;
        }

        SDL_Keycode keyCode = SDL_GetScancodeFromKey(evt.key.keysym.sym);
        if (keyCode == SDL_SCANCODE_DOWN)
        {
            MoveToMenuItemIdx(activeMenuItemIdx, 1);
            return true;
        }
        else if (keyCode == SDL_SCANCODE_UP)
        {
            MoveToMenuItemIdx(activeMenuItemIdx, -1);
            return true;
        }
        else if (m_KeyToEventMap.find(keyCode) != m_KeyToEventMap.end())
        {
            // HACK:
            if (keyCode == SDL_SCANCODE_ESCAPE ||
                keyCode == SDL_SCANCODE_SPACE || 
                keyCode == SDL_SCANCODE_RETURN)
            {
                SoundInfo soundInfo(SOUND_MENU_SELECT_MENU_ITEM);
                IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
                    new EventData_Request_Play_Sound(soundInfo)));
            }
            IEventMgr::Get()->VQueueEvent(m_KeyToEventMap[keyCode]);
            return true;
        }
        else if (keyCode == SDL_SCANCODE_SPACE ||
                 keyCode == SDL_SCANCODE_RETURN ||
                 keyCode == SDL_SCANCODE_KP_ENTER)
        {
            if (shared_ptr<ScreenElementMenuItem> pActiveMenuItem = GetActiveMenuItem())
            {
                if (!pActiveMenuItem->Press())
                {
                    LOG_WARNING("No event is assigned to button: " + pActiveMenuItem->GetName());
                }
                return true;
            }
            else
            {
                LOG_WARNING("Could not find any active menu item !");
            }
        }
    }
    else if (evt.type == SDL_MOUSEBUTTONDOWN)
    {
        if (evt.button.button == SDL_BUTTON_LEFT)
        {
            SDL_Rect clickRect;
            clickRect.x = (int)(evt.button.x / g_MenuScale.x);
            clickRect.y = (int)(evt.button.y / g_MenuScale.y);
            clickRect.w = 1;
            clickRect.h = 1;

            for (shared_ptr<ScreenElementMenuItem> pMenuItem : m_MenuItems)
            {
                SDL_Rect itemRect = pMenuItem->GetMenuItemRect();
                if (SDL_HasIntersection(&clickRect, &itemRect))
                {
                    if (pMenuItem->CanBeFocused())
                    {
                        DeactivateAllMenuItems();
                        pMenuItem->Press();
                        return true;
                    }
                }
            }
        }
    }

    for (shared_ptr<ScreenElementMenuItem> pMenuItem : m_MenuItems)
    {
        if (pMenuItem->VOnEvent(evt))
        {
            return true;
        }
    }

    return false;
}

bool ScreenElementMenuPage::Initialize(TiXmlElement* pElem)
{
    std::string pageName;
    ParseValueFromXmlElem(&pageName, pElem->FirstChildElement("PageName"));
    m_PageType = StringToMenuPageEnum(pageName);

    // This can be nullptr
    m_pBackground = LoadImageFromXmlElement(pElem->FirstChildElement("BackgroundImage"));

    // Load all menu items
    for (TiXmlElement* pMenuItemElem = pElem->FirstChildElement("MenuItem");
        pMenuItemElem != NULL;
        pMenuItemElem = pMenuItemElem->NextSiblingElement("MenuItem"))
    {
        shared_ptr<ScreenElementMenuItem> pItem(new ScreenElementMenuItem(m_pRenderer));
        if (!pItem->Initialize(pMenuItemElem))
        {
            LOG_ERROR("Could not initialize menu item");
            return false;
        }

        m_MenuItems.push_back(pItem);
    }

    // Load all key events
    for (TiXmlElement* pKeyboardEvent = pElem->FirstChildElement("KeyboardEvent");
        pKeyboardEvent != NULL;
        pKeyboardEvent = pKeyboardEvent->NextSiblingElement("KeyboardEvent"))
    {
        std::string keyStr;
        DO_AND_CHECK(ParseValueFromXmlElem(&keyStr, pKeyboardEvent->FirstChildElement("KeyType")));

        auto findIt = g_StringToSDLKeyCodeMap.find(keyStr);
        if (findIt == g_StringToSDLKeyCodeMap.end())
        {
            LOG_ERROR("Failed to find corresponding SDL key to : " + keyStr);
            return false;
        }

        IEventDataPtr pGeneratedEvent = 
            XmlElemToGeneratedEvent(pKeyboardEvent->FirstChildElement("GeneratedEvent"));
        if (pGeneratedEvent == nullptr)
        {
            LOG_ERROR("Failed to create generated event for menu keyboard key: " + keyStr);
            return false;
        }

        m_KeyToEventMap.insert(std::make_pair(findIt->second, pGeneratedEvent));
    }

    return true;
}

void ScreenElementMenuPage::DeactivateAllMenuItems()
{
    for (shared_ptr<ScreenElementMenuItem> pMenuItem : m_MenuItems)
    {
        if (pMenuItem->GetState() == MenuItemState_Active)
        {
            pMenuItem->SetState(MenuItemState_Inactive);
        }
    }
}

int ScreenElementMenuPage::GetActiveMenuItemIdx()
{
    int idx = 0;
    for (shared_ptr<ScreenElementMenuItem> pMenuItem : m_MenuItems)
    {
        if (pMenuItem->GetState() == MenuItemState_Active)
        {
            return idx;
        }
        idx++;
    }

    return -1;
}

shared_ptr<ScreenElementMenuItem> ScreenElementMenuPage::GetActiveMenuItem()
{
    for (shared_ptr<ScreenElementMenuItem> pMenuItem : m_MenuItems)
    {
        if (pMenuItem->GetState() == MenuItemState_Active)
        {
            return pMenuItem;
        }
    }

    return nullptr;
}

bool ScreenElementMenuPage::MoveToMenuItemIdx(int oldIdx, int idxIncrement, bool playSound)
{
    if (m_MenuItems.empty())
    {
        return false;
    }

    DeactivateAllMenuItems();

    // Moving up
    int buttonIdx = oldIdx + idxIncrement;
    int tryCount = 0;

    while (true)
    {
        assert(tryCount <= (int)m_MenuItems.size() && "Could not find any button to switch focus to");

        // At first button and we pressed "up" -> move to last button
        if (buttonIdx < 0 && idxIncrement < 0)
        {
            buttonIdx = m_MenuItems.size() - 1;
        }
        // At last button
        if (buttonIdx >= (int)m_MenuItems.size() && idxIncrement > 0)
        {
            buttonIdx = 0;
        }

        if (m_MenuItems[buttonIdx]->Focus())
        {
            break;
        }

        buttonIdx += idxIncrement;
        tryCount++;
    }

    if (playSound)
    {
        // Play sound
        SoundInfo soundInfo(SOUND_MENU_CHANGE_MENU_ITEM);
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));
    }

    return true;
}

void ScreenElementMenuPage::OnPageLoaded()
{
    // Focus on first active button
    MoveToMenuItemIdx(m_MenuItems.size() - 1, 1, false);
}

shared_ptr<ScreenElementMenuItem> ScreenElementMenuPage::FindMenuItemByName(std::string name)
{
    for (shared_ptr<ScreenElementMenuItem> pMenuItem : m_MenuItems)
    {
        if (pMenuItem->GetName() == name)
        {
            return pMenuItem;
        }
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
//
// ScreenElementMenuItem implementation
//
//-----------------------------------------------------------------------------

ScreenElementMenuItem::ScreenElementMenuItem(SDL_Renderer* pRenderer)
    :
    m_pRenderer(pRenderer),
    m_State(MenuItemState_None),
    m_Hotkey(SDL_SCANCODE_UNKNOWN),
    m_bVisible(true)
{

}

ScreenElementMenuItem::~ScreenElementMenuItem()
{

}

void ScreenElementMenuItem::VOnUpdate(uint32 msDiff)
{
    // This is a bit hacky but implementing this via XML would be pain in the ass right now...
    if (m_Name == "SOUND_KNOB")
    {
        int soundVolume = g_pApp->GetAudio()->GetSoundVolume();
        m_Position.SetX(m_DefaultPosition.x + (soundVolume / 10) * 20);
    }
    else if (m_Name == "MUSIC_KNOB")
    {
        int musicVolume = g_pApp->GetAudio()->GetMusicVolume();
        m_Position.SetX(m_DefaultPosition.x + (musicVolume / 2) * 20);
    }
}

void ScreenElementMenuItem::VOnRender(uint32 msDiff)
{
    shared_ptr<Image> pCurrImage = m_Images[m_State];
    if (pCurrImage == nullptr)
    {
        LOG_ERROR("Offending button: " + m_Name + " for state: " + ToStr(m_State));
        assert(false && "Image is in invalid state - no image for given state was found");
    }

    SDL_Rect renderRect;
    renderRect.x = (int)((m_Position.x + pCurrImage->GetOffsetX()) * g_MenuScale.x);
    renderRect.y = (int)(m_Position.y * g_MenuScale.y);
    renderRect.w = (int)(pCurrImage->GetWidth() * g_MenuScale.x);
    renderRect.h = (int)(pCurrImage->GetHeight() * g_MenuScale.y);

    SDL_RenderCopy(m_pRenderer, pCurrImage->GetTexture(), NULL, &renderRect);
}

bool ScreenElementMenuItem::VOnEvent(SDL_Event& evt)
{
    if (evt.type == SDL_KEYDOWN && 
        SDL_GetScancodeFromKey(evt.key.keysym.sym) == m_Hotkey &&
        m_bVisible)
    {
        return Press();
    }

    // Does not make sense to try to process event on inactive menu item
    if (m_State != MenuItemState_Active)
    {
        return false;
    }

    if (evt.type == SDL_KEYDOWN)
    {
        auto findIt = m_KeyToEventMap.find(SDL_GetScancodeFromKey(evt.key.keysym.sym));
        if (findIt != m_KeyToEventMap.end())
        {
            for (IEventDataPtr pEvent : findIt->second)
            {
                IEventMgr::Get()->VQueueEvent(pEvent);
            }
        }
    }

    return false;
}

bool ScreenElementMenuItem::Initialize(TiXmlElement* pElem)
{
    SetPointIfDefined(&m_Position, pElem->FirstChildElement("Position"), "x", "y");
    SetPointIfDefined(&m_DefaultPosition, pElem->FirstChildElement("Position"), "x", "y");
    ParseValueFromXmlElem(&m_Name, pElem->FirstChildElement("Name"));

    std::string menuItemStateStr;
    if (!ParseValueFromXmlElem(&menuItemStateStr, pElem->FirstChildElement("State")))
    {
        LOG_ERROR("No default state for menu item: " + m_Name);
        return false;
    }
    m_State = StringToMenuItemStateEnum(menuItemStateStr);

    if (TiXmlElement* pStateConditionElem = pElem->FirstChildElement("StateCondition"))
    {
        std::string conditionForStateStr, conditionTypeStr;
        ParseValueFromXmlElem(&conditionForStateStr, pStateConditionElem->FirstChildElement("ConditionForState"));
        ParseValueFromXmlElem(&conditionTypeStr, pStateConditionElem->FirstChildElement("ConditionType"));
        if (conditionTypeStr == "CheckpointReached")
        {
            int level = -1;
            int checkpoint = -1;
            ParseValueFromXmlElem(&level, pStateConditionElem->FirstChildElement("Level"));
            ParseValueFromXmlElem(&checkpoint, pStateConditionElem->FirstChildElement("Checkpoint"));
            assert(level != -1 && checkpoint != -1);

            if (g_pApp->GetGameLogic()->GetGameSaveMgr()->HasCheckpointSave(level, checkpoint))
            {
                ParseValueFromXmlElem(&menuItemStateStr, pStateConditionElem->FirstChildElement("ConditionForState"));
                m_State = StringToMenuItemStateEnum(menuItemStateStr);
            }
        }
    }

    if (TiXmlElement* pVisibilityConditionElem = pElem->FirstChildElement("VisibilityCondition"))
    {
        std::string conditionTypeStr;
        ParseValueFromXmlElem(&conditionTypeStr, pVisibilityConditionElem->FirstChildElement("ConditionType"));
        
        // This element is visible only when certain condition is met
        // so it is safe to assume it is not visible by default
        m_bVisible = false;

        if (conditionTypeStr == "SoundOn")
        {
            if (g_pApp->GetAudio()->IsSoundActive())
            {
                m_bVisible = true;
            }
        }
        else if (conditionTypeStr == "SoundOff")
        {
            if (!g_pApp->GetAudio()->IsSoundActive())
            {
                m_bVisible = true;
            }
        }
        else if (conditionTypeStr == "MusicOn")
        {
            if (g_pApp->GetAudio()->IsMusicActive())
            {
                m_bVisible = true;
            }
        }
        else if (conditionTypeStr == "MusicOff")
        {
            if (!g_pApp->GetAudio()->IsMusicActive())
            {
                m_bVisible = true;
            }
        }
        // TODO: Implement these ones
        else if (conditionTypeStr == "VoiceOn")
        {
            m_bVisible = true;
        }
        else if (conditionTypeStr == "VoiceOn")
        {
            m_bVisible = false;
        }
        else if (conditionTypeStr == "AmbientOn")
        {
            m_bVisible = true;
        }
        else if (conditionTypeStr == "AmbientOff")
        {
            m_bVisible = false;
        }
    }
    std::string menuItemTypeStr;
    ParseValueFromXmlElem(&menuItemTypeStr, pElem->FirstChildElement("Type"));
    m_Type = StringToMenuItemType(menuItemTypeStr);

    ParseValueFromXmlElem(&m_bVisible, pElem->FirstChildElement("IsVisible"));

    if (shared_ptr<Image> pImage = LoadImageFromXmlElement(pElem->FirstChildElement("DisabledImage")))
    {
        m_Images.insert(std::make_pair(MenuItemState_Disabled, pImage));
    }
    if (shared_ptr<Image> pImage = LoadImageFromXmlElement(pElem->FirstChildElement("InactiveImage")))
    {
        m_Images.insert(std::make_pair(MenuItemState_Inactive, pImage));
    }
    if (shared_ptr<Image> pImage = LoadImageFromXmlElement(pElem->FirstChildElement("ActiveImage")))
    {
        m_Images.insert(std::make_pair(MenuItemState_Active, pImage));
    }

    /*for (TiXmlElement* pMenuItemImageElem = pElem->FirstChildElement("MenuItemImage");
        pMenuItemImageElem != NULL;
        pMenuItemImageElem = pMenuItemImageElem->NextSiblingElement("MenuItemImage"))
    {
        shared_ptr<MenuItemImage> pMenuItemImage(new MenuItemImage);
        pMenuItemImage->pImage = LoadImageFromXmlElement(pMenuItemImageElem->FirstChildElement("Image"));
        ParseValueFromXmlElem(&pMenuItemImage->imageTag, pMenuItemImageElem->FirstChildElement("ImageTag"));

        std::string stateStr;
        pMenuItemImage->currentState = StringToMenuItemStateEnum(stateStr);

        if (pMenuItemImage->pImage == nullptr)
        {
            LOG_ERROR("Failed to load image with tag: " + pMenuItemImage->imageTag);
            return false;
        }

        m_MenuItemImageList.push_back(pMenuItemImage);
    }*/

    std::string hotkeyStr;
    if (ParseValueFromXmlElem(&hotkeyStr, pElem->FirstChildElement("Hotkey")))
    {
        m_Hotkey = StringToSDLKeycode(hotkeyStr);
    }

    for (TiXmlElement* pGeneratedEventElem = pElem->FirstChildElement("GeneratedEvent");
         pGeneratedEventElem != NULL;
         pGeneratedEventElem = pGeneratedEventElem->NextSiblingElement("GeneratedEvent"))
    {
        IEventDataPtr pEvent = XmlElemToGeneratedEvent(pGeneratedEventElem);
        if (pEvent == nullptr)
        {
            LOG_ERROR("Failed to create generated event for menu item: " + m_Name);
            return false;
        }
        m_GeneratedEventList.push_back(pEvent);
    }

    if (TiXmlElement* pStateEnterEventsElem = pElem->FirstChildElement("StateEnterEvents"))
    {
        for (TiXmlElement* pEventElem = pStateEnterEventsElem->FirstChildElement("Event");
            pEventElem != NULL;
            pEventElem = pEventElem->NextSiblingElement("Event"))
        {
            std::string forStateStr;
            if (!ParseValueFromXmlElem(&forStateStr, pEventElem->FirstChildElement("ForState")))
            {
                LOG_ERROR("StateEnterEvent is missing state element !");
                return false;
            }
            MenuItemState forState = StringToMenuItemStateEnum(forStateStr);

            IEventDataPtr pEvent = XmlElemToGeneratedEvent(pEventElem);
            if (pEvent == nullptr)
            {
                LOG_ERROR("Failed to create generated event for menu item: " + m_Name);
                return false;
            }
            
            m_StateEnterEventMap[forState].push_back(pEvent);
        }
    }

    if (TiXmlElement* pStateEnterEventsElem = pElem->FirstChildElement("StateLeaveEvents"))
    {
        for (TiXmlElement* pEventElem = pStateEnterEventsElem->FirstChildElement("Event");
            pEventElem != NULL;
            pEventElem = pEventElem->NextSiblingElement("Event"))
        {
            std::string forStateStr;
            if (!ParseValueFromXmlElem(&forStateStr, pEventElem->FirstChildElement("ForState")))
            {
                LOG_ERROR("StateEnterEvent is missing state element !");
                return false;
            }
            MenuItemState forState = StringToMenuItemStateEnum(forStateStr);

            IEventDataPtr pEvent = XmlElemToGeneratedEvent(pEventElem);
            if (pEvent == nullptr)
            {
                LOG_ERROR("Failed to create generated event for menu item: " + m_Name);
                return false;
            }

            m_StateLeaveEventMap[forState].push_back(pEvent);
        }
    }

    if (TiXmlElement* pKeyAllKeyEventsElem = pElem->FirstChildElement("KeyEvents"))
    {
        for (TiXmlElement* pKeyEventElem = pKeyAllKeyEventsElem->FirstChildElement("KeyEvent");
            pKeyEventElem != NULL;
            pKeyEventElem = pKeyEventElem->NextSiblingElement("KeyEvent"))
        {
            std::string keyStr;
            if (!ParseValueFromXmlElem(&keyStr, pKeyEventElem->FirstChildElement("Key")))
            {
                LOG_ERROR("KeyEvent has no Key defined !");
                return false;
            }
            SDL_Scancode keyCode = StringToSDLKeycode(keyStr);

            for (TiXmlElement* pEventElem = pKeyEventElem->FirstChildElement("Event");
                pEventElem != NULL;
                pEventElem = pEventElem->NextSiblingElement("Event"))
            {
                IEventDataPtr pEvent = XmlElemToGeneratedEvent(pEventElem);
                if (pEvent == nullptr)
                {
                    LOG_ERROR("Failed to create KeyEvent for menu item: " + m_Name);
                    return false;
                }

                m_KeyToEventMap[keyCode].push_back(pEvent);
            }
        }
    }

    if (m_Images.empty())
    {
        LOG_ERROR("No images for menu item: " + m_Name);
        return false;
    }

    return true;
}

bool ScreenElementMenuItem::Press()
{
    if (CanBeFocused())
    {
        if (m_GeneratedEventList.empty())
        {
            LOG_WARNING("No press events for item: " + m_Name);
        }

        Focus();

        for (IEventDataPtr pEvent : m_GeneratedEventList)
        {
            IEventMgr::Get()->VQueueEvent(pEvent);
        }
        
        SoundInfo soundInfo(SOUND_MENU_SELECT_MENU_ITEM);
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));

        return true;
    }

    return false;
}

bool ScreenElementMenuItem::SetState(MenuItemState state)
{
    if (m_Images.find(state) != m_Images.end())
    {
        if (state != m_State)
        {
            OnStateChanged(state, m_State);
        }

        m_State = state;
        return true;
    }

    return false;
}

bool ScreenElementMenuItem::CanBeFocused()
{
    if (!m_bVisible)
    {
        return false;
    }

    // Only buttons at this time can be focued
    if (m_Type != MenuItemType_Button)
    {
        return false;
    }

    return (m_Images.find(MenuItemState_Active) != m_Images.end() &&
            m_State != MenuItemState_Disabled);
}

bool ScreenElementMenuItem::Focus()
{
    if (!CanBeFocused())
    {
        return false;
    }

    SetState(MenuItemState_Active);
    return true;
}

void ScreenElementMenuItem::OnStateChanged(MenuItemState newState, MenuItemState oldState)
{
    auto stateEnterFindIt = m_StateEnterEventMap.find(newState);
    if (stateEnterFindIt != m_StateEnterEventMap.end())
    {
        for (IEventDataPtr pEvent : stateEnterFindIt->second)
        {
            IEventMgr::Get()->VQueueEvent(pEvent);
        }
    }

    auto stateLeaveFindIt = m_StateLeaveEventMap.find(oldState);
    if (stateLeaveFindIt != m_StateLeaveEventMap.end())
    {
        for (IEventDataPtr pEvent : stateLeaveFindIt->second)
        {
            IEventMgr::Get()->VQueueEvent(pEvent);
        }
    }
}

SDL_Rect ScreenElementMenuItem::GetMenuItemRect()
{
    int itemWidth, itemHeight;
    SDL_QueryTexture(m_Images[m_State]->GetTexture(), NULL, NULL, &itemWidth, &itemHeight);

    SDL_Rect itemRect;
    itemRect.x = (int)m_Position.x;
    itemRect.y = (int)m_Position.y;
    itemRect.w = itemWidth;
    itemRect.h = itemHeight;

    return itemRect;
}