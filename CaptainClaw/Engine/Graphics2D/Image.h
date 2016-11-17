#ifndef IMAGE_H_
#define IMAGE_H_

#include <libwap.h>
#include <SDL.h>

class Image
{
public:
    Image();
    ~Image();

    static SDL_Texture* GetTextureFromPid(WapPid* pid, SDL_Renderer* renderer);
    static Image* CreateImage(WapPid* pid, SDL_Renderer* renderer);

    inline SDL_Texture* GetTexture() { return _texture; }
    inline uint32_t GetWidth() { return _width; }
    inline uint32_t GetHeight() { return _height; }
    inline int32_t GetOffsetX() { return _offsetX; }
    inline int32_t GetOffsetY() { return _offsetY; }

    SDL_Rect GetPositonRect(int32_t x, int32_t y);

private:
    bool Initialize(WapPid* pid, SDL_Renderer* renderer);

    SDL_Texture* _texture;
    uint32_t _width;
    uint32_t _height;
    int32_t _offsetX;
    int32_t _offsetY;
};

#endif