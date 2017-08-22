#include "ClawGameApp.h"
#include "ClawHumanView.h"
#include "ClawGameLogic.h"

BaseGameLogic* ClawGameApp::VCreateGameAndView()
{
    m_pGame = new ClawGameLogic();
    if (!m_pGame->Initialize())
    {
        LOG_ERROR("Could not initialize Claw Game Logic.");
        return NULL;
    }

    shared_ptr<HumanView> clawHumanView(new ClawHumanView(GetRenderer()));
    m_pGame->VAddView(clawHumanView);

    // Register command handler
    clawHumanView->RegisterConsoleCommandHandler(CommandHandler::HandleCommand, NULL);

    return m_pGame;
}