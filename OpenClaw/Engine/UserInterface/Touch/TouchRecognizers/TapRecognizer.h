#ifndef __TAPRECOGNIZER_H__
#define __TAPRECOGNIZER_H__

#include <map>
#include "AbstractRecognizer.h"

class TapRecognizer : public AbstractRecognizer {
public:
    TapRecognizer(int id, int zIndex) : AbstractRecognizer(id, zIndex) {};

    Touch_Event VGetEvent(SDL_FingerID finger) override;
    RecognizerState VGetState(SDL_FingerID finger) override;
    void VFingerDetached(SDL_FingerID finger) override;

protected:
    RecognizerState VOnFingerDown(const SDL_TouchFingerEvent &evt) override;
    RecognizerState VOnFingerMotion(const SDL_TouchFingerEvent &evt) override;
    RecognizerState VOnFingerUp(const SDL_TouchFingerEvent &evt) override;

    std::map<SDL_FingerID, std::pair<SDL_TouchFingerEvent, bool>> m_Events;
};


#endif
