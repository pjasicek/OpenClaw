#include <cassert>
#include <SDL2/SDL_timer.h>
#include "PressRecognizer.h"
#include "../../../Util/Point.h"

Touch_Event PressRecognizer::VGetEvent(SDL_FingerID finger) {
    auto it = m_Events.find(finger);
    assert (it != m_Events.end());
    auto state = it->second.second;
    assert (state == EventState::DOWN_READY || state == EventState::UP_READY);
    const auto &sdlEvent = it->second.first;
    if (state == EventState::DOWN_READY) {
        Touch_Event evt{GetId(), PRESS_START, Touch_PressEvent{sdlEvent}};
        it->second.second = EventState::DOWN;
        return evt;
    } else /*if (state == EventState::UP_READY)*/ {
        Touch_Event evt{GetId(), PRESS_END, Touch_PressEvent{sdlEvent}};
        it->second.second = EventState::UP;
        return evt;
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
    if (thresholdMs == 0) {
        m_Events.insert(std::make_pair(evt.fingerId, std::make_pair(evt, EventState::DOWN_READY)));
        return RecognizerStateByEventState(EventState::DOWN_READY);
    } else {
        m_Candidates.insert(std::make_pair(evt.fingerId, evt));
        return RecognizerState::Recognizing;
    }
}

RecognizerState PressRecognizer::VOnFingerMotion(const SDL_TouchFingerEvent &evt) {
    {
        auto it = m_Candidates.find(evt.fingerId);
        if (it != m_Candidates.end()) {
            const auto &firstEvent = it->second;

            Point vector{evt.x - firstEvent.x, evt.y - firstEvent.y};
            if (vector.Length() >= thresholdDistance) {
                m_Candidates.erase(it);
                return RecognizerState::Failed;
            }

            if (evt.timestamp - firstEvent.timestamp >= thresholdMs) {
                // time is out
                m_Events.insert(std::make_pair(evt.fingerId, std::make_pair(firstEvent, EventState::DOWN_READY)));
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
    for (auto it = m_Candidates.begin(); it != m_Candidates.end();) {
        const auto &fingerId = it->first;
        const auto &firstEvent = it->second;
        if (SDL_GetTicks() - firstEvent.timestamp > thresholdMs) {
            // time is out
            m_Events.insert(std::make_pair(fingerId, std::make_pair(firstEvent, EventState::DOWN_READY)));
            it = m_Candidates.erase(it);
        } else {
            ++it;
        }
    }
}
