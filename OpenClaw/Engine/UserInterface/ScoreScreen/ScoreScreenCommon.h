#ifndef __SCORE_SCREEN_COMMON_H__
#define __SCORE_SCREEN_COMMON_H__

#include "../../SharedDefines.h"
#include "../../Process/ProcessMgr.h"
#include "../../Events/Events.h"
#include "../../Events/EventMgr.h"

//=================================================================================================
// Common structs
//=================================================================================================

struct ScoreRowDef
{
    ScoreRowDef()
    {
        scoreItemImagePath = "";
        scoreItemPickupSound = "";
        scoreItemPointsWorth = 0;
        countOfPickedUpScoreItems = 0;
        countOfMapScoreItems = 0;
    }

    std::string scoreItemImagePath;
    AnimationDef scoreItemAnimationDef;
    std::string scoreItemPickupSound;
    Point rowStartPosition;
    int scoreItemPointsWorth;
    int countOfPickedUpScoreItems;
    int countOfMapScoreItems;
    Point collectedScoreItemSpawnPosition;
    std::vector<std::string> alternativeImagesList;
};

//=================================================================================================
// Events bound to ScoreScreen
//=================================================================================================

class EventData_Finished_Loading_Row : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Finished_Loading_Row() { }

    virtual const EventType& VGetEventType(void) const { return sk_EventType; }
    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Finished_Loading_Row());
    }
    virtual void VSerialize(std::ostringstream& out) const { assert(false && "Cannot be serialized"); }
    virtual void VDeserialize(std::istringstream& in) { assert(false && "Cannot be serialized"); }

    virtual const char* GetName(void) const { return "EventData_Finished_Loading_Row"; }
};

class EventData_ScoreScreen_Level_Score_Added : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_ScoreScreen_Level_Score_Added(int addedScore) : m_AddedScore(addedScore) { }

    virtual const EventType& VGetEventType(void) const { return sk_EventType; }
    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_ScoreScreen_Level_Score_Added(m_AddedScore));
    }
    virtual void VSerialize(std::ostringstream& out) const { assert(false && "Cannot be serialized"); }
    virtual void VDeserialize(std::istringstream& in) { assert(false && "Cannot be serialized"); }

    virtual const char* GetName(void) const { return "EventData_ScoreScreen_Level_Score_Added"; }

    int GetAddedScore() { return m_AddedScore; }

private:
    int m_AddedScore;
};

//=================================================================================================
// Common functions
//=================================================================================================

StrongActorPtr SpawnImageActor(const std::string& imagePath, const Point& position, const AnimationDef& aniDef = AnimationDef());
void UpdateScoreImageNumbers(int newNumber, ActorList& imageNumberActorList);
void AddNumberImageActorsToList(int numberToDisplay, int futureMaximumNumber, Point position, ActorList& toList);
int DigitCharToInt(char c);

#endif