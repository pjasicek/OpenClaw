#ifndef __CONVEYOR_BELT_COMPONENT_H__
#define __CONVEYOR_BELT_COMPONENT_H__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"

class ConveyorBeltComponent : public ActorComponent
{
public:
    ConveyorBeltComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;

    virtual void VUpdate(uint32 msDiff) override;

    // Internal
    void OnActorBeginContact(Actor* pActor);
    void OnActorEndContact(Actor* pActor);

private:
    double m_Speed;

    ActorList m_StandingActorsList;
};

#endif //__CONVEYOR_BELT_COMPONENT_H__