#include <Tinyxml/tinyxml.h>
#include "BaseTriggerComponent.h"
#include "../PositionComponent.h"

const char* BaseTriggerComponent::g_Name = "BaseTriggerComponent";

bool BaseTriggerComponent::VInit(TiXmlElement* data)
{
    assert(data != NULL);

    TiXmlElement* pTriggerAreaElem = data->FirstChildElement("Area");
    if (pTriggerAreaElem != NULL)
    {
        pTriggerAreaElem->Attribute("width", &m_TriggerSize.x);
        pTriggerAreaElem->Attribute("height", &m_TriggerSize.y);
    }
    else
    {
        return false;
    }

    return true;
}

TiXmlElement* BaseTriggerComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    //

    return baseElement;
}

SDL_Rect BaseTriggerComponent::GetTriggerArea()
{
    SDL_Rect triggerArea = { 0 };

    shared_ptr<PositionComponent> pPositionComponent =
        MakeStrongPtr(_owner->GetComponent<PositionComponent>(PositionComponent::g_Name));
    if (!pPositionComponent)
    {
        return triggerArea;
    }

    Point triggerPosition = pPositionComponent->GetPosition();
    triggerArea = { triggerPosition.x, triggerPosition.y, m_TriggerSize.x, m_TriggerSize.y };

    return triggerArea;
}