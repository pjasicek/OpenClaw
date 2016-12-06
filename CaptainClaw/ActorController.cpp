#include "ActorController.h"
#include "Engine/Scene/SceneNodes.h"
#include "Engine/Events/EventMgr.h"
#include "Engine/Events/Events.h"
#include "ClawEvents.h"

ActorController::ActorController(shared_ptr<SceneNode> controlledObject, float speed)
{
    m_pControlledObject = controlledObject;
    m_Speed = speed;
    m_pKeyStates = NULL;
    m_MouseLeftButtonDown = m_MouseRightButtonDown = false;
}

void ActorController::OnUpdate(uint32 msDiff)
{
    int count;
    m_pKeyStates = SDL_GetKeyboardState(&count);

    float moveX = 0.0f;
    float moveY = 0.0f;

    float climbY = 0.0f;

    // We need two conditions because I want behaviour such as when both right and left
    // buttons are pressed, I dont want actor to move, e.g. to have the move effect nullyfied
    if (m_pKeyStates[SDL_SCANCODE_RIGHT])
    {
        moveX += m_Speed * (float)msDiff;
    }
    if (m_pKeyStates[SDL_SCANCODE_LEFT])
    {
        moveX -= m_Speed * (float)msDiff;
    }

    // CLimbing
    if (m_pKeyStates[SDL_SCANCODE_DOWN])
    {
        climbY += 5.0;
    }
    if (m_pKeyStates[SDL_SCANCODE_UP])
    {
        climbY -= 5.0;
    }

    // Jumping
    if (m_pKeyStates[SDL_SCANCODE_SPACE])
    {
        moveY -= m_Speed * (float)msDiff;
    }

    if (m_pKeyStates[SDL_SCANCODE_LSHIFT] || m_pKeyStates[SDL_SCANCODE_RSHIFT])
    {
        moveX *= 10;
        moveY *= 10;
    }

    if (fabs(climbY) > FLT_EPSILON)
    {
        shared_ptr<EventData_Start_Climb> pClimbEvent(new EventData_Start_Climb(m_pControlledObject->VGetProperties()->GetActorId(), Point(0, climbY)));
        IEventMgr::Get()->VTriggerEvent(pClimbEvent);
    }
    if (fabs(moveX) > FLT_EPSILON || fabs(moveY) > FLT_EPSILON)
    {
        shared_ptr<EventData_Actor_Start_Move> pMoveEvent(new EventData_Actor_Start_Move(m_pControlledObject->VGetProperties()->GetActorId(), Point(moveX, moveY)));
        IEventMgr::Get()->VTriggerEvent(pMoveEvent);
    }

    //LOG("Setting new position");

    /*Point newPosition = m_pControlledObject->VGetProperties()->GetPosition();
    newPosition.Set(newPosition.x + moveX, newPosition.y + moveY);
    
    m_pControlledObject->VSetPosition(newPosition);*/
}

bool ActorController::VOnKeyDown(SDL_Keycode key)
{
    if (SDL_GetScancodeFromKey(key) == SDL_SCANCODE_LALT)
    {
        shared_ptr<EventData_Actor_Fire> pClimbEvent(new EventData_Actor_Fire(m_pControlledObject->VGetProperties()->GetActorId()));
        IEventMgr::Get()->VTriggerEvent(pClimbEvent);
        return true;
    }
    else if (SDL_GetScancodeFromKey(key) == SDL_SCANCODE_LCTRL)
    {
        shared_ptr<EventData_Actor_Attack> pClimbEvent(new EventData_Actor_Attack(m_pControlledObject->VGetProperties()->GetActorId()));
        IEventMgr::Get()->VTriggerEvent(pClimbEvent);
        return true;
    }
    else if (SDL_GetScancodeFromKey(key) == SDL_SCANCODE_LSHIFT)
    {
        shared_ptr<EventData_Request_Change_Ammo_Type> pEvent(new EventData_Request_Change_Ammo_Type(m_pControlledObject->VGetProperties()->GetActorId()));
        IEventMgr::Get()->VTriggerEvent(pEvent);
        return true;
    }

    return false;
}

bool ActorController::VOnKeyUp(SDL_Keycode key)
{
    return false;
}

bool ActorController::VOnPointerMove(SDL_MouseMotionEvent& mouseEvent)
{
    return false;
}

bool ActorController::VOnPointerButtonDown(SDL_MouseButtonEvent& mouseEvent)
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

bool ActorController::VOnPointerButtonUp(SDL_MouseButtonEvent& mouseEvent)
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