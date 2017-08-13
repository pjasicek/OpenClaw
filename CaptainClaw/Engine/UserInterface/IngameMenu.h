#ifndef __INGAME_MENU_H__
#define __INGAME_MENU_H__

#include "../Interfaces.h"
#include "../SharedDefines.h"

class Image;
class CameraNode;
class ScreenElementIngameMenu : public IScreenElement
{
public:
    ScreenElementIngameMenu();
    virtual ~ScreenElementIngameMenu();

    bool Initialize(SDL_Renderer* pRenderer, shared_ptr<CameraNode> pCamera);
    void SetActive(bool bActive);

    virtual void VOnLostDevice() override;
    virtual void VOnRender(uint32 msDiff) override;
    virtual void VOnUpdate(uint32 msDiff) override;

    virtual int32_t VGetZOrder() const override { return 10000; }
    virtual void VSetZOrder(int32 const zOrder) override { }
    virtual bool VIsVisible() override { return m_IsVisible; }
    virtual void VSetVisible(bool visible) override;

    virtual bool VOnEvent(SDL_Event& evt) override;

private:
    bool m_IsVisible;

    SDL_Texture* m_pBackground;

    SDL_Renderer* m_pRenderer;
    shared_ptr<CameraNode> m_pCamera;
};

#endif //__INGAME_MENU_H__
