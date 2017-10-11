#ifndef __GABRIEL_ENCOUNTER_H__
#define __GABRIEL_ENCOUNTER_H__

#include "../EnemyAIStateComponent.h"
#include "../../TriggerComponents/TriggerComponent.h"

enum GabrielActionType
{
    GabrielActionType_None,
    GabrielActionType_ThrowBombs,
    GabrielActionType_SummonPirate,
    GabrielActionType_FireCannon
};

// I can hardcode this, no point to make this data driven
struct GabrielAction
{
    GabrielAction()
    {
        actionType = GabrielActionType_None;
        probability = 0;
        reuseTime = 0;
        timeSinceLastUse = 0;
    }

    bool CanBeUsed()
    {
        return reuseTime < timeSinceLastUse;
    }

    GabrielActionType actionType;
    int probability;
    int reuseTime;
    int timeSinceLastUse;
};

typedef std::vector<GabrielAction> GabrielActionList;

class GabrielCannonComponent;
class FollowableComponent;
class GabrielAIStateComponent : public BaseBossAIStateComponennt, public AnimationObserver
{
public:
    GabrielAIStateComponent();
    virtual ~GabrielAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;

    virtual void VOnWorldFinishedLoading() override;

    // EnemyAIStateComponent API

    virtual void VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState) override;
    virtual void VOnStateLeave(BaseEnemyAIStateComponent* pNextState) override;
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_BrainGabriel; }

    virtual void VUpdate(uint32 msDiff) override;

    // Can enemy enter this state ?
    virtual bool VCanEnter() override { return true; }

    //virtual int VGetPriority() override;

    // AnimationObserver API
    virtual void VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame) override;
    virtual void VOnAnimationLooped(Animation* pAnimation) override;

    // HealthObserver API
    virtual void VOnHealthChanged(int32 oldHealth, int32 newHealth, DamageType damageType, Point impactPoint, int sourceActorId) override;

protected:
    virtual void VOnBossFightStarted() override;
    virtual void VOnBossFightEnded(bool isBossDead) override;

private:
    std::string m_IdleAnim;
    std::string m_ThrowBombAnim;
    std::string m_FireCannonAnim;
    std::string m_SpawnPirateAnim;

    SoundList m_ThrowBombSoundList;
    SoundList m_SpawnPirateSoundList;
    SoundList m_FireCannonSoundList;

    int m_ThrowBombAnimFrameIdx;
    int m_FireCannonAnimFrameIdx;

    int m_NumThrownBombs;
    int m_NumSpawnedPirates;
    int m_BombDamage;

    int m_ActionMinInterval;
    int m_ActionMaxInterval;

    GabrielActionList m_GabrielActionList;

    //------------------------------------
    GabrielCannonComponent* m_pGabrielCannonComponent;
    AnimationComponent* m_pAnimationComponent;
    FollowableComponent* m_pQuestionMark;

    int m_TimeSinceLastAction;
    int m_CurrActionDelay;

    bool m_bShouldThrowBombs;
};

class GabrielCannonButtonComponent : public ActorComponent, public TriggerObserver
{
public:
    GabrielCannonButtonComponent();
    virtual ~GabrielCannonButtonComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;
    virtual void VUpdate(uint32 msDiff) override;

    virtual void VOnWorldFinishedLoading() override;

    void Reset();

    // TriggerObserver API
    virtual void VOnActorEnteredTrigger(Actor* pActorWhoEntered, FixtureType triggerType);

private:
    std::string m_BlinkingAnim;
    std::string m_PressedImageName;
    std::string m_ButtonPressSound;

    //---------------
    GabrielCannonComponent* m_pGabrielCannonComponent;
    AnimationComponent* m_pAnimationComponent;
    ActorRenderComponent* m_pRenderComponent;
};

class GabrielCannonComponent : public ActorComponent, public AnimationObserver
{
public:
    GabrielCannonComponent();
    virtual ~GabrielCannonComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;
    virtual void VUpdate(uint32 msDiff) override;

    virtual void VOnWorldFinishedLoading() override;

    void Fire();
    void Rise();
    bool IsReadyToFire();

    // AnimationObserver API
    virtual void VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame) override;
    virtual void VOnAnimationLooped(Animation* pAnimation) override;
    virtual void VOnAnimationAtLastFrame(Animation* pAnimation) override;
    virtual void VOnAnimationChanged(Animation* pOldAnimation, Animation* pNewAnimation) override;

private:
    void BossFightStartedDelegate(IEventDataPtr pEvent);
    void BossFightEndedDelegate(IEventDataPtr pEvent);

    std::string m_HorizontalFireAnim;
    std::string m_VerticalFireAnim;
    std::string m_ToVerticalAnim;
    std::string m_ToHorizontalAnim;
    std::string m_IdleAnim;

    std::string m_CannonRiseSound;
    std::string m_CannonDropSound;

    int m_HorizontalFireAnimIdx;
    int m_VerticalFireAnimIdx;

    //-----------
    GabrielCannonButtonComponent* m_pCannonButton;
    HealthComponent* m_pGabrielHealthComponent;
    AnimationComponent* m_pAnimationComponent;
    bool m_bQueuedRise;
    bool m_bActive;
};

#endif