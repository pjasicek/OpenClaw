#include "ClawGameLogic.h"
#include "ClawHumanView.h"
#include "ClawGameApp.h"
#include "Engine/Events/EventMgr.h"
#include "Engine/Events/Events.h"
#include "ClawEvents.h"

#include "Engine/Actor/Components/PhysicsComponent.h"
#include "Engine/Actor/Components/ControllableComponent.h"
#include "Engine/Physics/ClawPhysics.h"

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
}

void ClawGameLogic::RemoveAllDelegates()
{
    IEventMgr* pGlobalEventManager = IEventMgr::Get();
    pGlobalEventManager->VRemoveListener(MakeDelegate(this, &ClawGameLogic::PlayerActorAssignmentDelegate), EventData_Attach_Actor::sk_EventType);
    pGlobalEventManager->VRemoveListener(MakeDelegate(this, &ClawGameLogic::ControlledActorStartMoveDelegate), EventData_Actor_Start_Move::sk_EventType);
    pGlobalEventManager->VRemoveListener(MakeDelegate(this, &ClawGameLogic::ControlledActorStartClimbDelegate), EventData_Start_Climb::sk_EventType);
    pGlobalEventManager->VRemoveListener(MakeDelegate(this, &ClawGameLogic::ActorFireDelegate), EventData_Actor_Fire::sk_EventType);
    pGlobalEventManager->VRemoveListener(MakeDelegate(this, &ClawGameLogic::ActorAttackDelegate), EventData_Actor_Attack::sk_EventType);
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