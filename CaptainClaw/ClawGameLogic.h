#ifndef __CLAWGAMELOGIC_H__
#define __CLAWGAMELOGIC_H__

#include "Engine/GameApp/BaseGameLogic.h"

class ClawGameLogic : public BaseGameLogic
{
public:
    ClawGameLogic();
    virtual ~ClawGameLogic();

    virtual void VMoveActor(const uint32_t actorId, Point newPosition);

    virtual void VChangeState(GameState newState);
    virtual void VAddView(shared_ptr<IGameView> pView, uint32 actorId = INVALID_ACTOR_ID);

    // Delegates
    void PlayerActorAssignmentDelegate(IEventDataPtr pEventData);
    void ControlledActorStartMoveDelegate(IEventDataPtr pEventData);
    void ControlledActorStartClimbDelegate(IEventDataPtr pEventData);
    void ActorFireDelegate(IEventDataPtr pEventData);
    void ActorFireEndedDelegate(IEventDataPtr pEventData);
    void ActorAttackDelegate(IEventDataPtr pEventData);
    void NewLifeDelegate(IEventDataPtr pEventData);
    void TeleportActorDelegate(IEventDataPtr pEventData);
    void RequestChangeAmmoTypeDelegate(IEventDataPtr pEventData);
    void ModifyActorStatDelegate(IEventDataPtr pEventData);
    void CheckpointReachedDelegate(IEventDataPtr pEventData);
    void ClawDiedDelegate(IEventDataPtr pEventData);
    void UpdatedPowerupStatusDelegate(IEventDataPtr pEventData);

protected:
    virtual bool VLoadGameDelegate(TiXmlElement* pLevelData);

private:
    void RegisterAllDelegates();
    void RemoveAllDelegates();
};

#endif