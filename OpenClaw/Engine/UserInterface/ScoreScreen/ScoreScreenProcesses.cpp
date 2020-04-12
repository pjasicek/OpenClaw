#include "ScoreScreenProcesses.h"
#include "../../Actor/ActorTemplates.h"
#include "../../Actor/Actor.h"
#include "../../Actor/Components/PositionComponent.h"
#include "../../Actor/Components/RenderComponent.h"

//=================================================================================================
// DelayedProcess implementation
//=================================================================================================

DelayedProcess::DelayedProcess(int delay)
    :
    m_Delay(delay),
    Process()
{

}

void DelayedProcess::VOnUpdate(uint32 msDiff)
{
    m_Delay -= msDiff;
    if (m_Delay <= 0)
    {
        Succeed();
    }
}


//=================================================================================================
// ImageSpawnProcess implementation
//=================================================================================================

ImageSpawnProcess::ImageSpawnProcess(const std::string& imagePath, const Point& position, const AnimationDef& aniDef)
    :
    m_ImagePath(imagePath),
    m_Position(position),
    m_AniDef(aniDef),
    Process()
{

}

void ImageSpawnProcess::VOnUpdate(uint32 msDiff)
{
    SpawnImageActor(m_ImagePath, m_Position, m_AniDef);
    Succeed();
}

//=================================================================================================
// PlaySoundProcess implementation
//=================================================================================================

PlaySoundProcess::PlaySoundProcess(const SoundInfo& sound)
    :
    m_Sound(sound),
    Process()
{
}

void PlaySoundProcess::VOnUpdate(uint32 msDiff)
{
    IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Request_Play_Sound(m_Sound)));
    Succeed();
}

//=================================================================================================
// FireEventProcess implementation
//=================================================================================================

FireEventProcess::FireEventProcess(IEventDataPtr pEvent, bool isTriggered)
    :
    m_pEvent(pEvent),
    m_bIsTriggered(isTriggered),
    Process()
{
}

void FireEventProcess::VOnUpdate(uint32 msDiff)
{
    IEventMgr::Get()->VTriggerEvent(m_pEvent);
    Succeed();
}

//=================================================================================================
// SpawnScoreRowProcess implementation
//=================================================================================================

SpawnScoreRowProcess::SpawnScoreRowProcess(const ScoreRowDef& scoreRowDef)
        :
        m_ScoreRowDef(scoreRowDef),
        m_State(ScoreRowState_None),
        m_TimeSinceLastSpawnedItem(0),
        m_CountOfSpawnedScoreItems(0),
        m_CountOfCollectedSpawnedScoreItems(0),
        m_CurrentScore(0),
        Process()
{
    // Initialize the actor category -> actor list map
    m_ActorCategoryToActorListMap[ScoreRowActorType_Numbers_ItemsPickedUp] = {};
    m_ActorCategoryToActorListMap[ScoreRowActorType_Numbers_ScorePointsFromThisRow] = {};
    m_ActorCategoryToActorListMap[ScoreRowActorType_MovingScoreItems] = {};

    m_ScoreItemDefaultSpeed = 300.0;
}

SpawnScoreRowProcess::~SpawnScoreRowProcess()
{
    for (Actor* pActor : m_ChildrenActorList)
    {
        assert(pActor != NULL);

        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Destroy_Actor(pActor->GetGUID())));
    }

    for (Actor* pActor : m_ActorCategoryToActorListMap[ScoreRowActorType_MovingScoreItems])
    {
        assert(pActor != NULL);

        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Destroy_Actor(pActor->GetGUID())));
    }
}

//-----------------------------------------------------------------------------
// VOnInit()
//-----------------------------------------------------------------------------
void SpawnScoreRowProcess::VOnInit()
{
    Process::VOnInit();

    if (m_State == ScoreRowState_None)
    {
        m_State = ScoreRowState_SpawnInitialScoreItem;
    }
}

//-----------------------------------------------------------------------------
// VOnUpdate() - Handle states as described in header, FSM
//-----------------------------------------------------------------------------
void SpawnScoreRowProcess::VOnUpdate(uint32 msDiff)
{
    assert(m_State != ScoreRowState_None);

    if (m_State == ScoreRowState_SpawnInitialScoreItem)
    {
        //=========================================================================
        // STEP 1) - Spawn initial element
        //=========================================================================

        // Same Y as row and not yet visible, left to the screen
        Point initialPosition(-50, m_ScoreRowDef.rowStartPosition.y);

        StrongActorPtr pInitialScoreItemActor = SpawnImageActor(
            m_ScoreRowDef.scoreItemImagePath,
            initialPosition,
            m_ScoreRowDef.scoreItemAnimationDef);

        m_pInitialScoreItemActor = pInitialScoreItemActor.get();

        m_ChildrenActorList.push_back(pInitialScoreItemActor.get());

        // Change state
        m_State = ScoreRowState_MoveInitialScoreItem;
    }
    else if (m_State == ScoreRowState_MoveInitialScoreItem)
    {
        //=========================================================================
        // STEP 2) - Move initial score item to its destination
        //=========================================================================

        // Only the treasure is present at the moment
        assert(m_ChildrenActorList.size() == 1);
        assert(m_pInitialScoreItemActor != NULL);

        // Update its position
        Point scoreItemSpeed(m_ScoreItemDefaultSpeed, 0);

        Point updatedPos = CalculateUpdatedPosition(
            msDiff,
            scoreItemSpeed,
            m_pInitialScoreItemActor->GetPositionComponent()->GetPosition());

        // Check if it arrived at its destination
        if (updatedPos.x > m_ScoreRowDef.rowStartPosition.x)
        {
            IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Move_Actor(
                m_pInitialScoreItemActor->GetGUID(), 
                m_ScoreRowDef.rowStartPosition)));

            // Change state
            m_State = ScoreRowState_SpawnScoreRowImages;
            return;
        }
        else
        {
            // If not, update its position
            IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Move_Actor(
                m_pInitialScoreItemActor->GetGUID(),
                updatedPos)));
        }
    }
    else if (m_State == ScoreRowState_SpawnScoreRowImages)
    {
        //=========================================================================
        // STEP 3) - Spawn all score row children - numbers, symbols
        //=========================================================================

        SoundInfo sound(m_ScoreRowDef.scoreItemPickupSound);
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Request_Play_Sound(sound)));

        // TODO: Is it okay to just keep the magic constants here ?
        // It is done after the original score screen...

        // This all can be data driven, but I see no point at all, since it is exactly the same
        // in every level's score screen

        //
        // NUMBERS: Total collected items
        //
        {
            Point totalCollectedItemsRowOffset(40, 0);

            AddNumberImageActors(
                0,
                m_ScoreRowDef.countOfPickedUpScoreItems,
                m_ScoreRowDef.rowStartPosition + totalCollectedItemsRowOffset,
                ScoreRowActorType_Numbers_ItemsPickedUp);
        }

        //
        // SYMBOL: "OF" image
        //
        {
            Point ofSymbolRowOffset(111, 4);
            std::string symbolImagePath = "/STATES/BOOTY/IMAGES/SYMBOLS/OF1.PID";

            StrongActorPtr pOfSymbolImageActor = SpawnImageActor(
                symbolImagePath,
                m_ScoreRowDef.rowStartPosition + ofSymbolRowOffset);

            m_ChildrenActorList.push_back(pOfSymbolImageActor.get());
        }

        //
        // NUMBERS: Total count of these items in level
        //
        {
            Point totalMapItemsRowOffset(155, 0);

            AddNumberImageActors(
                m_ScoreRowDef.countOfMapScoreItems,
                m_ScoreRowDef.countOfMapScoreItems,
                m_ScoreRowDef.rowStartPosition + totalMapItemsRowOffset,
                ScoreRowActorType_None);
        }

        //
        // SYMBOL: "X" image
        //
        {
            Point xSymbolRowOffset(220, 4);
            std::string symbolImagePath = "/STATES/BOOTY/IMAGES/SYMBOLS/TIMES2.PID";

            StrongActorPtr pXSymbolImageActor = SpawnImageActor(
                symbolImagePath,
                m_ScoreRowDef.rowStartPosition + xSymbolRowOffset);

            m_ChildrenActorList.push_back(pXSymbolImageActor.get());
        }

        //
        // NUMBERS: Score per score item number
        //
        {
            Point scorePerScoreNumberItemOffset(238, 0);

            AddNumberImageActors(
                m_ScoreRowDef.scoreItemPointsWorth,
                m_ScoreRowDef.scoreItemPointsWorth,
                m_ScoreRowDef.rowStartPosition + scorePerScoreNumberItemOffset,
                ScoreRowActorType_None);
        }

        //
        // SYMBOL: "=" image
        //
        {
            Point equalsSymbolRowOffset(312, 2);
            std::string symbolImagePath = "/STATES/BOOTY/IMAGES/SYMBOLS/EQUALS3.PID";

            StrongActorPtr pEqualsSymbolImageActor = SpawnImageActor(
                symbolImagePath,
                m_ScoreRowDef.rowStartPosition + equalsSymbolRowOffset);

            m_ChildrenActorList.push_back(pEqualsSymbolImageActor.get());
        }

        //
        // NUMBERS: Total score number
        //
        {
            Point totalScoreNumberRowOffset(338, 0);

            AddNumberImageActors(
                0,
                m_ScoreRowDef.countOfPickedUpScoreItems * m_ScoreRowDef.scoreItemPointsWorth,
                m_ScoreRowDef.rowStartPosition + totalScoreNumberRowOffset,
                ScoreRowActorType_Numbers_ScorePointsFromThisRow);
        }

        // Change state
        m_State = ScoreRowState_SpawnCollectedItems;
    }
    else if (m_State == ScoreRowState_SpawnCollectedItems)
    {
        //=========================================================================
        // STEP 4) - Spawn number of collected score items one by one
        //=========================================================================

        // Check if we already spawned all collected score items for this row
        if (m_CountOfSpawnedScoreItems == m_ScoreRowDef.countOfPickedUpScoreItems &&
            m_ActorCategoryToActorListMap[ScoreRowActorType_MovingScoreItems].empty())
        {
            // CHANGE STATE
            m_State = ScoreRowState_FinalizeScoreRowLoading;
            return;
        }

        Point spawnedScoreItemSpeed = CalculateSpawnedScoreItemSpeed();
        int spawnInterval = CalculateScoreItemSpawnInterval();

        UpdateSpawnedScoreItemPositions(msDiff, spawnedScoreItemSpeed);

        // Check if we can spawn a new one
        m_TimeSinceLastSpawnedItem += msDiff;
        if ((m_CountOfSpawnedScoreItems < m_ScoreRowDef.countOfPickedUpScoreItems) &&
            (m_TimeSinceLastSpawnedItem > spawnInterval))
        {
            std::string spawnedImagePath = m_ScoreRowDef.scoreItemImagePath;
            // Check if can choose from multiple images
            if (m_ScoreRowDef.alternativeImagesList.size() > 0)
            {
                spawnedImagePath = Util::GetRandomValueFromVector(m_ScoreRowDef.alternativeImagesList);
            }

            StrongActorPtr pSpawnedItemActor = SpawnImageActor(
                spawnedImagePath,
                m_ScoreRowDef.collectedScoreItemSpawnPosition);

            m_ActorCategoryToActorListMap[ScoreRowActorType_MovingScoreItems].push_back(pSpawnedItemActor.get());
            m_CountOfSpawnedScoreItems++;
            m_TimeSinceLastSpawnedItem = 0;
        }
    }
    else if (m_State == ScoreRowState_FinalizeScoreRowLoading)
    {
        //=========================================================================
        // STEP 5) - Spawn sparkle animation and signalize that this row is loaded
        //=========================================================================

        // TODO: Spawn sparkles

        IEventMgr::Get()->VQueueEvent(IEventDataPtr(new EventData_Finished_Loading_Row()));
        m_State = ScoreRowState_FinishedLoading;
    }
    else if (m_State == ScoreRowState_FinishedLoading)
    {
        //=========================================================================
        // STEP 6) - Do nothing until this row needs to be destroyed
        //=========================================================================
    }
    else
    {
        LOG_ERROR("Unknown state: " + ToStr((int)m_State));
        assert(false && "Unknown state !");
    }
}

//-----------------------------------------------------------------------------
// ForceSpawnImmediately() - Forces row to finish loading
//-----------------------------------------------------------------------------
void SpawnScoreRowProcess::ForceSpawnImmediately()
{
    // Nothing to do if already loaded
    if (m_State == ScoreRowState_FinishedLoading)
    {
        return;
    }

    // Do all the steps
    if (m_State == ScoreRowState_None)
    {
        m_State = ScoreRowState_SpawnInitialScoreItem;
    }
    if (m_State == ScoreRowState_SpawnInitialScoreItem)
    {
        VOnUpdate(0);
        assert(m_State == ScoreRowState_MoveInitialScoreItem);
    }
    if (m_State == ScoreRowState_MoveInitialScoreItem)
    {
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Move_Actor(
            m_pInitialScoreItemActor->GetGUID(),
            m_ScoreRowDef.rowStartPosition)));

        // Change state
        m_State = ScoreRowState_SpawnScoreRowImages;
    }
    if (m_State == ScoreRowState_SpawnScoreRowImages)
    {
        VOnUpdate(0);
        assert(m_State == ScoreRowState_SpawnCollectedItems);
    }
    if (m_State == ScoreRowState_SpawnCollectedItems)
    {
        SetScore(m_ScoreRowDef.countOfPickedUpScoreItems * m_ScoreRowDef.scoreItemPointsWorth);
        SetCollectedSpawnedScoreItems(m_ScoreRowDef.countOfPickedUpScoreItems);

        // If we already spawned some moving items, destroy them
        for (auto iter = m_ActorCategoryToActorListMap[ScoreRowActorType_MovingScoreItems].begin();
            iter != m_ActorCategoryToActorListMap[ScoreRowActorType_MovingScoreItems].end();
            /*iter++*/)
        {
            IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
                new EventData_Destroy_Actor((*iter)->GetGUID())));

            iter = m_ActorCategoryToActorListMap[ScoreRowActorType_MovingScoreItems].erase(iter);
        }

        m_State = ScoreRowState_FinalizeScoreRowLoading;
    }
    if (m_State == ScoreRowState_FinalizeScoreRowLoading)
    {
        VOnUpdate(0);
        assert(m_State == ScoreRowState_FinishedLoading);
    }
    
    m_State = ScoreRowState_FinishedLoading;
}

//=============================================================================
//
// Private methods
//
//=============================================================================

// Calculated new position from current position, speed and elapsed time
Point SpawnScoreRowProcess::CalculateUpdatedPosition(uint32 msDiff, const Point& speed, const Point& currentPosition)
{
    double secondsDiff = (double)msDiff / 1000.0;
    Point positionDiff(speed.x * secondsDiff, speed.y * secondsDiff);

    return currentPosition + positionDiff;
}

//-----------------------------------------------------------------------------
// CalculateSpawnedScoreItemSpeed() - Calculates speed of spawned items
//   the more spawned items, the faster
//-----------------------------------------------------------------------------
Point SpawnScoreRowProcess::CalculateSpawnedScoreItemSpeed()
{
    double xDifference = m_ScoreRowDef.rowStartPosition.x - m_ScoreRowDef.collectedScoreItemSpawnPosition.x;
    double yDifference = m_ScoreRowDef.rowStartPosition.y - m_ScoreRowDef.collectedScoreItemSpawnPosition.y;

    double speedFactor = (double)m_CountOfSpawnedScoreItems / 15.0;
    double speedIncrement = speedFactor * m_ScoreItemDefaultSpeed;

    double speed = m_ScoreItemDefaultSpeed + speedIncrement;

    // Clamp to maximum value
    if (speed > 3 * m_ScoreItemDefaultSpeed)
    {
        speed = 3 * m_ScoreItemDefaultSpeed;
    }

    return Point(speed, speed * (yDifference / xDifference));
}

//-----------------------------------------------------------------------------
// CalculateScoreItemSpawnInterval() - Calculates spawn time of next score item
//   the more past spawned items, the lower the interval is
//-----------------------------------------------------------------------------
int SpawnScoreRowProcess::CalculateScoreItemSpawnInterval()
{
    const int spawnInterval = 250;

    double spawnFactor = (double)m_CountOfSpawnedScoreItems / 15.0;
    double intervalDecrement = spawnFactor * spawnInterval / 2;

    // Clamp to maximum value
    if (intervalDecrement > 175)
    {
        intervalDecrement = 175;
    }

    return spawnInterval - (int)intervalDecrement;
}

void SpawnScoreRowProcess::UpdateSpawnedScoreItemPositions(uint32 msDiff, const Point& speed)
{
    auto findIt = m_ActorCategoryToActorListMap.find(ScoreRowActorType_MovingScoreItems);
    // Has to be present
    assert(findIt != m_ActorCategoryToActorListMap.end());

    ActorList& movingSpawnedScoreItemsList = findIt->second;

    for (auto iter = movingSpawnedScoreItemsList.begin(); iter != movingSpawnedScoreItemsList.end();)
    {
        Actor* pSpawnedElem = (*iter);
        assert(pSpawnedElem != NULL);

        Point updatedPos = CalculateUpdatedPosition(
            msDiff,
            speed,
            pSpawnedElem->GetPositionComponent()->GetPosition());

        // Check if the spawned score item reached its destination
        if (updatedPos.x > (m_ScoreRowDef.rowStartPosition.x - 10))
        {
            // Play the yolo sound
            SoundInfo sound("/STATES/BOOTY/SOUNDS/BOUNCE1.WAV");
            IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Request_Play_Sound(sound)));
            IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Destroy_Actor(pSpawnedElem->GetGUID())));
            iter = movingSpawnedScoreItemsList.erase(iter);

            AddScore(m_ScoreRowDef.scoreItemPointsWorth);
            IncrementCollectedSpawnedScoreItems();
        }
        else
        {
            IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Move_Actor(pSpawnedElem->GetGUID(), updatedPos)));
            ++iter;
        }
    }
}

void SpawnScoreRowProcess::AddNumberImageActors(int numberToDisplay, int futureMaximumNumber, Point position, ScoreRowActorType numberType)
{
    std::string numberStr = ToStr(numberToDisplay);
    std::string maximumNumberStr = ToStr(futureMaximumNumber);
    if (numberStr.length() != maximumNumberStr.length())
    {
        assert(maximumNumberStr.length() > numberStr.length());
        numberStr.reserve(maximumNumberStr.length());
        int zerosToAdd = maximumNumberStr.length() - numberStr.length();
        for (int i = 0; i < zerosToAdd; i++)
        {
            numberStr.insert(0, "0");
        }
    }

    m_ChildrenActorList.reserve(m_ChildrenActorList.size() + numberStr.length());
    for (char charDigit : numberStr)
    {
        int digit = DigitCharToInt(charDigit);
        std::string numberImagePath;

        // If the displayed number will never change, it does not need all loaded digits
        if (numberType == ScoreRowActorType_None)
        {
            numberImagePath = "/STATES/BOOTY/IMAGES/SCORENUMBERS/" +
                Util::ConvertToThreeDigitsString(digit) + ".PID";
        }
        else
        {
            numberImagePath = "/STATES/BOOTY/IMAGES/SCORENUMBERS/*.PID";
        }

        StrongActorPtr pNumberActor = SpawnImageActor(numberImagePath, position);

        m_ChildrenActorList.push_back(pNumberActor.get());

        if (numberType != ScoreRowActorType_None)
        {
            auto findIt = m_ActorCategoryToActorListMap.find(numberType);
            assert(findIt != m_ActorCategoryToActorListMap.end());

            ActorList& dynamicNumberImagesList = findIt->second;
            dynamicNumberImagesList.push_back(pNumberActor.get());
        }

        const Point numberOffset(12, 0);

        position += numberOffset;
    }
}

void SpawnScoreRowProcess::AddScore(int addedScore)
{
    SetScore(m_CurrentScore + addedScore);
}

void SpawnScoreRowProcess::SetScore(int score)
{
    int scoreDiff = score - m_CurrentScore;
    assert(scoreDiff >= 0 && "Setting lesser score ?");

    m_CurrentScore = score;

    auto findIt = m_ActorCategoryToActorListMap.find(ScoreRowActorType_Numbers_ScorePointsFromThisRow);
    assert(findIt != m_ActorCategoryToActorListMap.end());

    ActorList& totalScoreImageNumberList = findIt->second;

    UpdateScoreImageNumbers(m_CurrentScore, totalScoreImageNumberList);

    // Notify everyone who cares about this
    IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_ScoreScreen_Level_Score_Added(scoreDiff)));
}

// This looks pretty much redundant
void SpawnScoreRowProcess::IncrementCollectedSpawnedScoreItems()
{
    m_CountOfCollectedSpawnedScoreItems++;
    SetCollectedSpawnedScoreItems(m_CountOfCollectedSpawnedScoreItems);
}

void SpawnScoreRowProcess::SetCollectedSpawnedScoreItems(int newCount)
{
    m_CountOfCollectedSpawnedScoreItems = newCount;

    auto findIt = m_ActorCategoryToActorListMap.find(ScoreRowActorType_Numbers_ItemsPickedUp);
    assert(findIt != m_ActorCategoryToActorListMap.end());

    ActorList& totalCollectedSpawnedItemsActorList = findIt->second;

    UpdateScoreImageNumbers(m_CountOfCollectedSpawnedScoreItems, totalCollectedSpawnedItemsActorList);
}
