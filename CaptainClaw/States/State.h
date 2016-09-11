#ifndef STATE_H_
#define STATE_H_

#include "../Game.h"

class MenuState;

class State
{
public:
    static MenuState menuState;

    State(Game* game) : _game(game) { };
    virtual ~State() {}
    virtual State* HandleEvent(SDL_Event* event) = 0;
    virtual void Update(uint32_t msDiff) = 0;
    virtual void OnEnter() { };
    virtual void OnExit() { };

protected:
    Game* _game;
};

#endif