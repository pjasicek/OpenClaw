#include "PredefinedMoveComponent.h"
#include "../Actor.h"

#include "PositionComponent.h"

#include "../../Events/EventMgr.h"
#include "../../Events/Events.h"

const char* PredefinedMoveComponent::g_Name = "PredefinedMoveComponent";

bool PredefinedMoveComponent::VInit(TiXmlElement* data)
{
    assert(data != NULL);

    m_CurrMoveIdx = 0;
    m_CurrMoveTime = 0;

    for (TiXmlElement* pPredefinedMoveElem = data->FirstChildElement("PredefinedMove");
        pPredefinedMoveElem != NULL; pPredefinedMoveElem = pPredefinedMoveElem->NextSiblingElement("PredefinedMove"))
    {
        PredefinedMove move;
        if (TiXmlElement* pElem = pPredefinedMoveElem->FirstChildElement("DurationMiliseconds"))
        {
            move.msDuration = std::stoi(pElem->GetText());
        }
        if (TiXmlElement* pElem = pPredefinedMoveElem->FirstChildElement("PixelsPerSecond"))
        {
            pElem->Attribute("x", &move.pixelsPerSecond.x);
            pElem->Attribute("y", &move.pixelsPerSecond.y);
        }
        if (TiXmlElement* pElem = pPredefinedMoveElem->FirstChildElement("Sound"))
        {
            move.soundToPlay = pElem->GetText();
        }

        m_PredefinedMoves.push_back(move);
    }
    
    if (TiXmlElement* pElem = data->FirstChildElement("IsInfinite"))
    {
        m_bIsInfinite = std::string(pElem->GetText()) == "true";
    }

    assert(m_PredefinedMoves.size() > 0 && "PredefinedMoveComponent had no moves defined");

    return true;
}

TiXmlElement* PredefinedMoveComponent::VGenerateXml()
{
    return NULL;
}

void PredefinedMoveComponent::VPostInit()
{
    m_pPositonComponent = m_pOwner->GetPositionComponent().get();
    assert(m_pPositonComponent && "Cannot have PredefinedMoveComponent without PositionComponent");
}

void PredefinedMoveComponent::VUpdate(uint32 msDiff)
{
    // If there are no more cycles to loop through, popup is at end
    if (!m_bIsInfinite && m_CurrMoveIdx >= m_PredefinedMoves.size())
    {
        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(m_pOwner->GetGUID()));
        IEventMgr::Get()->VQueueEvent(pEvent);
    }
    else
    {
        // Update position with regards to current direction and speed
        Point currentPos = m_pPositonComponent->GetPosition();
        Point moveDelta = Point(
            m_PredefinedMoves[m_CurrMoveIdx].pixelsPerSecond.x * (double)(msDiff / 1000.0),
            m_PredefinedMoves[m_CurrMoveIdx].pixelsPerSecond.y * (double)(msDiff / 1000.0));

        m_pPositonComponent->SetPosition(currentPos + moveDelta);

        shared_ptr<EventData_Move_Actor> pEvent(new EventData_Move_Actor(m_pOwner->GetGUID(), m_pPositonComponent->GetPosition()));
        IEventMgr::Get()->VTriggerEvent(pEvent);

        m_CurrMoveTime += msDiff;
        if (m_CurrMoveTime >= m_PredefinedMoves[0].msDuration)
        {
            m_CurrMoveIdx++;
            m_CurrMoveTime = 0;
        }
    }
}