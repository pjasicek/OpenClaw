#ifndef __TOUCHEVENTS_H__
#define __TOUCHEVENTS_H__

#include <SDL2/SDL_events.h>

#define SDL_UserTouchEvent (SDL_USEREVENT + 1)

enum Touch_EventType : Sint32 {
    TAP,
    JOYSTICK_MOVE,
    JOYSTICK_RESET,
    PRESS_START,
    PRESS_END,
    SWIPE_START,
    SWIPE_END
};

struct Touch_TapEvent {
    SDL_FingerID fingerId;
    float x;            /**< Normalized in the range 0...1 */
    float y;            /**< Normalized in the range 0...1 */

    Touch_TapEvent() = default;

    Touch_TapEvent(const SDL_TouchFingerEvent &tfinger) : fingerId(tfinger.fingerId),
                                                          x(tfinger.x),
                                                          y(tfinger.y) {}
};

struct Touch_JoystickEvent {
    SDL_FingerID fingerId;
    float x;            /**< Normalized in the range 0...1 */
    float y;            /**< Normalized in the range 0...1 */
    float dx;           /**< Normalized in the range -1...1 */
    float dy;           /**< Normalized in the range -1...1 */

    Touch_JoystickEvent() = default;

    Touch_JoystickEvent(const SDL_TouchFingerEvent &end) : fingerId(end.fingerId),
                                                           x(0),
                                                           y(0),
                                                           dx(0),
                                                           dy(0) {}

    Touch_JoystickEvent(const SDL_TouchFingerEvent &start,
                        const SDL_TouchFingerEvent &end) : fingerId(start.fingerId),
                                                           x(start.x),
                                                           y(start.y),
                                                           dx(end.x - start.x),
                                                           dy(end.y - start.y) {}
};

struct Touch_PressEvent {
    SDL_FingerID fingerId;
    float x;            /**< Normalized in the range 0...1 */
    float y;            /**< Normalized in the range 0...1 */

    Touch_PressEvent() = default;

    Touch_PressEvent(const SDL_TouchFingerEvent &tfinger) : fingerId(tfinger.fingerId),
                                                            x(tfinger.x),
                                                            y(tfinger.y) {}
};

struct Touch_SwipeEvent {
    SDL_FingerID fingerId;
    float x;            /**< Normalized in the range 0...1 */
    float y;            /**< Normalized in the range 0...1 */
    // dx > 0 -> right
    double dx;          /**< Normalized in the range -1...1 */
    // dy > 0 -> down
    double dy;          /**< Normalized in the range -1...1 */

    Touch_SwipeEvent() = default;

    Touch_SwipeEvent(const SDL_TouchFingerEvent &tfinger,
                     double dx = 0, double dy = 0) : fingerId(tfinger.fingerId),
                                                     x(tfinger.x),
                                                     y(tfinger.y),
                                                     dx(dx),
                                                     dy(dy) {}
};

struct Touch_Event {
    // sdlEvent.user.data1 is a pointer to Touch_Event.
    SDL_Event sdlEvent;
    int recognizerId;
    union {
        Touch_TapEvent tap;
        Touch_JoystickEvent joystick;
        Touch_PressEvent press;
        Touch_SwipeEvent swipe;
    } customData;

    Touch_Event() {
        sdlEvent.type = SDL_UserTouchEvent;
        sdlEvent.user.code = 0;
        sdlEvent.user.data1 = this;
        recognizerId = 0;
    };

    Touch_Event(int recognizerId, Touch_EventType type, const Touch_TapEvent &tap) : Touch_Event() {
        customData.tap = tap;
        sdlEvent.user.code = type;
        this->recognizerId = recognizerId;
    }

    Touch_Event(int recognizerId, Touch_EventType type, const Touch_JoystickEvent &joystick) : Touch_Event() {
        customData.joystick = joystick;
        sdlEvent.user.code = type;
        this->recognizerId = recognizerId;
    }

    Touch_Event(int recognizerId, Touch_EventType type, const Touch_PressEvent &press) : Touch_Event() {
        customData.press = press;
        sdlEvent.user.code = type;
        this->recognizerId = recognizerId;
    }

    Touch_Event(int recognizerId, Touch_EventType type, const Touch_SwipeEvent &swipe) : Touch_Event() {
        customData.swipe = swipe;
        sdlEvent.user.code = type;
        this->recognizerId = recognizerId;
    }
};

#endif
