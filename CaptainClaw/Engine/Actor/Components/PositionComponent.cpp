#include "PositionComponent.h"

const char* PositionComponent::g_Name = "PositionComponent";

bool PositionComponent::VInit(TiXmlElement* data)
{
    assert(data != NULL);

    TiXmlElement* positionElement = data->FirstChildElement("Position");
    if (positionElement != NULL)
    {
        positionElement->Attribute("x", &m_Position.x);
        positionElement->Attribute("y", &m_Position.y);
    }
    else
    {
        return false;
    }

    return true;
}

TiXmlElement* PositionComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    TiXmlElement* positionElement = new TiXmlElement("Position");
    positionElement->SetAttribute("x", std::to_string(m_Position.x).c_str());
    positionElement->SetAttribute("y", std::to_string(m_Position.y).c_str());
    baseElement->LinkEndChild(positionElement);

    return baseElement;
}