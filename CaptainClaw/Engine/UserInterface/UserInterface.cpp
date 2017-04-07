#include "UserInterface.h"

#include "../Graphics2D/Image.h"
#include "../GameApp/BaseGameApp.h"
#include "../GameApp/BaseGameLogic.h"
#include "../GameApp/GameSaves.h"
#include "../Resource/Loaders/PcxLoader.h"
#include "../Resource/Loaders/PngLoader.h"
#include "../Resource/ResourceMgr.h"

std::map<std::string, MenuPage> g_StringToMenuPageEnumMap =
{
    { "MenuPage_Main",                          MenuPage_Main },
    { "MenuPage_SinglePlayer",                  MenuPage_SinglePlayer },
    { "MenuPage_Multiplayer",                   MenuPage_Multiplayer },
    { "MenuPage_ReplayMovies",                  MenuPage_ReplayMovies },
    { "MenuPage_Options",                       MenuPage_Options },
    { "MenuPage_Credits",                       MenuPage_Credits },
    { "MenuPage_Help",                          MenuPage_Help },
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
    { "MenuPage_Options_Controls",              MenuPage_Options_Controls },
    { "MenuPage_Options_Display",               MenuPage_Options_Display },
    { "MenuPage_Options_Audio",                 MenuPage_Options_Audio },
    { "MenuPage_Multiplayer_LevelRacing",       MenuPage_Multiplayer_LevelRacing },
    { "MenuPage_Multiplayer_EditMacros",        MenuPage_Multiplayer_EditMacros }
};

std::map<std::string, SDL_Keycode> g_StringToSDLKeyCodeMap =
{
    { "Escape", SDL_SCANCODE_ESCAPE },
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
    { "9", SDL_SCANCODE_9 },
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
    }
    else if (eventType == "QuitGame")
    {
        pEventData.reset(new EventData_Quit_Game());
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
    m_pRenderer(pRenderer)
{
    IEventMgr::Get()->VAddListener(MakeDelegate(
        this, &ScreenElementMenu::SwitchPageDelegate), EventData_Menu_SwitchPage::sk_EventType);
}

ScreenElementMenu::~ScreenElementMenu()
{
    // Restore scale
    Point scale = Point(g_pApp->GetGameConfig()->scale, g_pApp->GetGameConfig()->scale);
    SDL_RenderSetScale(m_pRenderer, (float)scale.x, (float)scale.y);

    m_MenuPageMap.clear();

    IEventMgr::Get()->VRemoveListener(MakeDelegate(
        this, &ScreenElementMenu::SwitchPageDelegate), EventData_Menu_SwitchPage::sk_EventType);
}

void ScreenElementMenu::VOnUpdate(uint32 msDiff)
{
    assert(m_pActiveMenuPage);
    m_pActiveMenuPage->VOnUpdate(msDiff);
}

void ScreenElementMenu::VOnRender(uint32 msDiff)
{
    assert(m_pBackground != nullptr);
    assert(m_pBackground->GetTexture() != NULL);

    SDL_Rect backgroundRect = GetScreenRect();
    SDL_RenderCopy(m_pRenderer, m_pBackground->GetTexture(), &backgroundRect, NULL);

    assert(m_pActiveMenuPage);
    m_pActiveMenuPage->VOnRender(msDiff);
}

bool ScreenElementMenu::VOnEvent(SDL_Event& evt)
{
    return m_pActiveMenuPage->VOnEvent(evt);
}

bool ScreenElementMenu::Initialize(TiXmlElement* pElem)
{
    // Menu DOES NOT use renderer scaling
    SDL_RenderSetScale(m_pRenderer, 1.0f, 1.0f);

    std::string defaultMenuPageName;
    ParseValueFromXmlElem(&defaultMenuPageName, pElem->FirstChildElement("StartingPage"));
    MenuPage defaultPage = StringToMenuPageEnum(defaultMenuPageName);

    std::string backgroundImagePath;
    ParseValueFromXmlElem(&backgroundImagePath, pElem->FirstChildElement("BackgroundImage"));

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

    m_pActiveMenuPage = m_MenuPageMap[defaultPage];
    assert(m_pActiveMenuPage != nullptr);

    // Load background image
    m_pBackground = PcxResourceLoader::LoadAndReturnImage(backgroundImagePath.c_str());
    assert(m_pBackground != nullptr);

    // Lets just assume that the background will take the whole screen
    // From that we can calculate current scale (can't use predefined scale here)
    Point windowSize = g_pApp->GetWindowSize();
    g_MenuScale.Set(windowSize.x / m_pBackground->GetWidth(), windowSize.y / m_pBackground->GetHeight());

    // Play some music
    IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
        new EventData_Request_Play_Sound(SOUND_MENU_MENUMUSIC, 100, false, -1)));

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
        pMenuItem->VOnRender(msDiff);
    }
}

bool ScreenElementMenuPage::VOnEvent(SDL_Event& evt)
{
    int activeMenuItemIdx = GetActiveMenuItemIdx();
    assert(activeMenuItemIdx >= 0);

    //LOG("Event");

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
        else if (keyCode == SDL_SCANCODE_LEFT)
        {
            LOG_WARNING("Left arrow not handled at this time !")
        }
        else if (keyCode == SDL_SCANCODE_RIGHT)
        {
            LOG_WARNING("Right arrow not handled at this time !")
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
            }
            else
            {
                LOG_WARNING("Could not find any active menu item !");
            }
            return true;
        }
        else if (m_KeyToEventMap.find(keyCode) != m_KeyToEventMap.end())
        {
            IEventMgr::Get()->VQueueEvent(m_KeyToEventMap[keyCode]);
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
        ParseValueFromXmlElem(&keyStr, pKeyboardEvent->FirstChildElement("KeyType"));
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

bool ScreenElementMenuPage::MoveToMenuItemIdx(int oldIdx, int idxIncrement)
{
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

    // Play sound
    IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
        new EventData_Request_Play_Sound(SOUND_MENU_CHANGE_MENU_ITEM, 100)));

    return true;
}

void ScreenElementMenuPage::OnPageLoaded()
{
    // Focus on first active button
    MoveToMenuItemIdx(m_MenuItems.size() - 1, 1);
}

//-----------------------------------------------------------------------------
//
// ScreenElementMenuItem implementation
//
//-----------------------------------------------------------------------------

ScreenElementMenuItem::ScreenElementMenuItem(SDL_Renderer* pRenderer)
    :
    m_pRenderer(pRenderer),
    m_State(MenuItemState_None)
{

}

ScreenElementMenuItem::~ScreenElementMenuItem()
{

}

void ScreenElementMenuItem::VOnUpdate(uint32 msDiff)
{

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
    renderRect.x = (int)(m_Position.x * g_MenuScale.x);
    renderRect.y = (int)(m_Position.y * g_MenuScale.y);
    renderRect.w = (int)(pCurrImage->GetWidth() * g_MenuScale.x);
    renderRect.h = (int)(pCurrImage->GetHeight() * g_MenuScale.y);

    SDL_RenderCopy(m_pRenderer, pCurrImage->GetTexture(), NULL, &renderRect);
}

bool ScreenElementMenuItem::VOnEvent(SDL_Event& evt)
{
    // Does not make sense to try to process event on inactive menu item
    if (m_State != MenuItemState_Active)
    {
        return false;
    }

    return false;
}

bool ScreenElementMenuItem::Initialize(TiXmlElement* pElem)
{
    SetPointIfDefined(&m_Position, pElem->FirstChildElement("Position"), "x", "y");
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

    if (shared_ptr<Image> pImage = TryLoadPcxImageFromXmlElement(pElem->FirstChildElement("DisabledImage")))
    {
        m_Images.insert(std::make_pair(MenuItemState_Disabled, pImage));
    }
    if (shared_ptr<Image> pImage = TryLoadPcxImageFromXmlElement(pElem->FirstChildElement("InactiveImage")))
    {
        m_Images.insert(std::make_pair(MenuItemState_Inactive, pImage));
    }
    if (shared_ptr<Image> pImage = TryLoadPcxImageFromXmlElement(pElem->FirstChildElement("ActiveImage")))
    {
        m_Images.insert(std::make_pair(MenuItemState_Active, pImage));
    }
    // TODO: Think of a better way. Should be probably MenuItemState_Custom, not inactive
    if (TiXmlElement* pCustomImageElem = pElem->FirstChildElement("CustomImage"))
    {
        std::string imagePath = pCustomImageElem->GetText();
        shared_ptr<Image> pImage = PngResourceLoader::LoadAndReturnImage(imagePath.c_str());
        assert(pImage != nullptr);
        m_Images.insert(std::make_pair(MenuItemState_Inactive, pImage));
    }

    if (TiXmlElement* pGeneratedEventElem = pElem->FirstChildElement("GeneratedEvent"))
    {
        m_pGeneratedEvent = XmlElemToGeneratedEvent(pGeneratedEventElem);
        if (m_pGeneratedEvent == nullptr)
        {
            LOG_ERROR("Failed to create generated event for menu item: " + m_Name);
            return false;
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
    if (m_pGeneratedEvent)
    {
        IEventMgr::Get()->VQueueEvent(m_pGeneratedEvent);
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(SOUND_MENU_SELECT_MENU_ITEM, 100)));
        return true;
    }

    return false;
}

bool ScreenElementMenuItem::SetState(MenuItemState state)
{
    if (m_Images.find(state) != m_Images.end())
    {
        m_State = state;
        return true;
    }

    return false;
}

bool ScreenElementMenuItem::CanBeFocused()
{
    return (m_Images.find(MenuItemState_Active) != m_Images.end() &&
            m_State != MenuItemState_Disabled);
}

bool ScreenElementMenuItem::Focus()
{
    if (!CanBeFocused())
    {
        return false;
    }

    m_State = MenuItemState_Active;
    return true;
}