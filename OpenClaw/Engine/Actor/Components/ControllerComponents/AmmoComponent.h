#ifndef __AMMOCOMPONENT_H__
#define __AMMOCOMPONENT_H__

#include "../../ActorComponent.h"

typedef std::map<AmmoType, uint32> AmmoMap;

class AmmoComponent : public ActorComponent
{
public:
    AmmoComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual void VPostInit() override;
    virtual TiXmlElement* VGenerateXml() override;

    void AddAmmo(AmmoType ammoType, int32 ammoCount);
    void SetAmmo(AmmoType ammoType, int32 ammoCount);
    void SetActiveAmmo(AmmoType activeAmmoType);
    AmmoType GetActiveAmmoType() { return m_ActiveAmmoType; }
    uint32 GetRemainingActiveAmmo() { return m_AmmoMap[m_ActiveAmmoType]; }
    uint32 GetRemainingAmmo(AmmoType ammoType) { return m_AmmoMap[ammoType]; }

    bool CanFire() { return GetRemainingActiveAmmo() > 0; }

    void OnFired() { SetAmmo(m_ActiveAmmoType, --m_AmmoMap[m_ActiveAmmoType]); }

private:
    void BroadcastAmmoChanged(AmmoType ammoType, uint32 ammoCount);

    AmmoType m_ActiveAmmoType;
    AmmoMap m_AmmoMap;
};

#endif
