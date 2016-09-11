#include <assert.h>
#include <string>
#include <sstream>

#include "Util.h"

namespace Util
{
    std::string ConvertToThreeDigitsString(int32_t num)
    {
        assert(num >= 0 && num < 1000);

        std::stringstream ss;
        if (num >= 100)
        {
            ss << num;
        }
        else if (num >= 10)
        {
            ss << (int32_t)0 << num;
        }
        else
        {
            ss << (int32_t)0 << (int32_t)0 << num;
        }

        return ss.str();
    }

    SDL_Rect WwdRectToSDLRect(WwdRect& rect)
    {
        SDL_Rect sdlRect;
        sdlRect.x = rect.left;
        sdlRect.y = rect.top;
        sdlRect.w = rect.right - rect.left;
        sdlRect.h = rect.bottom - rect.top;

        return sdlRect;
    }

    inline bool IsInBetween(int32_t num, int32_t leftLimit, int32_t rightLimit)
    {
        return ((num > leftLimit) && (num < rightLimit));
    }

    void ParseCollisionRectanglesFromTile(TilePrototype* tilePrototype, WwdTileDescription* tileDesc, int32_t tileId)
    {
        SDL_Rect tileRect = WwdRectToSDLRect(tileDesc->rect);
        // This is full clear tile
        if (tileId == -1)
        {
            tilePrototype->collisionRectangles.push_back({ WAP_TILE_ATTRIBUTE_CLEAR, tileRect });
            // myfile << "Tile -1" << endl;
            return;
        }

        if (tileDesc->type == WAP_TILE_TYPE_SINGLE)
        {
            //myfile << "Tile 0" << endl;
            tilePrototype->collisionRectangles.push_back({ tileDesc->insideAttrib, tileRect });
            return;
        }
        else if (tileDesc->type == WAP_TILE_TYPE_DOUBLE)
        {
            // Everything inside tileDesc->rect is clear, outside of that eveything is
            // tileDesc->outsideAttrib
            // Though I dont think I have to specify this

            // My paper case (1)
            // xxxx
            // oooo
            // oooo
            // This yields 2 rectangles
            if ((tileDesc->rect.left == 0 && tileDesc->rect.top == 0) && // Starting in upper left corner
                (tileDesc->rect.right == tileDesc->width - 1)) // Ending on right side of the rect
            {
                // Inside rect
                SDL_Rect rect1 = { tileRect.x, tileRect.y, tileDesc->width, tileDesc->rect.bottom };
                // Outside rect
                SDL_Rect rect2 = { tileRect.x, tileRect.y + tileDesc->rect.bottom + 1, tileDesc->width, tileDesc->height - tileDesc->rect.bottom };
                tilePrototype->collisionRectangles.push_back({tileDesc->insideAttrib, rect1 });
                tilePrototype->collisionRectangles.push_back({ tileDesc->outsideAttrib, rect2 });
                //myfile << "Tile 1" << endl;
            }
            // My paper case (2)
            // ooox
            // ooox
            // ooox
            // This yields 2 rectangles
            else if ((tileDesc->rect.left > 0 && tileDesc->rect.top == 0) && // Starting in between right and left corners with top being on the top side
                (tileDesc->rect.right == tileDesc->width - 1) && (tileDesc->rect.bottom == tileDesc->height - 1)) // Ends in bottom right corner of the tile
            {
                // Inside rect
                SDL_Rect rect1 = { tileRect.x + tileDesc->rect.left, tileRect.y, tileDesc->width - tileDesc->rect.left, tileDesc->height };
                // Outside rect
                SDL_Rect rect2 = { tileRect.x, tileRect.y, tileDesc->rect.left, tileDesc->height };
                tilePrototype->collisionRectangles.push_back({tileDesc->insideAttrib, rect1 });
                tilePrototype->collisionRectangles.push_back({ tileDesc->outsideAttrib, rect2 });
                /*myfile << "Inside Rect x-y:w-h = " << rect1.x << "-" << rect1.y << ":" << rect1.w << "-" << rect1.h << endl;
                myfile << "Outside Rect x-y:w-h = " << rect2.x << "-" << rect2.y << ":" << rect2.w << "-" << rect2.h << endl;
                myfile << "Tile 2" << endl;*/
            }
            // My paper case (3)
            // This yields 2 rectangles
            else if ((tileDesc->rect.left == 0) && (tileDesc->rect.top > 0) && // Starting on left side between top and bottom
                (tileDesc->rect.right == tileDesc->width - 1) && (tileDesc->rect.bottom == tileDesc->height - 1)) // ending in bottom right corner
            {
                // Inside rect
                SDL_Rect rect1 = { tileRect.x, tileRect.y + tileDesc->rect.top, tileDesc->width, tileDesc->height - tileDesc->rect.top };
                // Outside rect
                SDL_Rect rect2 = { tileRect.x, tileRect.y, tileDesc->width, tileDesc->rect.top };
                tilePrototype->collisionRectangles.push_back({tileDesc->insideAttrib, rect1 });
                tilePrototype->collisionRectangles.push_back({ tileDesc->outsideAttrib, rect2 });
                /*myfile << "Inside Rect x-y:w-h = " << rect1.x << "-" << rect1.y << ":" << rect1.w << "-" << rect1.h << endl;
                myfile << "Outside Rect x-y:w-h = " << rect2.x << "-" << rect2.y << ":" << rect2.w << "-" << rect2.h << endl;
                myfile << "Tile 3" << endl;*/
            }
            // My paper case (4)
            // This yields 2 rectangles
            else if ((tileDesc->rect.left == 0) && (tileDesc->rect.top == 0) && // Starting in upper left corner
                (tileDesc->rect.right > 0) && (tileDesc->rect.bottom == tileDesc->height - 1)) // Ending on the bottom line somewhere between left and right
            {
                // Inside rect
                SDL_Rect rect1 = { tileRect.x, tileRect.y, tileDesc->rect.right, tileDesc->height };
                // Outside rect
                SDL_Rect rect2 = { tileRect.x + tileDesc->rect.left, tileRect.y, tileDesc->width - tileDesc->rect.right, tileDesc->height };
                tilePrototype->collisionRectangles.push_back({tileDesc->insideAttrib, rect1 });
                tilePrototype->collisionRectangles.push_back({ tileDesc->outsideAttrib, rect2 });
                /*myfile << "Inside Rect x-y:w-h = " << rect1.x << "-" << rect1.y << ":" << rect1.w << "-" << rect1.h << endl;
                myfile << "Outside Rect x-y:w-h = " << rect2.x << "-" << rect2.y << ":" << rect2.w << "-" << rect2.h << endl;
                myfile << "Tile 4" << endl;*/
            }
            // !!! UNUSED !!!
            // My paper case (5)
            // This yields 3 rectangles
            else if ((tileDesc->rect.left == 0) && (tileDesc->rect.top > 0) && // Starting on the left side somewhere between top and bottom
                (tileDesc->rect.right == tileDesc->width - 1) && (tileDesc->rect.bottom > 0)) // !!! Not 100% but it works because we have case (3) ending on right side somewhere between top and bottom
            {
                //myfile << "Tile 5" << endl;
            }
            // !!! UNUSED !!!
            // My paper case (6)
            // This yields 3 rectangles
            else if ((tileDesc->rect.left > 0) && (tileDesc->rect.top == 0) && // Starting on top side somewhere between left and right
                (tileDesc->rect.right > 0) && (tileDesc->rect.bottom == tileDesc->height - 1)) // !!! similiar to (2)
            {
                //myfile << "Tile 6" << endl;
            }
            // My paper case (7) --- PAPERFIED
            // This yields 3 rectangles
            else if ((tileDesc->rect.left == 0) && (tileDesc->rect.top == 0) && // Starting in top left corner
                (tileDesc->rect.right != tileDesc->width - 1) && (tileDesc->rect.bottom != tileDesc->height - 1)) // Ending somewhere inside the rect
            {
                SDL_Rect rect1 = { tileRect.x, tileRect.y, tileDesc->rect.right, tileDesc->rect.bottom };
                SDL_Rect rect2 = { tileRect.x, tileRect.y + tileDesc->rect.bottom, tileDesc->rect.right, tileDesc->height - tileDesc->rect.bottom };
                SDL_Rect rect3 = { tileRect.x + tileDesc->rect.right, tileRect.y, tileDesc->width - tileDesc->rect.right, tileDesc->height };
                tilePrototype->collisionRectangles.push_back({tileDesc->insideAttrib, rect1 });
                tilePrototype->collisionRectangles.push_back({ tileDesc->outsideAttrib, rect2 });
                tilePrototype->collisionRectangles.push_back({ tileDesc->outsideAttrib, rect3 });
                /*myfile << "Inside Rect x-y:w-h = " << rect1.x << "-" << rect1.y << ":" << rect1.w << "-" << rect1.h << endl;
                myfile << "Outside Rect 1 x-y:w-h = " << rect2.x << "-" << rect2.y << ":" << rect2.w << "-" << rect2.h << endl;
                myfile << "Outside Rect 2 x-y:w-h = " << rect3.x << "-" << rect3.y << ":" << rect3.w << "-" << rect3.h << endl;
                myfile << "Tile 7" << endl;*/
            }
            // My paper case (8) --- PAPERFIED
            // This yields 3 rectangles
            else if ((tileDesc->rect.left > 0) && (tileDesc->rect.top == 0) && // Starting somewhere on the top between left and right
                (tileDesc->rect.right == tileDesc->width - 1) && (tileDesc->rect.bottom != tileDesc->height - 1)) // ending somewhere on the right side between top and bottom
            {
                SDL_Rect rect1 = { tileRect.x + tileDesc->rect.left, tileRect.y, tileDesc->width - tileDesc->rect.left, tileDesc->rect.bottom };
                SDL_Rect rect2 = { tileRect.x, tileRect.y, tileDesc->rect.left, tileDesc->height };
                SDL_Rect rect3 = { tileRect.x + tileDesc->rect.left, tileRect.y + tileDesc->rect.bottom, tileDesc->width - tileDesc->rect.left, tileDesc->height - tileDesc->rect.bottom };
                tilePrototype->collisionRectangles.push_back({tileDesc->insideAttrib, rect1 });
                tilePrototype->collisionRectangles.push_back({ tileDesc->outsideAttrib, rect2 });
                tilePrototype->collisionRectangles.push_back({ tileDesc->outsideAttrib, rect3 });
                /*myfile << "Inside Rect x-y:w-h = " << rect1.x << "-" << rect1.y << ":" << rect1.w << "-" << rect1.h << endl;
                myfile << "Outside Rect 1 x-y:w-h = " << rect2.x << "-" << rect2.y << ":" << rect2.w << "-" << rect2.h << endl;
                myfile << "Outside Rect 2 x-y:w-h = " << rect3.x << "-" << rect3.y << ":" << rect3.w << "-" << rect3.h << endl;
                myfile << "Tile 8" << endl;*/
            }
            // My paper case (9) --- PAPERFIED
            // This yields 3 rectangles
            else if ((tileDesc->rect.left > 0) && (tileDesc->rect.top > 0) &&
                (tileDesc->rect.right == tileDesc->width - 1) && (tileDesc->rect.bottom == tileDesc->height - 1))
            {
                SDL_Rect rect1 = { tileRect.x + tileDesc->rect.left, tileRect.y + tileDesc->rect.top, tileDesc->width - tileDesc->rect.left, tileDesc->height - tileDesc->rect.top };
                SDL_Rect rect2 = { tileRect.x, tileRect.y, tileDesc->rect.left, tileDesc->height };
                SDL_Rect rect3 = { tileRect.x + tileDesc->rect.left, tileRect.y, tileDesc->width - tileDesc->rect.left, tileDesc->rect.top };
                tilePrototype->collisionRectangles.push_back({tileDesc->insideAttrib, rect1 });
                tilePrototype->collisionRectangles.push_back({ tileDesc->outsideAttrib, rect2 });
                tilePrototype->collisionRectangles.push_back({ tileDesc->outsideAttrib, rect3 });
                /*myfile << "Inside Rect x-y:w-h = " << rect1.x << "-" << rect1.y << ":" << rect1.w << "-" << rect1.h << endl;
                myfile << "Outside Rect 1 x-y:w-h = " << rect2.x << "-" << rect2.y << ":" << rect2.w << "-" << rect2.h << endl;
                myfile << "Outside Rect 2 x-y:w-h = " << rect3.x << "-" << rect3.y << ":" << rect3.w << "-" << rect3.h << endl;
                myfile << "Tile 9" << endl;*/
            }
            // My paper case (10) --- PAPERFIED
            // This yields 3 rectangles
            else if ((tileDesc->rect.left == 0) && (tileDesc->rect.top > 0) && // Beginning somewhere on the left side between top and bottom
                (tileDesc->rect.right != tileDesc->width - 1) && (tileDesc->rect.bottom == tileDesc->height - 1))
            {
                SDL_Rect rect1 = { tileRect.x, tileRect.y + tileDesc->rect.top, tileDesc->rect.right, tileDesc->height - tileDesc->rect.top };
                SDL_Rect rect2 = { tileRect.x, tileRect.y, tileDesc->rect.right, tileDesc->rect.top };
                SDL_Rect rect3 = { tileRect.x + tileDesc->rect.right, tileRect.y, tileDesc->width - tileDesc->rect.right, tileDesc->height };
                tilePrototype->collisionRectangles.push_back({tileDesc->insideAttrib, rect1 });
                tilePrototype->collisionRectangles.push_back({ tileDesc->outsideAttrib, rect2 });
                tilePrototype->collisionRectangles.push_back({ tileDesc->outsideAttrib, rect3 });
                /*myfile << "Inside Rect x-y:w-h = " << rect1.x << "-" << rect1.y << ":" << rect1.w << "-" << rect1.h << endl;
                myfile << "Outside Rect 1 x-y:w-h = " << rect2.x << "-" << rect2.y << ":" << rect2.w << "-" << rect2.h << endl;
                myfile << "Outside Rect 2 x-y:w-h = " << rect3.x << "-" << rect3.y << ":" << rect3.w << "-" << rect3.h << endl;
                myfile << "Tile 10" << endl;*/
            }
            // This is where the rekt begins

            // My paper case (11)
            // This yields 4 rectangles
            else if (IsInBetween(tileDesc->rect.left, 0, tileDesc->width - 1) && (tileDesc->rect.top == 0) &&
                (IsInBetween(tileDesc->rect.right, 0, tileDesc->width - 1)) && (IsInBetween(tileDesc->rect.bottom, 0, tileDesc->height - 1)))
            {
                //myfile << "Tile 11" << endl;
            }
            // My paper case (12)
            // This yields 4 rectangles
            else if (IsInBetween(tileDesc->rect.left, 0, tileDesc->width - 1) && IsInBetween(tileDesc->rect.top, 0, tileDesc->height - 1) &&
                (tileDesc->rect.right == tileDesc->width - 1) && IsInBetween(tileDesc->rect.bottom, 0, tileDesc->height - 1))
            {
                //myfile << "Tile 12" << endl;
            }
            // My paper case (13) --- PAPERFIED
            // This yields 4 rectangles
            else if (IsInBetween(tileDesc->rect.left, 0, tileDesc->width - 1) && IsInBetween(tileDesc->rect.top, 0, tileDesc->height - 1) &&
                IsInBetween(tileDesc->rect.right, 0, tileDesc->width - 1) && (tileDesc->rect.bottom == tileDesc->height - 1))
            {
                SDL_Rect rect1 = { tileRect.x + tileDesc->rect.left, tileRect.y + tileDesc->rect.top, tileDesc->rect.right - tileDesc->rect.left, tileDesc->height - tileDesc->rect.top };
                SDL_Rect rect2 = { tileRect.x, tileRect.y, tileDesc->rect.left, tileDesc->height };
                SDL_Rect rect3 = { tileRect.x + tileDesc->rect.left, tileRect.y, tileDesc->rect.right - tileDesc->rect.left, tileDesc->rect.top };
                SDL_Rect rect4 = { tileRect.x + tileDesc->rect.right, tileRect.y, tileDesc->width - tileDesc->rect.right, tileDesc->height };
                tilePrototype->collisionRectangles.push_back({tileDesc->insideAttrib, rect1 });
                tilePrototype->collisionRectangles.push_back({ tileDesc->outsideAttrib, rect2 });
                tilePrototype->collisionRectangles.push_back({ tileDesc->outsideAttrib, rect3 });
                tilePrototype->collisionRectangles.push_back({ tileDesc->outsideAttrib, rect4 });
                /*myfile << "Inside Rect x-y:w-h = " << rect1.x << "-" << rect1.y << ":" << rect1.w << "-" << rect1.h << endl;
                myfile << "Outside Rect 1 x-y:w-h = " << rect2.x << "-" << rect2.y << ":" << rect2.w << "-" << rect2.h << endl;
                myfile << "Outside Rect 2 x-y:w-h = " << rect3.x << "-" << rect3.y << ":" << rect3.w << "-" << rect3.h << endl;
                myfile << "Outside Rect 3 x-y:w-h = " << rect4.x << "-" << rect4.y << ":" << rect4.w << "-" << rect4.h << endl;
                myfile << "Tile 13" << endl;*/
            }
            // My paper case (14)
            // This yields 4 rectangles
            else if ((tileDesc->rect.left == 0) && IsInBetween(tileDesc->rect.top, 0, tileDesc->height - 1) &&
                IsInBetween(tileDesc->rect.right, 0, tileDesc->width - 1) && (tileDesc->rect.bottom == tileDesc->height - 1))
            {
                //myfile << "Tile 14" << endl;
            }
            // My paper case (15)
            // This yields 5 rectangles
            else if (IsInBetween(tileDesc->rect.left, 0, tileDesc->width - 1) && IsInBetween(tileDesc->rect.top, 0, tileDesc->height - 1) &&
                IsInBetween(tileDesc->rect.right, 0, tileDesc->width - 1) && (tileDesc->rect.bottom == tileDesc->height - 1))
            {
                //myfile << "Tile 15" << endl;
            }
        }
        else
        {
            assert(false);
        }

        assert(!tilePrototype->collisionRectangles.empty());
    }
}