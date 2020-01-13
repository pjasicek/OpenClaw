#ifndef __JOYSTICKRECOGNIZER_H__
#define __JOYSTICKRECOGNIZER_H__

#include <map>
#include "AbstractRecognizer.h"

class JoystickRecognizer : public AbstractRecognizer {
public:
    JoystickRecognizer(int id, int zIndex,
                       float thresholdDistance, int thresholdMs) : AbstractRecognizer(id, zIndex),
                                                                   thresholdDistance(thresholdDistance),
                                                                   thresholdMs(thresholdMs) {};

    Touch_Event VGetEvent(SDL_FingerID finger) override;
    RecognizerState VGetState(SDL_FingerID finger) override;
    void VFingerDetached(SDL_FingerID finger) override;

    void VOnUpdate() override;

protected:
    RecognizerState VOnFingerDown(const SDL_TouchFingerEvent &evt) override;
    RecognizerState VOnFingerMotion(const SDL_TouchFingerEvent &evt) override;
    RecognizerState VOnFingerUp(const SDL_TouchFingerEvent &evt) override;

private:
    enum class EventState {
        MOVE_READY,
        MOVED,
        RESET_READY,
        RESETED
    };

    RecognizerState RecognizerStateByEventState(EventState eventState);

    std::map<SDL_FingerID, SDL_TouchFingerEvent> m_FirstTouches;
    std::map<SDL_FingerID, std::pair<SDL_TouchFingerEvent, EventState>> m_Events;

    // Collect new fingers and wait thresholdDistance during thresholdMs
    std::map<SDL_FingerID, SDL_TouchFingerEvent> m_Candidates;
    float thresholdDistance;
    int thresholdMs;
};


#endif
