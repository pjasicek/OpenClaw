#include <tinyxml.h>

#include "ActorFactory.h"
#include "Actor.h"
#include "../Logger/Logger.h"
#include "../SharedDefines.h"
#include "../Util/Util.h"
#include "../GameApp/BaseGameApp.h"

#include "../Resource/Loaders/XmlLoader.h"

// Components
#include "Components/PositionComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/PhysicsComponent.h"
#include "Components/ControllableComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/SoundComponent.h"
#include "Components/RenderComponent.h"
#include "Components/KinematicComponent.h"
#include "Components/AIComponents/TogglePegAIComponent.h"
#include "Components/AIComponents/CrumblingPegAIComponent.h"
#include "Components/TriggerComponents/TriggerComponent.h"
#include "Components/PickupComponents/PickupComponent.h"
#include "Components/ControllerComponents/ScoreComponent.h"
#include "Components/ControllerComponents/LifeComponent.h"
#include "Components/ControllerComponents/HealthComponent.h"
#include "Components/ControllerComponents/AmmoComponent.h"
#include "Components/ControllerComponents/PowerupComponent.h"
#include "Components/PowerupSparkleAIComponent.h"
#include "Components/AIComponents/ProjectileAIComponent.h"
#include "Components/LootComponent.h"
#include "Components/DestroyableComponent.h"
#include "Components/ExplodeableComponent.h"
#include "Components/AreaDamageComponent.h"
#include "Components/GlitterComponent.h"
#include "Components/CheckpointComponent.h"
#include "Components/EnemyAI/EnemyAIComponent.h"
#include "Components/EnemyAI/EnemyAIStateComponent.h"
#include "Components/PredefinedMoveComponent.h"
#include "Components/TriggerComponents/SoundTriggerComponent.h"
#include "Components/GlobalAmbientSoundComponent.h"
#include "Components/FollowableComponent.h"
#include "Components/AuraComponents/AuraComponent.h"
#include "Components/SingleAnimationComponent.h"
#include "Components/ProjectileSpawnerComponent.h"
#include "Components/LocalAmbientSoundComponent.h"
#include "Components/TriggerComponents/BossStagerTriggerComponent.h"
#include "Components/PathElevatorComponent.h"
#include "Components/FloorSpikeComponent.h"
#include "Components/RopeComponent.h"
#include "Components/SteppingGroundComponent.h"
#include "Components/SpringBoardComponent.h"
#include "Components/ActorSpawnerComponent.h"
#include "Components/EnemyAI/PunkRatAIStateComponent.h"
#include "Components/EnemyAI/Gabriel/GabrielEncounter.h"
#include "Components/SawBladeComponent.h"
#include "Components/ConveyorBeltComponent.h"
#include "Components/EnemyAI/Marrow/MarrowEncounter.h"
#include "Components/EnemyAI/Aquatis/AquatisEncounter.h"
#include "Components/EnemyAI/RedTail/RedTailEncounter.h"

ActorFactory::ActorFactory()
{
    _lastActorGUID = 0;

    _componentFactory.Register<PositionComponent>(ActorComponent::GetIdFromName(PositionComponent::g_Name));
    _componentFactory.Register<CollisionComponent>(ActorComponent::GetIdFromName(CollisionComponent::g_Name));
    _componentFactory.Register<PhysicsComponent>(ActorComponent::GetIdFromName(PhysicsComponent::g_Name));
    _componentFactory.Register<AnimationComponent>(ActorComponent::GetIdFromName(AnimationComponent::g_Name));
    _componentFactory.Register<SoundComponent>(SoundComponent::GetIdFromName(SoundComponent::g_Name));
    _componentFactory.Register<ActorRenderComponent>(ActorRenderComponent::GetIdFromName(ActorRenderComponent::g_Name));
    _componentFactory.Register<TilePlaneRenderComponent>(TilePlaneRenderComponent::GetIdFromName(TilePlaneRenderComponent::g_Name));
    _componentFactory.Register<HUDRenderComponent>(HUDRenderComponent::GetIdFromName(HUDRenderComponent::g_Name));
    _componentFactory.Register<ClawControllableComponent>(ClawControllableComponent::GetIdFromName(ClawControllableComponent::g_Name));
    _componentFactory.Register<KinematicComponent>(KinematicComponent::GetIdFromName(KinematicComponent::g_Name));
    _componentFactory.Register<TogglePegAIComponent>(TogglePegAIComponent::GetIdFromName(TogglePegAIComponent::g_Name));
    _componentFactory.Register<CrumblingPegAIComponent>(CrumblingPegAIComponent::GetIdFromName(CrumblingPegAIComponent::g_Name));
    _componentFactory.Register<TriggerComponent>(TriggerComponent::GetIdFromName(TriggerComponent::g_Name));
    _componentFactory.Register<TreasurePickupComponent>(TreasurePickupComponent::GetIdFromName(TreasurePickupComponent::g_Name));
    _componentFactory.Register<LifePickupComponent>(LifePickupComponent::GetIdFromName(LifePickupComponent::g_Name));
    _componentFactory.Register<HealthPickupComponent>(HealthPickupComponent::GetIdFromName(HealthPickupComponent::g_Name));
    _componentFactory.Register<ScoreComponent>(ScoreComponent::GetIdFromName(ScoreComponent::g_Name));
    _componentFactory.Register<LifeComponent>(LifeComponent::GetIdFromName(LifeComponent::g_Name));
    _componentFactory.Register<HealthComponent>(HealthComponent::GetIdFromName(HealthComponent::g_Name));
    _componentFactory.Register<TeleportPickupComponent>(TeleportPickupComponent::GetIdFromName(TeleportPickupComponent::g_Name));
    _componentFactory.Register<AmmoComponent>(AmmoComponent::GetIdFromName(AmmoComponent::g_Name));
    _componentFactory.Register<PowerupComponent>(PowerupComponent::GetIdFromName(PowerupComponent::g_Name));
    _componentFactory.Register<PowerupPickupComponent>(PowerupPickupComponent::GetIdFromName(PowerupPickupComponent::g_Name));
    _componentFactory.Register<AmmoPickupComponent>(AmmoPickupComponent::GetIdFromName(AmmoPickupComponent::g_Name));
    _componentFactory.Register<EndLevelPickupComponent>(EndLevelPickupComponent::GetIdFromName(EndLevelPickupComponent::g_Name));
    _componentFactory.Register<PowerupSparkleAIComponent>(PowerupSparkleAIComponent::GetIdFromName(PowerupSparkleAIComponent::g_Name));
    _componentFactory.Register<ProjectileAIComponent>(ProjectileAIComponent::GetIdFromName(ProjectileAIComponent::g_Name));
    _componentFactory.Register<LootComponent>(LootComponent::GetIdFromName(LootComponent::g_Name));
    _componentFactory.Register<DestroyableComponent>(DestroyableComponent::GetIdFromName(DestroyableComponent::g_Name));
    _componentFactory.Register<ExplodeableComponent>(ExplodeableComponent::GetIdFromName(ExplodeableComponent::g_Name));
    _componentFactory.Register<AreaDamageComponent>(AreaDamageComponent::GetIdFromName(AreaDamageComponent::g_Name));
    _componentFactory.Register<GlitterComponent>(GlitterComponent::GetIdFromName(GlitterComponent::g_Name));
    _componentFactory.Register<CheckpointComponent>(CheckpointComponent::GetIdFromName(CheckpointComponent::g_Name));
    _componentFactory.Register<EnemyAIComponent>(EnemyAIComponent::GetIdFromName(EnemyAIComponent::g_Name));
    _componentFactory.Register<PatrolEnemyAIStateComponent>(PatrolEnemyAIStateComponent::GetIdFromName(PatrolEnemyAIStateComponent::g_Name));
    _componentFactory.Register<ParryEnemyAIStateComponent>(ParryEnemyAIStateComponent::GetIdFromName(ParryEnemyAIStateComponent::g_Name));
    _componentFactory.Register<MeleeAttackAIStateComponent>(MeleeAttackAIStateComponent::GetIdFromName(MeleeAttackAIStateComponent::g_Name));
    _componentFactory.Register<DuckMeleeAttackAIStateComponent>(DuckMeleeAttackAIStateComponent::GetIdFromName(DuckMeleeAttackAIStateComponent::g_Name));
    _componentFactory.Register<RangedAttackAIStateComponent>(RangedAttackAIStateComponent::GetIdFromName(RangedAttackAIStateComponent::g_Name));
    _componentFactory.Register<DuckRangedAttackAIStateComponent>(DuckRangedAttackAIStateComponent::GetIdFromName(DuckRangedAttackAIStateComponent::g_Name));
    _componentFactory.Register<DiveAttackAIStateComponent>(DiveAttackAIStateComponent::GetIdFromName(DiveAttackAIStateComponent::g_Name));
    _componentFactory.Register<PredefinedMoveComponent>(PredefinedMoveComponent::GetIdFromName(PredefinedMoveComponent::g_Name));
    _componentFactory.Register<SoundTriggerComponent>(SoundTriggerComponent::GetIdFromName(SoundTriggerComponent::g_Name));
    _componentFactory.Register<GlobalAmbientSoundComponent>(GlobalAmbientSoundComponent::GetIdFromName(GlobalAmbientSoundComponent::g_Name));
    _componentFactory.Register<FollowableComponent>(FollowableComponent::GetIdFromName(FollowableComponent::g_Name));
    _componentFactory.Register<DamageAuraComponent>(DamageAuraComponent::GetIdFromName(DamageAuraComponent::g_Name));
    _componentFactory.Register<SingleAnimationComponent>(SingleAnimationComponent::GetIdFromName(SingleAnimationComponent::g_Name));
    _componentFactory.Register<TakeDamageAIStateComponent>(TakeDamageAIStateComponent::GetIdFromName(TakeDamageAIStateComponent::g_Name));
    _componentFactory.Register<ProjectileSpawnerComponent>(ProjectileSpawnerComponent::GetIdFromName(ProjectileSpawnerComponent::g_Name));
    _componentFactory.Register<LocalAmbientSoundComponent>(LocalAmbientSoundComponent::GetIdFromName(LocalAmbientSoundComponent::g_Name));
    _componentFactory.Register<BossStagerTriggerComponent>(BossStagerTriggerComponent::GetIdFromName(BossStagerTriggerComponent::g_Name));
    _componentFactory.Register<LaRauxBossAIStateComponent>(LaRauxBossAIStateComponent::GetIdFromName(LaRauxBossAIStateComponent::g_Name));
    _componentFactory.Register<PathElevatorComponent>(PathElevatorComponent::GetIdFromName(PathElevatorComponent::g_Name));
    _componentFactory.Register<FloorSpikeComponent>(FloorSpikeComponent::GetIdFromName(FloorSpikeComponent::g_Name));
    _componentFactory.Register<RopeComponent>(RopeComponent::GetIdFromName(RopeComponent::g_Name));
    _componentFactory.Register<SteppingGroundComponent>(SteppingGroundComponent::GetIdFromName(SteppingGroundComponent::g_Name));
    _componentFactory.Register<SpringBoardComponent>(SpringBoardComponent::GetIdFromName(SpringBoardComponent::g_Name));
    _componentFactory.Register<KatherineBossAIStateComponent>(KatherineBossAIStateComponent::GetIdFromName(KatherineBossAIStateComponent::g_Name));
    _componentFactory.Register<WolvingtonBossAIStateComponent>(WolvingtonBossAIStateComponent::GetIdFromName(WolvingtonBossAIStateComponent::g_Name));
    _componentFactory.Register<FallAIStateComponent>(FallAIStateComponent::GetIdFromName(FallAIStateComponent::g_Name));
    _componentFactory.Register<ActorSpawnerComponent>(ActorSpawnerComponent::GetIdFromName(ActorSpawnerComponent::g_Name));
    _componentFactory.Register<PunkRatAIStateComponent>(PunkRatAIStateComponent::GetIdFromName(PunkRatAIStateComponent::g_Name));
    _componentFactory.Register<GabrielAIStateComponent>(GabrielAIStateComponent::GetIdFromName(GabrielAIStateComponent::g_Name));
    _componentFactory.Register<GabrielCannonButtonComponent>(GabrielCannonButtonComponent::GetIdFromName(GabrielCannonButtonComponent::g_Name));
    _componentFactory.Register<GabrielCannonComponent>(GabrielCannonComponent::GetIdFromName(GabrielCannonComponent::g_Name));
    _componentFactory.Register<SawBladeComponent>();
    _componentFactory.Register<RollEnemyAIStateComponent>();
    _componentFactory.Register<ConveyorBeltComponent>();
    _componentFactory.Register<MarrowAIStateComponent>();
    _componentFactory.Register<MarrowParrotAIStateComponent>();
    _componentFactory.Register<MarrowFloorComponent>();
    _componentFactory.Register<AquatisAIStateComponent>();
    _componentFactory.Register<RedTailAIStateComponent>();
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
    actor->PostPostInit();

    return actor;
}

StrongActorPtr ActorFactory::CreateActor(const char* actorResource, TiXmlElement* overrides)
{
    // Grab the root XML node
    auto deleter = [](TiXmlElement *e) { delete e->GetDocument(); };

    std::unique_ptr<TiXmlElement, decltype(deleter)> root { XmlResourceLoader::LoadAndReturnRootXmlElement(actorResource, true), deleter};
    if (root == NULL)
    {
        LOG_ERROR("Could not load XML root node");
        return nullptr;
    }

    return CreateActor(root.get(), overrides);
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
