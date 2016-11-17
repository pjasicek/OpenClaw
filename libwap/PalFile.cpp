#include <fstream>
#include <stdint.h>

#include "libwap.h"
#include "IO.h"

WapPal* WAP_PalLoadFromData(char* data, size_t size)
{
    uint32_t i;
    WapPal* wapPal = NULL;

    if ((data == NULL) || (size == 0) || (size != WAP_PALETTE_SIZE_BYTES))
    {
        return NULL;
    }

    wapPal = new WapPal;

    InputStream palFileStream(data, size);

    // Read whole palette, there is no header
    for (i = 0; i < WAP_PALETTE_SIZE_BYTES / 3; i++)
    {
        palFileStream.read(wapPal->colors[i].r,
            wapPal->colors[i].g,
            wapPal->colors[i].b);

        // First pixel in palette is transparent
        if (i == 0)
        {
            wapPal->colors[i].a = 1;
        }
        else
        {
            wapPal->colors[i].a = 255;
        }
    }

    return wapPal;
}

WapPal* WAP_PalLoadFromFile(const char* palFilePath)
{
    std::ifstream palFileStream(palFilePath, std::ios::binary);
    if (!palFileStream.is_open())
    {
        return NULL;
    }

    // Read whole file
    std::vector<char> palFileContents((std::istreambuf_iterator<char>(palFileStream)), std::istreambuf_iterator<char>());
    if (!palFileStream.good())
    {
        return NULL;
    }

    return WAP_PalLoadFromData(palFileContents.data(), palFileContents.size());
}

WapPal* WAP_PalLoadFromRezFile(RezFile* rezFile)
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

    return WAP_PalLoadFromData(data, rezFile->size);
}

WapPal* WAP_PalLoadFromRezArchive(RezArchive* rezArchive, const char* palRezPath)
{
    // Check input validity
    if ((rezArchive == NULL) || (palRezPath == NULL))
    {
        return NULL;
    }

    RezFile* palRezFile = WAP_GetRezFileFromRezArchive(rezArchive, palRezPath);
    if (palRezFile == NULL)
    {
        return NULL;
    }

    return WAP_PalLoadFromRezFile(palRezFile);
}

void WAP_PalDestroy(WapPal* wapPal)
{
    if (wapPal == NULL)
    {
        return;
    }

    delete wapPal;
    wapPal = NULL;  
}