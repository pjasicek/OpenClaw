#include "UserInterface.h"

#include "../Graphics2D/Image.h"
#include "../Events/Events.h"
#include "../Events/EventMgr.h"
#include "../GameApp/BaseGameApp.h"

static SDL_Rect GetScreenRect()
{
    Point windowSize = g_pApp->GetWindowSize();
    Point scale = g_pApp->GetScale();
    int targetWidth = (int)(windowSize.x / scale.x);
    int targetHeight = (int)(windowSize.y / scale.y);

    return { 0, 0, targetWidth, targetHeight };
}

//-----------------------------------------------------------------------------
//
// ScreenElementMenu implementation
//
//-----------------------------------------------------------------------------

ScreenElementMenu::ScreenElementMenu(SDL_Renderer* pRenderer)
    :
    m_pBackground(NULL),
    m_pRenderer(pRenderer)
{

}

ScreenElementMenu::~ScreenElementMenu()
{

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
    return true;
}

bool ScreenElementMenu::Initialize(const char* menuXmlPath)
{
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
    return true;
}

//-----------------------------------------------------------------------------
//
// ScreenElementMenuItem implementation
//
//-----------------------------------------------------------------------------

ScreenElementMenuItem::ScreenElementMenuItem(SDL_Renderer* pRenderer)
    :
    m_pRenderer(pRenderer)
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

}

bool ScreenElementMenuItem::VOnEvent(SDL_Event& evt)
{
    return false;
}

bool ScreenElementMenuItem::Initialize(TiXmlElement* pElem)
{
    return true;
}