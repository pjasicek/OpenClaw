#ifndef __PUNK_RAT_AI_STATE_COMPONENT_H__
#define __PUNK_RAT_AI_STATE_COMPONENT_H__

#include "EnemyAIStateComponent.h"

class PunkRatAIStateComponent : public BaseEnemyAIStateComponent, public AnimationObserver
{
public:
    PunkRatAIStateComponent();
    virtual ~PunkRatAIStateComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;

    // EnemyAIStateComponent API

    virtual void VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState) override;
    virtual void VOnStateLeave(BaseEnemyAIStateComponent* pNextState) override;
    virtual EnemyAIState VGetStateType() const override { return EnemyAIState_PunkRat; }

    // Can enemy enter this state ?
    virtual bool VCanEnter() override;

    //virtual int VGetPriority() override;

    // AnimationObserver API
    virtual void VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame) override;
    virtual void VOnAnimationLooped(Animation* pAnimation) override;

private:
    ActorPrototype m_CannonActorProto;
    Point m_CannonSpawnOffset;
    std::string m_RatFireAnim;
    std::string m_RatRecoilAnim;
    int m_RatFireAnimFrameIdx;

    std::string m_CannonFireAnim;
    std::string m_CannonIdleAnim;
    int m_CannonFireAnimFrameIdx;

    bool m_bIsMirrored;
    ActorPrototype m_ProjectileActorPrototype;
    Point m_ProjectileSpawnOffset;

    AnimationComponent* m_pAnimationComponent;

    Actor* m_pCannonActor;
    AnimationComponent* m_pCannonAnimationComponent;
};

#endif