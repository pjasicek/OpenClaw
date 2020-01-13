#ifndef __MOVEMENTCONTROLLER_H__
#define __MOVEMENTCONTROLLER_H__

#include "../Interfaces.h"
#include "../SharedDefines.h"

class SceneNode;
class MovementController : public IKeyboardHandler, public IPointerHandler, public ITouchHandler
{
public:
    MovementController(shared_ptr<SceneNode> controlledObject, float speed = 0.36f);
    
    void SetControlledObject(shared_ptr<SceneNode> controlledObject) { m_pControlledObject = controlledObject; }
    void OnUpdate(uint32 msDiff);

    // Interface
    bool VOnKeyDown(SDL_Keycode key);
    bool VOnKeyUp(SDL_Keycode key);

    bool VOnPointerMove(SDL_MouseMotionEvent& mouseEvent);
    bool VOnPointerButtonDown(SDL_MouseButtonEvent& mouseEvent);
    bool VOnPointerButtonUp(SDL_MouseButtonEvent& mouseEvent);

    std::vector<std::shared_ptr<AbstractRecognizer>> VRegisterRecognizers() override;
    bool VOnTouch(const Touch_Event &evt) override;

private:
    shared_ptr<SceneNode> m_pControlledObject;
    float m_Speed;

    // SDL_Scancode array
    const uint8* m_pKeyStates;

    bool m_MouseLeftButtonDown;
    bool m_MouseRightButtonDown;
};

#endif