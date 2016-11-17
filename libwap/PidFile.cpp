#include <fstream>
#include <vector>
#include <stdint.h>

#include "libwap.h"
#include "IO.h"

#include <iostream>
using namespace std;
WapPid* WAP_PidLoadFromData(char* data, size_t size, WapPal* palette)
{
    uint32_t x, y;
    uint8_t byte;
    WapPid* wapPid = NULL;

    if ((data == NULL) || (size == 0))
    {
        return NULL;
    }

    /********************** PID HEADER/PROPERTIES **********************/

    wapPid = new WapPid;

    // Set default values;
    (*wapPid) = { 0 };

    InputStream pidFileStream(data, size);

    pidFileStream.read(wapPid->fileDesc,
        wapPid->flags,
        wapPid->width,
        wapPid->height,
        wapPid->offsetX,
        wapPid->offsetY,
        wapPid->unk0,
        wapPid->unk1);

    /********************** PID PALETTE **********************/

    WapPal* imagePalette = NULL;

    // If image has embedded palette within it, extract it
    if (wapPid->flags & WAP_PID_FLAG_EMBEDDED_PALETTE)
    {
        uint32_t paletteOffset = size - WAP_PALETTE_SIZE_BYTES;
        char* paletteData = &(data[paletteOffset]);
        imagePalette = WAP_PalLoadFromData(paletteData, paletteOffset);
    }
    else
    {
        imagePalette = palette;
    }

    // Make sure we have loaded a palette
    if (imagePalette == NULL)
    {
        return NULL;
    }

    /********************** PID PIXELS **********************/

    wapPid->colorsCount = wapPid->width * wapPid->height;
    wapPid->colors = new WAP_ColorRGBA[wapPid->colorsCount];

    if (wapPid->colors == NULL)
    {
        delete wapPid;
        return NULL;
    }

    x = 0;
    y = 0;
    // PID is compressed, RLE
    try {
        if (wapPid->flags & WAP_PID_FLAG_COMPRESSION)
        {
            while (y < wapPid->height)
            {
                pidFileStream.read(byte);

                if (byte > 128)
                {
                    int32_t i = byte - 128;
                    while ((i > 0) && (y < wapPid->height))
                    {
                        wapPid->colors[y * wapPid->width + x] = WAP_ColorRGBA{ 0, 0, 0, 1 };
                        x++;
                        if (x == wapPid->width)
                        {
                            x = 0;
                            y++;
                        }
                        i--;
                    }
                }
                else
                {
                    int32_t i = byte;
                    while ((i > 0) && (y < wapPid->height))
                    {
                        pidFileStream.read(byte);

                        wapPid->colors[y * wapPid->width + x] = WAP_ColorRGBA{ imagePalette->colors[byte].r,
                            imagePalette->colors[byte].g,
                            imagePalette->colors[byte].b,
                            imagePalette->colors[byte].a };

                        x++;
                        if (x == wapPid->width)
                        {
                            x = 0;
                            y++;
                        }
                        i--;
                    }
                }
            }
        }
        else
        {
            while (y < wapPid->height)
            {
                int32_t i = 1;
                pidFileStream.read(byte);

                // PID related encoding probably, this means how many same pixels are following.
                // e.g. if byte = 220, then 220-192=28 same pixels are next to each other
                if (byte > 192)
                {
                    i = byte - 192;
                    pidFileStream.read(byte);
                }

                while ((i > 0) && (y < wapPid->height))
                {
                    wapPid->colors[y * wapPid->width + x] = WAP_ColorRGBA{ imagePalette->colors[byte].r,
                        imagePalette->colors[byte].g,
                        imagePalette->colors[byte].b,
                        imagePalette->colors[byte].a };

                    x++;
                    if (x == wapPid->width)
                    {
                        x = 0;
                        y++;
                    }
                    i--;
                }
            }
        }
    }
    catch (...)
    {
        // If we created new palette, destroy it
        if (wapPid->flags & WAP_PID_FLAG_EMBEDDED_PALETTE)
        {
            WAP_PalDestroy(imagePalette);
        }
        
        WAP_PidDestroy(wapPid);

        return NULL;
    }

    // If we created new palette, destroy it
    if (wapPid->flags & WAP_PID_FLAG_EMBEDDED_PALETTE)
    {
        WAP_PalDestroy(imagePalette);
    }

    return wapPid;
}

WapPid* WAP_PidLoadFromFile(const char* pidFilePath, WapPal* palette)
{
    std::ifstream pidFileStream(pidFilePath, std::ios::binary);
    if (!pidFileStream.is_open())
    {
        return NULL;
    }

    // Read whole file
    std::vector<char> pidFileContents((std::istreambuf_iterator<char>(pidFileStream)), std::istreambuf_iterator<char>());
    if (!pidFileStream.good())
    {
        return NULL;
    }

    return WAP_PidLoadFromData(pidFileContents.data(), pidFileContents.size(), palette);
}

WapPid* WAP_PidLoadFromRezFile(RezFile* rezFile, WapPal* palette)
{
    // Check input validity
    if (rezFile == NULL)
    {
        return NULL;
    }

    char* data = WAP_GetRezFileData(rezFile);
    if (data == NULL)
    {
        return NULL;
    }

    return WAP_PidLoadFromData(data, rezFile->size, palette);
}

WapPid* WAP_PidLoadFromRezArchive(RezArchive* rezArchive, const char* pidRezPath, WapPal* palette)
{
    // Check input validity
    if ((rezArchive == NULL) || (pidRezPath == NULL))
    {
        return NULL;
    }

    RezFile* pidRezFile = WAP_GetRezFileFromRezArchive(rezArchive, pidRezPath);
    if (pidRezFile == NULL)
    {
        return NULL;
    }

    return WAP_PidLoadFromRezFile(pidRezFile, palette);
}

void WAP_PidDestroy(WapPid* wapPid)
{
    if (wapPid == NULL)
    {
        return;
    }

    delete[] wapPid->colors;
    delete wapPid;
    wapPid = NULL;  
}