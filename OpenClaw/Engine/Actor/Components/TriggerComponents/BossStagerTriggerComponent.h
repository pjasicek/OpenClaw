#ifndef __BOSS_STAGER_TRIGGER_COMPONENT_H__
#define __BOSS_STAGER_TRIGGER_COMPONENT_H__

#include "../../ActorComponent.h"
#include "../PickupComponents/PickupComponent.h"
#include "TriggerComponent.h"

//=====================================================================================================================
// BossStagerTriggerComponent
//=====================================================================================================================

enum BossStagerState
{
    BossStagerState_None,
    BossStagerState_MovingToBoss,
    BossStagerState_PlayingBossDialogSound,
    BossStagerState_MovingToClaw,
    BossStagerState_PlayingClawDialogSound,
    BossStagerState_MovingPopupDown,
    BossStagerState_PlayingPopupSound1,
    BossStagerState_PlayingPopupSound2,
    BossStagerState_MovingPopupRight,
    BossStagerState_Done
};

class BossStagerTriggerComponent : public ActorComponent, public TriggerObserver
{
public:
    BossStagerTriggerComponent();
    virtual ~BossStagerTriggerComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual void VPostInit() override;
    virtual void VUpdate(uint32 msDiff) override;

    virtual TiXmlElement* VGenerateXml() override;

    virtual void VOnActorEnteredTrigger(Actor* pActorWhoEntered, FixtureType triggerType) override;
    virtual void VOnActorLeftTrigger(Actor* pActorWhoLeft, FixtureType triggerType) override;

private:
    void BossFightEndedDelegate(IEventDataPtr pEvent);
    void ClawRespawnedDelegate(IEventDataPtr pEvent);

    // XML Data
    int m_BossDistance;
    int m_CameraSpeed;
    std::string m_ClawDialogSound;
    std::string m_BossDialogSound;
    std::string m_PopupTitleImageSet;
    std::string m_PopupTitleSound;
    int m_PopupTitleSpeed;

    // Internal state
    bool m_bActivated;
    bool m_bDone;
    Actor* m_pPopupTitleActor;
    BossStagerState m_State;

    uint32 m_ActorWhoEnteredId;

    int m_Delay;
    int m_CurrentDelay;

    shared_ptr<CameraNode> m_pCamera;

    Actor* m_pOverlappingActor;
};

#endif