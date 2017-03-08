#include "BaseGameApp.h"
#include "BaseGameLogic.h"
#include "../UserInterface/Console.h"

#include "../Actor/Components/ControllerComponents/PowerupComponent.h"

std::vector<std::string> g_AvailableCheats;

#define COMMAND_SET_BOOL_VALUE(targetCommandName, targetValue) \
{ \
    std::string targetCommandNameStr = std::string(targetCommandName); \
    std::transform(targetCommandNameStr.begin(), targetCommandNameStr.end(), targetCommandNameStr.begin(), ::tolower); \
    if (commandStr.find(targetCommandNameStr) != std::string::npos) \
    { \
        targetValue = commandStr.find("1") != std::string::npos; \
        if (!targetValue) targetValue = commandStr.find("On") != std::string::npos; \
        if (!targetValue) targetValue = commandStr.find("on") != std::string::npos; \
        if (!targetValue) targetValue = commandStr.find("True") != std::string::npos; \
        if (!targetValue) targetValue = commandStr.find("true") != std::string::npos; \
        if (!targetValue) targetValue = commandStr.find("Yes") != std::string::npos; \
        if (!targetValue) targetValue = commandStr.find("yes") != std::string::npos; \
        \
        std::string status = "On"; \
        if (!targetValue) status = "Off"; \
        \
        pConsole->AddLine(targetCommandNameStr + ": " + status, COLOR_GREEN); \
        \
        wasCommandExecuted = true; \
    } \
}

void CommandHandler::HandleCommand(const char* command, void* userdata)
{
    Console* pConsole = static_cast<Console*>(userdata);
    assert(pConsole);


    std::string commandStr = command;
    std::transform(commandStr.begin(), commandStr.end(), commandStr.begin(), ::tolower);
    bool wasCommandExecuted = false;

    COMMAND_SET_BOOL_VALUE("InfiniteAmmo", g_pApp->m_GameCheats.clawInfiniteAmmo);
    COMMAND_SET_BOOL_VALUE("Invincible", g_pApp->m_GameCheats.clawInvincible);
    COMMAND_SET_BOOL_VALUE("InfiniteJump", g_pApp->m_GameCheats.clawInfiniteJump);
    COMMAND_SET_BOOL_VALUE("PhysicsDebug", g_pApp->m_GameCheats.showPhysicsDebug);

    if (commandStr == "catnip")
    {
        if (StrongActorPtr pClaw = g_pApp->GetGameLogic()->GetClawActor())
        {
            shared_ptr<PowerupComponent> pPowerupComponent =
                MakeStrongPtr(pClaw->GetComponent<PowerupComponent>(PowerupComponent::g_Name));
            assert(pPowerupComponent);

            pPowerupComponent->ApplyPowerup(PowerupType_Catnip, 30000);
            wasCommandExecuted = true;
            pConsole->AddLine("Catnip buff ON for 30 seconds.", COLOR_GREEN);
        }
        else
        {
            pConsole->AddLine("Claw is not yet created, cannot apply Catnip buff", COLOR_RED);
            return;
        }
    }

    if (!wasCommandExecuted)
    {
        pConsole->AddLine("Unknown command: \"" + commandStr + "\"", COLOR_RED);
    }
}