#ifndef __USERINTERFACE_H__
#define __USERINTERFACE_H__

#include "../Scene/Scene.h"
#include "../SharedDefines.h"

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
    // First page you see when you launch the game
    MenuPage_Main,

    // Pages accessible from MenuPage_Main
    MenuPage_SinglePlayer,
    MenuPage_Multiplayer,                  // Unused
    MenuPage_ReplayMovies,                 // Unused
    MenuPage_Options, 
    MenuPage_Credits,                      // Unused
    MenuPage_Help,                         // Unused
    
    // Pages accessible from MenuPage_SinglePlayer
    MenuPage_SinglePlayer_NewGame,
    MenuPage_SinglePlayer_LoadGame,
    MenuPage_SinglePlayer_LoadCustomLevel, // Unused
    MenuPage_SinglePlayer_SaveGame,        // Unused
    MenuPage_SinglePlayer_UploadScores,     // Unused

    // Some more but currently unusable...
    MenuPage_Options_EditPlayers,
    MenuPage_Options_Controls,
    MenuPage_Options_Display,
    MenuPage_Options_Audio,

    MenuPage_Multiplayer_LevelRacing,
    MenuPage_Multiplayer_EditMacros
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

    virtual int32 VGetZOrder() const { return 0; }
    virtual void VSetZOrder(int32 const zOrder) { }
    virtual bool VIsVisible() { return true; }
    virtual void VSetVisible(bool visible) { }

    virtual bool VOnEvent(SDL_Event& evt);

    bool Initialize(TiXmlElement* pElem);
    bool Initialize(const char* menuXmlPath);

private:
    shared_ptr<Image> m_pBackground;
    SDL_Renderer* m_pRenderer;

    MenuPageMap m_MenuPageMap;
    shared_ptr<ScreenElementMenuPage> m_pActiveMenuPage;
};

class ScreenElementMenuItem;

typedef std::vector<shared_ptr<ScreenElementMenuItem>> MenuItemList;

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

private:
    MenuItemList m_MenuItems;

    shared_ptr<Image> m_pBackground;
    SDL_Renderer* m_pRenderer;
};

enum MenuItemType
{
    MenuItemType_Text,
    MenuItemType_Button,
    MenuItemType_Slider
};

enum MenuItemState
{
    MenuItemState_Disabled,
    MenuItemState_Inactive,
    MenuItemState_Active
};

// This is menu item like button, slider, text, etc.
class ScreenElementMenuItem : public IScreenElement
{
    typedef std::map<MenuItemState, shared_ptr<Image>> MenuItemImageMap;

public:
    ScreenElementMenuItem(SDL_Renderer* pRenderer);
    virtual ~ScreenElementMenuItem();

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
    void SetState(MenuItemState state) { m_State = state; }

private:
    MenuItemState m_State;

    MenuItemImageMap m_pImages;
    SDL_Renderer* m_pRenderer;
};

#endif
