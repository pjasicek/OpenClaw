#include <assert.h>
#include <SDL2/SDL_image.h>
#include "Image.h"
#include "../SharedDefines.h"

Image::Image()
    :
    m_Width(0),
    m_Height(0),
    m_OffsetX(0),
    m_OffsetY(0),
    m_pTexture(nullptr)
{
    
}

Image::Image(SDL_Texture* pSDLTexture)
    :
    m_pTexture(pSDLTexture),
    m_OffsetX(0),
    m_OffsetY(0)
{
    assert(pSDLTexture != nullptr);
    SDL_QueryTexture(pSDLTexture, nullptr, nullptr, &m_Width, &m_Height);
}

Image::~Image()
{
    SDL_DestroyTexture(m_pTexture);
    m_pTexture = nullptr;
}

SDL_Rect Image::GetPositonRect(int32_t x, int32_t y)
{
    int positionX = x - m_Width / 2 + m_OffsetX;
    int positionY = y - m_Height / 2 + m_OffsetY;

    SDL_Rect rect = { positionX, positionY, m_Width, m_Height };

    return rect;
}

static void PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        }
        else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

SDL_Texture* Image::GetTextureFromPid(WapPid* pid, SDL_Renderer* renderer)
{
    assert(pid != nullptr);
    assert(renderer != nullptr);
    
    uint32_t rmask, gmask, bmask, amask;
    uint32_t width = pid->width;
    uint32_t height = pid->height;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);
    assert(surface != nullptr);

    uint32_t colorIdx;
    uint32_t colorsCount = pid->colorsCount;
    for (colorIdx = 0; colorIdx < colorsCount; colorIdx++)
    {
        WAP_ColorRGBA color = pid->colors[colorIdx];
        uint32_t x = colorIdx % width;
        uint32_t y = colorIdx / width;

        PutPixel(surface, x, y, SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a));
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    assert(texture != nullptr);

    SDL_FreeSurface(surface);

    return texture;
}

Image* Image::CreateImage(WapPid* pid, SDL_Renderer* renderer)
{
    Image* image = new Image();
    if (!image->Initialize(pid, renderer))
    {
        delete image;
        return nullptr;
    }

    return image;
}

Image* Image::CreatePcxImage(char* rawBuffer, uint32_t size, SDL_Renderer* renderer, bool useColorKey, SDL_Color colorKey)
{
    Image* pImage = new Image();
    SDL_RWops* pRWops = SDL_RWFromMem((void*)rawBuffer, size);
    SDL_Surface* pSurface = IMG_LoadPCX_RW(pRWops);
    if (pSurface == nullptr)
    {
        LOG_ERROR(IMG_GetError());
        return nullptr;
    }

    if (useColorKey)
    {
        SDL_SetColorKey(pSurface, SDL_TRUE, SDL_MapRGB(pSurface->format, colorKey.r, colorKey.g, colorKey.b));
    }

    SDL_Texture* pTexture = SDL_CreateTextureFromSurface(renderer, pSurface);
    SDL_FreeSurface(pSurface);

    if (pTexture == nullptr)
    {
        LOG_ERROR(IMG_GetError());
        return nullptr;
    }

    if (!pImage->Initialize(pTexture))
    {
        LOG_ERROR(IMG_GetError());
        return nullptr;
    }

    return pImage;
}

Image* Image::CreatePngImage(char* rawBuffer, uint32_t size, SDL_Renderer* renderer)
{
    Image* pImage = new Image();
    SDL_RWops* pRWops = SDL_RWFromMem((void*)rawBuffer, size);
    SDL_Surface* pSurface = IMG_LoadPNG_RW(pRWops);
    if (pSurface == nullptr)
    {
        LOG_ERROR(IMG_GetError());
        return nullptr;
    }

    SDL_Texture* pTexture = SDL_CreateTextureFromSurface(renderer, pSurface);
    SDL_FreeSurface(pSurface);

    if (pTexture == nullptr)
    {
        LOG_ERROR(IMG_GetError());
        delete pImage;
        return nullptr;
    }

    if (!pImage->Initialize(pTexture))
    {
        LOG_ERROR(IMG_GetError());
        delete pImage;
        SDL_DestroyTexture(pTexture);
        return nullptr;
    }

    return pImage;
}

Image* Image::CreateImageFromColor(SDL_Color color, int w, int h, SDL_Renderer* pRenderer)
{
    Image* pImage = new Image();

    SDL_Surface* pSurface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
    SDL_FillRect(pSurface, nullptr, SDL_MapRGB(pSurface->format, color.r, color.g, color.b));
    SDL_Texture* pTextureRect = SDL_CreateTextureFromSurface(pRenderer, pSurface);

    SDL_FreeSurface(pSurface);

    if (pTextureRect == nullptr)
    {
        LOG_ERROR(IMG_GetError());
        delete pImage;
        return nullptr;
    }

    if (!pImage->Initialize(pTextureRect))
    {
        LOG_ERROR(IMG_GetError());
        delete pImage;
        SDL_DestroyTexture(pTextureRect);
        return nullptr;
    }

    return pImage;
}

bool Image::Initialize(WapPid* pid, SDL_Renderer* renderer)
{
    if (pid == nullptr || renderer == nullptr)
    {
        return false;
    }

    m_Width = pid->width;
    m_Height = pid->height;
    m_OffsetX = pid->offsetX;
    m_OffsetY = pid->offsetY;

    m_pTexture = GetTextureFromPid(pid, renderer);
    if (m_pTexture == nullptr)
    {
        return false;
    }

    return true;
}

bool Image::Initialize(SDL_Texture* pTexture)
{
    if (pTexture == nullptr)
    {
        return false;
    }

    SDL_QueryTexture(pTexture, nullptr, nullptr, &m_Width, &m_Height);
    m_OffsetX = m_OffsetY = 0;
    m_pTexture = pTexture;

    return true;
}