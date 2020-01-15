#ifndef __PRESSRECOGNIZER_H__
#define __PRESSRECOGNIZER_H__

#include <map>
#include "AbstractRecognizer.h"

class PressRecognizer : public AbstractRecognizer {
public:
    PressRecognizer(int id, int zIndex) : PressRecognizer(id, zIndex, 0, 0) {};

    PressRecognizer(int id, int zIndex,
                    float thresholdDistance, int thresholdTime) : AbstractRecognizer(id, zIndex),
                                                                  m_ThresholdDistance(thresholdDistance),
                                                                  m_ThresholdTime(thresholdTime) {};

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
        DOWN_READY,
        DOWN,
        UP_READY,
        UP
    };

    RecognizerState RecognizerStateByEventState(EventState eventState);

    std::map<SDL_FingerID, std::pair<Touch_PressEvent, EventState>> m_Events;

    // Collect new fingers and expect thresholdDistance won't be reached during thresholdMs
    std::map<SDL_FingerID, SDL_TouchFingerEvent> m_Candidates;
    float m_ThresholdDistance;
    int m_ThresholdTime;
};


#endif
