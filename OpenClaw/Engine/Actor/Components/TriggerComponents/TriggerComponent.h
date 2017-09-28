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
    void NotifyEnterTrigger(Actor* pActorWhoEntered, FixtureType triggerType);
    void NotifyLeaveTrigger(Actor* pActorWhoLeft, FixtureType triggerType);
};

class TriggerObserver
{
public:
    virtual void VOnActorEnteredTrigger(Actor* pActorWhoEntered, FixtureType triggerType) { }
    virtual void VOnActorLeftTrigger(Actor* pActorWhoLeft, FixtureType triggerType) { }
};

typedef std::map<FixtureType, std::string> TriggerToSoundMap;
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

    void Deactivate() { m_pPhysics->VDeactivate(m_pOwner->GetGUID()); }
    void Activate() { m_pPhysics->VActivate(m_pOwner->GetGUID()); }
    void Destroy() { m_pPhysics->VRemoveActor(m_pOwner->GetGUID()); }

    // Has to be reversed since default value in map for bool value is false
    void Deactivate(FixtureType triggerType) { m_DeactivatedTriggerTypesMap[triggerType] = true; }
    void Activate(FixtureType triggerType) { m_DeactivatedTriggerTypesMap[triggerType] = false; }

    void OnActorEntered(Actor* pActor, FixtureType triggerType);
    void OnActorLeft(Actor* pActor, FixtureType triggerType);

private:
    bool HasOverlappingActor(Actor* pActor);
    void AddOverlappingActor(Actor* pActor);
    void RemoveOverlappingActor(Actor* pActor);

    bool m_IsTriggerUnlimited;
    bool m_IsTriggerOnce;
    int m_TriggerRemaining;
    Point m_Size;
    bool m_IsStatic;
    
    TriggerToSoundMap m_TriggerEnterSoundMap;
    TriggerToSoundMap m_TriggerLeaveSoundMap;

    std::map<FixtureType, bool> m_DeactivatedTriggerTypesMap;

    std::vector<Actor*> m_ActorsInsideList;

    shared_ptr<IGamePhysics> m_pPhysics;
};

#endif
