#include "UserInterface.h"

#include "../Graphics2D/Image.h"
#include "../Events/Events.h"
#include "../Events/EventMgr.h"
#include "../GameApp/BaseGameApp.h"

#include "../Resource/Loaders/PcxLoader.h"

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
    if (str == "MenuPage_Main")
    {
        return MenuPage_Main;
    }
    else if (str == "MenuPage_SinglePlayer")
    {
        return MenuPage_SinglePlayer;
    }
    // Rest ...

    LOG_ERROR("Conflicting string: " + str);
    assert(false && "Unknown menu page string");

    return MenuPage_None;
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

}

ScreenElementMenu::~ScreenElementMenu()
{
    // Restore scale
    Point scale = Point(g_pApp->GetGameConfig()->scale, g_pApp->GetGameConfig()->scale);
    SDL_RenderSetScale(m_pRenderer, (float)scale.x, (float)scale.y);

    m_MenuPageMap.clear();
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

    return true;
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

        if (SDL_GetScancodeFromKey(evt.key.keysym.sym) == SDL_SCANCODE_DOWN)
        {
            MoveToMenuItemIdx(activeMenuItemIdx, 1);
            return true;
        }
        else if (SDL_GetScancodeFromKey(evt.key.keysym.sym) == SDL_SCANCODE_UP)
        {
            MoveToMenuItemIdx(activeMenuItemIdx, -1);
            return true;
        }
        else if (SDL_GetScancodeFromKey(evt.key.keysym.sym) == SDL_SCANCODE_LEFT)
        {
            LOG_WARNING("Left arrow not handled at this time !")
        }
        else if (SDL_GetScancodeFromKey(evt.key.keysym.sym) == SDL_SCANCODE_RIGHT)
        {
            LOG_WARNING("Right arrow not handled at this time !")
        }
        else if (SDL_GetScancodeFromKey(evt.key.keysym.sym) == SDL_SCANCODE_SPACE ||
                 SDL_GetScancodeFromKey(evt.key.keysym.sym) == SDL_SCANCODE_RETURN ||
                 SDL_GetScancodeFromKey(evt.key.keysym.sym) == SDL_SCANCODE_KP_ENTER)
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

    return true;
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
    assert(pCurrImage != nullptr && "Image is in invalid state - no image for given state was found");

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

    if (TiXmlElement* pGeneratedEventElem = pElem->FirstChildElement("GeneratedEvent"))
    {
        std::string eventType;
        ParseValueFromXmlElem(&eventType, pGeneratedEventElem->FirstChildElement("Type"));
        if (eventType == "SwitchPage")
        {
            std::string pageName;
            ParseValueFromXmlElem(&pageName, pGeneratedEventElem->FirstChildElement("PageName"));

            m_pGeneratedEvent.reset(new EventData_Menu_SwitchPage(pageName));
        }
        else if (eventType == "LoadGame")
        {
            bool isNewGame;
            int levelNumber, checkpointNumber;
            ParseValueFromXmlElem(&isNewGame, pGeneratedEventElem->FirstChildElement("IsNewGame"));
            ParseValueFromXmlElem(&levelNumber, pGeneratedEventElem->FirstChildElement("LevelNumber"));
            ParseValueFromXmlElem(&checkpointNumber, pGeneratedEventElem->FirstChildElement("CheckpointNumber"));

            m_pGeneratedEvent.reset(new EventData_Menu_LoadGame(levelNumber, isNewGame, checkpointNumber));
        }
        else if (eventType == "QuitGame")
        {
            m_pGeneratedEvent.reset(new EventData_Quit_Game());
        }
        else
        {
            LOG_ERROR("Unknown event type: " + eventType + " for menu item: " + m_Name);
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