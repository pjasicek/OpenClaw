#pragma once

#include "../EnemyAIStateComponent.h"
#include "../../TriggerComponents/TriggerComponent.h"

class RedTailAIStateComponent : public BaseBossAIStateComponennt, public AnimationObserver
{
public:
    RedTailAIStateComponent();
    virtual ~RedTailAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;

    virtual void VOnWorldFinishedLoading() override;

    // EnemyAIStateComponent API

    virtual void VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState) override;
    virtual void VOnStateLeave(BaseEnemyAIStateComponent* pNextState) override;
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_BrainRedTail; }

    virtual void VUpdate(uint32 msDiff) override;

    // Can enemy enter this state ?
    virtual bool VCanEnter() override { return m_bBossFightStarted == false; }

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

};
