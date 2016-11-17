#ifndef STATE_H_
#define STATE_H_

#include "../GameApplication.h"

class MenuState;

class IState
{
public:
    static MenuState menuState;

    virtual ~IState() {}
    virtual IState* HandleEvent(SDL_Event* event) = 0;
    virtual void Update(uint32_t msDiff) = 0;
    virtual void OnEnter() { };
    virtual void OnExit() { };
};

#endif