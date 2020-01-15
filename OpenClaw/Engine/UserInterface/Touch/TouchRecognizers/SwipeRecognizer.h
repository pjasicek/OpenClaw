#ifndef __SWIPERECOGNIZER_H__
#define __SWIPERECOGNIZER_H__

#include <map>
#include "AbstractRecognizer.h"

class SwipeRecognizer : public AbstractRecognizer {
public:
    SwipeRecognizer(int id, int zIndex,
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
        START_READY,
        STARTED,
        END_READY,
        ENDED
    };

    RecognizerState RecognizerStateByEventState(EventState eventState);

    std::map<SDL_FingerID, std::pair<Touch_SwipeEvent, EventState>> m_Events;

    // Collect new fingers and wait thresholdDistance during thresholdMs
    std::map<SDL_FingerID, SDL_TouchFingerEvent> m_Candidates;
    float m_ThresholdDistance;
    int m_ThresholdTime;
};


#endif
