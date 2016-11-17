#include "Camera.h"

#include <iostream>
using namespace std;

Camera::Camera(SDL_Window* window, SDL_Renderer* renderer)
{
    _x = 0;
    _y = 0;
    _gameWindow = window;
    _gameRenderer = renderer;
    _padding = CAMERA_PADDING;

    _clampViewport = false;
    _maxX = INT_MAX;
    _maxY = INT_MAX;
}

Camera::~Camera()
{

}

bool Camera::IsCheckRectVisible(SDL_Rect* rect)
{
    return false;
}

float Camera::GetScaleX()
{
    float scaleX;
    SDL_RenderGetScale(_gameRenderer, &scaleX, NULL);

    return scaleX;
}

float Camera::GetScaleY()
{
    float scaleY;
    SDL_RenderGetScale(_gameRenderer, NULL, &scaleY);

    return scaleY;
}

void Camera::GetScale(float* scaleX, float* scaleY)
{
    SDL_RenderGetScale(_gameRenderer, scaleX, scaleY);
}

int32_t Camera::GetCameraWidth()
{
    int width;
    SDL_GetWindowSize(_gameWindow, &width, NULL);

    return width;
}

int32_t Camera::GetCameraHeight()
{
    int height;
    SDL_GetWindowSize(_gameWindow, NULL, &height);

    return height;
}

void Camera::GetCameraSize(uint32_t* width, uint32_t* height)
{
    SDL_GetWindowSize(_gameWindow, (int*)width, (int*)height);
}

void Camera::SetPositionCentered(int32_t centerX, int32_t centerY)
{
    int w, h;
    SDL_GetWindowSize(_gameWindow, &w, &h);

    float scaleX, scaleY;
    GetScale(&scaleX, &scaleY);

    w = (int)(w / scaleX);
    h = (int)(h / scaleY);

    SetPosition(centerX - w / 2, centerY - h / 2);
}

void Camera::ClampViewport(int32_t maxX, int32_t maxY)
{
    _clampViewport = true;
    _maxX = maxX;
    _maxY = maxY;
}