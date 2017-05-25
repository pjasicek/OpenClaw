#include "AmmoComponent.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"

const char* AmmoComponent::g_Name = "AmmoComponent";

AmmoComponent::AmmoComponent()
    :
    m_ActiveAmmoType(AmmoType_Pistol)
{
    m_AmmoMap[AmmoType_Pistol] = 0;
    m_AmmoMap[AmmoType_Magic] = 0;
    m_AmmoMap[AmmoType_Dynamite] = 0;
}

bool AmmoComponent::VInit(TiXmlElement* pData)
{
    if (TiXmlElement* pElem = pData->FirstChildElement("Pistol"))
    {
        m_AmmoMap[AmmoType_Pistol] = std::stoi(pElem->GetText());
    }
    if (TiXmlElement* pElem = pData->FirstChildElement("Magic"))
    {
        m_AmmoMap[AmmoType_Magic] = std::stoi(pElem->GetText());
    }
    if (TiXmlElement* pElem = pData->FirstChildElement("Dynamite"))
    {
        m_AmmoMap[AmmoType_Dynamite] = std::stoi(pElem->GetText());
    }

    return true;
}

void AmmoComponent::VPostInit()
{
    BroadcastAmmoChanged(m_ActiveAmmoType, m_AmmoMap[m_ActiveAmmoType]);
}

TiXmlElement* AmmoComponent::VGenerateXml()
{
    // TODO:
    return NULL;
}

void AmmoComponent::AddAmmo(AmmoType ammoType, int32 ammoCount)
{
    SetAmmo(ammoType, m_AmmoMap[ammoType] + ammoCount);
}

void AmmoComponent::SetAmmo(AmmoType ammoType, int32 ammoCount)
{
    uint32 oldAmmo = m_AmmoMap[ammoType];
    if (ammoCount < 0)
    {
        ammoCount = 0;
    }
    if (ammoCount > 99)
    {
        ammoCount = 99;
    }

    m_AmmoMap[ammoType] = ammoCount;

    BroadcastAmmoChanged(ammoType, m_AmmoMap[ammoType]);
}

void AmmoComponent::SetActiveAmmo(AmmoType activeAmmoType)
{ 
    m_ActiveAmmoType = activeAmmoType; 
    BroadcastAmmoChanged(m_ActiveAmmoType, m_AmmoMap[m_ActiveAmmoType]); 
}

void AmmoComponent::BroadcastAmmoChanged(AmmoType ammoType, uint32 ammoCount)
{
    shared_ptr<EventData_Updated_Ammo> pEvent(new EventData_Updated_Ammo(ammoType, ammoCount));
    IEventMgr::Get()->VQueueEvent(pEvent);
}