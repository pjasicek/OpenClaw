#include <cassert>
#include <SDL2/SDL_timer.h>
#include "JoystickRecognizer.h"
#include "../../../Util/Point.h"

Touch_Event JoystickRecognizer::VGetEvent(SDL_FingerID finger) {
    auto it = m_Events.find(finger);
    assert (it != m_Events.end());
    auto state = it->second.second;
    assert (state == EventState::MOVE_READY || state == EventState::RESET_READY);
    if (state == EventState::MOVE_READY) {
        auto firstEventIt = m_FirstTouches.find(finger);
        assert (firstEventIt != m_FirstTouches.end());
        auto &currentEvent = it->second.first;

        Touch_Event evt{GetId(), JOYSTICK_MOVE, Touch_JoystickEvent{firstEventIt->second, currentEvent}};
        it->second.second = EventState::MOVED;
        return evt;
    } else /*if (state == EventState::RESET_READY)*/ {
        Touch_Event evt{GetId(), JOYSTICK_RESET, Touch_JoystickEvent{it->second.first}};
        it->second.second = EventState::RESETED;
        return evt;
    }

}

RecognizerState JoystickRecognizer::VGetState(SDL_FingerID finger) {
    if (m_Candidates.find(finger) != m_Candidates.end()) {
        return RecognizerState::Recognizing;
    }

    auto it = m_Events.find(finger);
    if (it != m_Events.end()) {
        return RecognizerStateByEventState(it->second.second);
    }
    return RecognizerState::Failed;
}

void JoystickRecognizer::VFingerDetached(SDL_FingerID finger) {
    m_Candidates.erase(finger);
    m_FirstTouches.erase(finger);
    m_Events.erase(finger);
}

RecognizerState JoystickRecognizer::VOnFingerDown(const SDL_TouchFingerEvent &evt) {
    if (thresholdMs == 0 || thresholdDistance == 0) {
        m_FirstTouches.insert(std::make_pair(evt.fingerId, evt));
        m_Events.insert(std::make_pair(evt.fingerId, std::make_pair(evt, EventState::MOVE_READY)));
        return RecognizerStateByEventState(EventState::MOVE_READY);
    } else {
        m_Candidates.insert(std::make_pair(evt.fingerId, evt));
        return RecognizerState::Recognizing;
    }
}

RecognizerState JoystickRecognizer::VOnFingerMotion(const SDL_TouchFingerEvent &evt) {
    {
        auto it = m_Candidates.find(evt.fingerId);
        if (it != m_Candidates.end()) {
            const auto &firstEvent = it->second;
            if (evt.timestamp - firstEvent.timestamp > thresholdMs) {
                // time is out
                m_Candidates.erase(it);
                return RecognizerState::Failed;
            } else {
                Point vector{evt.x - firstEvent.x, evt.y - firstEvent.y};
                if (vector.Length() >= thresholdDistance) {
                    m_FirstTouches.insert(std::make_pair(evt.fingerId, firstEvent));
                    m_Events.insert(std::make_pair(evt.fingerId, std::make_pair(evt, EventState::MOVE_READY)));
                    m_Candidates.erase(it);
                    return RecognizerStateByEventState(EventState::MOVE_READY);
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
            it->second.first = evt;
            it->second.second = EventState::MOVE_READY;
            return RecognizerStateByEventState(EventState::MOVE_READY);
        }
    }
    return RecognizerState::Failed;
}

RecognizerState JoystickRecognizer::VOnFingerUp(const SDL_TouchFingerEvent &evt) {
    auto it = m_Events.find(evt.fingerId);
    if (it != m_Events.end()) {
        it->second.first = evt;
        it->second.second = EventState::RESET_READY;
        return RecognizerStateByEventState(EventState::RESET_READY);
    }
    return RecognizerState::Failed;
}

void JoystickRecognizer::VOnUpdate() {
    for (auto it = m_Candidates.begin(); it != m_Candidates.end();) {
        const auto &firstEvent = it->second;
        if (SDL_GetTicks() - firstEvent.timestamp > thresholdMs) {
            // time is out
            it = m_Candidates.erase(it);
        } else {
            ++it;
        }
    }
}

RecognizerState JoystickRecognizer::RecognizerStateByEventState(JoystickRecognizer::EventState eventState) {
    switch (eventState) {
        case EventState::MOVE_READY:
        case EventState::RESET_READY:
            return RecognizerState::EventReady;
        case EventState::MOVED:
            return RecognizerState::Hold;
        case EventState::RESETED:
            return RecognizerState::Done;
        default:
            assert (false && "Unknown state !");
            return RecognizerState::Failed;
    }
}
