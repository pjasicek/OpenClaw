#ifndef IMAGE_H_
#define IMAGE_H_

#include <libwap.h>
#include <SDL2/SDL.h>

class Image
{
public:
    Image();
    ~Image();

    static SDL_Texture* GetTextureFromPid(WapPid* pid, SDL_Renderer* renderer);
    static Image* CreateImage(WapPid* pid, SDL_Renderer* renderer);

    inline SDL_Texture* GetTexture() { return _texture; }
    inline int GetWidth() { return _width; }
    inline int GetHeight() { return _height; }
    inline int GetOffsetX() { return _offsetX; }
    inline int GetOffsetY() { return _offsetY; }

    void SetOffset(int x, int y) { _offsetX = x; _offsetY = y; }

    SDL_Rect GetPositonRect(int32_t x, int32_t y);

private:
    bool Initialize(WapPid* pid, SDL_Renderer* renderer);

    SDL_Texture* _texture;
    int _width;
    int _height;
    int _offsetX;
    int _offsetY;
};

#endif
