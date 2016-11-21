#ifndef __PICKUPCOMPONENT_H__
#define __PICKUPCOMPONENT_H__

#include "../../ActorComponent.h"
#include "../TriggerComponents/TriggerComponent.h"

class PickupComponent : public ActorComponent, public TriggerObserver
{
public:
    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual void VPostInit() override;
    virtual TiXmlElement* VGenerateXml() override;

    virtual bool VOnApply(Actor* pActorWhoPickedThis) = 0;
    virtual void VOnActorEnteredTrigger(Actor* pActorWhoEntered) override;

protected:
    virtual bool VDelegateInit(TiXmlElement* data) { return true; }
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement) = 0;
};

class TreasurePickupComponent : public PickupComponent
{
public:
    TreasurePickupComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }
    virtual void VUpdate(uint32 msDiff);

    virtual bool VOnApply(Actor* pActorWhoPickedThis);

protected:
    virtual bool VDelegateInit(TiXmlElement* data);
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement);

private:
    int32 m_ScorePoints;
    bool m_IsPickedUp;
};

#endif