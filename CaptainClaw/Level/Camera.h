#ifndef CAMERA_H_
#define CAMERA_H_

#include <SDL.h>

#define CAMERA_PADDING 100

class Camera 
{
public:
    Camera(SDL_Window* window, SDL_Renderer* renderer);
    ~Camera();

    inline void MoveX(int32_t delta) { _x += delta; }
    inline void MoveY(int32_t delta) { _y += delta; }

    inline int32_t GetX() { return _x; }
    inline int32_t GetY() { return _y; }

    inline void SetX(int32_t x) { _x = x; }
    inline void SetY(int32_t y) { _y = y; }

    inline void SetPosition(uint32_t x, uint32_t y) { _x = x; _y = y; }

    uint32_t GetCameraWidth();
    uint32_t GetCameraHeight();

    float GetScaleX();
    float GetScaleY();

    void GetScale(float* scaleX, float* scaleY);

    void SetScale(float scaleX, float scaleY) { SDL_RenderSetScale(_gameRenderer, scaleX, scaleY); }

    void SetPadding(int32_t padding) { _padding = padding; }
    int32_t GetPadding() { return _padding; }

    bool IsCheckRectVisible(SDL_Rect* rect);

private:
    int32_t _x;
    int32_t _y;

    int32_t _padding;

    SDL_Window* _gameWindow;
    SDL_Renderer* _gameRenderer;
};

#endif