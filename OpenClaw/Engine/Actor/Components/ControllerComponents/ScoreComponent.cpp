#include "ScoreComponent.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"
#include "../../Actor.h"

const char* ScoreComponent::g_Name = "ScoreComponent";

ScoreComponent::ScoreComponent()
    :
    m_CurrentScore(0)
{ }

bool ScoreComponent::VInit(TiXmlElement* pData)
{
    if (TiXmlElement* pElem = pData->FirstChildElement("Score"))
    {
        m_CurrentScore = std::stoi(pElem->GetText());
    }

    return true;
}

void ScoreComponent::VPostInit()
{
    BroadcastScoreChanged(0, m_CurrentScore, true);
}

TiXmlElement* ScoreComponent::VGenerateXml()
{
    // TODO:
    return NULL;
}

void ScoreComponent::AddScorePoints(uint32 points)
{
    uint32 oldScore = m_CurrentScore;
    m_CurrentScore += points;

    BroadcastScoreChanged(oldScore, m_CurrentScore);
}

void ScoreComponent::SetCurrentScore(uint32 newScore, bool isInitial)
{
    uint32 oldScore = m_CurrentScore;
    m_CurrentScore = newScore;

    BroadcastScoreChanged(oldScore, m_CurrentScore, isInitial);
}

void ScoreComponent::BroadcastScoreChanged(uint32 oldScore, uint32 newScore, bool isInitial)
{
    shared_ptr<EventData_Updated_Score> pEvent(new EventData_Updated_Score(m_pOwner->GetGUID(), oldScore, newScore, isInitial));
    IEventMgr::Get()->VQueueEvent(pEvent);
}