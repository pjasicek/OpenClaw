#include "CollisionComponent.h"

const char* CollisionComponent::g_Name = "CollisionComponent";

bool CollisionComponent::VInit(TiXmlElement* data)
{
    assert(data != NULL);

    TiXmlElement* collisionSizeElement = data->FirstChildElement("CollisionSize");
    if (collisionSizeElement != NULL)
    {
        collisionSizeElement->Attribute("width", &_collisionWidth);
        collisionSizeElement->Attribute("height", &_collisionHeight);
    }
    else
    {
        return false;
    }

    return true;
}

TiXmlElement* CollisionComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    TiXmlElement* collisionSizeElement = new TiXmlElement("CollisionSize");
    collisionSizeElement->SetAttribute("width", std::to_string(_collisionWidth).c_str());
    collisionSizeElement->SetAttribute("height", std::to_string(_collisionHeight).c_str());
    baseElement->LinkEndChild(collisionSizeElement);

    return baseElement;
}