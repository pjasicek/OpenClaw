#ifndef ___HEALTHCOMPONENT_H__
#define ___HEALTHCOMPONENT_H__

#include "../../ActorComponent.h"
#include "../../../Util/Subject.h"

class HealthObserver;
class HealthSubject : public Subject<HealthObserver>
{
public:
    void NotifyHealthChanged(int32 oldHealth, int32 newHealth);
    void NotifyHealthBelowZero();
};

class HealthObserver
{
public:
    virtual void VOnHealthChanged(int32 oldHealth, int32 newHealth) { }
    virtual void VOnHealthBelowZero() { }
};

class HealthComponent : public ActorComponent, public HealthSubject
{
public:
    HealthComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual void VPostInit() override;
    virtual TiXmlElement* VGenerateXml() override;

    void AddHealth(uint32 health);
    void SetCurrentHealth(uint32 health);
    int32 GetCurrentHealth() { return m_CurrentHealth; }
    bool HasMaxHealth() { return m_CurrentHealth >= m_MaxHealth; }

private:
    void BroadcastHealthChanged(uint32 oldHealth, uint32 newHealth, bool isInitial = false);

    int32 m_MaxHealth;
    int32 m_CurrentHealth;
};

#endif