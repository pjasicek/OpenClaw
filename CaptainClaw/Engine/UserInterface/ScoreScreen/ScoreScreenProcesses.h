#ifndef __SCORE_SCREEN_PROCESSES_H__
#define __SCORE_SCREEN_PROCESSES_H__

#include "ScoreScreenCommon.h"

//=================================================================================================
// Processes
//=================================================================================================

//=================================================================================================
// DelayedProcess
//
//    Purpose: Delay execution of another process by a given amount of time (in miliseconds)
//=================================================================================================

class DelayedProcess : public Process
{
public:
    DelayedProcess(int delay);

    virtual void VOnUpdate(uint32 msDiff) override;

private:
    int m_Delay;
};

//=================================================================================================
// ImageSpawnProcess
//
//    Purpose: Spawn a static image in specified position
//=================================================================================================

class ImageSpawnProcess : public Process
{
public:
    ImageSpawnProcess(const std::string& imagePath, const Point& position, const AnimationDef& aniDef);

    virtual void VOnUpdate(uint32 msDiff) override;

private:
    const std::string m_ImagePath;
    const Point m_Position;
    const AnimationDef m_AniDef;
};

//=================================================================================================
// PlaySoundProcess
//
//    Purpose: Plays a sound
//=================================================================================================

class PlaySoundProcess : public Process
{
public:
    PlaySoundProcess(const SoundInfo& sound);

    virtual void VOnUpdate(uint32 msDiff) override;

private:
    const SoundInfo m_Sound;
};

//=================================================================================================
// FireEventProcess
//
//   Purpose: Triggers or queues specified event
//=================================================================================================

class FireEventProcess : public Process
{
public:
    FireEventProcess(IEventDataPtr pEvent, bool isTriggered);

    virtual void VOnUpdate(uint32 msDiff) override;

private:
    IEventDataPtr m_pEvent;
    bool m_bIsTriggered;
};

//=================================================================================================
// SpawnScoreRowProcess implementation
//
//   Purpose: Spawns a single score row in level finished screen. 
//            Sequence is as follows:
//
//            1) Spawn specified item defining this row outside the screen (Jewelled skull, Coin, Scepter...)
//            2) Move initial score item to its destination
//            3) Spawn whole row with all numbers (How many items of this kind were collected,
//               maximum collectible number of these items in finished level, score points worth,
//               score gained from collecting these items) and symbols (=, X, OF)
//            4) Spawn from the upper left corner number of collected items of this kind and
//               forward them to the item's default location (defined in step (1) )
//            5) Add sparkle animation to the item and fire EventData_ScoreScreen_Finished_Loading_ScoreRow event
//            6) Wait for further request (dissolving or destroying this row)
//=================================================================================================

enum ScoreRowState
{
    ScoreRowState_None,
    ScoreRowState_SpawnInitialScoreItem,    // 1)
    ScoreRowState_MoveInitialScoreItem,     // 2)
    ScoreRowState_SpawnScoreRowImages,      // 3)
    ScoreRowState_SpawnCollectedItems,      // 4)
    ScoreRowState_FinalizeScoreRowLoading,  // 5)
    ScoreRowState_FinishedLoading           // 6)
};

enum ScoreRowActorType
{
    ScoreRowActorType_None,
    ScoreRowActorType_Numbers_ItemsPickedUp,
    ScoreRowActorType_Numbers_ScorePointsFromThisRow,
    ScoreRowActorType_MovingScoreItems
};

typedef std::map<ScoreRowActorType, ActorList> ScoreRowActorListMap;
class SpawnScoreRowProcess : public Process
{
public:
    SpawnScoreRowProcess(const ScoreRowDef& scoreRowDef);
    virtual ~SpawnScoreRowProcess();

    virtual void VOnInit() override;
    virtual void VOnUpdate(uint32 msDiff) override;

    void ForceSpawnImmediately();

private:
    Point CalculateUpdatedPosition(uint32 msDiff, const Point& speed, const Point& currentPosition);
    Point CalculateSpawnedScoreItemSpeed();
    int CalculateScoreItemSpawnInterval();
    void UpdateSpawnedScoreItemPositions(uint32 msDiff, const Point& speed);
    void AddNumberImageActors(int numberToDisplay, int futureMaximumNumber, Point position, ScoreRowActorType numberType);
    void AddScore(int addedScore);
    void SetScore(int newScore);
    void IncrementCollectedSpawnedScoreItems();
    void SetCollectedSpawnedScoreItems(int newCount);

    ScoreRowDef m_ScoreRowDef;

    ScoreRowState m_State;

    // Contains all actors spawned by this score row
    ActorList m_ChildrenActorList;

    // Contains actor groups belonging to defined category - e.g. actors I need to 
    // access and modify later for whatever reason, e.g. spawned items, updating score, etc.
    ScoreRowActorListMap m_ActorCategoryToActorListMap;

    // Initial score item
    Actor* m_pInitialScoreItemActor;

    // Initial and spawned score items X speed in pixels / s (y speed is calculated)
    double m_ScoreItemDefaultSpeed;

    // How long has it been since we spawned one collected item
    int m_TimeSinceLastSpawnedItem;
    // How many collected items we spawned so far (not all of them had to arrive to its destination yet)
    int m_CountOfSpawnedScoreItems;
    // How many collected items ARRIVED at their destination so far
    int m_CountOfCollectedSpawnedScoreItems;
    // Current score of the row calculated from the count of colleted spawned score items and score items score points
    int m_CurrentScore;
};


#endif