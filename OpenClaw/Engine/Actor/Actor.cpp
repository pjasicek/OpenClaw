#include <tinyxml.h>

#include "../SharedDefines.h"
#include "Actor.h"
#include "ActorComponent.h"

#include "Components/PositionComponent.h"

Actor::Actor(uint32 actorGUID)
{
    _GUID = actorGUID;
    _name = "Unknown";
    _resource = "Unknown";
}

Actor::~Actor()
{
    //LOG_TAG("Actor", "Destructor: Destroying actor: " + _name + ", GUID: " + std::to_string(_GUID));
    assert(_components.empty() && "Actors components are not empty !");
}

bool Actor::Init(TiXmlElement* data)
{
    if (data->Attribute("Type") != NULL)
    {
        _name = data->Attribute("Type");
    }
    else
    {
        _name = "UNSET";
    }
    //_resource = data->Attribute("resource");

    //LOG_TAG("Actor", "Constructor: Initializing actor: " + _name + ", GUID: " + std::to_string(_GUID) + 
        //" from resource: " + _resource);

    return true;
}

void Actor::PostInit()
{
    m_pPositionComponent = MakeStrongPtr(GetComponent<PositionComponent>(PositionComponent::g_Name));

    for (auto component : _components)
    {
        component.second->VPostInit();
    }
}

void Actor::PostPostInit()
{
    for (auto component : _components)
    {
        component.second->VPostPostInit();
    }
}

void Actor::Destroy()
{
    //LOG("Destroying actor: " + _name);
    m_pPositionComponent.reset();
    _components.clear();
}

void Actor::Update(uint32 msDiff)
{
    for (auto component : _components)
    {
        component.second->VUpdate(msDiff);
    }
}

std::string Actor::ToXML()
{
    TiXmlDocument outXml;

    return "";
}

void Actor::AddComponent(StrongActorComponentPtr component)
{
    std::pair<ActorComponentsMap::iterator, bool> success =
        _components.insert(std::make_pair(component->VGetId(), component));

    assert(success.second);
}

void Actor::OnWorldFinishedLoading()
{
    for (const auto& componentPair : _components)
    {
        componentPair.second->VOnWorldFinishedLoading();
    }
}