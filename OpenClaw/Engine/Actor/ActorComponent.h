#ifndef ACTORCOMPONENT_H_
#define ACTORCOMPONENT_H_

#include <memory>
#include <stdint.h>
#include <functional>

#include "../Util/StringUtil.h"

#include "ActorTemplates.h"

#include "../SharedDefines.h"
#include "ActorFactory.h"

class ActorComponent
{
    friend class ActorFactory;

public:
    virtual ~ActorComponent() { m_pOwner.reset(); }

    // TODO: Implement since this should be used throughout the whole codebase lol
    // void Destroy();

    // These functions are meant to be overriden by the implementation classes of the components
    virtual bool VInit(TiXmlElement* data) = 0;
    virtual void VPostInit() { }
    virtual void VPostPostInit() { }
    virtual void VUpdate(uint32 msDiff) { }
    virtual void VOnChanged() { }

    // For potential editor
    virtual TiXmlElement* VGenerateXml() { return NULL; }

    // This function has to be be overriden by the interface class
    virtual const char* VGetName() const = 0;

    virtual uint32 VGetId() const { return GetIdFromName(VGetName()); }
    static uint32 GetIdFromName(const char* componentName)
    {
        return HashName(componentName);
    }

    virtual void VOnWorldFinishedLoading() { }

protected:
    StrongActorPtr m_pOwner;

private:
    void SetOwner(StrongActorPtr pOwner) { m_pOwner = pOwner; }
};

#endif
