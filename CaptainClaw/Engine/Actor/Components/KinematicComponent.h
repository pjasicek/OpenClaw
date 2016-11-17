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
    virtual const char* VGetName() const { return g_Name; }
    virtual void VPostInit() override;

    virtual void VUpdate(uint32 msDiff) override;

    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;

    Point GetSize() { return m_Size; }
    Point GetSpeed() { return m_Speed; }
    Point GetMinPosition() { return m_MinPosition; }
    Point GetMaxPosition() { return m_MaxPosition; }

    void AddCarriedBody(b2Body* pBody);
    void RemoveCarriedBody(b2Body* pBody);

    void OnMoved(Point newPosition);

private:
    // Type
    bool m_IsStartElevator;
    bool m_IsTriggerElevator;
    bool m_IsTriggered;

    Point m_Size;
    Point m_Speed;
    Point m_MinPosition;
    Point m_MaxPosition;

    Point m_CurrentSpeed;

    Point m_LastPosition;

    PositionComponent* m_pPositionComponent;

    shared_ptr<IGamePhysics> m_pPhysics;

    std::vector<b2Body*> m_CarriedBodiesList;
};

#endif