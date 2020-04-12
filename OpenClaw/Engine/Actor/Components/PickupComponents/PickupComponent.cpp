#include "PickupComponent.h"
#include "../TriggerComponents/TriggerComponent.h"
#include "../ControllerComponents/ScoreComponent.h"
#include "../ControllerComponents/LifeComponent.h"
#include "../ControllerComponents/HealthComponent.h"
#include "../ControllerComponents/PowerupComponent.h"
#include "../ControllerComponents/AmmoComponent.h"
#include "../ControllableComponent.h"
#include "../RenderComponent.h"
#include "../PositionComponent.h"
#include "../GlitterComponent.h"

#include "../../../GameApp/BaseGameApp.h"
#include "../../../UserInterface/HumanView.h"
#include "../../../Scene/SceneNodes.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"

const char* PickupComponent::g_Name = "PickupComponent";
const char* TreasurePickupComponent::g_Name = "TreasurePickupComponent";
const char* LifePickupComponent::g_Name = "LifePickupComponent";
const char* HealthPickupComponent::g_Name = "HealthPickupComponent";
const char* TeleportPickupComponent::g_Name = "TeleportPickupComponent";
const char* PowerupPickupComponent::g_Name = "PowerupPickupComponent";
const char* AmmoPickupComponent::g_Name = "AmmoPickupComponent";
const char* EndLevelPickupComponent::g_Name = "EndLevelPickupComponent";

//=====================================================================================================================
//
// PickupComponent Implementation
//
//=====================================================================================================================

bool PickupComponent::VInit(TiXmlElement* data)
{
    assert(data != NULL);

    ParseValueFromXmlElem(&m_PickupSound, data->FirstChildElement("PickupSound"));

    m_PickupType = PickupType_None;

    int pickupType = -1;
    ParseValueFromXmlElem(&pickupType, data->FirstChildElement("PickupType"));
    if (pickupType != -1)
    {
        m_PickupType = PickupType(pickupType);
    }
    

    if (m_PickupSound.empty())
    {
        //LOG_ERROR("No pickup sound for actor: " + std::string(data->Parent()->ToElement()->Attribute("Type")));
        //assert(false && "No pickup sound for TreasurePickupComponent");
    }

    if (!VDelegateInit(data))
    {
        return false;
    }

    return true;
}

void PickupComponent::VPostInit()
{
    shared_ptr<TriggerComponent> pTriggerComponent =
        MakeStrongPtr(m_pOwner->GetComponent<TriggerComponent>(TriggerComponent::g_Name));
    assert(pTriggerComponent);

    pTriggerComponent->AddObserver(this);
}

TiXmlElement* PickupComponent::VGenerateXml()
{
    // TODO: Implement
    return NULL;
}

void PickupComponent::VOnActorEnteredTrigger(Actor* pActorWhoEntered, FixtureType triggerType)
{
    // Check if pickup was really "picked up"
    // Note: Actor is not destroyed here. subclasses need to handle it.
    if (VOnApply(pActorWhoEntered))
    {
        if (m_PickupType != PickupType_None)
        {
            IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Item_Picked_Up(m_PickupType)));
        }

        // TODO: Is this necessary ?
        shared_ptr<TriggerComponent> pTriggerComponent =
            MakeStrongPtr(m_pOwner->GetComponent<TriggerComponent>(TriggerComponent::g_Name));
        if (pTriggerComponent)
        {
            //pTriggerComponent->Deactivate();
            pTriggerComponent->Destroy();
        }

        // Play pickup sound if applicable
        if (m_PickupSound.length() > 0)
        {
            SoundInfo soundInfo(m_PickupSound);
            IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
                new EventData_Request_Play_Sound(soundInfo)));
        }

        //LOG("Pickup up");
    }
    else
    {
        //LOG("Not picked up");
    }
}

//=====================================================================================================================
//
// TreasurePickupComponent Implementation
//
//=====================================================================================================================

TreasurePickupComponent::TreasurePickupComponent()
    :
    m_ScorePoints(0),
    m_IsPickedUp(false)
{
}

bool TreasurePickupComponent::VDelegateInit(TiXmlElement* data)
{
    assert(data);

    if (TiXmlElement* pElem = data->FirstChildElement("ScorePoints"))
    {
        m_ScorePoints = std::stoi(pElem->GetText());
    }

    return true;
}

void TreasurePickupComponent::VPostInit()
{
    PickupComponent::VPostInit();

    m_pRenderComponent = MakeStrongPtr(m_pOwner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name));
    m_pPositionComponent = m_pOwner->GetPositionComponent();
    assert(m_pRenderComponent);
    assert(m_pPositionComponent);
}

void TreasurePickupComponent::VCreateInheritedXmlElements(TiXmlElement* pBaseElement)
{

}

bool TreasurePickupComponent::VOnApply(Actor* pActorWhoPickedThis)
{
    shared_ptr<ScoreComponent> pScoreComponent =
        MakeStrongPtr(pActorWhoPickedThis->GetComponent<ScoreComponent>(ScoreComponent::g_Name));
    if (pScoreComponent && !m_IsPickedUp)
    {
        pScoreComponent->AddScorePoints(m_ScorePoints);

        m_IsPickedUp = true;

        // Destroy glitter if possible
        shared_ptr<GlitterComponent> pGlitterComponent =
            MakeStrongPtr(m_pOwner->GetComponent<GlitterComponent>(GlitterComponent::g_Name));
        if (pGlitterComponent)
        {
            pGlitterComponent->Deactivate();
        }

        ActorTemplates::CreateScorePopupActor(m_pPositionComponent->GetPosition(), m_ScorePoints);

        return true;
    }

    return false;
}

void TreasurePickupComponent::VUpdate(uint32 msDiff)
{
    if (m_IsPickedUp)
    {
        // I want it to disappear after ~900ms
        Point moveDelta(-(800 / 900.0 * msDiff), -(800 / 900.0f * msDiff));
        m_pPositionComponent->SetPosition(m_pPositionComponent->GetX() + moveDelta.x, m_pPositionComponent->GetY() + moveDelta.y);

        // This feels like a hack
        if (HumanView* pHumanView = g_pApp->GetHumanView())
        {
            shared_ptr<CameraNode> pCamera = pHumanView->GetCamera();
            if (pCamera)
            {
                shared_ptr<EventData_Move_Actor> pEvent(new EventData_Move_Actor(m_pOwner->GetGUID(), m_pPositionComponent->GetPosition()));
                IEventMgr::Get()->VTriggerEvent(pEvent);

                SDL_Rect renderRect = m_pRenderComponent->VGetPositionRect();
                SDL_Rect cameraRect = pCamera->GetCameraRect();
                if (!SDL_HasIntersection(&renderRect, &cameraRect))
                {
                    shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(m_pOwner->GetGUID()));
                    IEventMgr::Get()->VQueueEvent(pEvent);
                }
            }
            else
            {
                LOG_ERROR("Could not retrieve camera");
            }
        }
    }
}

//=====================================================================================================================
//
// LifePickupComponent Implementation
//
//=====================================================================================================================

LifePickupComponent::LifePickupComponent()
    :
    m_NumLives(1)
{ }

bool LifePickupComponent::VDelegateInit(TiXmlElement* data)
{
    assert(data);

    if (TiXmlElement* pElem = data->FirstChildElement("Lives"))
    {
        m_NumLives = std::stoi(pElem->GetText());
    }

    return true;
}

void LifePickupComponent::VCreateInheritedXmlElements(TiXmlElement* pBaseElement)
{

}

bool LifePickupComponent::VOnApply(Actor* pActorWhoPickedThis)
{
    shared_ptr<LifeComponent> pLifeComponent =
        MakeStrongPtr(pActorWhoPickedThis->GetComponent<LifeComponent>(LifeComponent::g_Name));
    if (pLifeComponent)
    {
        pLifeComponent->AddLives(m_NumLives);

        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(m_pOwner->GetGUID()));
        IEventMgr::Get()->VQueueEvent(pEvent);



        return true;
    }

    return false;
}

//=====================================================================================================================
//
// HealthPickupComponent Implementation
//
//=====================================================================================================================

HealthPickupComponent::HealthPickupComponent()
    :
    m_NumRestoredHealth(0)
{ }

bool HealthPickupComponent::VDelegateInit(TiXmlElement* data)
{
    assert(data);

    if (TiXmlElement* pElem = data->FirstChildElement("Health"))
    {
        m_NumRestoredHealth = std::stoi(pElem->GetText());
    }

    return true;
}

void HealthPickupComponent::VCreateInheritedXmlElements(TiXmlElement* pBaseElement)
{

}

bool HealthPickupComponent::VOnApply(Actor* pActorWhoPickedThis)
{
    shared_ptr<HealthComponent> pHealthComponent =
        MakeStrongPtr(pActorWhoPickedThis->GetComponent<HealthComponent>(HealthComponent::g_Name));
    if (pHealthComponent)
    {
        if (pHealthComponent->HasMaxHealth())
        {
            return false;
        }

        pHealthComponent->AddHealth(m_NumRestoredHealth, DamageType_None, Point(0, 0), m_pOwner->GetGUID());

        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(m_pOwner->GetGUID()));
        IEventMgr::Get()->VQueueEvent(pEvent);

        return true;
    }

    return false;
}

//=====================================================================================================================
//
// TeleportPickupComponent Implementation
//
//=====================================================================================================================

TeleportPickupComponent::TeleportPickupComponent()
    :
    m_Destination(Point(0, 0)),
    m_bIsBossWarp(false)
{ }

bool TeleportPickupComponent::VDelegateInit(TiXmlElement* data)
{
    assert(data);

    if (TiXmlElement* pElem = data->FirstChildElement("Destination"))
    {
        pElem->Attribute("x", &m_Destination.x);
        pElem->Attribute("y", &m_Destination.y);
    }
    else
    {
        LOG_ERROR("TeleportPickupComponent does not have destination set.");
        return false;
    }

    ParseValueFromXmlElem(&m_bIsBossWarp, data->FirstChildElement("IsBossWarp"));

    if (m_Destination.x <= 0 || m_Destination.y <= 0)
    {
        LOG_ERROR("Teleport destination has invalid coordinates");
        return false;
    }

    return true;
}

void TeleportPickupComponent::VCreateInheritedXmlElements(TiXmlElement* pBaseElement)
{

}

bool TeleportPickupComponent::VOnApply(Actor* pActorWhoPickedThis)
{
    shared_ptr<EventData_Teleport_Actor> pTeleportEvent(new EventData_Teleport_Actor(pActorWhoPickedThis->GetGUID(), m_Destination, true));
    IEventMgr::Get()->VQueueEvent(pTeleportEvent);

    // HACK: ...
    if (m_bIsBossWarp)
    {
        SoundInfo soundInfo(m_PickupSound);
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));

        shared_ptr<EventData_Checkpoint_Reached> pEvent(new EventData_Checkpoint_Reached(
            pActorWhoPickedThis->GetGUID(),
            m_Destination,
            false,
            0));
        IEventMgr::Get()->VTriggerEvent(pEvent);
    }

    shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(m_pOwner->GetGUID()));
    IEventMgr::Get()->VQueueEvent(pEvent);

    /*SoundInfo soundInfo(SOUND_GAME_ENTER_WARP);
    IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
        new EventData_Request_Play_Sound(soundInfo)));*/

    return true;
}

//=====================================================================================================================
//
// PowerupPickupComponent Implementation
//
//=====================================================================================================================

PowerupPickupComponent::PowerupPickupComponent()
    :
    m_PowerupType(PowerupType_None),
    m_PowerupDuration(0)
{ }

bool PowerupPickupComponent::VDelegateInit(TiXmlElement* data)
{
    assert(data);

    std::string powerupTypeStr;
    if (TiXmlElement* pElem = data->FirstChildElement("Type"))
    {
        powerupTypeStr = pElem->GetText();
    }
    if (powerupTypeStr == "Invulnerability") { m_PowerupType = PowerupType_Invulnerability; }
    else if (powerupTypeStr == "Invisibility") { m_PowerupType = PowerupType_Invisibility; }
    else if (powerupTypeStr == "Catnip") { m_PowerupType = PowerupType_Catnip; }
    else if (powerupTypeStr == "IceSword") { m_PowerupType = PowerupType_FrostSword; }
    else if (powerupTypeStr == "FireSword") { m_PowerupType = PowerupType_FireSword; }
    else if (powerupTypeStr == "LightningSword") { m_PowerupType = PowerupType_LightningSword; }

    if (TiXmlElement* pElem = data->FirstChildElement("Duration"))
    {
        m_PowerupDuration = std::stoi(pElem->GetText());
    }

    
    if (m_PowerupType == PowerupType_None || m_PowerupType >= PowerupType_Max)
    {
        LOG_ERROR("Invalid powerup type.");
        return false;
    }
    if (m_PowerupDuration <= 0)
    {
        LOG_ERROR("Invalid powerup duration.");
        return false;
    }

    return true;
}

void PowerupPickupComponent::VCreateInheritedXmlElements(TiXmlElement* pBaseElement)
{

}

bool PowerupPickupComponent::VOnApply(Actor* pActorWhoPickedThis)
{
    shared_ptr<PowerupComponent> pPowerupComponent =
        MakeStrongPtr(pActorWhoPickedThis->GetComponent<PowerupComponent>(PowerupComponent::g_Name));
    if (pPowerupComponent)
    {
        pPowerupComponent->ApplyPowerup(m_PowerupType, m_PowerupDuration);

        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(m_pOwner->GetGUID()));
        IEventMgr::Get()->VQueueEvent(pEvent);

        return true;
    }

    return false;
}

//=====================================================================================================================
//
// AmmoPickupComponent Implementation
//
//=====================================================================================================================

AmmoPickupComponent::AmmoPickupComponent()
{ }

bool AmmoPickupComponent::VDelegateInit(TiXmlElement* data)
{
    assert(data);

    for (TiXmlElement* pElem = data->FirstChildElement("Ammo");
        pElem != NULL; pElem = pElem->NextSiblingElement("Ammo"))
    {
        std::string ammoTypeStr = pElem->Attribute("ammoType");
        int ammoCount = std::stoi(pElem->Attribute("ammoCount"));

        AmmoType ammoType;
        if (ammoTypeStr == "Pistol") { ammoType = AmmoType_Pistol; }
        else if (ammoTypeStr == "Magic") { ammoType = AmmoType_Magic;; }
        else if (ammoTypeStr == "Dynamite") { ammoType = AmmoType_Dynamite; }
        else
        {
            LOG_ERROR("Unknown ammo type: " + ammoTypeStr);
            return false;
        }

        assert(ammoType > AmmoType_None && ammoType < AmmoType_Max);
        assert(ammoCount > 0);

        m_AmmoPickupList.push_back(std::make_pair(ammoType, ammoCount));
    }

    assert(m_AmmoPickupList.size() > 0);

    return true;
}

void AmmoPickupComponent::VCreateInheritedXmlElements(TiXmlElement* pBaseElement)
{

}

bool AmmoPickupComponent::VOnApply(Actor* pActorWhoPickedThis)
{
    shared_ptr<AmmoComponent> pAmmoComponent =
        MakeStrongPtr(pActorWhoPickedThis->GetComponent<AmmoComponent>(AmmoComponent::g_Name));
    if (pAmmoComponent)
    {
        for (const auto &ammoPair : m_AmmoPickupList)
        {
            pAmmoComponent->AddAmmo(ammoPair.first, ammoPair.second);
        }

        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(m_pOwner->GetGUID()));
        IEventMgr::Get()->VQueueEvent(pEvent);

        return true;
    }

    return false;
}

//=====================================================================================================================
//
// EndLevelPickupComponent Implementation
//
//=====================================================================================================================

EndLevelPickupComponent::EndLevelPickupComponent()
{ }

bool EndLevelPickupComponent::VDelegateInit(TiXmlElement* data)
{
    assert(data);

    return true;
}

void EndLevelPickupComponent::VCreateInheritedXmlElements(TiXmlElement* pBaseElement)
{

}

bool EndLevelPickupComponent::VOnApply(Actor* pActorWhoPickedThis)
{
    // Only Claw should be able to pick this
    shared_ptr<ClawControllableComponent> pClawComponent = MakeStrongPtr(pActorWhoPickedThis->GetComponent<ClawControllableComponent>());
    assert(pClawComponent != nullptr && "Only claw should be able to pick end level item !");

    shared_ptr<EventData_Finished_Level> pEvent(new EventData_Finished_Level());
    IEventMgr::Get()->VQueueEvent(pEvent);

    // Play sound here
    assert(!m_PickupSound.empty());
    SoundInfo soundInfo(m_PickupSound);
    IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
        new EventData_Request_Play_Sound(soundInfo)));

    // TODO: This is hack. But it suffices.

    // Lets wait a bit
    int waitDuration = 1500;
    SDL_Event evt;
    while (waitDuration > 0)
    {
        // Eat events
        while (SDL_PollEvent(&evt));
        SDL_Delay(10);
        waitDuration -= 10;
    }

    // We already played it
    m_PickupSound = "";

    return true;
}
