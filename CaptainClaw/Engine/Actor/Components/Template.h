#ifndef concreteCOMPONENT_H_
#define concreteCOMPONENT_H_

#include "../../SharedDefines.h"
#include "../ActorComponent.h"

class ConcreteComponent : public ActorComponent
{
public:
    ConcreteComponent() { }

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual TiXmlElement* VGenerateXml() override;

private:

};

#endif