#include "ClawGameLogic.h"
#include "ClawHumanView.h"
#include "ClawGameApp.h"
#include "Engine/Events/EventMgr.h"
#include "Engine/Events/Events.h"
#include "ClawEvents.h"

#include "Engine/Actor/Components/PhysicsComponent.h"
#include "Engine/Actor/Components/ControllableComponent.h"
#include "Engine/Actor/Components/ControllerComponents/LifeComponent.h"
#include "Engine/Actor/Components/ControllerComponents/HealthComponent.h"
#include "Engine/Actor/Components/ControllerComponents/ScoreComponent.h"
#include "Engine/Actor/Components/ControllerComponents/AmmoComponent.h"
#include "Engine/Physics/ClawPhysics.h"
#include "Engine/GameApp/GameSaves.h"

ClawGameLogic::ClawGameLogic()
{
    RegisterAllDelegates();

    m_pPhysics.reset(CreateClawPhysics());
}

ClawGameLogic::~ClawGameLogic()
{
    RemoveAllDelegates();
}

void ClawGameLogic::VMoveActor(const uint32_t actorId, Point newPosition)
{

}

void ClawGameLogic::VChangeState(GameState newState)
{
    BaseGameLogic::VChangeState(newState);
}

void ClawGameLogic::VAddView(shared_ptr<IGameView> pView, uint32 actorId)
{
    BaseGameLogic::VAddView(pView, actorId);
}

bool ClawGameLogic::VLoadGameDelegate(TiXmlElement* pLevelData)
{
    return true;
}

void ClawGameLogic::RegisterAllDelegates()
{
    IEventMgr* pGlobalEventManager = IEventMgr::Get();
    pGlobalEventManager->VAddListener(MakeDelegate(this, &ClawGameLogic::PlayerActorAssignmentDelegate), EventData_Attach_Actor::sk_EventType);
    pGlobalEventManager->VAddListener(MakeDelegate(this, &ClawGameLogic::ControlledActorStartMoveDelegate), EventData_Actor_Start_Move::sk_EventType);
    pGlobalEventManager->VAddListener(MakeDelegate(this, &ClawGameLogic::ControlledActorStartClimbDelegate), EventData_Start_Climb::sk_EventType);
    pGlobalEventManager->VAddListener(MakeDelegate(this, &ClawGameLogic::ActorFireDelegate), EventData_Actor_Fire::sk_EventType);
    pGlobalEventManager->VAddListener(MakeDelegate(this, &ClawGameLogic::ActorAttackDelegate), EventData_Actor_Attack::sk_EventType);
    pGlobalEventManager->VAddListener(MakeDelegate(this, &ClawGameLogic::NewLifeDelegate), EventData_New_Life::sk_EventType);
    pGlobalEventManager->VAddListener(MakeDelegate(this, &ClawGameLogic::TeleportActorDelegate), EventData_Teleport_Actor::sk_EventType);
    pGlobalEventManager->VAddListener(MakeDelegate(this, &ClawGameLogic::RequestChangeAmmoTypeDelegate), EventData_Request_Change_Ammo_Type::sk_EventType);
    pGlobalEventManager->VAddListener(MakeDelegate(this, &ClawGameLogic::ModifyActorStatDelegate), EventData_Modify_Player_Stat::sk_EventType);
    pGlobalEventManager->VAddListener(MakeDelegate(this, &ClawGameLogic::CheckpointReachedDelegate), EventData_Checkpoint_Reached::sk_EventType);
    pGlobalEventManager->VAddListener(MakeDelegate(this, &ClawGameLogic::ClawDiedDelegate), EventData_Claw_Died::sk_EventType);
    pGlobalEventManager->VAddListener(MakeDelegate(this, &ClawGameLogic::UpdatedPowerupStatusDelegate), EventData_Updated_Powerup_Status::sk_EventType);
}

void ClawGameLogic::RemoveAllDelegates()
{
    IEventMgr* pGlobalEventManager = IEventMgr::Get();
    pGlobalEventManager->VRemoveListener(MakeDelegate(this, &ClawGameLogic::PlayerActorAssignmentDelegate), EventData_Attach_Actor::sk_EventType);
    pGlobalEventManager->VRemoveListener(MakeDelegate(this, &ClawGameLogic::ControlledActorStartMoveDelegate), EventData_Actor_Start_Move::sk_EventType);
    pGlobalEventManager->VRemoveListener(MakeDelegate(this, &ClawGameLogic::ControlledActorStartClimbDelegate), EventData_Start_Climb::sk_EventType);
    pGlobalEventManager->VRemoveListener(MakeDelegate(this, &ClawGameLogic::ActorFireDelegate), EventData_Actor_Fire::sk_EventType);
    pGlobalEventManager->VRemoveListener(MakeDelegate(this, &ClawGameLogic::ActorAttackDelegate), EventData_Actor_Attack::sk_EventType);
    pGlobalEventManager->VRemoveListener(MakeDelegate(this, &ClawGameLogic::NewLifeDelegate), EventData_New_Life::sk_EventType);
    pGlobalEventManager->VRemoveListener(MakeDelegate(this, &ClawGameLogic::TeleportActorDelegate), EventData_Teleport_Actor::sk_EventType);
    pGlobalEventManager->VRemoveListener(MakeDelegate(this, &ClawGameLogic::RequestChangeAmmoTypeDelegate), EventData_Request_Change_Ammo_Type::sk_EventType);
    pGlobalEventManager->VRemoveListener(MakeDelegate(this, &ClawGameLogic::ModifyActorStatDelegate), EventData_Modify_Player_Stat::sk_EventType);
    pGlobalEventManager->VRemoveListener(MakeDelegate(this, &ClawGameLogic::CheckpointReachedDelegate), EventData_Checkpoint_Reached::sk_EventType);
    pGlobalEventManager->VRemoveListener(MakeDelegate(this, &ClawGameLogic::ClawDiedDelegate), EventData_Claw_Died::sk_EventType);
    pGlobalEventManager->VRemoveListener(MakeDelegate(this, &ClawGameLogic::UpdatedPowerupStatusDelegate), EventData_Updated_Powerup_Status::sk_EventType);
}

//=====================================================================================================================
// Claw game specific delegates
//=====================================================================================================================

void ClawGameLogic::PlayerActorAssignmentDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Attach_Actor> pCastEventData = static_pointer_cast<EventData_Attach_Actor>(pEventData);

    for (auto it = m_GameViews.begin(); it != m_GameViews.end(); ++it)
    {
        shared_ptr<IGameView> pView = *it;
        if (pView->VGetType() == GameView_Human)
        {
            shared_ptr<ClawHumanView> pHumanView = static_pointer_cast<ClawHumanView, IGameView>(pView);
            pHumanView->VSetControlledActor(pCastEventData->GetActorId());
            return;
        }
    }

    LOG_ERROR("Could not find HumanView to attach actor to!");
}

void ClawGameLogic::ControlledActorStartMoveDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Actor_Start_Move> pCastEventData = static_pointer_cast<EventData_Actor_Start_Move>(pEventData);

    StrongActorPtr pActor = MakeStrongPtr(VGetActor(pCastEventData->GetActorId()));
    if (!pActor)
    {
        return;
    }

    shared_ptr<PhysicsComponent> pPhysicsComponent = 
        MakeStrongPtr(pActor->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
    if (!pPhysicsComponent)
    {
        return;
    }

    pPhysicsComponent->SetCurrentSpeed(pCastEventData->GetMove());
}

void ClawGameLogic::ControlledActorStartClimbDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Start_Climb> pCastEventData = static_pointer_cast<EventData_Start_Climb>(pEventData);

    StrongActorPtr pActor = MakeStrongPtr(VGetActor(pCastEventData->GetActorId()));
    if (!pActor)
    {
        return;
    }

    shared_ptr<PhysicsComponent> pPhysicsComponent =
        MakeStrongPtr(pActor->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
    if (!pPhysicsComponent)
    {
        return;
    }

    pPhysicsComponent->RequestClimb(pCastEventData->GetClimbMovement());
}

void ClawGameLogic::ActorFireDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Actor_Fire> pCastEventData = static_pointer_cast<EventData_Actor_Fire>(pEventData);

    StrongActorPtr pActor = MakeStrongPtr(VGetActor(pCastEventData->GetActorId()));
    if (!pActor)
    {
        return;
    }

    shared_ptr<ClawControllableComponent> pControllableComponent =
        MakeStrongPtr(pActor->GetComponent<ClawControllableComponent>(ClawControllableComponent::g_Name));
    if (!pControllableComponent)
    {
        return;
    }

    pControllableComponent->OnFire(true);
}

void ClawGameLogic::ActorAttackDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Actor_Attack> pCastEventData = static_pointer_cast<EventData_Actor_Attack>(pEventData);

    StrongActorPtr pActor = MakeStrongPtr(VGetActor(pCastEventData->GetActorId()));
    if (!pActor)
    {
        return;
    }

    shared_ptr<ClawControllableComponent> pControllableComponent =
        MakeStrongPtr(pActor->GetComponent<ClawControllableComponent>(ClawControllableComponent::g_Name));
    if (!pControllableComponent)
    {
        return;
    }
    pControllableComponent->OnAttack();
}

void ClawGameLogic::NewLifeDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_New_Life> pCastEventData = static_pointer_cast<EventData_New_Life>(pEventData);

    StrongActorPtr pActor = MakeStrongPtr(VGetActor(pCastEventData->GetActorId()));
    if (!pActor)
    {
        return;
    }

    shared_ptr<LifeComponent> pLifeComponent = MakeStrongPtr(pActor->GetComponent<LifeComponent>(LifeComponent::g_Name));
    if (!pLifeComponent)
    {
        LOG_WARNING("Life component not present in actor: " + pActor->GetName());
        return;
    }
    pLifeComponent->AddLives(pCastEventData->GetNumNewLives());
}

void ClawGameLogic::TeleportActorDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Teleport_Actor> pCastEventData = static_pointer_cast<EventData_Teleport_Actor>(pEventData);

    StrongActorPtr pActor = MakeStrongPtr(VGetActor(pCastEventData->GetActorId()));
    if (!pActor)
    {
        return;
    }

    m_pPhysics->VSetPosition(pCastEventData->GetActorId(), pCastEventData->GetDestination());
}

void ClawGameLogic::RequestChangeAmmoTypeDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Request_Change_Ammo_Type> pCastEventData = static_pointer_cast<EventData_Request_Change_Ammo_Type>(pEventData);

    StrongActorPtr pActor = MakeStrongPtr(VGetActor(pCastEventData->GetActorId()));
    if (!pActor)
    {
        return;
    }

    shared_ptr<AmmoComponent> pAmmoComponent =
        MakeStrongPtr(pActor->GetComponent<AmmoComponent>(AmmoComponent::g_Name));
    if (!pAmmoComponent)
    {
        return;
    }

    AmmoType newAmmoType = AmmoType((pAmmoComponent->GetActiveAmmoType() + 1) % AmmoType_Max);
    pAmmoComponent->SetActiveAmmo(newAmmoType);

    shared_ptr<EventData_Updated_Ammo_Type> pEvent(new EventData_Updated_Ammo_Type(pCastEventData->GetActorId(), newAmmoType));
    IEventMgr::Get()->VTriggerEvent(pEvent);
}

void ClawGameLogic::ModifyActorStatDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Modify_Player_Stat> pCastEventData = static_pointer_cast<EventData_Modify_Player_Stat>(pEventData);

    StrongActorPtr pActor = MakeStrongPtr(VGetActor(pCastEventData->GetActorId()));
    if (!pActor)
    {
        return;
    }

    if (pCastEventData->GetStatType() == PlayerStat_Score)
    {
        shared_ptr<ScoreComponent> pScoreComponent = MakeStrongPtr(pActor->GetComponent<ScoreComponent>(ScoreComponent::g_Name));
        if (!pScoreComponent)
        {
            assert(false && "Actor does not have ScoreComponent !");
            return;
        }
        if (pCastEventData->AddToExistingStat())
        {
            pScoreComponent->AddScorePoints(pCastEventData->GetStatValue());
        }
        else
        {
            pScoreComponent->SetCurrentScore(pCastEventData->GetStatValue(), true);
        }
    }
    else if (pCastEventData->GetStatType() == PlayerStat_Health)
    {
        shared_ptr<HealthComponent> pHealthComponent = MakeStrongPtr(pActor->GetComponent<HealthComponent>(HealthComponent::g_Name));
        if (!pHealthComponent)
        {
            assert(false && "Actor does not have HealthComponent !");
            return;
        }
        if (pCastEventData->AddToExistingStat())
        {
            pHealthComponent->AddHealth(pCastEventData->GetStatValue());
        }
        else
        {
            pHealthComponent->SetCurrentHealth(pCastEventData->GetStatValue());
        }
    }
    else if (pCastEventData->GetStatType() == PlayerStat_Lives)
    {
        shared_ptr<LifeComponent> pLifeComponent = MakeStrongPtr(pActor->GetComponent<LifeComponent>(LifeComponent::g_Name));
        if (!pLifeComponent)
        {
            assert(false && "Actor does not have LifeComponent !");
            return;
        }
        if (pCastEventData->AddToExistingStat())
        {
            pLifeComponent->AddLives(pCastEventData->GetStatValue());
        }
        else
        {
            pLifeComponent->SetCurrentLives(pCastEventData->GetStatValue());
        }
    }
    else if (pCastEventData->GetStatType() == PlayerStat_Bullets || 
        pCastEventData->GetStatType() == PlayerStat_Magic ||
        pCastEventData->GetStatType() == PlayerStat_Dynamite)
    {
        shared_ptr<AmmoComponent> pAmmoComponent =
            MakeStrongPtr(pActor->GetComponent<AmmoComponent>(AmmoComponent::g_Name));
        if (!pAmmoComponent)
        {
            assert(false && "Actor does not have AmmoComponent !");
            return;
        }

        AmmoType ammoType = AmmoType_Max;
        if (pCastEventData->GetStatType() == PlayerStat_Bullets) { ammoType = AmmoType_Pistol; }
        else if (pCastEventData->GetStatType() == PlayerStat_Magic) { ammoType = AmmoType_Magic; }
        else if (pCastEventData->GetStatType() == PlayerStat_Dynamite) { ammoType = AmmoType_Dynamite; }

        if (ammoType == AmmoType_Max)
        {
            assert(false && "Unknown player stat / ammo type !");
        }

        if (pCastEventData->AddToExistingStat())
        {
            pAmmoComponent->AddAmmo(ammoType, pCastEventData->GetStatValue());
        }
        else
        {
            pAmmoComponent->SetAmmo(ammoType, pCastEventData->GetStatValue());
        }
    }
    else
    {
        assert(false && "Unknown player stat !");
    }
}

void ClawGameLogic::CheckpointReachedDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Checkpoint_Reached> pCastEventData = static_pointer_cast<EventData_Checkpoint_Reached>(pEventData);

    StrongActorPtr pActor = MakeStrongPtr(VGetActor(pCastEventData->GetActorId()));
    if (!pActor)
    {
        return;
    }

    m_CurrentSpawnPosition = pCastEventData->GetSpawnPoint();

    if (pCastEventData->IsSaveCheckpoint())
    {
        CheckpointSave checkpointSave;
        
        auto pScoreComponent = MakeStrongPtr(pActor->GetComponent<ScoreComponent>(ScoreComponent::g_Name));
        auto pHealthComponent = MakeStrongPtr(pActor->GetComponent<HealthComponent>(HealthComponent::g_Name));
        auto pLifeComponent = MakeStrongPtr(pActor->GetComponent<LifeComponent>(LifeComponent::g_Name));
        auto pAmmoComponent = MakeStrongPtr(pActor->GetComponent<AmmoComponent>(AmmoComponent::g_Name));
        assert(pScoreComponent);
        assert(pHealthComponent);
        assert(pLifeComponent);
        assert(pAmmoComponent);

        checkpointSave.checkpointIdx = pCastEventData->GetSaveCheckpointNumber();
        checkpointSave.score = pScoreComponent->GetScore();
        checkpointSave.health = pHealthComponent->GetHealth();
        checkpointSave.lives = pLifeComponent->GetLives();
        checkpointSave.bulletCount = pAmmoComponent->GetRemainingAmmo(AmmoType_Pistol);
        checkpointSave.magicCount = pAmmoComponent->GetRemainingAmmo(AmmoType_Magic);
        checkpointSave.dynamiteCount = pAmmoComponent->GetRemainingAmmo(AmmoType_Dynamite);

        m_pGameSaveMgr->AddCheckpointSave(m_pCurrentLevel->GetLevelNumber(), checkpointSave);
    }
}

void ClawGameLogic::ClawDiedDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Checkpoint_Reached> pCastEventData = static_pointer_cast<EventData_Checkpoint_Reached>(pEventData);

    StrongActorPtr pActor = MakeStrongPtr(VGetActor(pCastEventData->GetActorId()));
    if (!pActor)
    {
        return;
    }

    IEventMgr* pEventMgr = IEventMgr::Get();
    pEventMgr->VTriggerEvent(IEventDataPtr(new EventData_Modify_Player_Stat(pCastEventData->GetActorId(), PlayerStat_Lives, -1, true)));

    pEventMgr->VQueueEvent(IEventDataPtr(new EventData_Modify_Player_Stat(pCastEventData->GetActorId(), PlayerStat_Health, 1000, true)));
    pEventMgr->VQueueEvent(IEventDataPtr(new EventData_Teleport_Actor(pCastEventData->GetActorId(), m_CurrentSpawnPosition)));
}

void ClawGameLogic::UpdatedPowerupStatusDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Updated_Powerup_Status> pCastEventData = static_pointer_cast<EventData_Updated_Powerup_Status>(pEventData);

    StrongActorPtr pActor = MakeStrongPtr(VGetActor(pCastEventData->GetActorId()));
    if (!pActor)
    {
        return;
    }

    if (pCastEventData->GetPowerupType() == PowerupType_Catnip)
    {
        shared_ptr<PhysicsComponent> pPhysicsComponent =
            MakeStrongPtr(pActor->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
        assert(pPhysicsComponent);

        if (pCastEventData->IsPowerupFinished())
        {
            pPhysicsComponent->SetMaxJumpHeight(130);
        }
        else
        {
            pPhysicsComponent->SetMaxJumpHeight(170);
        }
    }
}