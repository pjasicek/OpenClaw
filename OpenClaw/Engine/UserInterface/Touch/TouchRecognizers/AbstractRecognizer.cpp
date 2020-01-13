
#include "AbstractRecognizer.h"

AbstractRecognizer::AbstractRecognizer(int id, int zIndex) : id(id), zIndex(zIndex) {
    this->enabled = true;
    frame.x = 0;
    frame.y = 0;
    frame.w = 1;
    frame.h = 1;
}

bool AbstractRecognizer::isTouchWithinFrame(float x, float y) {
    return x > frame.x && x < frame.x + frame.w &&
           y > frame.y && y < frame.y + frame.h;
}

RecognizerState AbstractRecognizer::OnFingerDown(const SDL_TouchFingerEvent &evt) {
    if (IsEnabled() && isTouchWithinFrame(evt.x, evt.y)) {
        return VOnFingerDown(evt);
    }
    return RecognizerState::DoNothing;
}

RecognizerState AbstractRecognizer::OnFingerMotion(const SDL_TouchFingerEvent &evt) {
    if (IsEnabled()) {
        return VOnFingerMotion(evt);
    }
    return RecognizerState::Failed;
}

RecognizerState AbstractRecognizer::OnFingerUp(const SDL_TouchFingerEvent &evt) {
    if (IsEnabled()) {
        return VOnFingerUp(evt);
    }
    return RecognizerState::Failed;
}
