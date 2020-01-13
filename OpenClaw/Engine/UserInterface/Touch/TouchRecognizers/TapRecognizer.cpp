#include <cassert>
#include "TapRecognizer.h"

Touch_Event TapRecognizer::VGetEvent(SDL_FingerID finger) {
    auto it = m_Events.find(finger);
    assert (it != m_Events.end());
    Touch_Event evt{GetId(), TAP, Touch_TapEvent{it->second.first}};
    m_Events.erase(it);
    return evt;
}

RecognizerState TapRecognizer::VGetState(SDL_FingerID finger) {
    auto it = m_Events.find(finger);
    if (it != m_Events.end()) {
        return it->second.second ? RecognizerState::EventReady : RecognizerState::Recognizing;
    }
    return RecognizerState::Failed;
}

void TapRecognizer::VFingerDetached(SDL_FingerID finger) {
    m_Events.erase(finger);
}

RecognizerState TapRecognizer::VOnFingerDown(const SDL_TouchFingerEvent &evt) {
    m_Events.insert(std::make_pair(evt.fingerId, std::make_pair(evt, false)));
    return RecognizerState::Recognizing;
}

RecognizerState TapRecognizer::VOnFingerMotion(const SDL_TouchFingerEvent &evt) {
    auto it = m_Events.find(evt.fingerId);
    if (it != m_Events.end()) {
        return it->second.second ? RecognizerState::EventReady : RecognizerState::Recognizing;
    }
    return RecognizerState::Failed;
}

RecognizerState TapRecognizer::VOnFingerUp(const SDL_TouchFingerEvent &evt) {
    auto it = m_Events.find(evt.fingerId);
    if (it != m_Events.end()) {
        it->second.second = true;
        return RecognizerState::EventReady;
    }
    return RecognizerState::Failed;
}
