#ifndef __POWERUPCOMPONENT_H__
#define __POWERUPCOMPONENT_H__

#include "../../ActorComponent.h"

typedef std::map<AmmoType, uint32> AmmoMap;

class PowerupComponent : public ActorComponent
{
public:
    PowerupComponent();
    virtual ~PowerupComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual void VPostInit() override;
    virtual TiXmlElement* VGenerateXml() override;
    virtual void VUpdate(uint32 msDiff) override;

    void ApplyPowerup(PowerupType powerupType, int32 msDuration);
    int32 GetRemainingPowerupTime() { return m_RemainingPowerupTime; }
    bool HasPowerup() { return m_ActivePowerup != PowerupType_None; }
    bool HasPowerup(PowerupType ofType) { return ofType == m_ActivePowerup; }

    void ClawDiedDelegate(IEventDataPtr pEventData);

private:
    void BroadcastPowerupTimeUpdated(uint32 actorId, PowerupType powerupType, int32 secondsRemaining);
    void BroadcastPowerupStatusUpdated(uint32 actorId, PowerupType powerupType, bool isPowerupFinished);

    void SetPowerupSparklesVisibility(bool visible);

    int32 m_RemainingPowerupTime;
    PowerupType m_ActivePowerup;

    std::vector<StrongActorPtr> m_PowerupSparkles;
};

#endif
