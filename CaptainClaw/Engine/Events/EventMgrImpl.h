#ifndef __EVENTMGRIMPL_H__
#define __EVENTMGRIMPL_H__

#include <map>
#include <list>

#include "EventMgr.h"

const unsigned int EVENTMANAGER_NUM_QUEUES = 2;

class EventMgr : public IEventMgr
{
public:
    EventMgr(const char* pName, bool setAsGlobal);
    virtual ~EventMgr();

    virtual bool VAddListener(const EventListenerDelegate& eventDelegate, const EventType& type);
    virtual bool VRemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type);

    virtual bool VTriggerEvent(const IEventDataPtr& pEvent) const;
    virtual bool VQueueEvent(const IEventDataPtr& pEvent);
    virtual bool VThreadSafeQueueEvent(const IEventDataPtr& pEvent);
    virtual bool VAbortEvent(const EventType& type, bool allOfType = false);

    virtual bool VUpdate(unsigned long maxMilis = kINFINITE);

private:
    typedef std::list<EventListenerDelegate> EventListenerList;
    typedef std::map<EventType, EventListenerList> EventListenerMap;
    typedef std::list<IEventDataPtr> EventQueue;

    EventListenerMap m_EventListeners;
    EventQueue m_Queues[EVENTMANAGER_NUM_QUEUES];
    int m_ActiveQueue;  // index of actively processing queue; events enque to the opposing queue

    //ThreadSafeEventQueue m_realtimeEventQueue;
};

#endif