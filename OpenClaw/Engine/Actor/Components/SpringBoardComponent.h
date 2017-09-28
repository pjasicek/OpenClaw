#ifndef __SPRING_BOARD_COMPONENT_H__
#define __SPRING_BOARD_COMPONENT_H__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "AnimationComponent.h"
#include "TriggerComponents/TriggerComponent.h"

class SpringBoardComponent : public ActorComponent, public AnimationObserver, public TriggerObserver
{
public:
    SpringBoardComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;

    virtual TiXmlElement* VGenerateXml() override { assert(false && "Unimplemented"); return NULL; }

    virtual void VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame) override;
    virtual void VOnAnimationLooped(Animation* pAnimation) override;

    virtual void VOnActorEnteredTrigger(Actor* pActorWhoEntered, FixtureType triggerType) override;
    virtual void VOnActorLeftTrigger(Actor* pActorWhoLeft, FixtureType triggerType) override;

    // Internal
    void OnActorBeginContact(Actor* pActor);
    void OnActorEndContact(Actor* pActor);

private:
    // XML Data
    SpringBoardDef m_Properties;

    bool m_bHasIdleAnimation;

    ActorList m_StandingActorsList;

    bool m_bIsSteppedOn;

    shared_ptr<IGamePhysics> m_pPhysics;
    AnimationComponent* m_pAnimationComponent;
};

#endif //__SPRING_BOARD_COMPONENT_H__