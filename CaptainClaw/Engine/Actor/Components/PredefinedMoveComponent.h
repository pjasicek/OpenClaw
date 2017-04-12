#ifndef __PREDEFINED_MOVE_COMPONENT__
#define __PREDEFINED_MOVE_COMPONENT__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"

class PositionComponent;
class PredefinedMoveComponent : public ActorComponent
{
public:
    PredefinedMoveComponent() { }

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;
    virtual TiXmlElement* VGenerateXml() override;

    virtual void VUpdate(uint32 msDiff) override;

private:
    std::vector<PredefinedMove> m_PredefinedMoves;
    uint32 m_CurrMoveIdx;
    uint32 m_CurrMoveTime;
    bool m_bIsInfinite;

    PositionComponent* m_pPositonComponent;
};

#endif