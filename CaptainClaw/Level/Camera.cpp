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

uint32_t Camera::GetCameraWidth()
{
    int width;
    SDL_GetWindowSize(_gameWindow, &width, NULL);

    return (uint32_t)width;
}

uint32_t Camera::GetCameraHeight()
{
    int height;
    SDL_GetWindowSize(_gameWindow, NULL, &height);

    return (uint32_t)height;
}