#include "PickupComponent.h"
#include "../TriggerComponents/TriggerComponent.h"
#include "../ControllerComponents/ScoreComponent.h"
#include "../ControllerComponents/LifeComponent.h"
#include "../ControllerComponents/HealthComponent.h"
#include "../ControllerComponents/PowerupComponent.h"
#include "../ControllerComponents/AmmoComponent.h"
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

//=====================================================================================================================
//
// PickupComponent Implementation
//
//=====================================================================================================================

bool PickupComponent::VInit(TiXmlElement* data)
{
    if (TiXmlElement* pElem = data->FirstChildElement("PickupSound"))
    {
        m_PickupSound = pElem->GetText();
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
        MakeStrongPtr(_owner->GetComponent<TriggerComponent>(TriggerComponent::g_Name));
    assert(pTriggerComponent);

    pTriggerComponent->AddObserver(this);
}

TiXmlElement* PickupComponent::VGenerateXml()
{
    // TODO: Implement
    return NULL;
}

void PickupComponent::VOnActorEnteredTrigger(Actor* pActorWhoEntered)
{
    // Check if pickup was really "picked up"
    // Note: Actor is not destroyed here. subclasses need to handle it.
    if (VOnApply(pActorWhoEntered))
    {
        // TODO: Is this necessary ?
        shared_ptr<TriggerComponent> pTriggerComponent =
            MakeStrongPtr(_owner->GetComponent<TriggerComponent>(TriggerComponent::g_Name));
        if (pTriggerComponent)
        {
            //pTriggerComponent->Deactivate();
            pTriggerComponent->Destroy();
        }

        // Play pickup sound if applicable
        if (m_PickupSound.length() > 0)
        {
            IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Request_Play_Sound(m_PickupSound.c_str(), 100, false)));
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

    m_pRenderComponent = MakeStrongPtr(_owner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name));
    m_pPositionComponent = MakeStrongPtr(_owner->GetComponent<PositionComponent>(PositionComponent::g_Name));
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
            MakeStrongPtr(_owner->GetComponent<GlitterComponent>(GlitterComponent::g_Name));
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
                shared_ptr<EventData_Move_Actor> pEvent(new EventData_Move_Actor(_owner->GetGUID(), m_pPositionComponent->GetPosition()));
                IEventMgr::Get()->VTriggerEvent(pEvent);

                SDL_Rect dummy;
                SDL_Rect renderRect = m_pRenderComponent->VGetPositionRect();
                SDL_Rect cameraRect = pCamera->GetCameraRect();
                if (!SDL_IntersectRect(&renderRect, &cameraRect, &dummy))
                {
                    shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(_owner->GetGUID()));
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

        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(_owner->GetGUID()));
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

        pHealthComponent->AddHealth(m_NumRestoredHealth, DamageType_None, Point(0, 0));

        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(_owner->GetGUID()));
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
    m_Destination(Point(0, 0))
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

    shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(_owner->GetGUID()));
    IEventMgr::Get()->VQueueEvent(pEvent);

    IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
        new EventData_Request_Play_Sound(SOUND_GAME_ENTER_WARP, 100, false)));

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
    else if (powerupTypeStr == "IceSword") { m_PowerupType = PowerupType_IceSword; }
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

        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(_owner->GetGUID()));
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
        for (auto ammoPair : m_AmmoPickupList)
        {
            LOG("Ammo count: " + ToStr(ammoPair.second));
            pAmmoComponent->AddAmmo(ammoPair.first, ammoPair.second);
        }

        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(_owner->GetGUID()));
        IEventMgr::Get()->VQueueEvent(pEvent);

        return true;
    }

    return false;
}