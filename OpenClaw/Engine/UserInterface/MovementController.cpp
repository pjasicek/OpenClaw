#include "MovementController.h"
#include "../Scene/SceneNodes.h"

MovementController::MovementController(shared_ptr<SceneNode> controlledObject, float speed)
{
    m_pControlledObject = controlledObject;
    m_Speed = speed;
    m_pKeyStates = NULL;
    m_MouseLeftButtonDown = m_MouseRightButtonDown = false;
}

void MovementController::OnUpdate(uint32 msDiff)
{
    int count;
    m_pKeyStates = SDL_GetKeyboardState(&count);

    float moveX = 0.0f;
    float moveY = 0.0f;

    if (m_pKeyStates[SDL_SCANCODE_RIGHT] || m_pKeyStates[SDL_SCANCODE_LEFT])
    {
        moveX += m_Speed * (float)msDiff;
        if (m_pKeyStates[SDL_SCANCODE_LEFT])
        {
            moveX *= -1;
        }
    }
    if (m_pKeyStates[SDL_SCANCODE_DOWN] || m_pKeyStates[SDL_SCANCODE_UP])
    {
        moveY -= m_Speed * (float)msDiff;
        if (m_pKeyStates[SDL_SCANCODE_DOWN])
        {
            moveY *= -1;
        }
    }

    if (m_pKeyStates[SDL_SCANCODE_LSHIFT] || m_pKeyStates[SDL_SCANCODE_RSHIFT])
    {
        moveX *= 10;
        moveY *= 10;
    }

    Point newPosition = m_pControlledObject->VGetProperties()->GetPosition();
    newPosition.Set(newPosition.x + moveX, newPosition.y + moveY);

    m_pControlledObject->VSetPosition(newPosition);
}

bool MovementController::VOnKeyDown(SDL_Keycode key)
{
    return false;
}

bool MovementController::VOnKeyUp(SDL_Keycode key)
{
    return false;
}

bool MovementController::VOnPointerMove(SDL_MouseMotionEvent& mouseEvent)
{
    return false;
}

bool MovementController::VOnPointerButtonDown(SDL_MouseButtonEvent& mouseEvent)
{
    if (mouseEvent.button == SDL_BUTTON_LEFT)
    {
        m_MouseLeftButtonDown = true;
        return true;
    }
    else if (mouseEvent.button == SDL_BUTTON_RIGHT)
    {
        m_MouseRightButtonDown = true;
        return true;
    }

    return false;
}

bool MovementController::VOnPointerButtonUp(SDL_MouseButtonEvent& mouseEvent)
{
    if (mouseEvent.button == SDL_BUTTON_LEFT)
    {
        m_MouseLeftButtonDown = false;
        return true;
    }
    else if (mouseEvent.button == SDL_BUTTON_RIGHT)
    {
        m_MouseRightButtonDown = false;
        return true;
    }

    return false;
}

std::vector<std::shared_ptr<AbstractRecognizer>> MovementController::VRegisterRecognizers() {
    return std::vector<std::shared_ptr<AbstractRecognizer>>();
}

bool MovementController::VOnTouch(const Touch_Event &evt) {
    return false;
}
