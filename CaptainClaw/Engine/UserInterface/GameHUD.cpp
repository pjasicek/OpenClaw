#include "GameHUD.h"
#include "../GameApp/BaseGameApp.h"
#include "../Resource/ResourceCache.h"
#include "../Scene/SceneNodes.h"
#include "../Resource/Loaders/PidLoader.h"
#include "../Graphics2D/Image.h"
#include "../UserInterface/HumanView.h"

#include <SDL2/SDL_ttf.h>

//=============================================================================
// List of exposed HUD elements from scene:
//
// INGAME: 
//
// "score"     - treasure chest in upper left corner
// "stopwatch" - stopwatch under treasure chest in upper left corner - not visible by default
// "health"    - pumping heart in upper right corner
// "pistol"    - ammo  
// "magic"     - ammo
// "dynamite"  - ammo
// "lives"     - claw's head under ammo in upper right corner
//
// IN MAIN MENU:
// ??
//=============================================================================

ScreenElementHUD::ScreenElementHUD()
    :
    m_IsVisible(true),
    m_pFPSTexture(NULL),
    m_pPositionTexture(NULL)
{
   
}

ScreenElementHUD::~ScreenElementHUD()
{
    m_HUDElementsMap.clear();
}

bool ScreenElementHUD::Initialize(SDL_Renderer* pRenderer, shared_ptr<CameraNode> pCamera)
{
    m_pRenderer = pRenderer;
    m_pCamera = pCamera;

    for (uint32 i = 0; i < SCORE_NUMBERS_COUNT; i++)
    {
        m_ScoreNumbers[i] = PidResourceLoader::LoadAndReturnImage("/game/images/interface/scorenumbers/000.pid", g_pApp->GetCurrentPalette());
    }

    for (uint32 i = 0; i < STOPWATCH_NUMBERS_COUNT; i++)
    {
        m_StopwatchNumbers[i] = PidResourceLoader::LoadAndReturnImage("/game/images/interface/scorenumbers/000.pid", g_pApp->GetCurrentPalette());
    }

    for (uint32 i = 0; i < HEALTH_NUMBERS_COUNT; i++)
    {
        m_HealthNumbers[i] = PidResourceLoader::LoadAndReturnImage("/game/images/interface/healthnumbers/000.pid", g_pApp->GetCurrentPalette());
    }

    for (uint32 i = 0; i < AMMO_NUMBERS_COUNT; i++)
    {
        m_AmmoNumbers[i] = PidResourceLoader::LoadAndReturnImage("/game/images/interface/smallnumbers/000.pid", g_pApp->GetCurrentPalette());
    }

    for (uint32 i = 0; i < LIVES_NUMBERS_COUNT; i++)
    {
        m_LivesNumbers[i] = PidResourceLoader::LoadAndReturnImage("/game/images/interface/smallnumbers/000.pid", g_pApp->GetCurrentPalette());
    }

    UpdateFPS(0);

    return true;
}

void ScreenElementHUD::VOnLostDevice()
{

}

void ScreenElementHUD::VOnRender(uint32 msDiff)
{
    int cameraWidth = m_pCamera->GetWidth();

    if (IsElementVisible("score"))
    {
        // Render score numbers
        for (int i = 0; i < SCORE_NUMBERS_COUNT; i++)
        {
            SDL_Rect renderRect = { 40 + i * 13, 5, m_ScoreNumbers[i]->GetWidth(), m_ScoreNumbers[i]->GetHeight() };
            SDL_RenderCopy(m_pRenderer, m_ScoreNumbers[i]->GetTexture(), NULL, &renderRect);
        }
    }

    if (IsElementVisible("health"))
    {
        // Render health numbers
        for (int i = 0; i < HEALTH_NUMBERS_COUNT; i++)
        {
            SDL_Rect renderRect = { cameraWidth - 60 + i * (m_HealthNumbers[i]->GetWidth() - 2), 2, m_HealthNumbers[i]->GetWidth(), m_HealthNumbers[i]->GetHeight() };
            SDL_RenderCopy(m_pRenderer, m_HealthNumbers[i]->GetTexture(), NULL, &renderRect);
        }
    }

    if (IsElementVisible("pistol") || IsElementVisible("dynamite") || IsElementVisible("magic"))
    {
        // Render ammo numbers
        for (int i = 0; i < AMMO_NUMBERS_COUNT; i++)
        {
            SDL_Rect renderRect = { cameraWidth - 46 + i * (m_AmmoNumbers[i]->GetWidth()), 43, m_AmmoNumbers[i]->GetWidth(), m_AmmoNumbers[i]->GetHeight() };
            SDL_RenderCopy(m_pRenderer, m_AmmoNumbers[i]->GetTexture(), NULL, &renderRect);
        }
    }

    if (IsElementVisible("lives"))
    {
        // Render lives numbers
        for (int i = 0; i < LIVES_NUMBERS_COUNT; i++)
        {
            SDL_Rect renderRect = { cameraWidth - 36 + i * (m_AmmoNumbers[i]->GetWidth()), 71, m_LivesNumbers[i]->GetWidth(), m_LivesNumbers[i]->GetHeight() };
            SDL_RenderCopy(m_pRenderer, m_LivesNumbers[i]->GetTexture(), NULL, &renderRect);
        }
    }

    if (IsElementVisible("stopwatch"))
    {
        // Render stopwatch numbers
        for (int i = 0; i < STOPWATCH_NUMBERS_COUNT; i++)
        {
            SDL_Rect renderRect = { 40 + i * 13, 45, m_StopwatchNumbers[i]->GetWidth(), m_StopwatchNumbers[i]->GetHeight() };
            SDL_RenderCopy(m_pRenderer, m_StopwatchNumbers[i]->GetTexture(), NULL, &renderRect);
        }
    }

    if (m_pFPSTexture)
    {
        SDL_Rect renderRect;
        SDL_QueryTexture(m_pFPSTexture, NULL, NULL, &renderRect.w, &renderRect.h);
        renderRect.x = m_pCamera->GetWidth() / 2 - 20;
        renderRect.y = 15;
        SDL_RenderCopy(m_pRenderer, m_pFPSTexture, NULL, &renderRect);
    }

    if (m_pPositionTexture)
    {
        SDL_Rect renderRect;
        SDL_QueryTexture(m_pPositionTexture, NULL, NULL, &renderRect.w, &renderRect.h);
        renderRect.x = m_pCamera->GetWidth() - renderRect.w - 1;
        renderRect.y = m_pCamera->GetHeight() - renderRect.h - 1;
        SDL_RenderCopy(m_pRenderer, m_pPositionTexture, NULL, &renderRect);
    }
}

void ScreenElementHUD::VOnUpdate(uint32 msDiff)
{
    static int msAccumulation = 0;
    static int framesAccumulation = 0;

    UpdateCameraPosition();

    msAccumulation += msDiff;
    framesAccumulation++;
    if (msAccumulation > 1000)
    {
        UpdateFPS(framesAccumulation);
        msAccumulation = 0;
        framesAccumulation = 0;
    }
}

bool ScreenElementHUD::VOnEvent(SDL_Event& evt)
{
    return false;
}

static void SetImageText(uint32 newValue, uint32 divider, shared_ptr<Image>* pField, uint32 fieldSize, std::string textResourcePrefixPath)
{
    for (uint32 i = 0; i < fieldSize; i++)
    {
        uint32 num = (newValue / divider) % 10;
        std::string numStr = ToStr(num);
        std::string resourcePath = textResourcePrefixPath + numStr + ".pid";
        pField[i] = PidResourceLoader::LoadAndReturnImage(resourcePath.c_str(), g_pApp->GetCurrentPalette());
        divider /= 10;
    }
}

void ScreenElementHUD::UpdateScore(uint32 newScore)
{
    SetImageText(newScore, 10000000, m_ScoreNumbers, SCORE_NUMBERS_COUNT, "/game/images/interface/scorenumbers/00");
}

void ScreenElementHUD::UpdateHealth(uint32 newHealth)
{
    if (newHealth > 999)
    {
        LOG_WARNING("Health was to be updated to: " + ToStr(newHealth) + ". Clamping to 999. This should be handled by logic before it got here !");
        newHealth = 999;
    }

    SetImageText(newHealth, 100, m_HealthNumbers, HEALTH_NUMBERS_COUNT, "/game/images/interface/healthnumbers/00");
}

void ScreenElementHUD::ChangeAmmoType(AmmoType newAmmoType)
{
    
}

void ScreenElementHUD::UpdateAmmo(uint32 newAmmo)
{
    if (newAmmo > 99)
    {
        LOG_WARNING("Ammo was to be updated to: " + ToStr(newAmmo) + ". Clamping to 99. This should be handled by logic before it got here !");
        newAmmo = 99;
    }

    SetImageText(newAmmo, 10, m_AmmoNumbers, AMMO_NUMBERS_COUNT, "/game/images/interface/smallnumbers/00");
}

void ScreenElementHUD::UpdateLives(uint32 newLives)
{
    if (newLives > 9)
    {
        LOG_WARNING("Lives were to be updated to: " + ToStr(newLives) + ". Clamping to 9. This should be handled by logic before it got here !");
        newLives = 9;
    }

    SetImageText(newLives, 1, m_LivesNumbers, LIVES_NUMBERS_COUNT, "/game/images/interface/smallnumbers/00");
}

void ScreenElementHUD::UpdateStopwatchTime(uint32 newTime)
{
    SetImageText(newTime, 100, m_StopwatchNumbers, STOPWATCH_NUMBERS_COUNT, "/game/images/interface/scorenumbers/00");
}

void ScreenElementHUD::UpdateFPS(uint32 newFPS)
{
    if (m_pFPSTexture)
    {
        SDL_DestroyTexture(m_pFPSTexture);
    }

    std::string fpsString = "FPS: " + ToStr(newFPS);
    SDL_Surface* pFPSSurface = TTF_RenderText_Blended(g_pApp->GetConsoleFont(), fpsString.c_str(), { 255, 255, 255, 255 });
    m_pFPSTexture = SDL_CreateTextureFromSurface(m_pRenderer, pFPSSurface);
    SDL_FreeSurface(pFPSSurface);
}

void ScreenElementHUD::UpdateCameraPosition()
{
    if (m_pPositionTexture)
    {
        SDL_DestroyTexture(m_pPositionTexture);
    }

    Point cameraCenter = Point(m_pCamera->GetPosition().x + m_pCamera->GetWidth() / 2,
        m_pCamera->GetPosition().y + m_pCamera->GetHeight() / 2);

    std::string positionString = "Position: [X = " + ToStr((int)cameraCenter.x) +
        ", Y = " + ToStr((int)cameraCenter.y) + "]";

    SDL_Surface* pPositionSurface = TTF_RenderText_Blended(g_pApp->GetConsoleFont(), positionString.c_str(), { 255, 255, 255, 255 });
    m_pPositionTexture = SDL_CreateTextureFromSurface(m_pRenderer, pPositionSurface);
    SDL_FreeSurface(pPositionSurface);
}

bool ScreenElementHUD::SetElementVisible(std::string element, bool visible)
{
    auto iter = m_HUDElementsMap.find(element);
    if (iter != m_HUDElementsMap.end())
    {
        iter->second->SetVisible(visible);
        return true;
    }

    return false;
}

bool ScreenElementHUD::IsElementVisible(std::string element)
{
    auto iter = m_HUDElementsMap.find(element);
    if (iter != m_HUDElementsMap.end())
    {
        return iter->second->IsVisible(NULL);
    }

    return false;
}
