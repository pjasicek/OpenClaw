#include "ClawHumanView.h"
#include "ActorController.h"
#include "Engine/UserInterface/GameHUD.h"

//---------------------------------------------------------------------------------------------------------------------
// ClawHumanView
//---------------------------------------------------------------------------------------------------------------------

ClawHumanView::ClawHumanView(SDL_Renderer* renderer)
    : HumanView(renderer)
{
    RegisterAllDelegates();

    m_pCamera->SetSize(g_pApp->GetWindowSize().x, g_pApp->GetWindowSize().y);
    m_pCamera->VSetPosition(Point(400, 4500));

    m_pFreeCameraController.reset(new MovementController(m_pCamera));

    m_pKeyboardHandler = m_pFreeCameraController;
    m_pPointerHandler = m_pFreeCameraController;
}

ClawHumanView::~ClawHumanView()
{
    RemoveAllDelegates();
}

bool ClawHumanView::VOnEvent(SDL_Event& event)
{
    if (HumanView::VOnEvent(event))
    {
        return true;
    }

    // Handle game specific events
    return false;
}

void ClawHumanView::VOnUpdate(uint32 msDiff)
{
    // Update engine, then game specific stuff
    HumanView::VOnUpdate(msDiff);

    // Actor controller has higher priority
    if (m_pActorController)
    {
        m_pActorController->OnUpdate(msDiff);
    }
    else if (m_pFreeCameraController)
    {
        m_pFreeCameraController->OnUpdate(msDiff);
    }
}

void ClawHumanView::VOnAttach(uint32 gameViewId, uint32 actorId)
{
    HumanView::VOnAttach(gameViewId, actorId);
}

void ClawHumanView::VSetControlledActor(uint32 actorId)
{
    m_pControlledActor = static_pointer_cast<SceneNode>(m_pScene->FindActor(actorId));

    if (!m_pControlledActor)
    {
        LOG_ERROR("Trying to assign control to invalid actor. Id: " + ToStr(actorId));
        return;
    }

    HumanView::VSetControlledActor(actorId);

    /*m_pFreeCameraController.reset(new MovementController(m_pCamera));
    m_pKeyboardHandler = m_pFreeCameraController;
    m_pPointerHandler = m_pFreeCameraController;*/

    m_pActorController.reset(new ActorController(m_pControlledActor));
    m_pKeyboardHandler = m_pActorController;
    m_pPointerHandler = m_pActorController;

    m_pCamera->SetTarget(m_pControlledActor);
}

bool ClawHumanView::VLoadGameDelegate(TiXmlElement* pLoadData)
{
    if (!HumanView::VLoadGameDelegate(pLoadData))
    {
        return false;
    }

    if (!m_pHUD->Initialize(g_pApp->GetRenderer(), m_pCamera))
    {
        LOG_ERROR("Failed to create in-game HUD");
        return false;
    }

    m_ScreenElements.push_back(m_pHUD);

    return true;
}

//=====================================================================================================================
// ClawHumanView: Private implementations
//=====================================================================================================================

void ClawHumanView::RegisterAllDelegates()
{

}

void ClawHumanView::RemoveAllDelegates()
{

}