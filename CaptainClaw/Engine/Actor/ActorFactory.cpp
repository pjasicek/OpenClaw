#include <Tinyxml\tinyxml.h>

#include "ActorFactory.h"
#include "Actor.h"
#include "../Logger/Logger.h"
#include "../SharedDefines.h"
#include "../Util/Util.h"
#include "../GameApp/BaseGameApp.h"

#include "../Resource/Loaders/XmlLoader.h"

// Components
#include "Components\PositionComponent.h"
#include "Components\CollisionComponent.h"
#include "Components\PhysicsComponent.h"
#include "Components\ControllableComponent.h"
#include "Components\AnimationComponent.h"
#include "Components\SoundComponent.h"
#include "Components\RenderComponent.h"
#include "Components\KinematicComponent.h"
#include "Components\AIComponents\TogglePegAIComponent.h"
#include "Components\AIComponents\CrumblingPegAIComponent.h"


ActorFactory::ActorFactory()
{
    _lastActorGUID = 0;

    _componentFactory.Register<PositionComponent>(ActorComponent::GetIdFromName(PositionComponent::g_Name));
    _componentFactory.Register<CollisionComponent>(ActorComponent::GetIdFromName(CollisionComponent::g_Name));
    _componentFactory.Register<PhysicsComponent>(ActorComponent::GetIdFromName(PhysicsComponent::g_Name));
    //_componentFactory.Register<ControllableComponent>(ActorComponent::GetIdFromName(ControllableComponent::g_Name));
    _componentFactory.Register<AnimationComponent>(ActorComponent::GetIdFromName(AnimationComponent::g_Name));
    _componentFactory.Register<SoundComponent>(SoundComponent::GetIdFromName(SoundComponent::g_Name));
    _componentFactory.Register<ActorRenderComponent>(ActorRenderComponent::GetIdFromName(ActorRenderComponent::g_Name));
    _componentFactory.Register<TilePlaneRenderComponent>(TilePlaneRenderComponent::GetIdFromName(TilePlaneRenderComponent::g_Name));
    _componentFactory.Register<ClawControllableComponent>(ClawControllableComponent::GetIdFromName(ClawControllableComponent::g_Name));
    _componentFactory.Register<KinematicComponent>(KinematicComponent::GetIdFromName(KinematicComponent::g_Name));
    _componentFactory.Register<TogglePegAIComponent>(TogglePegAIComponent::GetIdFromName(TogglePegAIComponent::g_Name));
    _componentFactory.Register<CrumblingPegAIComponent>(CrumblingPegAIComponent::GetIdFromName(CrumblingPegAIComponent::g_Name));
}

StrongActorPtr ActorFactory::CreateActor(TiXmlElement* pActorRoot, TiXmlElement* overrides)
{
    //PROFILE_CPU("Create actor");
    uint32 nextActorGUID = GetNextActorGUID();
    StrongActorPtr actor(new Actor(nextActorGUID));
    if (!actor->Init(pActorRoot))
    {
        LOG_ERROR("Failed to initialize actor.");
        return NULL;
    }

    // Loop through each child element and load the component
    for (TiXmlElement* node = pActorRoot->FirstChildElement(); node != NULL; node = node->NextSiblingElement())
    {
        StrongActorComponentPtr component = VCreateComponent(node);
        if (component)
        {
            actor->AddComponent(component);
            component->SetOwner(actor);
        }
        else
        {
            LOG_ERROR("Failed to create component from node: " + std::string(node->Value()));
            actor->Destroy();
            return nullptr;
        }
    }

    if (overrides)
    {
        ModifyActor(actor, overrides);
    }

    // Place for some hacks

    actor->PostInit();

    return actor;
}

StrongActorPtr ActorFactory::CreateActor(const char* actorResource, TiXmlElement* overrides)
{
    // Grab the root XML node

    TiXmlElement* root = XmlResourceLoader::LoadAndReturnRootXmlElement(actorResource, true);
    if (root == NULL)
    {
        LOG_ERROR("Could not load XML root node");
        return false;
    }

    return CreateActor(root, overrides);
}

void ActorFactory::ModifyActor(StrongActorPtr actor, TiXmlElement* overrides)
{
    for (TiXmlElement* node = overrides->FirstChildElement(); node != NULL; node = node->NextSiblingElement())
    {
        uint32 componentId = ActorComponent::GetIdFromName(node->Value());
        StrongActorComponentPtr component = MakeStrongPtr(actor->GetComponent<ActorComponent>(componentId));
        {
            if (component)
            {
                component->VInit(node);
                component->VOnChanged();
            }
            else
            {
                component = VCreateComponent(node);
                if (component)
                {
                    actor->AddComponent(component);
                    component->SetOwner(actor);
                }
            }
        }
    }
}

StrongActorComponentPtr ActorFactory::VCreateComponent(TiXmlElement* data)
{
    const char* name = data->Value();
    StrongActorComponentPtr component(_componentFactory.Create(ActorComponent::GetIdFromName(name)));

    // Initialize the component if we found one
    if (component)
    {
        if (!component->VInit(data))
        {
            LOG_ERROR("Component: " + std::string(name) + " failed to initialize");
            return StrongActorComponentPtr();
        }
    }
    else
    {
        LOG_ERROR("Could not find ActorComponent named: " + std::string(name));
        return NULL;
    }

    return component;
}