#ifndef __ABSTRACTRECOGNIZER_H__
#define __ABSTRACTRECOGNIZER_H__

#include "../TouchEvents.h"

enum class RecognizerState {
    DoNothing,   // Recognizer does not track this finger. TouchManager remove it.
    Recognizing, // Recognizer tracks this finger. Wait other inputs, Touch manager calls another recognizers

    EventReady, // Event is ready. Touch manager may call VGetEvent()
    Hold, // All available events were fired. Wait other inputs. Detach another recognizers

    Failed, // Recognizer does not track this finger anymore. TouchManager remove it.
    Done // Event is end. TouchManager remove it.
};

struct Rect {
    float x, y, w, h;

    Rect() = default;
    Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
};

class AbstractRecognizer {
private:
    const int id;
    bool enabled;
    const int zIndex;

protected:
    Rect frame;

    virtual RecognizerState VOnFingerDown(const SDL_TouchFingerEvent &evt) = 0;
    virtual RecognizerState VOnFingerMotion(const SDL_TouchFingerEvent &evt) = 0;
    virtual RecognizerState VOnFingerUp(const SDL_TouchFingerEvent &evt) = 0;

    bool isTouchWithinFrame(float x, float y);

public:

    AbstractRecognizer(int id, int zIndex);

    int GetId() const {
        return id;
    }

    bool IsEnabled() const {
        return enabled;
    }

    void SetEnabled(bool enabled) {
        this->enabled = enabled;
    }

    void SetFrame(const Rect &rect) {
        frame = rect;
    }

    virtual void VOnUpdate() {}

    virtual Touch_Event VGetEvent(SDL_FingerID finger) = 0;

    virtual RecognizerState VGetState(SDL_FingerID finger) = 0;

    virtual void VFingerDetached(SDL_FingerID finger) = 0;

    RecognizerState OnFingerDown(const SDL_TouchFingerEvent &evt);
    RecognizerState OnFingerMotion(const SDL_TouchFingerEvent &evt);
    RecognizerState OnFingerUp(const SDL_TouchFingerEvent &evt);

    bool operator<(const AbstractRecognizer &b) const {
        return this->zIndex < b.zIndex;
    }

};

#endif
