#ifndef ___HEALTHCOMPONENT_H__
#define ___HEALTHCOMPONENT_H__

#include "../../ActorComponent.h"
#include "../../../Util/Subject.h"

class HealthObserver;
class HealthSubject : public Subject<HealthObserver>
{
public:
    void NotifyHealthChanged(int32 oldHealth, int32 newHealth, DamageType damageType, Point impactPoint);
    void NotifyHealthBelowZero(DamageType damageType);
};

class HealthObserver
{
public:
    virtual void VOnHealthChanged(int32 oldHealth, int32 newHealth, DamageType damageType, Point impactPoint) { }
    virtual void VOnHealthBelowZero(DamageType damageType) { }
};

class HealthComponent : public ActorComponent, public HealthSubject
{
public:
    HealthComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual void VPostInit() override;
    virtual TiXmlElement* VGenerateXml() override;

    int32 GetHealth() { return m_CurrentHealth; }

    void AddHealth(int32 health, DamageType damageType, Point impactPoint);
    void SetCurrentHealth(int32 health);
    int32 GetCurrentHealth() { return m_CurrentHealth; }
    bool HasMaxHealth() { return m_CurrentHealth >= m_MaxHealth; }
    void SetMaxHealth() { SetCurrentHealth(m_MaxHealth); }

    bool IsInvulnerable() { return m_bInvulnerable; }
    void SetInvulnerable(bool invulnerable) { m_bInvulnerable = invulnerable; }

private:
    void BroadcastHealthChanged(int32 oldHealth, int32 newHealth, DamageType damageType, Point impactPoint, bool isInitial = false);

    bool m_IsController;
    bool m_bInvulnerable;

    int32 m_MaxHealth;
    int32 m_CurrentHealth;
};

#endif
