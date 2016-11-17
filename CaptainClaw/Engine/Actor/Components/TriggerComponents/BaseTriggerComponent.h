#ifndef __BASETRIGGERCOMPONENT_H__
#define __BASETRIGGERCOMPONENT_H__

#include "../../../SharedDefines.h"
#include "../../ActorComponent.h"

class BaseTriggerComponent : public ActorComponent
{
public:
    BaseTriggerComponent() { }

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;

    // API
    virtual void VOnTriggerEnter(StrongActorPtr pActorWhoEntered) = 0;
    virtual void VOnTriggerLeave(StrongActorPtr pActorWhoLeft) = 0;
    virtual SDL_Rect GetTriggerArea();

private:
    Point m_TriggerSize;
};

#endif