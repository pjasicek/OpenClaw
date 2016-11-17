#ifndef POSITIONCOMPONENT_H_
#define POSITIONCOMPONENT_H_

#include "../../SharedDefines.h"
#include "../ActorComponent.h"

class PositionComponent : public ActorComponent
{
public:
    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;

    // API
    Point GetPosition() const { return &m_Position; } 
    double GetX() const { return m_Position.x; }
    double GetY() const { return m_Position.y; }
    void SetPosition(double x, double y) { m_Position.Set(x, y); }
    void SetPosition(Point newPos) { m_Position = newPos; }
    void SetX(double x) { m_Position.x = x; }
    void SetY(double y) { m_Position.y = y; }

private:
    Point m_Position;
};

#endif