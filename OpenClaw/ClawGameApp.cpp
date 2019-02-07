#include "ClawGameApp.h"
#include "ClawHumanView.h"
#include "ClawGameLogic.h"

BaseGameLogic* ClawGameApp::VCreateGameAndView()
{
    m_pGame = new ClawGameLogic();
    if (!m_pGame->Initialize())
    {
        LOG_ERROR("Could not initialize Claw Game Logic.");
        return nullptr;
    }

    shared_ptr<HumanView> clawHumanView(new ClawHumanView(GetRenderer()));
    m_pGame->VAddView(clawHumanView);

    // Register command handler
    clawHumanView->RegisterConsoleCommandHandler(CommandHandler::HandleCommand, nullptr);

    return m_pGame;
}