#include <assert.h>
#include <string>
#include <sstream>
#include <random>
#include <iostream>

#include "Util.h"
#include "../SharedDefines.h"
#include "../GameApp/BaseGameLogic.h"

#include "../Events/EventMgr.h"
#include "../Events/Events.h"

#include "../GameApp/BaseGameApp.h"

#include "../Resource/Loaders/WavLoader.h"

//#include "../Level/Level.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace Util
{
    void SplitStringIntoVector(std::string str, std::vector<std::string>& vec)
    {
        std::stringstream ss(str);
        ss >> std::noskipws;
        std::string field;
        char ws_delim;
        while (1)
        {
            if (ss >> field)
            {
                vec.push_back(field);
            }
            else if (ss.eof())
            {
                break;
            }
            else
            {
                vec.push_back(std::string());
            }
            ss.clear();
            ss >> ws_delim;
        }

        if (vec.empty())
        {
            vec.push_back(str);
        }

        if (str.back() == ' ')
        {
            vec.push_back(std::string(""));
        }
    }

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

    void PrintRect(SDL_Rect rect, std::string comment)
    {
        std::cout << comment.c_str() << ": X = " << rect.x << ", Y = " << rect.y << ", Width = " << rect.w << ", Height = " << rect.h << std::endl;
    }

    inline bool IsInBetween(int32_t num, int32_t leftLimit, int32_t rightLimit)
    {
        return ((num > leftLimit) && (num < rightLimit));
    }
    using namespace std;
    void PrintWwdRect(WwdRect rect)
    {
        cout << "WWD RECT: " << "left = " << rect.left << ", top = " << rect.top << ", right = " << rect.right << ", bottom = " << rect.bottom << endl;
    }

    void ParseCollisionRectanglesFromTile(TileCollisionPrototype* tilePrototype, TileDescription* tileDesc)
    {
        SDL_Rect tileRect = { 0, 0, 64, 64 };//WwdRectToSDLRect(tileDesc->rect);
        SDL_Rect fullRect = { 0, 0, tileDesc->width, tileDesc->height };
        //PrintRect(tileRect, to_string(tileId));
        // This is full clear tile
        /*if (tileId == -1)
        {
        tilePrototype->collisionRectangles.push_back({ WAP_TILE_ATTRIBUTE_CLEAR, fullRect });
        // myfile << "Tile -1" << endl;
        return;
        }*/

        if (tileDesc->type == WAP_TILE_TYPE_SINGLE)
        {
            //myfile << "Tile 0" << endl;
            SDL_Rect r = { 0, 0, tileDesc->width, tileDesc->height };
            tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->insideAttrib), fullRect });
            return;
        }
        else if (tileDesc->type == WAP_TILE_TYPE_DOUBLE)
        {
            //PrintWwdRect(tileDesc->rect);
            // Everything inside tileDesc->rect is clear, outside of that eveything is
            // CollisionType(tileDesc->outsideAttrib)
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
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->insideAttrib), rect1 });
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->outsideAttrib), rect2 });
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
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->insideAttrib), rect1 });
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->outsideAttrib), rect2 });
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
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->insideAttrib), rect1 });
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->outsideAttrib), rect2 });
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
                SDL_Rect rect2 = { tileRect.x + tileDesc->rect.right + 1, tileRect.y, tileDesc->width - tileDesc->rect.right, tileDesc->height };
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->insideAttrib), rect1 });
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->outsideAttrib), rect2 });
                /*myfile << "Inside Rect x-y:w-h = " << rect1.x << "-" << rect1.y << ":" << rect1.w << "-" << rect1.h << endl;
                myfile << "Outside Rect x-y:w-h = " << rect2.x << "-" << rect2.y << ":" << rect2.w << "-" << rect2.h << endl;
                myfile << "Tile 4" << endl;*/

                /*cout << "left = " << tileDesc->rect.left << endl;

                cout << "tileId = " << tilePrototype->id << endl;
                PrintRect(rect1, "4.1");
                PrintRect(rect2, "4.2");*/
            }
            // My paper case (5)
            // This yields 3 rectangles
            else if ((tileDesc->rect.left == 0) && (tileDesc->rect.top > 0) && // Starting on the left side somewhere between top and bottom
                (tileDesc->rect.right == tileDesc->width - 1) && (tileDesc->rect.bottom > 0)) // !!! Not 100% but it works because we have case (3) ending on right side somewhere between top and bottom
            {
                SDL_Rect rect1 = { 0, 0, tileDesc->width, tileDesc->rect.top };
                SDL_Rect rect2 = { 0, 0, tileDesc->width, tileDesc->rect.bottom - tileDesc->rect.top };
                SDL_Rect rect3 = { 0, tileDesc->rect.bottom, tileDesc->width, tileDesc->height - tileDesc->rect.bottom };
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->outsideAttrib), rect1 });
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->insideAttrib), rect2 });
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->outsideAttrib), rect3 });

                //LOG("TileId: " + ToStr(tileDesc->tileId));

                //myfile << "Tile 5" << endl;
                //assert(false && "Paring rects case: 5");
            }
            // My paper case (6)
            // This yields 3 rectangles
            else if ((tileDesc->rect.left > 0) && (tileDesc->rect.top == 0) && // Starting on top side somewhere between left and right
                (tileDesc->rect.right > 0) && (tileDesc->rect.bottom == tileDesc->height - 1)) // !!! similiar to (2)
            {
                //myfile << "Tile 6" << endl;
                //cout << "tileId: " << tileDesc->tileId << endl;
                //cout << "left: " << tileDesc->rect.left << "right: " << tileDesc->rect.right << "bott: " << tileDesc->rect.bottom << "top: " << tileDesc->rect.top << endl;
                SDL_Rect rect1 = { 0, 0, tileDesc->rect.left, tileDesc->height };
                SDL_Rect rect2 = { tileDesc->rect.left, 0, tileDesc->rect.right - tileDesc->rect.left, tileDesc->height };
                SDL_Rect rect3 = { tileDesc->rect.right, 0, tileDesc->width - tileDesc->rect.right, tileDesc->height };
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->outsideAttrib), rect1 });
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->insideAttrib), rect2 });
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->outsideAttrib), rect3 });
                //assert(false && "Paring rects case: 6");
            }
            // My paper case (7) --- PAPERFIED
            // This yields 3 rectangles
            else if ((tileDesc->rect.left == 0) && (tileDesc->rect.top == 0) && // Starting in top left corner
                (tileDesc->rect.right != tileDesc->width - 1) && (tileDesc->rect.bottom != tileDesc->height - 1)) // Ending somewhere inside the rect
            {
                SDL_Rect rect1 = { tileRect.x, tileRect.y, tileDesc->rect.right, tileDesc->rect.bottom };
                SDL_Rect rect2 = { tileRect.x, tileRect.y + tileDesc->rect.bottom + 1, tileDesc->rect.right + 1, tileDesc->height - tileDesc->rect.bottom };
                SDL_Rect rect3 = { tileRect.x + tileDesc->rect.right + 1, tileRect.y, tileDesc->width - tileDesc->rect.right, tileDesc->height };
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->insideAttrib), rect1 });
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->outsideAttrib), rect2 });
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->outsideAttrib), rect3 });
                /*myfile << "Inside Rect x-y:w-h = " << rect1.x << "-" << rect1.y << ":" << rect1.w << "-" << rect1.h << endl;
                myfile << "Outside Rect 1 x-y:w-h = " << rect2.x << "-" << rect2.y << ":" << rect2.w << "-" << rect2.h << endl;
                myfile << "Outside Rect 2 x-y:w-h = " << rect3.x << "-" << rect3.y << ":" << rect3.w << "-" << rect3.h << endl;
                myfile << "Tile 7" << endl;*/

                /*cout << "tileId = " << tilePrototype->id << endl;
                PrintRect(rect1, "7.1");
                PrintRect(rect2, "7.2");
                PrintRect(rect3, "7.3");*/
            }
            // My paper case (8) --- PAPERFIED
            // This yields 3 rectangles
            else if ((tileDesc->rect.left > 0) && (tileDesc->rect.top == 0) && // Starting somewhere on the top between left and right
                (tileDesc->rect.right == tileDesc->width - 1) && (tileDesc->rect.bottom != tileDesc->height - 1)) // ending somewhere on the right side between top and bottom
            {
                SDL_Rect rect1 = { tileRect.x + tileDesc->rect.left, tileRect.y, tileDesc->width - tileDesc->rect.left, tileDesc->rect.bottom };
                SDL_Rect rect2 = { tileRect.x, tileRect.y, tileDesc->rect.left - 1, tileDesc->height };
                SDL_Rect rect3 = { tileRect.x + tileDesc->rect.left, tileRect.y + tileDesc->rect.bottom + 1, tileDesc->width - tileDesc->rect.left, tileDesc->height - tileDesc->rect.bottom - 1 };
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->insideAttrib), rect1 });
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->outsideAttrib), rect2 });
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->outsideAttrib), rect3 });
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
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->insideAttrib), rect1 });
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->outsideAttrib), rect2 });
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->outsideAttrib), rect3 });
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
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->insideAttrib), rect1 });
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->outsideAttrib), rect2 });
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->outsideAttrib), rect3 });
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
                SDL_Rect rect1 = { tileDesc->rect.left, tileDesc->rect.top, tileDesc->rect.right - tileDesc->rect.left, tileDesc->rect.bottom };
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->insideAttrib), rect1 });

                assert(tileDesc->insideAttrib != WAP_TILE_ATTRIBUTE_CLEAR && tileDesc->outsideAttrib == WAP_TILE_ATTRIBUTE_CLEAR);
            }
            // My paper case (12)
            // This yields 4 rectangles
            else if (IsInBetween(tileDesc->rect.left, 0, tileDesc->width - 1) && IsInBetween(tileDesc->rect.top, 0, tileDesc->height - 1) &&
                (tileDesc->rect.right == tileDesc->width - 1) && IsInBetween(tileDesc->rect.bottom, 0, tileDesc->height - 1))
            {
                LOG_ERROR("Case 12: Tile " + ToStr(tileDesc->tileId) + " not implemented !");
                //myfile << "Tile 12" << endl;
                //assert(false && "Paring rects case: 12");
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
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->insideAttrib), rect1 });
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->outsideAttrib), rect2 });
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->outsideAttrib), rect3 });
                tilePrototype->collisionRectangles.push_back({ CollisionType(tileDesc->outsideAttrib), rect4 });
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
                assert(false && "Paring rects case: 14");
            }
            // My paper case (15)
            // This yields 5 rectangles
            else if (IsInBetween(tileDesc->rect.left, 0, tileDesc->width - 1) && IsInBetween(tileDesc->rect.top, 0, tileDesc->height - 1) &&
                IsInBetween(tileDesc->rect.right, 0, tileDesc->width - 1) && (tileDesc->rect.bottom == tileDesc->height - 1))
            {
                //myfile << "Tile 15" << endl;
                assert(false && "Paring rects case: 16");
            }
        }
        else
        {
            assert(false);
        }

        if (tilePrototype->collisionRectangles.empty())
        {
            LOG_ERROR("Collision rectangles are empty for tile: " + ToStr(tileDesc->tileId));
            SDL_Rect rect;
            rect.x = tileDesc->rect.left;
            rect.y = tileDesc->rect.top;
            rect.w = tileDesc->rect.right - tileDesc->rect.left;
            rect.h = tileDesc->rect.bottom - tileDesc->rect.top;
            PrintRect(rect, "Unknown Rect");
            //assert(!tilePrototype->collisionRectangles.empty());
        }

        /*for (auto a : tilePrototype->collisionRectangles)
        {
        PrintRect(a.collisionRect, to_string(tileId));
        }*/
    }

    int GetRandomNumber(int fromRange, int toRange)
    {
        static std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<int> uni(fromRange, toRange);

        return uni(rng);
    }

    bool RollDice(int chanceToSucceed)
    {
        return (GetRandomNumber(0, 100) < chanceToSucceed);
    }

    std::string PlayRandomSoundFromList(const std::vector<std::string>& sounds, int volume)
    {
        if (!sounds.empty())
        {
            int soundIdx = Util::GetRandomNumber(0, sounds.size() - 1);

            SoundInfo soundInfo(sounds[soundIdx]);
            soundInfo.soundVolume = volume;
            IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
                new EventData_Request_Play_Sound(soundInfo)));

            return soundInfo.soundToPlay;
        }

        return "";
    }

    void PlaySimpleSound(const std::string& sound, int volume)
    {
        SoundInfo soundInfo(sound);
        soundInfo.soundVolume = volume;
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));
    }

    int GetSoundDurationMs(const std::string& soundPath)
    {
        shared_ptr<Mix_Chunk> pSound = WavResourceLoader::LoadAndReturnSound(soundPath.c_str());
        assert(pSound != nullptr);
        return GetSoundDurationMs(pSound.get());
    }

#ifndef __EMSCRIPTEN__
    int GetSoundDurationMs(Mix_Chunk* pSound)
    {
        uint32 points = 0;
        uint32 frames = 0;
        int frequency = 0;
        uint16 format = 0;
        int channels = 0;

        if (Mix_QuerySpec(&frequency, &format, &channels) == 0)
        {
            return -1;
        }

        points = (pSound->alen / ((format & 0xFF) / 8));
        frames = points / channels;
        return ((frames * 1000) / frequency);
    }
#else
    int GetSoundDurationMs(Mix_Chunk* pSound)
    {
        int duration = -1;
        while (duration < 0) {
            duration = EM_ASM_INT(
                    {
                        var sdlAudio = SDL.audios[$0];
                        if (sdlAudio) {
                            var webAudio = sdlAudio.webAudio;
                            if (webAudio) {
                                var callbacks = webAudio.onDecodeComplete;
                                if (callbacks) {
                                    // The audio data is not decoded. We have to wait.
                                    return -1;
                                }
                                var buffer = webAudio.decodedBuffer;
                                if (buffer) {
                                    return (buffer.duration * 1000)|0;
                                }
                            } else {
                                // HTML5 AudioContext does not support this audio file format.
                                var html5Audio = sdlAudio.audio;
                                if (html5Audio) {
                                    var duration = html5Audio.duration;
                                    if (!duration) {
                                        // TODO: find the way to separate errors. It could be a livelock
                                        // The audio data is not loaded. We have to wait.
                                        return -1;
                                    } else {
                                        return (duration * 1000)|0;
                                    }
                                }
                            }
                        }
                        // Unknown error
                        return -2;
                    },
                    pSound);
            if (duration == -1) {
                emscripten_sleep(20);
            } else if (duration == -2) {
                LOG_WARNING("Could not get sound duration for audio #" + ToStr((int) pSound));
                duration = 0;
            }
        }
        return duration;
    }
#endif

    SDL_Surface* CreateRGBSurface(Uint32 flags, int width, int height, int depth) {
        Uint32 rmask, gmask, bmask, amask;

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
        return SDL_CreateRGBSurface(flags, width, height, depth, rmask, gmask, bmask, amask);
    }

    SDL_Texture* CreateSDLTextureFromRenderer(int rendererWidth, int rendererHeight, SDL_Renderer* pRenderer)
    {
        SDL_Surface* pSurface = CreateRGBSurface(0, rendererWidth, rendererHeight, 32);
        SDL_RenderReadPixels(pRenderer, NULL, SDL_PIXELFORMAT_ARGB8888, pSurface->pixels, pSurface->pitch);
        SDL_Texture* pTextureRect = SDL_CreateTextureFromSurface(pRenderer, pSurface);

        SDL_FreeSurface(pSurface);
        return pTextureRect;
    }

    SDL_Texture* CreateSDLTextureRect(int width, int height, SDL_Color color, SDL_Renderer* pRenderer)
    {
        SDL_Surface* pSurface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
        SDL_FillRect(pSurface, NULL, SDL_MapRGB(pSurface->format, color.r, color.g, color.b));
        SDL_Texture* pTextureRect = SDL_CreateTextureFromSurface(pRenderer, pSurface);

        SDL_FreeSurface(pSurface);
        return pTextureRect;
    }

    SDL_Texture* CreateSDLTextureRect(int width, int height, SDL_Color color, SDL_Renderer* pRenderer, uint8_t alpha)
    {
        SDL_Surface* pSurface = CreateRGBSurface(0, width, height, 32);
        SDL_FillRect(pSurface, NULL, SDL_MapRGBA(pSurface->format, color.r, color.g, color.b, alpha));
        SDL_Texture* pTextureRect = SDL_CreateTextureFromSurface(pRenderer, pSurface);

        SDL_FreeSurface(pSurface);

        return pTextureRect;
    }

    void RenderForcePresent(SDL_Renderer* pRenderer) {
        SDL_RenderPresent(pRenderer);
#ifdef __EMSCRIPTEN__
        // Update screen manually. SDL_RenderPresent does nothing.
        emscripten_sleep(0);
#endif
    }

    void Sleep(Uint32 ms) {
        if (ms > 0) {
#ifndef __EMSCRIPTEN__
            SDL_Delay(ms);
#else
            emscripten_sleep(ms);
#endif
        }
    }

    void PlayRandomHitSound()
    {
        static std::vector<std::string> hitSounds =
        { SOUND_GAME_HIT1, SOUND_GAME_HIT2, SOUND_GAME_HIT3, SOUND_GAME_HIT4 };

        PlayRandomSoundFromList(hitSounds);
    }

    // From MINIZ
    uint32 CalcCRC32(const char* pData, size_t dataLen)
    {
        static const uint32 s_crc32[16] = { 0, 0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
            0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c, 0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c };
        uint32 crcu32 = (uint32)0;
        if (!pData) return 0;
        crcu32 = ~crcu32; while (dataLen--) { uint8 b = *pData++; crcu32 = (crcu32 >> 4) ^ s_crc32[(crcu32 & 0xF) ^ (b & 0xF)]; crcu32 = (crcu32 >> 4) ^ s_crc32[(crcu32 & 0xF) ^ (b >> 4)]; }
        return ~crcu32;
    }

#ifdef __EMSCRIPTEN__
    bool GetCanvasSize(SDL_Point &canvasSize) {
        int width = EM_ASM_INT(
                {return (Module && Module.canvas) ? Module.canvas.scrollWidth : -1;}
        );
        int height = EM_ASM_INT(
                {return (Module && Module.canvas) ? Module.canvas.scrollHeight : -1;}
        );
        if (width >= 640 && height >= 480) {
            canvasSize.x = width;
            canvasSize.y = height;
            return true;
        }
        return false;
    }
#endif
};
