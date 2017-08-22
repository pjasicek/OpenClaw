#ifndef __LIFECOMPONENT_H__
#define __LIFECOMPONENT_H__

#include "../../ActorComponent.h"

class LifeComponent : public ActorComponent
{
public:
    LifeComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual void VPostInit() override;
    virtual TiXmlElement* VGenerateXml() override;

    int GetLives() { return m_CurrentLives; }

    void AddLives(uint32 numLives);
    void SetCurrentLives(uint32 numLives);

private:
    void BroadcastLivesChanged(uint32 oldLives, uint32 newLives, bool isInitial = false);

    int m_CurrentLives;
};

#endif
