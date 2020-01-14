#ifndef __USERINTERFACE_H__
#define __USERINTERFACE_H__

#include "../Scene/Scene.h"
#include "../SharedDefines.h"
#include "../Events/Events.h"
#include "../Events/EventMgr.h"
#include <SDL2/SDL.h>

// Doesnt really do anything, just implementation of empty methods to conform to
// IScreenElement interface
class ScreenElementScene : public IScreenElement, public Scene
{
public:
    ScreenElementScene(SDL_Renderer* pRenderer) : Scene(pRenderer) { }
    virtual ~ScreenElementScene() { }

    // IScreenElement implementation
    virtual void VOnLostDevice() { }
    virtual void VOnUpdate(uint32 msDiff) { OnUpdate(msDiff); }
    virtual void VOnRender(uint32 msDiff) { OnRender(); }

    virtual int32 VGetZOrder() const { return 0; }
    virtual void VSetZOrder(int32 const zOrder) { }
    virtual bool VIsVisible() { return true; }
    virtual void VSetVisible(bool visible) { }

    virtual bool VOnEvent(SDL_Event& evt) { return false; }
};

// These are PAGES - they define what buttons are present in given page
enum MenuPage
{
    MenuPage_None,

    // First page you see when you launch the game
    MenuPage_Main,

    // Pages accessible from MenuPage_Main
    MenuPage_SinglePlayer,
    MenuPage_Multiplayer,                  // Unused
    MenuPage_ReplayMovies,                 // Unused
    MenuPage_Options, 
    MenuPage_Credits,                      
    MenuPage_Help,                         
    MenuPage_QuitGame,

    // Pages accessible from MenuPage_Help
    MenuPage_Help_TouchScreen,
    
    // Pages accessible from MenuPage_SinglePlayer
    MenuPage_SinglePlayer_NewGame,
    MenuPage_SinglePlayer_LoadGame,
    MenuPage_SinglePlayer_LoadCustomLevel, // Unused
    MenuPage_SinglePlayer_SaveGame,        // Unused
    MenuPage_SinglePlayer_UploadScores,     // Unused

    MenuPage_SinglePlayer_LoadGame_Level1,
    MenuPage_SinglePlayer_LoadGame_Level2,
    MenuPage_SinglePlayer_LoadGame_Level3,
    MenuPage_SinglePlayer_LoadGame_Level4,
    MenuPage_SinglePlayer_LoadGame_Level5,
    MenuPage_SinglePlayer_LoadGame_Level6,
    MenuPage_SinglePlayer_LoadGame_Level7,
    MenuPage_SinglePlayer_LoadGame_Level8,
    MenuPage_SinglePlayer_LoadGame_Level9,
    MenuPage_SinglePlayer_LoadGame_Level10,
    MenuPage_SinglePlayer_LoadGame_Level11,
    MenuPage_SinglePlayer_LoadGame_Level12,
    MenuPage_SinglePlayer_LoadGame_Level13,
    MenuPage_SinglePlayer_LoadGame_Level14,

    MenuPage_Options_EditPlayers,
    MenuPage_Options_Difficulty,
    MenuPage_Options_Controls,
    MenuPage_Options_Display,
    MenuPage_Options_Audio,

    MenuPage_Multiplayer_LevelRacing,
    MenuPage_Multiplayer_EditMacros,

    // Ingame menu
    MenuPage_EndLife,
    MenuPage_EndGame
};

// These are MENU ITEMS - Buttons, sliders and such
enum MenuItem
{
    // Items in main menu (MenuPage_Main)
    MenuItem_MainMenu_Text,
    MenuItem_MainMenu_SinglePlayer_Button,
    MenuItem_MainMenu_Multiplayer_Button,
    MenuItem_MainMenu_ReplayMovies_Button,
    MenuItem_MainMenu_Options_Button,
    MenuItem_MainMenu_Credicts_Button,
    MenuItem_MainMenu_Help_Button,
    MenuItem_MainMenu_Quit_Button,

    // Items in single player menu (MenuPage_SinglePlayer)
    MenuItem_SinglePlayer_Text,
    MenuItem_SinglePlayer_NewGame_Button,
    MenuItem_SinglePlayer_LoadGame_Button,
    MenuItem_SinglePlayer_LoadCustomLevel_Button,
    MenuItem_SinglePlayer_SaveGame_Button,
    MenuItem_SinglePlayer_UploadScores_Button,
    MenuItem_SinglePlayer_Back_Button

    //....
};

enum MenuType
{
    MenuType_None,
    MenuType_MainMenu,
    MenuType_IngameMenu
};

class Image;
class ScreenElementMenuPage;

typedef std::map<MenuPage, shared_ptr<ScreenElementMenuPage>> MenuPageMap;

// This encapsulates menu background and its children (buttons)
class ScreenElementMenu : public IScreenElement
{
public:
    ScreenElementMenu(SDL_Renderer* pRenderer);
    virtual ~ScreenElementMenu();

    // IScreenElement implementation
    virtual void VOnLostDevice() { }
    virtual void VOnUpdate(uint32 msDiff);
    virtual void VOnRender(uint32 msDiff);

    virtual int32 VGetZOrder() const { return 20000; }
    virtual void VSetZOrder(int32 const zOrder) { }
    virtual bool VIsVisible() override { return m_bIsVisible; }
    virtual void VSetVisible(bool visible) override;

    virtual bool VOnEvent(SDL_Event& evt);

    bool Initialize(TiXmlElement* pElem);

private:
    void SwitchPageDelegate(IEventDataPtr pEventData);
    void ModifyMenuItemVisibilityDelegate(IEventDataPtr pEventData);
    void ModifyMenuItemStateDelegate(IEventDataPtr pEventData);
    void IngameMenuResumeGameDelegate(IEventDataPtr pEventData);
    void IngameMenuEndLifeDelegate(IEventDataPtr pEventData);
    void IngameMenuEndGameDelegate(IEventDataPtr pEventData);

    bool m_bIsVisible;

    MenuType m_MenuType;
    MenuPage m_DefaultMenuPage;

    std::string m_MenuEnterSound;

    shared_ptr<Image> m_pBackground;
    SDL_Renderer* m_pRenderer;

    MenuPageMap m_MenuPageMap;
    shared_ptr<ScreenElementMenuPage> m_pActiveMenuPage;
};

class ScreenElementMenuItem;

typedef std::vector<shared_ptr<ScreenElementMenuItem>> MenuItemList;
typedef std::map<SDL_Keycode, IEventDataPtr> KeyToEventMap;

// This is menupage like MenuPage_Main, MenuPage_SinglePlayer, etc.
class ScreenElementMenuPage : public IScreenElement
{
public:
    ScreenElementMenuPage(SDL_Renderer* pRenderer);
    virtual ~ScreenElementMenuPage();

    // IScreenElement implementation
    virtual void VOnLostDevice() { }
    virtual void VOnUpdate(uint32 msDiff);
    virtual void VOnRender(uint32 msDiff);

    virtual int32 VGetZOrder() const { return 0; }
    virtual void VSetZOrder(int32 const zOrder) { }
    virtual bool VIsVisible() { return true; }
    virtual void VSetVisible(bool visible) { }

    virtual bool VOnEvent(SDL_Event& evt);

    bool Initialize(TiXmlElement* pElem);

    void OnPageLoaded();

    shared_ptr<ScreenElementMenuItem> FindMenuItemByName(std::string name);

private:
    void DeactivateAllMenuItems();
    int GetActiveMenuItemIdx();
    bool MoveToMenuItemIdx(int oldIdx, int idxIncrement, bool playSound = true);
    shared_ptr<ScreenElementMenuItem> GetActiveMenuItem();

    KeyToEventMap m_KeyToEventMap;

    MenuPage m_PageType;
    MenuItemList m_MenuItems;

    shared_ptr<Image> m_pBackground;
    SDL_Renderer* m_pRenderer;
};

enum MenuItemType
{
    MenuItemType_None,
    MenuItemType_Text,
    MenuItemType_Button,
    MenuItemType_Slider,
    MenuItemType_Image
};

enum MenuItemState
{
    MenuItemState_None,
    MenuItemState_Disabled,
    MenuItemState_Inactive,
    MenuItemState_Active
};

/*
// This is a single image with given state
struct MenuItemImage
{
    MenuItemImage()
    {
        pImage = nullptr;
        imageState = MenuItemState_None;
    }

    shared_ptr<Image> pImage;
    MenuItemState imageState;
};

// This is a defined menu item with its images, e.g. menu item with active image and inactive image
struct MenuItemImageContainer
{
    std::string name;
    std::vector<shared_ptr<MenuItemImage>> imageList;
};
*/

// This is menu item like button, slider, text, etc.
class ScreenElementMenuItem : public IScreenElement
{
    typedef std::map<MenuItemState, shared_ptr<Image>> MenuItemImageMap;
    typedef std::vector<IEventDataPtr> EventList;
    typedef std::map<MenuItemState, EventList> StateChangeEventMap;
    typedef std::map<SDL_Keycode, EventList> KeyToEventMap;
    //typedef std::vector<shared_ptr<MenuItemImageContainer>> MenuItemImageContainerList;

public:
    ScreenElementMenuItem(SDL_Renderer* pRenderer);
    virtual ~ScreenElementMenuItem();

    // IScreenElement implementation
    virtual void VOnLostDevice() { }
    virtual void VOnUpdate(uint32 msDiff);
    virtual void VOnRender(uint32 msDiff);

    virtual int32 VGetZOrder() const { return 0; }
    virtual void VSetZOrder(int32 const zOrder) { }
    virtual bool VIsVisible() { return m_bVisible; }
    virtual void VSetVisible(bool visible) { m_bVisible = visible; }

    virtual bool VOnEvent(SDL_Event& evt);

    bool Initialize(TiXmlElement* pElem);

    std::string GetName() const { return m_Name; }

    MenuItemState GetState() { return m_State; }
    bool SetState(MenuItemState state);
    bool Focus();
    bool CanBeFocused();
    bool Press();
    SDL_Rect GetMenuItemRect();

    void OnStateChanged(MenuItemState newState, MenuItemState oldState);

private:
    std::string m_Name;
    MenuItemState m_State;
    Point m_DefaultPosition;
    Point m_Position;
    bool m_bVisible;
    MenuItemType m_Type;

    EventList m_GeneratedEventList;
    StateChangeEventMap m_StateEnterEventMap;
    StateChangeEventMap m_StateLeaveEventMap;
    KeyToEventMap m_KeyToEventMap;
    SDL_Scancode m_Hotkey;

    MenuItemImageMap m_Images;
    //MenuItemImageContainerList m_MenuItemImageContainerList;
    SDL_Renderer* m_pRenderer;
};

#endif
