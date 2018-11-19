#include "Template.h"

const char* ConcreteComponent::g_Name = "ConcreteComponent";

bool ConcreteComponent::VInit(TiXmlElement* data)
{
    assert(data != nullptr);

    TiXmlElement* concreteElement = data->FirstChildElement("Concrete");
    if (concreteElement != nullptr)
    {
        
    }
    else
    {
        return false;
    }

    return true;
}

void ConcreteComponent::VPostInit()
{

}

void ConcreteComponent::VPostPostInit()
{

}

void ConcreteComponent::VUpdate(uint32 msDiff)
{

}