#ifndef __SCORECOMPONENT_H__
#define __SCORECOMPONENT_H__

#include "../../ActorComponent.h"

class ScoreComponent : public ActorComponent
{
public:
    ScoreComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual void VPostInit() override;
    virtual TiXmlElement* VGenerateXml() override;

    uint32 GetScore() { return m_CurrentScore; }

    void AddScorePoints(uint32 points);
    void SetCurrentScore(uint32 newScore, bool isInitial = false);

private:
    void BroadcastScoreChanged(uint32 oldScore, uint32 newScore, bool isInitial = false);

    uint32 m_CurrentScore;
};

#endif
