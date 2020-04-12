#ifndef POSITIONCOMPONENT_H_
#define POSITIONCOMPONENT_H_

#include "../../SharedDefines.h"
#include "../ActorComponent.h"

class PositionComponent : public ActorComponent
{
public:
    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;

    // API
    inline Point GetPosition() const { return &m_Position; } 
    inline double GetX() const { return m_Position.x; }
    inline double GetY() const { return m_Position.y; }
    inline void SetPosition(double x, double y) { m_Position.Set(x, y); }
    inline void SetPosition(const Point &newPos) { m_Position = newPos; }
    inline void SetX(double x) { m_Position.x = x; }
    inline void SetY(double y) { m_Position.y = y; }

private:
    Point m_Position;
};

#endif
