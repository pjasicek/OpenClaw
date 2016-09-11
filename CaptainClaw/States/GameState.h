#ifndef GAMESTATE_H_
#define GAMESTATE_H_

#include "State.h"

class Level;

class GameState final : public State
{
public:
    GameState(LoadLevelInfo* loadLevelInfo, Game* game);
    ~GameState();

    virtual State* HandleEvent(SDL_Event* event);
    virtual void Update(uint32_t msDiff);

private:
    LoadLevelInfo* _loadLevelInfo;
    Level* _currentLevel;
};

#endif