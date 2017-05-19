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

    virtual bool VAddListener(const EventListenerDelegate& eventDelegate, const EventType& type) override;
    virtual bool VRemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type) override;

    virtual bool VTriggerEvent(const IEventDataPtr& pEvent) const override;
    virtual bool VQueueEvent(const IEventDataPtr& pEvent) override;
    virtual bool VThreadSafeQueueEvent(const IEventDataPtr& pEvent) override;
    virtual bool VAbortEvent(const EventType& type, bool allOfType = false) override;
    virtual void VAbortAllEvents() override;
    virtual bool VIsUpdating() override { return m_bIsUpdating; }

    virtual bool VUpdate(unsigned long maxMilis = kINFINITE) override;

private:
    typedef std::list<EventListenerDelegate> EventListenerList;
    typedef std::map<EventType, EventListenerList> EventListenerMap;
    typedef std::list<IEventDataPtr> EventQueue;

    EventListenerMap m_EventListeners;
    EventQueue m_Queues[EVENTMANAGER_NUM_QUEUES];
    int m_ActiveQueue;  // index of actively processing queue; events enque to the opposing queue
    bool m_bIsUpdating;

    //ThreadSafeEventQueue m_realtimeEventQueue;
};

#endif