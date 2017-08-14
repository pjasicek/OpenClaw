#include "IngameMenu.h"
#include "../GameApp/BaseGameApp.h"
#include "../GameApp/BaseGameLogic.h"
#include "HumanView.h"

ScreenElementIngameMenu::ScreenElementIngameMenu()
    :
    m_IsVisible(false),
    m_pRenderer(NULL),
    m_pCamera(nullptr),
    m_pBackground(NULL)
{

}

ScreenElementIngameMenu::~ScreenElementIngameMenu()
{
    SDL_DestroyTexture(m_pBackground);
}

bool ScreenElementIngameMenu::Initialize(SDL_Renderer* pRenderer, shared_ptr<CameraNode> pCamera)
{
    assert(pRenderer != NULL);
    assert(pCamera != nullptr);

    m_pRenderer = pRenderer;
    m_pCamera = pCamera;

    m_pBackground = Util::CreateSDLTextureRect(pCamera->GetWidth(), pCamera->GetHeight(), COLOR_BLACK, pRenderer, 127);
    assert(m_pBackground);

    return true;
}

void ScreenElementIngameMenu::VOnLostDevice()
{

}

void ScreenElementIngameMenu::VOnRender(uint32 msDiff)
{
    SDL_Rect renderRect = { 0, 0, (int)m_pCamera->GetWidth(), (int)m_pCamera->GetHeight() };
    SDL_RenderCopy(m_pRenderer, m_pBackground, NULL, &renderRect);
}

void ScreenElementIngameMenu::VOnUpdate(uint32 msDiff)
{

}

bool ScreenElementIngameMenu::VOnEvent(SDL_Event& evt)
{
    return false;
}

void ScreenElementIngameMenu::VSetVisible(bool visible)
{
    if (visible == m_IsVisible)
    {
        return;
    }

    if (visible)
    {
        if (g_pApp && g_pApp->GetGameLogic())
        {
            g_pApp->GetGameLogic()->SetRunning(false);
        }
    }
    else
    {
        if (g_pApp && g_pApp->GetGameLogic())
        {
            g_pApp->GetGameLogic()->SetRunning(true);
        }
    }

    m_IsVisible = visible;
}