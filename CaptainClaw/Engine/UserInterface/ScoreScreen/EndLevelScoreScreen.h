#ifndef __END_LEVEL_SCORE_SCREEN_H__
#define __END_LEVEL_SCORE_SCREEN_H__

#include "ScoreScreenCommon.h"

#include "../../Scene/Scene.h"
#include "../../SharedDefines.h"

#include <SDL2/SDL.h>

class Image;
class LevelData;

enum ScoreScreenState
{
    ScoreScreenState_None,
    ScoreScreenState_Intro,
    ScoreScreenState_SpawningScoreRows,
    ScoreScreenState_Finished,
    ScoreScreenState_Exiting,
};

// This encapsulates score screen background and its children
class ScreenElementScoreScreen : public IScreenElement, public Scene
{
public:
    ScreenElementScoreScreen(SDL_Renderer* pRenderer);
    virtual ~ScreenElementScoreScreen();

    // IScreenElement implementation
    virtual void VOnLostDevice() { }
    virtual void VOnUpdate(uint32 msDiff);
    virtual void VOnRender(uint32 msDiff);

    virtual int32 VGetZOrder() const { return 0; }
    virtual void VSetZOrder(int32 const zOrder) { }
    virtual bool VIsVisible() { return true; }
    virtual void VSetVisible(bool visible) { }

    virtual bool VOnEvent(SDL_Event& evt);

    bool Initialize(TiXmlElement* pScoreScreenRootElem);

private:
    // Delegates
    void FinishedLoadingRowDelegate(IEventDataPtr pEventData);
    void ScoreScreenFinishedIntroDelegate(IEventDataPtr pEventData);
    void ScoreScreenLevelScoreAddedDelegate(IEventDataPtr pEventData);

    void LoadNextLevel();
    void ForceNextState();
    void QueueDelayedProcess(StrongProcessPtr pProcess, int delay);
    void TestSpawnEverything();
    
    ScoreScreenState m_State;

    SDL_Renderer* m_pRenderer;
    Point m_OriginalScale;

    shared_ptr<Image> m_pBackground;
    shared_ptr<Image> m_pScoreBackgroundImage;

    std::vector<ScoreRowDef> m_ScoreRowList;

    int m_DisplayedLevelScore;
    int m_DisplayedGameScore;

    ActorList m_LevelScoreNumbersList;
    ActorList m_GameScoreNumbersList;

    int m_NextLevelNumber;
    int m_ScorePointsOnLevelStart;
    int m_ScorePointsCollectedInLevel;

    ProcessMgr* m_pProcessMgr;
    std::vector<StrongProcessPtr> m_SpawnRowProcessList;

    // Internal states
    bool m_bInScoreScreen;
};

#endif