#ifndef __EVENTMGR_H__
#define __EVENTMGR_H__

#include <memory>
#include <sstream>
#include <string>

#include <FastDelegate/FastDelegate.h>

#include "../Interfaces.h"

using fastdelegate::MakeDelegate;

//---------------------------------------------------------------------------------------------------------------------
// Forward declaration & typedefs
//---------------------------------------------------------------------------------------------------------------------
class IEventData;

typedef unsigned long EventType;
typedef std::shared_ptr<IEventData> IEventDataPtr;
typedef fastdelegate::FastDelegate1<IEventDataPtr> EventListenerDelegate;

//---------------------------------------------------------------------------------------------------------------------
// Macro for event registration
//---------------------------------------------------------------------------------------------------------------------
extern GenericObjectFactory<IEventData, EventType> g_EventFactory;
#define REGISTER_EVENT(eventClass) g_EventFactory.Register<eventClass>(eventClass::sk_EventType)
#define CREATE_EVENT(eventType) g_EventFactory.Create(eventType)


//---------------------------------------------------------------------------------------------------------------------
// IEventData
// Base type for event object hierarchy, may be used itself for simplest event notifications such as those that do 
// not carry additional payload data. If any event needs to propagate with payload data it must be defined separately.
//---------------------------------------------------------------------------------------------------------------------
class IEventData
{
public:
    virtual ~IEventData(void) {}
    virtual const EventType& VGetEventType(void) const = 0;
    virtual float GetTimeStamp(void) const = 0;
    virtual void VSerialize(std::ostringstream& out) const = 0;
    virtual void VDeserialize(std::istringstream& in) = 0;
    virtual IEventDataPtr VCopy(void) const = 0;
    virtual const char* GetName(void) const = 0;

    //GCC_MEMORY_WATCHER_DECLARATION();
};


//---------------------------------------------------------------------------------------------------------------------
// class BaseEventData
//---------------------------------------------------------------------------------------------------------------------
class BaseEventData : public IEventData
{
public:
    explicit BaseEventData(const float timeStamp = 0.0f) : m_TimeStamp(timeStamp) { }

    // Returns the type of the event
    virtual const EventType& VGetEventType(void) const = 0;

    float GetTimeStamp(void) const { return m_TimeStamp; }

    // Serializing for network input / output
    virtual void VSerialize(std::ostringstream &out) const    { }
    virtual void VDeserialize(std::istringstream& in) { }

private:
    const float m_TimeStamp;
};


//---------------------------------------------------------------------------------------------------------------------
// IEventManager Description
//
// This is the object which maintains the list of registered events and their listeners.
//
// This is a many-to-many relationship, as both one listener can be configured to process multiple event types and 
// of course multiple listeners can be registered to each event type.
//
// The interface to this construct uses smart pointer wrapped objects, the purpose being to ensure that no object 
// that the registry is referring to is destroyed before it is removed from the registry AND to allow for the registry 
// to be the only place where this list is kept ... the application code does not need to maintain a second list.
//
// Simply tearing down the registry (e.g.: destroying it) will automatically clean up all pointed-to objects (so long 
// as there are no other outstanding references, of course).
//---------------------------------------------------------------------------------------------------------------------
class IEventMgr
{
public:

    enum eConstants { kINFINITE = 0xffffffff };

    explicit IEventMgr(const char* pName, bool setAsGlobal);
    virtual ~IEventMgr(void);

    // Registers a delegate function that will get called when the event type is triggered.  Returns true if 
    // successful, false if not.
    virtual bool VAddListener(const EventListenerDelegate& eventDelegate, const EventType& type) = 0;

    // Removes a delegate / event type pairing from the internal tables.  Returns false if the pairing was not found.
    virtual bool VRemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type) = 0;

    // Fire off event NOW.  This bypasses the queue entirely and immediately calls all delegate functions registered 
    // for the event.
    virtual bool VTriggerEvent(const IEventDataPtr& pEvent) const = 0;

    // Fire off event.  This uses the queue and will call the delegate function on the next call to VTick(), assuming
    // there's enough time.
    virtual bool VQueueEvent(const IEventDataPtr& pEvent) = 0;
    virtual bool VThreadSafeQueueEvent(const IEventDataPtr& pEvent) = 0;

    // Find the next-available instance of the named event type and remove it from the processing queue.  This 
    // may be done up to the point that it is actively being processed ...  e.g.: is safe to happen during event
    // processing itself.
    //
    // if allOfType is true, then all events of that type are cleared from the input queue.
    //
    // returns true if the event was found and removed, false otherwise
    virtual bool VAbortEvent(const EventType& type, bool allOfType = false) = 0;

    // Abort all events in queue
    virtual void VAbortAllEvents() = 0;

    // Returns true is the EventMgr is currently processing events
    virtual bool VIsUpdating() = 0;

    // Allow for processing of any queued messages, optionally specify a processing time limit so that the event 
    // processing does not take too long. Note the danger of using this artificial limiter is that all messages 
    // may not in fact get processed.
    //
    // returns true if all messages ready for processing were completed, false otherwise (e.g. timeout )
    virtual bool VUpdate(unsigned long maxMillis = kINFINITE) = 0;

    // Getter for the main global event manager.  This is the event manager that is used by the majority of the 
    // engine, though you are free to define your own as long as you instantiate it with setAsGlobal set to false.
    // It is not valid to have more than one global event manager.
    static IEventMgr* Get(void);

};

#endif
