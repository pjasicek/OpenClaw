#ifndef __PICKUPCOMPONENT_H__
#define __PICKUPCOMPONENT_H__

#include "../../ActorComponent.h"
#include "../TriggerComponents/TriggerComponent.h"

//=====================================================================================================================
// PickupComponent - base class for derived pickup components
//=====================================================================================================================

class PositionComponent;
class ActorRenderComponent;
class PickupComponent : public ActorComponent, public TriggerObserver
{
public:
    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual void VPostInit() override;
    virtual TiXmlElement* VGenerateXml() override;

    virtual bool VOnApply(Actor* pActorWhoPickedThis) = 0;
    virtual void VOnActorEnteredTrigger(Actor* pActorWhoEntered, FixtureType triggerType) override;

    PickupType GetPickupType() { return m_PickupType; }

protected:
    virtual bool VDelegateInit(TiXmlElement* data) { return true; }
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement) = 0;

    PickupType m_PickupType;
    std::string m_PickupSound;
};

//=====================================================================================================================
// TreasurePickupComponent
//=====================================================================================================================

class TreasurePickupComponent : public PickupComponent
{
public:
    TreasurePickupComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }
    virtual void VUpdate(uint32 msDiff) override;
    virtual void VPostInit() override;

    virtual bool VOnApply(Actor* pActorWhoPickedThis) override;

protected:
    virtual bool VDelegateInit(TiXmlElement* data) override;
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement) override;

private:
    int32 m_ScorePoints;
    bool m_IsPickedUp;

    shared_ptr<PositionComponent> m_pPositionComponent;
    shared_ptr<ActorRenderComponent> m_pRenderComponent;
};

//=====================================================================================================================
// LifePickupComponent
//=====================================================================================================================

class LifePickupComponent : public PickupComponent
{
public:
    LifePickupComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VOnApply(Actor* pActorWhoPickedThis);

protected:
    virtual bool VDelegateInit(TiXmlElement* data);
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement);

private:
    int32 m_NumLives;
};

//=====================================================================================================================
// HealthPickupComponent
//=====================================================================================================================

class HealthPickupComponent : public PickupComponent
{
public:
    HealthPickupComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VOnApply(Actor* pActorWhoPickedThis);

protected:
    virtual bool VDelegateInit(TiXmlElement* data);
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement);

private:
    int32 m_NumRestoredHealth;
};

//=====================================================================================================================
// TeleportPickupComponent
//=====================================================================================================================

class TeleportPickupComponent : public PickupComponent
{
public:
    TeleportPickupComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VOnApply(Actor* pActorWhoPickedThis);

protected:
    virtual bool VDelegateInit(TiXmlElement* data);
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement);

private:

    bool m_bIsBossWarp;
    Point m_Destination;
};

//=====================================================================================================================
// PowerupPickupComponent
//=====================================================================================================================

class PowerupPickupComponent : public PickupComponent
{
public:
    PowerupPickupComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VOnApply(Actor* pActorWhoPickedThis);

protected:
    virtual bool VDelegateInit(TiXmlElement* data);
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement);

private:
    PowerupType m_PowerupType;
    int32 m_PowerupDuration;
};

//=====================================================================================================================
// AmmoPickupComponent
//=====================================================================================================================

typedef std::vector<std::pair<AmmoType, int>> AmmoList;

class AmmoPickupComponent : public PickupComponent
{
public:
    AmmoPickupComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VOnApply(Actor* pActorWhoPickedThis);

protected:
    virtual bool VDelegateInit(TiXmlElement* data);
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement);

private:
    AmmoList m_AmmoPickupList;
};

//=====================================================================================================================
// EndLevelPickupComponent
//=====================================================================================================================

class EndLevelPickupComponent : public PickupComponent
{
public:
    EndLevelPickupComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VOnApply(Actor* pActorWhoPickedThis);

protected:
    virtual bool VDelegateInit(TiXmlElement* data);
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement);
};

#endif
