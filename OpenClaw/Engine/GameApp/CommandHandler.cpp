#include "BaseGameApp.h"
#include "BaseGameLogic.h"
#include "../UserInterface/Console.h"

#include "../Actor/Components/ControllerComponents/PowerupComponent.h"

#include "../Events/EventMgr.h"
#include "../Events/Events.h"

#include "../Actor/ActorTemplates.h"
#include "../Actor/Components/PositionComponent.h"

#include "../Resource/Loaders/XmlLoader.h"

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

bool CommandHandler::AddPowerup(PowerupType type, int duration, bool& executed, std::string command, Console* pConsole)
{
    if (StrongActorPtr pClaw = g_pApp->GetGameLogic()->GetClawActor())
    {
        shared_ptr<PowerupComponent> pPowerupComponent =
            MakeStrongPtr(pClaw->GetComponent<PowerupComponent>(PowerupComponent::g_Name));
        assert(pPowerupComponent);

        pPowerupComponent->ApplyPowerup(type, duration);
        executed = true;
        pConsole->AddLine(command + " buff ON for 30 seconds.", COLOR_GREEN);
    }
    else
    {
        pConsole->AddLine("Claw is not yet created, cannot apply " + command + " buff", COLOR_RED);
        return false;
    }

    return true;
}

void CommandHandler::HandleCommand(const char* command, void* userdata)
{
    Console* pConsole = static_cast<Console*>(userdata);
    assert(pConsole);

    std::string commandStr = command;
    std::transform(commandStr.begin(), commandStr.end(), commandStr.begin(), ::tolower);
    bool wasCommandExecuted = false;

    // Ignore comments
    if (commandStr.length() > 0 && commandStr[0] == '#')
    {
        return;
    }

    std::vector<std::string> commandArgs;
    Util::SplitStringIntoVector(commandStr, commandArgs);

    COMMAND_SET_BOOL_VALUE("InfiniteAmmo", g_pApp->m_GameCheats.clawInfiniteAmmo);
    COMMAND_SET_BOOL_VALUE("Invincible", g_pApp->m_GameCheats.clawInvincible);
    COMMAND_SET_BOOL_VALUE("InfiniteJump", g_pApp->m_GameCheats.clawInfiniteJump);
    COMMAND_SET_BOOL_VALUE("PhysicsDebug", g_pApp->m_GameCheats.showPhysicsDebug);

    if (commandStr == "catnip")
    {
        AddPowerup(PowerupType_Catnip, 30000, wasCommandExecuted, commandStr, pConsole);
    }
    else if (commandStr == "firesword")
    {
        AddPowerup(PowerupType_FireSword, 30000, wasCommandExecuted, commandStr, pConsole);
    }
    else if (commandStr == "frostsword")
    {
        AddPowerup(PowerupType_FrostSword, 30000, wasCommandExecuted, commandStr, pConsole);
    }
    else if (commandStr == "lightningsword")
    {
        AddPowerup(PowerupType_LightningSword, 30000, wasCommandExecuted, commandStr, pConsole);
    }
    else if (commandStr == "invisible" || commandStr == "invisibility" || commandStr == "invis")
    {
        AddPowerup(PowerupType_Invisibility, 30000, wasCommandExecuted, commandStr, pConsole);
    }
    else if (commandStr == "invulnerable" || commandStr == "invlr")
    {
        AddPowerup(PowerupType_Invulnerability, 30000, wasCommandExecuted, commandStr, pConsole);
    }

    if (commandStr == "reset level" || commandStr == "rl")
    {
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Request_Reset_Level));
        pConsole->AddLine("Requested level reset.", COLOR_GREEN);
        wasCommandExecuted = true;
    }

    if (commandStr.find("spawn coin") == 0)
    {
        if (StrongActorPtr pClaw = g_pApp->GetGameLogic()->GetClawActor())
        {
            ActorTemplates::CreateActorPickup(
                PickupType_Treasure_Coins,
                pClaw->GetPositionComponent()->GetPosition() + Point(100, 0));
        }
        else
        {
            pConsole->AddLine("Claw is not yet created, cannot " + commandStr, COLOR_RED);
        }
        wasCommandExecuted = true;
    }
    if (commandStr.find("spawn skull") == 0)
    {
        if (StrongActorPtr pClaw = g_pApp->GetGameLogic()->GetClawActor())
        {
            ActorTemplates::CreateActorPickup(
                PickupType_Treasure_Skull_Green,
                pClaw->GetPositionComponent()->GetPosition() + Point(100, 0));
        }
        else
        {
            pConsole->AddLine("Claw is not yet created, cannot " + commandStr, COLOR_RED);
        }
        wasCommandExecuted = true;
    }

    if (commandStr.find("teleport ") != std::string::npos)
    {
        if (commandArgs.size() == 3)
        {
            int x = std::stoi(commandArgs[1]);
            int y = std::stoi(commandArgs[2]);

            if (StrongActorPtr pClaw = g_pApp->GetGameLogic()->GetClawActor())
            {
                shared_ptr<EventData_Teleport_Actor> pEvent(new EventData_Teleport_Actor(pClaw->GetGUID(), Point(x, y)));
                IEventMgr::Get()->VTriggerEvent(pEvent);
                wasCommandExecuted = true;
            }
            else
            {
                pConsole->AddLine("Claw is not yet created, cannot teleport", COLOR_RED);
                return;
            }
        }
    }

    if (commandStr == "menu")
    {
        IEventMgr::Get()->VQueueEvent(IEventDataPtr(new EventData_Enter_Menu()));
        wasCommandExecuted = true;
    }

    if (commandStr.find("cpudelay ") != std::string::npos && commandArgs.size() == 2)
    {
        g_pApp->m_DebugOptions.cpuDelayMs = std::stoi(commandArgs[1]);
        wasCommandExecuted = true;
    }

    if (commandStr == "reload levelmetadata")
    {
        g_pApp->ReadLevelMetadata(g_pApp->m_GameOptions);
        wasCommandExecuted = true;
    }
    else if (commandStr == "reload actorprototypes" || commandStr == "reload actorproto")
    {
        g_pApp->ReadActorXmlPrototypes(g_pApp->m_GameOptions);
        wasCommandExecuted = true;
    }
    else if (commandStr == "reload all" || commandStr == "ra")
    {
        g_pApp->ReadLevelMetadata(g_pApp->m_GameOptions);
        g_pApp->ReadActorXmlPrototypes(g_pApp->m_GameOptions);
        wasCommandExecuted = true;
    }

    if (commandStr.find("winresize ") != std::string::npos && commandArgs.size() == 4)
    {
        g_pApp->SetWindowSize(std::stoi(commandArgs[1]), std::stoi(commandArgs[2]), std::stod(commandArgs[3]));
        wasCommandExecuted = true;
    }

    if (!wasCommandExecuted)
    {
        pConsole->AddLine("Unknown command: \"" + commandStr + "\"", COLOR_RED);
    }
}
