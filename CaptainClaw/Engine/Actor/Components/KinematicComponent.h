#ifndef ___KINEMATICCOMPONENT_H__
#define ___KINEMATICCOMPONENT_H__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "../../Events/EventMgr.h"
#include "../../Events/Events.h"
#include <Box2D/Box2D.h>

class PositionComponent;
class KinematicComponent : public ActorComponent
{
public:
    KinematicComponent();
    virtual ~KinematicComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }
    virtual void VPostInit() override;

    virtual void VUpdate(uint32 msDiff) override;

    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;

    Point GetSpeed() { return m_Properties.speed; }
    Point GetMinPosition() { return m_Properties.minPosition; }
    Point GetMaxPosition() { return m_Properties.maxPosition; }
    const ElevatorDef* GetProperties() const { return &m_Properties; }

    void AddCarriedBody(b2Body* pBody);
    void RemoveCarriedBody(b2Body* pBody);

    void OnMoved(Point newPosition);

    void ClawDiedDelegate(IEventDataPtr pEventData);

private:
    // XML Data
    ElevatorDef m_Properties;

    // Internal state
    Point m_Speed;

    bool m_bIsTriggered;
    bool m_bIsDone;

    Point m_InitialPosition;
    Point m_LastPosition;

    Point m_CurrentSpeed;
    Point m_LastSpeed;
    Point m_InitialSpeed;

    // This is to get aroung Box2D's imperfections
    bool m_bCheckCarriedBodies;
    int m_TimeSinceLastCarriedBodiesCheck;

    PositionComponent* m_pPositionComponent;
    shared_ptr<IGamePhysics> m_pPhysics;

    std::vector<b2Body*> m_CarriedBodiesList;
};

#endif
