#ifndef __PATH_ELEVATOR_COMPONENT__
#define __PATH_ELEVATOR_COMPONENT__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"

class PathElevatorComponent : public ActorComponent
{
public:
    PathElevatorComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;

    virtual void VUpdate(uint32 msDiff) override;

    virtual TiXmlElement* VGenerateXml() override { assert(false && "Unimplemented"); return false; }

    void AddCarriedBody(b2Body* pBody);
    void RemoveCarriedBody(b2Body* pBody);

    void OnMoved(Point newPosition);

private:
    double CalculateElapsedDistance(const Point& lastPosition, const Point& currentPosition, Direction dir);
    Point CalculateSpeed(double speed, Direction dir);
    void ChangeToNextStep();

    // XML data
    PathElevatorDef m_Properties;

    // Internal properties
    ElevatorStepDef m_CurrentStepDef;
    int m_CurrentStepDefIdx;
    Direction m_CurrentDirection;
    int m_StepTime;
    double m_StepElapsedDistance;
    Point m_LastPosition;
    Point m_CurrentSpeed;

    shared_ptr<IGamePhysics> m_pPhysics;

    std::vector<b2Body*> m_CarriedBodiesList;
};

#endif //__PATH_ELEVATOR_COMPONENT__