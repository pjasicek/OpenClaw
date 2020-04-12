#ifndef __GAMEHUD_H__
#define __GAMEHUD_H__

#include "../Interfaces.h"
#include "../SharedDefines.h"
#include "../Scene/HUDSceneNode.h"

const uint32 SCORE_NUMBERS_COUNT = 8;
const uint32 HEALTH_NUMBERS_COUNT = 3;
const uint32 AMMO_NUMBERS_COUNT = 2;
const uint32 LIVES_NUMBERS_COUNT = 1;
const uint32 STOPWATCH_NUMBERS_COUNT = 3;

typedef std::map<std::string, shared_ptr<SDL2HUDSceneNode>> HUDElementsMap;

class Image;
class CameraNode;
class ScreenElementHUD : public IScreenElement
{
public:
    ScreenElementHUD();
    virtual ~ScreenElementHUD();
    
    bool Initialize(SDL_Renderer* pRenderer, shared_ptr<CameraNode> pCamera);

    virtual void VOnLostDevice() override;
    virtual void VOnRender(uint32 msDiff) override;
    virtual void VOnUpdate(uint32 msDiff) override;

    virtual int32_t VGetZOrder() const override { return 9000; }
    virtual void VSetZOrder(int32 const zOrder) override { }
    virtual bool VIsVisible() override { return m_IsVisible; }
    virtual void VSetVisible(bool visible) override { m_IsVisible = visible; }

    virtual bool VOnEvent(SDL_Event& evt) override;

    void AddHUDElement(const std::string& key, const shared_ptr<SDL2HUDSceneNode>& pHUDSceneNode) { m_HUDElementsMap[key] = pHUDSceneNode; }

    bool SetElementVisible(const std::string& element, bool visible);
    bool IsElementVisible(const std::string& element);

    void UpdateScore(uint32 newScore);
    void UpdateHealth(uint32 newHealth);
    void ChangeAmmoType(AmmoType newAmmoType);
    void UpdateAmmo(uint32 newAmmo);
    void UpdateLives(uint32 newLives);
    void UpdateStopwatchTime(uint32 newTime);

    void UpdateFPS(uint32 newFPS);

private:
    void BossHealthChangedDelegate(IEventDataPtr pEvent);
    void BossFightEndedDelegate(IEventDataPtr pEvent);

    void UpdateCameraPosition();

    bool m_IsVisible;
    shared_ptr<Image> m_ScoreNumbers[SCORE_NUMBERS_COUNT];
    shared_ptr<Image> m_HealthNumbers[HEALTH_NUMBERS_COUNT];
    shared_ptr<Image> m_AmmoNumbers[AMMO_NUMBERS_COUNT];
    shared_ptr<Image> m_LivesNumbers[LIVES_NUMBERS_COUNT];
    shared_ptr<Image> m_StopwatchNumbers[STOPWATCH_NUMBERS_COUNT];

    SDL_Renderer* m_pRenderer;
    shared_ptr<CameraNode> m_pCamera;

    HUDElementsMap m_HUDElementsMap;

    SDL_Texture* m_pFPSTexture;
    SDL_Texture* m_pPositionTexture;
    SDL_Texture* m_pBossBarTexture;
};

#endif
