#include "LootComponent.h"
#include "../ActorTemplates.h"
#include "PositionComponent.h"
#include "PhysicsComponent.h"

#include "../../Events/EventMgr.h"
#include "../../Events/Events.h"

const char* LootComponent::g_Name = "LootComponent";

LootComponent::LootComponent()
    : 
    m_LootSoundChance(0)
{

}

bool LootComponent::VInit(TiXmlElement* pData)
{
    assert(pData != NULL);

    for (TiXmlElement* pElem = pData->FirstChildElement("Item");
        pElem; pElem = pElem->NextSiblingElement("Item"))
    {
        PickupType loot = PickupType(std::stoi(pElem->GetText()));
        
        if (!(loot >= PickupType_Default && loot < PickupType_Max))
        {
            LOG_ERROR("Conflicting pickup: " + ToStr((int)loot));
            return false;
        }

        m_Loot.push_back(loot);
    }

    ParseValueFromXmlElem(&m_LootSoundChance, pData->FirstChildElement("LootSoundChance"));

    return true;
}

void LootComponent::VPostInit()
{
    shared_ptr<HealthComponent> pHealthComponent = MakeStrongPtr(m_pOwner->GetComponent<HealthComponent>(HealthComponent::g_Name));
    if (pHealthComponent)
    {
        pHealthComponent->AddObserver(this);
    }
    else
    {
        assert(false && "Loot component without health component. Is this intentional ?");
    }
}

TiXmlElement* LootComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    //

    return baseElement;
}

void LootComponent::VOnHealthBelowZero(DamageType damageType, int sourceActorId)
{
    bool hadRareTreasure = false;
    for (PickupType item : m_Loot)
    {
        shared_ptr<PositionComponent> pPositionComponent = m_pOwner->GetPositionComponent();
        assert(pPositionComponent);

        StrongActorPtr pLoot = ActorTemplates::CreateActorPickup(item, pPositionComponent->GetPosition(), false);
    }

    int randChance = Util::GetRandomNumber(1, 100);
    if (!m_Loot.empty() && randChance <= m_LootSoundChance)
    {
        SoundInfo sound(SOUND_GAME_TREASURE_RARE_SPAWNED);
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(sound)));
    }

    m_Loot.clear();
};