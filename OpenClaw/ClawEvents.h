#ifndef __CLAWEVENTS_H__
#define __CLAWEVENTS_H__

#include "Engine/Events/Events.h"

//---------------------------------------------------------------------------------------------------------------------
// EventData_Request_Move_Actor - sent when 
//---------------------------------------------------------------------------------------------------------------------
class EventData_Actor_Start_Move : public BaseEventData
{
public:
    static const EventType sk_EventType;

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    EventData_Actor_Start_Move(void)
    {
        m_Id = INVALID_ACTOR_ID;
    }

    EventData_Actor_Start_Move(uint32_t id, const Point& move)
        : m_Id(id), m_Move(move)
    {
        //
    }

    virtual void VSerialize(std::ostringstream &out) const
    {
        out << m_Id << " ";
        out << m_Move.x << " ";
        out << m_Move.y << " ";
    }

    virtual void VDeserialize(std::istringstream& in)
    {
        in >> m_Id;
        in >> m_Move.x;
        in >> m_Move.y;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Actor_Start_Move(m_Id, m_Move));
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Request_Move_Actor";
    }

    uint32_t GetActorId(void) const
    {
        return m_Id;
    }

    Point GetMove(void) const
    {
        return m_Move;
    }

private:
    uint32_t m_Id;
    Point m_Move;
};

#endif
