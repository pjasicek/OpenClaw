#include "UserInterface.h"

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

}

void ScreenElementMenu::VOnRender(uint32 msDiff)
{

}

bool ScreenElementMenu::VOnEvent(SDL_Event& evt)
{
    return true;
}