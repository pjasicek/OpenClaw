#include <cassert>
#include <SDL2/SDL_timer.h>
#include "PressRecognizer.h"
#include "../../../Util/Point.h"

Touch_Event PressRecognizer::VGetEvent(SDL_FingerID finger) {
    auto it = m_Events.find(finger);
    assert (it != m_Events.end());
    auto state = it->second.second;
    assert (state == EventState::DOWN_READY || state == EventState::UP_READY);
    if (state == EventState::DOWN_READY) {
        it->second.second = EventState::DOWN;
        return Touch_Event{GetId(), PRESS_START, it->second.first};
    } else /*if (state == EventState::UP_READY)*/ {
        it->second.second = EventState::UP;
        return Touch_Event{GetId(), PRESS_END, it->second.first};
    }
}

RecognizerState PressRecognizer::VGetState(SDL_FingerID finger) {
    if (m_Candidates.find(finger) != m_Candidates.end()) {
        return RecognizerState::Recognizing;
    }

    auto it = m_Events.find(finger);
    if (it != m_Events.end()) {
        return RecognizerStateByEventState(it->second.second);
    }
    return RecognizerState::Failed;
}

void PressRecognizer::VFingerDetached(SDL_FingerID finger) {
    m_Candidates.erase(finger);
    m_Events.erase(finger);
}

RecognizerState PressRecognizer::VOnFingerDown(const SDL_TouchFingerEvent &evt) {
    if (m_ThresholdTime == 0) {
        m_Events[evt.fingerId] = std::make_pair(Touch_PressEvent{evt}, EventState::DOWN_READY);
        return RecognizerStateByEventState(EventState::DOWN_READY);
    } else {
        m_Candidates[evt.fingerId] = evt;
        return RecognizerState::Recognizing;
    }
}

RecognizerState PressRecognizer::VOnFingerMotion(const SDL_TouchFingerEvent &evt) {
    {
        auto it = m_Candidates.find(evt.fingerId);
        if (it != m_Candidates.end()) {
            const auto &firstEvent = it->second;

            Point vector{evt.x - firstEvent.x, evt.y - firstEvent.y};
            if (vector.Length() >= m_ThresholdDistance) {
                m_Candidates.erase(it);
                return RecognizerState::Failed;
            }

            if (evt.timestamp - firstEvent.timestamp >= m_ThresholdTime) {
                // time is out
                m_Events[evt.fingerId] = std::make_pair(Touch_PressEvent{firstEvent}, EventState::DOWN_READY);
                m_Candidates.erase(it);
                return RecognizerStateByEventState(EventState::DOWN_READY);
            }
            return RecognizerState::Recognizing;
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

RecognizerState PressRecognizer::VOnFingerUp(const SDL_TouchFingerEvent &evt) {
    auto it = m_Events.find(evt.fingerId);
    if (it != m_Events.end()) {
        it->second.second = EventState::UP_READY;
        return RecognizerStateByEventState(EventState::UP_READY);
    }
    return RecognizerState::Failed;
}

RecognizerState PressRecognizer::RecognizerStateByEventState(PressRecognizer::EventState eventState) {
    switch (eventState) {
        case EventState::DOWN_READY:
        case EventState::UP_READY:
            return RecognizerState::EventReady;
        case EventState::DOWN:
            return RecognizerState::Hold;
        case EventState::UP:
            return RecognizerState::Done;
        default:
            assert (false && "Unknown state !");
            return RecognizerState::Failed;
    }
}

void PressRecognizer::VOnUpdate() {
    if (m_Candidates.empty()) {
        return;
    }
    for (auto it = m_Candidates.begin(); it != m_Candidates.end();) {
        const auto &fingerId = it->first;
        const auto &firstEvent = it->second;
        if (SDL_GetTicks() - firstEvent.timestamp > m_ThresholdTime) {
            // time is out
            m_Events[fingerId] = std::make_pair(Touch_PressEvent{firstEvent}, EventState::DOWN_READY);
            it = m_Candidates.erase(it);
        } else {
            ++it;
        }
    }
}
