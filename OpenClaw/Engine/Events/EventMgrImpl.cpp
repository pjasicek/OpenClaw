#include "EventMgrImpl.h"

#include "../SharedDefines.h"


//---------------------------------------------------------------------------------------------------------------------
// EventMgr::EventMgr
//---------------------------------------------------------------------------------------------------------------------
EventMgr::EventMgr(const char* pName, bool setAsGlobal)
    : IEventMgr(pName, setAsGlobal)
{
    m_ActiveQueue = 0;
    m_bIsUpdating = false;
}

//---------------------------------------------------------------------------------------------------------------------
// EventMgr::~EventMgr
//---------------------------------------------------------------------------------------------------------------------
EventMgr::~EventMgr()
{
}

//---------------------------------------------------------------------------------------------------------------------
// EventMgr::VAddListener
//---------------------------------------------------------------------------------------------------------------------
bool EventMgr::VAddListener(const EventListenerDelegate& eventDelegate, const EventType& type)
{
    //LOG_TAG("Events", "Attempting to add delegate function for event type: " + ToStr(type, 16));

    EventListenerList& eventListenerList = m_EventListeners[type];  // this will find or create the entry
    for (auto it = eventListenerList.begin(); it != eventListenerList.end(); ++it)
    {
        if (eventDelegate == (*it))
        {
            LOG_WARNING("Attempting to double-register a delegate");
            return false;
        }
    }

    eventListenerList.push_back(eventDelegate);
    //LOG_TAG("Events", "Successfully added delegate for event type: " + ToStr(type, 16));

    return true;
}

//---------------------------------------------------------------------------------------------------------------------
// EventMgr::VRemoveListener
//---------------------------------------------------------------------------------------------------------------------
bool EventMgr::VRemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type)
{
    //LOG_TAG("Events", "Attempting to remove delegate function from event type: " + ToStr(type, 16));
    bool success = false;

    auto findIt = m_EventListeners.find(type);
    if (findIt != m_EventListeners.end())
    {
        EventListenerList& listeners = findIt->second;
        for (auto it = listeners.begin(); it != listeners.end(); ++it)
        {
            if (eventDelegate == (*it))
            {
                listeners.erase(it);
                //LOG_TAG("Events", "Successfully removed delegate function from event type: " + ToStr(type, 16));
                success = true;
                break;  // we don't need to continue because it should be impossible for the same delegate function to be registered for the same event more than once
            }
        }
    }

    return success;
}


//---------------------------------------------------------------------------------------------------------------------
// EventMgr::VTrigger
//---------------------------------------------------------------------------------------------------------------------
bool EventMgr::VTriggerEvent(const IEventDataPtr& pEvent) const
{
    //LOG_TAG("Events", "Attempting to trigger event " + std::string(pEvent->GetName()));
    bool processed = false;

    auto findIt = m_EventListeners.find(pEvent->VGetEventType());
    if (findIt != m_EventListeners.end())
    {
        const EventListenerList& eventListenerList = findIt->second;
        for (EventListenerList::const_iterator it = eventListenerList.begin(); it != eventListenerList.end(); ++it)
        {
            EventListenerDelegate listener = (*it);
            //LOG_TAG("Events", "Sending Event " + std::string(pEvent->GetName()) + " to delegate.");
            listener(pEvent);  // call the delegate
            processed = true;
        }
    }

    return processed;
}


//---------------------------------------------------------------------------------------------------------------------
// EventMgr::VQueueEvent
//---------------------------------------------------------------------------------------------------------------------
bool EventMgr::VQueueEvent(const IEventDataPtr& pEvent)
{
    assert(m_ActiveQueue >= 0);
    assert(m_ActiveQueue < EVENTMANAGER_NUM_QUEUES);

    // make sure the event is valid
    if (!pEvent)
    {
        LOG_ERROR("Invalid event in VQueueEvent()");
        return false;
    }

    //LOG_TAG("Events", "Attempting to queue event: " + std::string(pEvent->GetName()));

    auto findIt = m_EventListeners.find(pEvent->VGetEventType());
    if (findIt != m_EventListeners.end())
    {
        m_Queues[m_ActiveQueue].push_back(pEvent);
        //LOG_TAG("Events", "Successfully queued event: " + std::string(pEvent->GetName()));
        return true;
    }
    else
    {
        //LOG_TAG("Events", "Skipping event since there are no delegates registered to receive it: " + std::string(pEvent->GetName()));
        return false;
    }
}


//---------------------------------------------------------------------------------------------------------------------
// EventMgr::VThreadSafeQueueEvent
//---------------------------------------------------------------------------------------------------------------------
bool EventMgr::VThreadSafeQueueEvent(const IEventDataPtr& pEvent)
{
    //m_RealtimeEventQueue.push(pEvent);
    return true;
}


//---------------------------------------------------------------------------------------------------------------------
// EventMgr::VAbortEvent
//---------------------------------------------------------------------------------------------------------------------
bool EventMgr::VAbortEvent(const EventType& inType, bool allOfType)
{
    assert(m_ActiveQueue >= 0);
    assert(m_ActiveQueue < EVENTMANAGER_NUM_QUEUES);

    bool success = false;
    EventListenerMap::iterator findIt = m_EventListeners.find(inType);

    if (findIt != m_EventListeners.end())
    {
        EventQueue& eventQueue = m_Queues[m_ActiveQueue];
        auto it = eventQueue.begin();
        while (it != eventQueue.end())
        {
            // Removing an item from the queue will invalidate the iterator, so have it point to the next member.  All
            // work inside this loop will be done using thisIt.
            auto thisIt = it;
            ++it;

            if ((*thisIt)->VGetEventType() == inType)
            {
                eventQueue.erase(thisIt);
                success = true;
                if (!allOfType)
                    break;
            }
        }
    }

    return success;
}

//---------------------------------------------------------------------------------------------------------------------
// EventMgr::VAbortAllEvents
//---------------------------------------------------------------------------------------------------------------------
void EventMgr::VAbortAllEvents()
{
    assert(m_ActiveQueue >= 0);
    assert(m_ActiveQueue < EVENTMANAGER_NUM_QUEUES);

    for (EventQueue& queue : m_Queues)
    {
        queue.clear();
    }
}

//---------------------------------------------------------------------------------------------------------------------
// EventMgr::VTick
//---------------------------------------------------------------------------------------------------------------------
bool EventMgr::VUpdate(unsigned long maxMillis)
{
    assert(!m_bIsUpdating && "Attempted to nest updating events - EventMgr::VUpdate inside EventMgr::VUpdate");

    m_bIsUpdating = true;
    unsigned long currMs = SDL_GetTicks();
    unsigned long maxMs = ((maxMillis == IEventMgr::kINFINITE) ? (IEventMgr::kINFINITE) : (currMs + maxMillis));

    // This section added to handle events from other threads.  Check out Chapter 20.
    /*IEventDataPtr pRealtimeEvent;
    while (m_RealtimeEventQueue.try_pop(pRealtimeEvent))
    {
        VQueueEvent(pRealtimeEvent);

        currMs = GetTickCount();
        if (maxMillis != IEventManager::kINFINITE)
        {
            if (currMs >= maxMs)
            {
                GCC_ERROR("A realtime process is spamming the event manager!");
            }
        }
    }*/

    // swap active queues and clear the new queue after the swap
    int queueToProcess = m_ActiveQueue;
    m_ActiveQueue = (m_ActiveQueue + 1) % EVENTMANAGER_NUM_QUEUES;
    m_Queues[m_ActiveQueue].clear();

    //LOG_TAG("EventLoop", "Processing Event Queue " + ToStr(queueToProcess) + "; " + ToStr((unsigned long)m_Queues[queueToProcess].size()) + " events to process");

    // Process the queue
    while (!m_Queues[queueToProcess].empty())
    {
        // pop the front of the queue
        IEventDataPtr pEvent = m_Queues[queueToProcess].front();
        m_Queues[queueToProcess].pop_front();
        //LOG_TAG("EventLoop", "\t\tProcessing Event " + std::string(pEvent->GetName()));

        const EventType& eventType = pEvent->VGetEventType();

        // find all the delegate functions registered for this event
        auto findIt = m_EventListeners.find(eventType);
        if (findIt != m_EventListeners.end())
        {
            const EventListenerList& eventListeners = findIt->second;
            //LOG_TAG("EventLoop", "\t\tFound " + ToStr((unsigned long)eventListeners.size()) + " delegates");

            // call each listener
            for (auto it = eventListeners.begin(); it != eventListeners.end(); ++it)
            {
                EventListenerDelegate listener = (*it);
                //LOG_TAG("EventLoop", "\t\tSending event " + std::string(pEvent->GetName()) + " to delegate");
                listener(pEvent);
            }
        }

        // check to see if time ran out
        currMs = SDL_GetTicks();
        if (maxMillis != IEventMgr::kINFINITE && currMs >= maxMs)
        {
            LOG_TAG("EventLoop", "Aborting event processing; time ran out");
            break;
        }
    }

    // If we couldn't process all of the events, push the remaining events to the new active queue.
    // Note: To preserve sequencing, go back-to-front, inserting them at the head of the active queue
    bool queueFlushed = (m_Queues[queueToProcess].empty());
    if (!queueFlushed)
    {
        while (!m_Queues[queueToProcess].empty())
        {
            IEventDataPtr pEvent = m_Queues[queueToProcess].back();
            m_Queues[queueToProcess].pop_back();
            m_Queues[m_ActiveQueue].push_front(pEvent);
        }
    }

    m_bIsUpdating = false;

    return queueFlushed;
}

