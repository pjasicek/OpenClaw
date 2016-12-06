#include "LootComponent.h"
#include "../ActorTemplates.h"
#include "PositionComponent.h"

const char* LootComponent::g_Name = "LootComponent";

bool LootComponent::VInit(TiXmlElement* pData)
{
    assert(pData != NULL);

    for (TiXmlElement* pElem = pData->FirstChildElement("Item");
        pElem; pElem = pElem->NextSiblingElement("Item"))
    {
        PickupType loot = PickupType(std::stoi(pElem->GetText()));
        assert(loot >= PickupType_Default && loot < PickupType_Max);

        m_Loot.push_back(loot);
    }

    return true;
}

TiXmlElement* LootComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    //

    return baseElement;
}

void LootComponent::VOnHealthBelowZero()
{
    for (PickupType item : m_Loot)
    {
        shared_ptr<PositionComponent> pPositionComponent =
            MakeStrongPtr(_owner->GetComponent<PositionComponent>(PositionComponent::g_Name));
        assert(pPositionComponent);

        ActorTemplates::CreateActorPickup(item, pPositionComponent->GetPosition(), false);
    }

    m_Loot.clear();
}