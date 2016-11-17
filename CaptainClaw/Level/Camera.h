#ifndef CAMERA_H_
#define CAMERA_H_

#include <SDL.h>

#define CAMERA_PADDING 100

class Camera 
{
public:
    Camera(SDL_Window* window, SDL_Renderer* renderer);
    ~Camera();

    inline void MoveX(double delta) { SetX(_x + delta); }
    inline void MoveY(double delta) { SetY(_y + delta); }

    int32_t GetX() { return (int32_t)_x; }
    int32_t GetY() { return (int32_t)_y; }

    // Critical performance functions - need to be inlined
    inline void SetX(double x)
    { 
        if (_clampViewport)
        {
            if (x < 0) { _x = 0; }
            else if (x >(_maxX - GetCameraWidth())) { _x = _maxX - GetCameraWidth(); }
            else { _x = x; }
        }
        else
        {
            _x = x;
        }
    }
    inline void SetY(double y)
    { 
        if (_clampViewport)
        {
            if (y < 0) { _y = 0; }
            else if (y >(_maxY - GetCameraHeight())) { _y = _maxY - GetCameraHeight(); }
            else { _y = y; }
        }
        else
        {
            _y = y;
        }
    }

    inline void SetPosition(int32_t x, int32_t y) { SetX(x); SetY(y); }
    void SetPositionCentered(int32_t centerX, int32_t centerY);

    int32_t GetCameraWidth();
    int32_t GetCameraHeight();
    void GetCameraSize(uint32_t* width, uint32_t* height);

    float GetScaleX();
    float GetScaleY();

    void GetScale(float* scaleX, float* scaleY);

    void SetScale(float scaleX, float scaleY) { SDL_RenderSetScale(_gameRenderer, scaleX, scaleY); }

    void SetPadding(int32_t padding) { _padding = padding; }
    int32_t GetPadding() { return _padding; }

    bool IsCheckRectVisible(SDL_Rect* rect);

    void ClampViewport(int32_t maxX, int32_t maxY);

private:
    double _x;
    double _y;

    int32_t _padding;

    SDL_Window* _gameWindow;
    SDL_Renderer* _gameRenderer;

    bool _clampViewport;
    int32_t _maxX;
    int32_t _maxY;
};

#endif