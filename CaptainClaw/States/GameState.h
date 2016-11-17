#ifndef GAMESTATE_H_
#define GAMESTATE_H_

#include "State.h"

class Level;

class GameState final : public IState
{
public:
    GameState(LoadLevelInfo* loadLevelInfo);
    ~GameState();

    virtual IState* HandleEvent(SDL_Event* event);
    virtual void Update(uint32_t msDiff);

private:
    double CalculateMoveStep(uint32_t msDiff);

    LoadLevelInfo* _loadLevelInfo;
    Level* _currentLevel;
};

#endif