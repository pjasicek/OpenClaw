#ifndef ACTORCOMPONENT_H_
#define ACTORCOMPONENT_H_

#include <memory>
#include <stdint.h>
#include <functional>

#include "Actor.h"
#include "ActorFactory.h"
#include "../Util/StringUtil.h"

#include "../SharedDefines.h"

class ActorComponent
{
    friend class ActorFactory;

public:
    virtual ~ActorComponent() { _owner.reset(); }

    // These functions are meant to be overriden by the implementation classes of the components
    virtual bool VInit(TiXmlElement* data) = 0;
    virtual void VPostInit() { }
    virtual void VUpdate(uint32 msDiff) { }
    virtual void VOnChanged() { }

    // For potential editor
    virtual TiXmlElement* VGenerateXml() = 0;

    // This function has to be be overriden by the interface class
    virtual const char* VGetName() const = 0;

    virtual uint32 VGetId() const { return GetIdFromName(VGetName()); }
    static uint32 GetIdFromName(const char* componentName)
    {
        void* rawId = HashName(componentName);
        return reinterpret_cast<uint32>(rawId);
    }

protected:
    StrongActorPtr _owner;

private:
    void SetOwner(StrongActorPtr owner) { _owner = owner; }
};

#endif