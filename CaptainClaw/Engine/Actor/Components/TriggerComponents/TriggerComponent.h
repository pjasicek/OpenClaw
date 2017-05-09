#ifndef __BASETRIGGERCOMPONENT_H__
#define __BASETRIGGERCOMPONENT_H__

#include "../../../SharedDefines.h"
#include "../../ActorComponent.h"
#include "../../../Util/Subject.h"
#include "../../Actor.h"

class TriggerObserver;
class TriggerSubject : public Subject<TriggerObserver>
{
public:
    void NotifyEnterTrigger(Actor* pActorWhoEntered);
    void NotifyLeaveTrigger(Actor* pActorWhoLeft);
};

class TriggerObserver
{
public:
    virtual void VOnActorEnteredTrigger(Actor* pActorWhoEntered) { }
    virtual void VOnActorLeftTrigger(Actor* pActorWhoLeft) { }
};

class TriggerComponent : public ActorComponent, public TriggerSubject
{
public:
    TriggerComponent();
    virtual ~TriggerComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;
    virtual void VPostInit() override;

    virtual SDL_Rect GetTriggerArea();

    // TOTO: Implement this in future
    // Different components can have different trigger areas, this should implement this request
    // OnActorEntered should also have SDL_Rect area of the trigger so that it could then iterate
    // over observers and call the appropriate ones
    //void AddTriggerArea(SDL_Rect triggerArea, TriggerObserver* pObserver)

    void Deactivate() { m_pPhysics->VDeactivate(_owner->GetGUID()); }
    void Activate() { m_pPhysics->VActivate(_owner->GetGUID()); }
    void Destroy() { m_pPhysics->VRemoveActor(_owner->GetGUID()); }

    void OnActorEntered(Actor* pActor);
    void OnActorLeft(Actor* pActor);

private:
    bool HasOverlappingActor(Actor* pActor);
    void AddOverlappingActor(Actor* pActor);
    void RemoveOverlappingActor(Actor* pActor);

    bool m_IsTriggerUnlimited;
    bool m_IsTriggerOnce;
    int m_TriggerRemaining;
    Point m_Size;
    bool m_IsStatic;

    std::vector<Actor*> m_ActorsInsideList;

    shared_ptr<IGamePhysics> m_pPhysics;
};

#endif
