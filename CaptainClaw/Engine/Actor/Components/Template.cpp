#include "Template.h"

const char* ConcreteComponent::g_Name = "ConcreteComponent";

bool ConcreteComponent::VInit(TiXmlElement* data)
{
    assert(data != NULL);

    TiXmlElement* concreteElement = data->FirstChildElement("Concrete");
    if (concreteElement != NULL)
    {
        
    }
    else
    {
        return false;
    }

    return true;
}