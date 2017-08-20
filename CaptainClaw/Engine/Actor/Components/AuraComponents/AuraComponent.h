#ifndef __AURA_COMPONENT_H__
#define __AURA_COMPONENT_H__

#include "../../ActorComponent.h"

//=====================================================================================================================
// BaseAuraComponent - base class for derived pickup components
//
// Note: Actor with this component has to provide AuraFixture in its physics body (sensor)
//=====================================================================================================================

// Each pulse is unique for each actor
struct PulseInfo
{
    PulseInfo()
    {
        pActor = NULL;
        timeSinceLastPulseMs = 0;
    }

    Actor* pActor;
    int timeSinceLastPulseMs;
};


class PositionComponent;
class ActorRenderComponent;
class BaseAuraComponent : public ActorComponent
{
    typedef std::vector<PulseInfo> ActivePulseList;

public:
    BaseAuraComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual void VPostInit() override;
    virtual void VPostPostInit() override;
    virtual TiXmlElement* VGenerateXml() override;

    virtual void VUpdate(uint32 msDiff) override final;

    void OnActorEntered(Actor* pActor);
    void OnActorLeft(Actor* pActor);

    virtual void VOnAuraApply(Actor* pActorInAura) { }
    virtual void VOnAuraRemove (Actor* pActorInAura) { }

    void SetEnabled(bool enabled);

protected:
    virtual bool VDelegateInit(TiXmlElement* data) { return true; }
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement) { };

    // XML Data members
    bool m_bIsPulsating;
    bool m_bIsGroupPulse;
    bool m_bApplyAuraOnEnter;
    bool m_bRemoveActorAfterPulse;
    int m_PulseInterval;
    ActorFixtureDef m_AuraFixtureDef;

    // Internal members
    ActivePulseList m_ActivePulseList;
    int m_TimeSinceLastPulse;
    bool m_bIsEnabled;
};

//=====================================================================================================================
// DamageAuraComponent
//=====================================================================================================================

class DamageAuraComponent : public BaseAuraComponent
{
public:
    DamageAuraComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual void VPostInit();

    virtual void VOnAuraApply(Actor* pActorInAura);

protected:
    virtual bool VDelegateInit(TiXmlElement* data);
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement);

private:
    int m_Damage;
    bool m_bIsEnemyUnit;
};

#endif