#ifndef __USERINTERFACE_H__
#define __USERINTERFACE_H__

#include "../Scene/Scene.h"
#include "../SharedDefines.h"

// Doesnt really do anything, just implementation of empty methods to conform to
// IScreenElement interface
class ScreenElementScene : public IScreenElement, public Scene
{
public:
    ScreenElementScene(SDL_Renderer* renderer) : Scene(renderer) { }
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

#endif
