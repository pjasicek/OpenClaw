#include <cassert>
#include <SDL2/SDL_timer.h>
#include "SwipeRecognizer.h"
#include "../../../Util/Point.h"

Touch_Event SwipeRecognizer::VGetEvent(SDL_FingerID finger) {
    auto it = m_Events.find(finger);
    assert (it != m_Events.end());
    auto state = it->second.second;
    assert (state == EventState::START_READY || state == EventState::END_READY);
    if (state == EventState::START_READY) {
        it->second.second = EventState::STARTED;
        return Touch_Event{GetId(), SWIPE_START, it->second.first};
    } else /*if (state == EventState::END_READY)*/ {
        it->second.second = EventState::ENDED;
        return Touch_Event{GetId(), SWIPE_END, it->second.first};
    }
}

RecognizerState SwipeRecognizer::VGetState(SDL_FingerID finger) {
    if (m_Candidates.find(finger) != m_Candidates.end()) {
        return RecognizerState::Recognizing;
    }

    auto it = m_Events.find(finger);
    if (it != m_Events.end()) {
        return RecognizerStateByEventState(it->second.second);
    }
    return RecognizerState::Failed;
}

void SwipeRecognizer::VFingerDetached(SDL_FingerID finger) {
    m_Candidates.erase(finger);
    m_Events.erase(finger);
}

RecognizerState SwipeRecognizer::VOnFingerDown(const SDL_TouchFingerEvent &evt) {
    m_Candidates[evt.fingerId] = evt;
    return RecognizerState::Recognizing;
}

RecognizerState SwipeRecognizer::VOnFingerMotion(const SDL_TouchFingerEvent &evt) {
    {
        auto it = m_Candidates.find(evt.fingerId);
        if (it != m_Candidates.end()) {
            const auto &firstEvent = it->second;
            if (evt.timestamp - firstEvent.timestamp > m_ThresholdTime) {
                // time is out
                m_Candidates.erase(it);
                return RecognizerState::Failed;
            } else {
                Point vector{evt.x - firstEvent.x, evt.y - firstEvent.y};
                if (vector.Length() >= m_ThresholdDistance) {
                    m_Events[evt.fingerId] = std::make_pair(
                            Touch_SwipeEvent{firstEvent, (float) vector.x, (float) vector.y}, EventState::START_READY);
                    m_Candidates.erase(it);
                    return RecognizerStateByEventState(EventState::START_READY);
                } else {
                    // time is not out but finger goes small distance
                    return RecognizerState::Recognizing;
                }
            }
        }
    }
    {
        auto it = m_Events.find(evt.fingerId);
        if (it != m_Events.end()) {
            return RecognizerStateByEventState(it->second.second);
        }
    }
    return RecognizerState::Failed;
}

RecognizerState SwipeRecognizer::VOnFingerUp(const SDL_TouchFingerEvent &evt) {
    auto it = m_Events.find(evt.fingerId);
    if (it != m_Events.end()) {
        it->second.second = EventState::END_READY;
        return RecognizerStateByEventState(EventState::END_READY);
    }
    return RecognizerState::Failed;
}

RecognizerState SwipeRecognizer::RecognizerStateByEventState(SwipeRecognizer::EventState eventState) {
    switch (eventState) {
        case EventState::START_READY:
        case EventState::END_READY:
            return RecognizerState::EventReady;
        case EventState::STARTED:
            return RecognizerState::Hold;
        case EventState::ENDED:
            return RecognizerState::Done;
        default:
            assert (false && "Unknown state !");
            return RecognizerState::Failed;
    }
}

void SwipeRecognizer::VOnUpdate() {
    if (m_Candidates.empty()) {
        return;
    }
    for (auto it = m_Candidates.begin(); it != m_Candidates.end();) {
        const auto &firstEvent = it->second;
        if (SDL_GetTicks() - firstEvent.timestamp > m_ThresholdTime) {
            // time is out
            it = m_Candidates.erase(it);
        } else {
            ++it;
        }
    }
}
