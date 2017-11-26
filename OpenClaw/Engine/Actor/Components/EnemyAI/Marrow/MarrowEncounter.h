#ifndef __MARROW_ENCOUNTER_H__
#define __MARROW_ENCOUNTER_H__

#include "../EnemyAIStateComponent.h"
#include "../../TriggerComponents/TriggerComponent.h"

enum MarrowActionType
{
    MarrowActionType_None,
};

typedef BossAction<MarrowActionType> MarrowAction;

typedef std::vector<MarrowAction> MarrowActionList;

class MarrowParrotAIStateComponent;
class FollowableComponent;
class MarrowAIStateComponent : public  BaseBossAIStateComponennt, public AnimationObserver
{
public:
    MarrowAIStateComponent();
    virtual ~MarrowAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;

    virtual void VOnWorldFinishedLoading() override;

    // EnemyAIStateComponent API
    virtual void VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState) override;
    virtual void VOnStateLeave(BaseEnemyAIStateComponent* pNextState) override;
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_BrainMarrow; }

    virtual void VUpdate(uint32 msDiff) override;

    // Can enemy enter this state ?
    virtual bool VCanEnter() override;

    //virtual int VGetPriority() override;

    // AnimationObserver API
    virtual void VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame) override;
    virtual void VOnAnimationLooped(Animation* pAnimation) override;

    // HealthObserver API
    virtual void VOnHealthChanged(int32 oldHealth, int32 newHealth, DamageType damageType, Point impactPoint, int sourceActorId) override;

    // Marrow will be the bossfight director
    void OnCanActivateFloor();

protected:
    virtual void VOnBossFightStarted() override;
    virtual void VOnBossFightEnded(bool isBossDead) override;

private:

    int m_ReqDamageToTransition;

    

    //------------------------------------
    void OnQuarterHealthGone();

    AnimationComponent* m_pAnimationComponent;
    FollowableComponent* m_pQuestionMark;
    MarrowParrotAIStateComponent* m_pParrot;
    ActorList m_MarrowFloorActorList;

    MarrowActionList m_GabrielActionList;
};

class MarrowFloorComponent;
class MarrowParrotAIStateComponent : public BaseEnemyAIStateComponent, public AnimationObserver, public HealthObserver
{
    enum MarrowParrotState
    {
        MarrowParrotState_None,
        MarrowParrotState_Charging,
        MarrowParrotState_SittingOnShoulder,
        MarrowParrotState_SittingOnPerch,
        MarrowParrotState_FlyingUp,
        MarrowParrotState_FlyingDown,
        MarrowParrotState_FlyingBack,
    };

public:
    MarrowParrotAIStateComponent();
    virtual ~MarrowParrotAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;
    virtual void VUpdate(uint32 msDiff) override;

    virtual void VOnWorldFinishedLoading() override;

    // EnemyAIStateComponent API
    virtual void VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState) override;
    virtual void VOnStateLeave(BaseEnemyAIStateComponent* pNextState) override;
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_BrainMarrowParrot; }
    virtual bool VCanEnter() override;

    // AnimationObserver API
    virtual void VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame) override;
    virtual void VOnAnimationLooped(Animation* pAnimation) override;
    virtual void VOnAnimationAtLastFrame(Animation* pAnimation) override;
    virtual void VOnAnimationChanged(Animation* pOldAnimation, Animation* pNewAnimation) override;

    void SitOnPerch();
    void SitOnShoulder(const Point& shoulderLocation);
    void DoAttack();

private:
    // XML Data
    std::string m_ChargeAnim;
    std::string m_IdleAnim;
    std::string m_FlyAnim;

    double m_ChargeSpeed;
    double m_FlyUpDownSpeed;
    double m_FlyBackSpeed;
    Point m_PerchLocation;

    // Internal
    void ActivateMarrowFloors();
    void OnGotHit(DamageType damageType);

    MarrowAIStateComponent* m_pMarrow;
    AnimationComponent* m_pAnimationComponent;
};

class MarrowFloorComponent : public ActorComponent
{
public:
    MarrowFloorComponent();
    virtual ~MarrowFloorComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;
    virtual void VUpdate(uint32 msDiff) override;

    virtual void VOnWorldFinishedLoading() override;

    void Activate();
    void Deactivate();

private:
    // XML Data
    Direction m_ActivateDirection;
    double m_Speed;
    int m_ActivatedBorderX;
    int m_DeactivatedBorderX;

    // Internal
    bool m_bActive;
};

#endif