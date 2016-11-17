#include <vector>
#include <fstream>
#include <algorithm>
#include <string.h>
#include <stdint.h>
#include <string.h>
#include <exception>

#include "libwap.h"
#include "IO.h"
#include "Miniz.h"
#include "Util.h"

const uint32_t EXPECTED_HEADER_SIZE = 1524;

static void ReadRect(InputStream &stream, WwdRect &rect) 
{
    stream.read(rect.left, rect.top, rect.right, rect.bottom);
}

static void ReadPlaneTiles(WwdPlane* plane, InputStream& inputStream)
{
    uint32_t i;

    if (plane->tilesCount > 0)
    {
        // Allocate space for plane's tiles
        plane->tiles = new int32_t[plane->tilesCount];

        // Move cursor to plane's tiles beginning
        inputStream.seek(plane->properties.tilesOffset);

        // Read plane's tiles
        for (i = 0; i < plane->tilesCount; i++)
        {
            inputStream.read(plane->tiles[i]);
        }
    }
}

static void ReadPlaneImageSets(WwdPlane* plane, InputStream& inputStream)
{
    uint32_t i;

    if (plane->imageSetsCount > 0)
    {
        // Allocate space for plane's image sets
        plane->imageSets = new char*[plane->imageSetsCount];

        // Move cursor to plane's image sets beginning
        inputStream.seek(plane->properties.imageSetsOffset);

        for (i = 0; i < plane->imageSetsCount; i++)
        {
            // Allocate and set read null terminated string
            plane->imageSets[i] = ReadNullTerminatedString(inputStream);
        }
    }
}

static void ReadPlaneObjects(WwdPlane* plane, InputStream& inputStream)
{
    uint32_t i;

    if (plane->objectsCount > 0)
    {
        // Allocate space for plane's objects
        plane->objects = new WwdObject[plane->objectsCount];

        // move cursor to plane's objects beginning
        inputStream.seek(plane->properties.objectsOffset);

        for (i = 0; i < plane->objectsCount; i++)
        {
            inputStream.read(plane->objects[i].id,
                plane->objects[i].nameLength,
                plane->objects[i].logicLength,
                plane->objects[i].imageSetLength,
                plane->objects[i].soundLength,
                plane->objects[i].x,
                plane->objects[i].y,
                plane->objects[i].z,
                plane->objects[i].i,
                plane->objects[i].addFlags,
                plane->objects[i].dynamicFlags,
                plane->objects[i].drawFlags,
                plane->objects[i].userFlags,
                plane->objects[i].score,
                plane->objects[i].points,
                plane->objects[i].powerup,
                plane->objects[i].damage,
                plane->objects[i].smarts,
                plane->objects[i].health);

            ReadRect(inputStream, plane->objects[i].moveRect);
            ReadRect(inputStream, plane->objects[i].hitRect);
            ReadRect(inputStream, plane->objects[i].attackRect);
            ReadRect(inputStream, plane->objects[i].clipRect);
            ReadRect(inputStream, plane->objects[i].userRect1);
            ReadRect(inputStream, plane->objects[i].userRect2);

            inputStream.read(plane->objects[i].userValue1,
                plane->objects[i].userValue2,
                plane->objects[i].userValue3,
                plane->objects[i].userValue4,
                plane->objects[i].userValue5,
                plane->objects[i].userValue6,
                plane->objects[i].userValue7,
                plane->objects[i].userValue8,
                plane->objects[i].minX,
                plane->objects[i].minY,
                plane->objects[i].maxX,
                plane->objects[i].maxY,
                plane->objects[i].speedX,
                plane->objects[i].speedY,
                plane->objects[i].tweakX,
                plane->objects[i].tweakY,
                plane->objects[i].counter,
                plane->objects[i].speed,
                plane->objects[i].width,
                plane->objects[i].height,
                plane->objects[i].direction,
                plane->objects[i].faceDir,
                plane->objects[i].timeDelay,
                plane->objects[i].frameDelay,
                plane->objects[i].objectType,
                plane->objects[i].hitTypeFlags,
                plane->objects[i].moveResX,
                plane->objects[i].moveResY);

            plane->objects[i].name = ReadAndAllocateString(inputStream, plane->objects[i].nameLength);
            plane->objects[i].logic = ReadAndAllocateString(inputStream, plane->objects[i].logicLength);
            plane->objects[i].imageSet = ReadAndAllocateString(inputStream, plane->objects[i].imageSetLength);
            plane->objects[i].sound = ReadAndAllocateString(inputStream, plane->objects[i].soundLength);
            //cout << "Logic = " << plane->objects[i].logic << endl;
        }
    }
}

static void ReadPlanes(WapWwd* wapWwd, InputStream& inputStream)
{
    uint32_t i;

    // Allocate space for WWD planes
    wapWwd->planes = new WwdPlane[wapWwd->planesCount];

    // Move cursor to WWD planes beginning
    inputStream.seek(wapWwd->properties.planesOffset);

    // Read WWD planes
    for (i = 0; i < wapWwd->planesCount; i++)
    {
        // Set default values
        wapWwd->planes[i] = { 0 };

        /********************** PLANE'S PROPERTIES **********************/

        // Read plane's properties
        inputStream.read(wapWwd->planes[i].properties.signature,
            wapWwd->planes[i].properties.null0,
            wapWwd->planes[i].properties.flags,
            wapWwd->planes[i].properties.null1,
            wapWwd->planes[i].properties.name,
            wapWwd->planes[i].properties.pixelWidth,
            wapWwd->planes[i].properties.pixelHeight,
            wapWwd->planes[i].properties.tilePixelWidth,
            wapWwd->planes[i].properties.tilePixelHeight,
            wapWwd->planes[i].properties.tilesOnAxisX,
            wapWwd->planes[i].properties.tilesOnAxisY,
            wapWwd->planes[i].properties.null2,
            wapWwd->planes[i].properties.null3,
            wapWwd->planes[i].properties.movementPercentX,
            wapWwd->planes[i].properties.movementPercentY,
            wapWwd->planes[i].properties.fillColor,
            wapWwd->planes[i].properties.imageSetsCount,
            wapWwd->planes[i].properties.objectsCount,
            wapWwd->planes[i].properties.tilesOffset,
            wapWwd->planes[i].properties.imageSetsOffset,
            wapWwd->planes[i].properties.objectsOffset,
            wapWwd->planes[i].properties.coordZ,
            wapWwd->planes[i].properties.null4,
            wapWwd->planes[i].properties.null5,
            wapWwd->planes[i].properties.null6);

        // Data duplication for sanity reasons
        wapWwd->planes[i].tilesCount = wapWwd->planes[i].properties.tilesOnAxisX * wapWwd->planes[i].properties.tilesOnAxisY;
        wapWwd->planes[i].imageSetsCount = wapWwd->planes[i].properties.imageSetsCount;
        wapWwd->planes[i].objectsCount = wapWwd->planes[i].properties.objectsCount;
    }

    // Read plane tiles, image sets and objects
    for (i = 0; i < wapWwd->planesCount; i++)
    {
        /********************** PLANE'S TILES **********************/

        ReadPlaneTiles(&(wapWwd->planes[i]), inputStream);

        /********************** PLANE'S IMAGE SETS **********************/

        ReadPlaneImageSets(&(wapWwd->planes[i]), inputStream);

        /********************** PLANE'S OBJECTS **********************/

        ReadPlaneObjects(&(wapWwd->planes[i]), inputStream);
    }
}

void ReadTileDescriptions(WapWwd* wapWwd, InputStream& inputStream)
{
    uint32_t i;

    // Move cursor to WWD tile descriptions beginning
    inputStream.seek(wapWwd->properties.tileDescriptionsOffset);

    // Read count of tile descriptions along with junk values
    inputStream.read(32, 0, wapWwd->tileDescriptionsCount, 0, 0, 0, 0, 0);

    // Allocate space for WWD tile descriptions
    wapWwd->tileDescriptions = new WwdTileDescription[wapWwd->tileDescriptionsCount];

    for (i = 0; i < wapWwd->tileDescriptionsCount; i++)
    {
        inputStream.read(wapWwd->tileDescriptions[i].type,
            wapWwd->tileDescriptions[i].unk0,
            wapWwd->tileDescriptions[i].width,
            wapWwd->tileDescriptions[i].height);

        if (wapWwd->tileDescriptions[i].type == WAP_TILE_TYPE_SINGLE)
        {
            inputStream.read(wapWwd->tileDescriptions[i].insideAttrib);
        }
        else
        {
            inputStream.read(wapWwd->tileDescriptions[i].outsideAttrib,
                wapWwd->tileDescriptions[i].insideAttrib);

            ReadRect(inputStream, wapWwd->tileDescriptions[i].rect);
        }
    }
}

WapWwd* WAP_TryWwdLoadFromImpl(char* data, uint32_t length)
{
    WapWwd* wapWwd = NULL;

    if (data == NULL)
    {
        return NULL;
    }

    /********************** WWD HEADER/PROPERTIES **********************/

    wapWwd = new WapWwd;

    // Set default values
    (*wapWwd) = { 0 };

    InputStream wwdFileStream(data, length);
    wwdFileStream.read(wapWwd->properties.wwdSignature);
    // Signature holds WWD header size, if size doesnt match then it is not
    // supported WWD file
    if (wapWwd->properties.wwdSignature != EXPECTED_HEADER_SIZE)
    {
        delete wapWwd;
        return NULL;
    }

    wwdFileStream.read(wapWwd->properties.null0,
        wapWwd->properties.flags,
        wapWwd->properties.null1,
        wapWwd->properties.levelName,
        wapWwd->properties.author,
        wapWwd->properties.birth,
        wapWwd->properties.rezFile,
        wapWwd->properties.imageDirectoryPath,
        wapWwd->properties.rezPalettePath,
        wapWwd->properties.startX,
        wapWwd->properties.startY,
        wapWwd->properties.null2,
        wapWwd->properties.numPlanes,
        wapWwd->properties.planesOffset,
        wapWwd->properties.tileDescriptionsOffset,
        wapWwd->properties.mainBlockLength,
        wapWwd->properties.checksum,
        wapWwd->properties.null3,
        wapWwd->properties.launchApp,
        wapWwd->properties.imageSet1,
        wapWwd->properties.imageSet2,
        wapWwd->properties.imageSet3,
        wapWwd->properties.imageSet4,
        wapWwd->properties.prefix1,
        wapWwd->properties.prefix2,
        wapWwd->properties.prefix3,
        wapWwd->properties.prefix4);

    // Data duplication for sanity reasons
    wapWwd->planesCount = wapWwd->properties.numPlanes;

    // Compressed WWD file payload info
    const char* compressedMainBlock = data + wapWwd->properties.planesOffset;
    size_t compressedMainBlockSize = length - wapWwd->properties.planesOffset;

    // Uncompressed WWD file payload info
    std::vector<char> decompressedMainBlockVector(wapWwd->properties.planesOffset + wapWwd->properties.mainBlockLength);
    memcpy(decompressedMainBlockVector.data(), data, wapWwd->properties.planesOffset);
    char* decompressedMainBlock = decompressedMainBlockVector.data() + wapWwd->properties.planesOffset;

    // Inflate compressed WWD file payload
    int32_t ret = uncompress((Bytef*)decompressedMainBlock, (uLong*)(&(wapWwd->properties.mainBlockLength)),
        (Bytef*)compressedMainBlock, compressedMainBlockSize);
    // Check if inflation succeeded, if not, free allocated resources and return NULL
    if (ret != Z_OK)
    {
        delete wapWwd;
        return NULL;
    }

    // Create new file stream from inflated WWD file payload
    InputStream wwdFileStreamInflated(decompressedMainBlockVector.data(), decompressedMainBlockVector.size());

    /**************************************************************************************/
    /***************************** READ LEVEL INFORMATIONS ********************************/
    /**************************************************************************************/

    /********************** WWD PLANES **********************/

    ReadPlanes(wapWwd, wwdFileStreamInflated);

    /********************** WWD TILE DESCRIPTIONS **********************/

    ReadTileDescriptions(wapWwd, wwdFileStreamInflated);

    /*******************************************************************/

    return wapWwd;
}

WapWwd* WAP_WwdLoadFromData(char* data, uint32_t length)
{
    try
    {
        return WAP_TryWwdLoadFromImpl(data, length);
    }
    catch (...)
    {
        return NULL;
    }
}

WapWwd* WAP_WwdLoadFromFile(char* wwdFilePath)
{
    std::ifstream wwdFileStream(wwdFilePath, std::ios::binary);
    if (!wwdFileStream.is_open())
    {
        return NULL;
    }

    // Read whole file
    std::vector<char> wwdFileContents((std::istreambuf_iterator<char>(wwdFileStream)), std::istreambuf_iterator<char>());
    if (!wwdFileStream.good())
    {
        return NULL;
    }

    return WAP_WwdLoadFromData(wwdFileContents.data(), wwdFileContents.size());
}

WapWwd* WAP_WwdLoadFromRezFile(RezFile* rezFile)
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

    return WAP_WwdLoadFromData(data, rezFile->size);
}

WapWwd* WAP_WwdLoadFromRezArchive(RezArchive* rezArchive, const char* wwdFilePath)
{
    // Check input validity
    if ((rezArchive == NULL) || (wwdFilePath == NULL))
    {
        return NULL;
    }

    RezFile* wwdRezFile = WAP_GetRezFileFromRezArchive(rezArchive, wwdFilePath);
    if (wwdRezFile == NULL)
    {
        return NULL;
    }

    return WAP_WwdLoadFromRezFile(wwdRezFile);
}

RezDirectory* FindRezSubDirectory(RezDirectory* rezDirectory, char* searchedName)
{
    uint32_t i;
    RezDirectory* searchedDirectory = NULL;
    // Check for validity
    if (rezDirectory == NULL || rezDirectory->directoryContents == NULL)
    {
        return NULL;
    }
    // Find folder according to levelName
    for (i = 0; i < rezDirectory->directoryContents->rezDirectoriesCount; i++)
    {
        if (strcmp(searchedName, rezDirectory->directoryContents->rezDirectories[i]->name) == 0)
        {
            searchedDirectory = rezDirectory->directoryContents->rezDirectories[i];
            break;
        }
    }

    return searchedDirectory;
}

RezFile* FindRezFile(RezDirectory* rezDirectory, char* name, char* extension)
{
    uint32_t i;
    RezFile* searchedRezFile = NULL;
    // Check for validity
    if (rezDirectory == NULL || rezDirectory->directoryContents == NULL)
    {
        return NULL;
    }

    // Find folder according to levelName
    for (i = 0; i < rezDirectory->directoryContents->rezFilesCount; i++)
    {
        if ((strcmp(name, rezDirectory->directoryContents->rezFiles[i]->name) == 0) &&
            (strcmp(extension, rezDirectory->directoryContents->rezFiles[i]->extension) == 0))
        {
            searchedRezFile = rezDirectory->directoryContents->rezFiles[i];
            break;
        }
    }

    return searchedRezFile;
}

// Valgrind - "All heap blocks were freed -- no leaks are possible"
void WAP_WwdDestroy(WapWwd* wapWwd)
{
    uint32_t i;

    // Check for sanity
    if (wapWwd == NULL)
    {
        return;
    }

    // Delete planes
    for (i = 0; i < wapWwd->planesCount; i++)
    {
        uint32_t j;

        // Delete plane's image sets
        for (j = 0; j < wapWwd->planes[i].imageSetsCount; j++)
        {
            delete[] wapWwd->planes[i].imageSets[j];
        }
        delete[] wapWwd->planes[i].imageSets;

        // Delete plane's tiles
        delete[] wapWwd->planes[i].tiles;

        // Delete plane's object names
        for (j = 0; j < wapWwd->planes[i].objectsCount; j++)
        {
            delete[] wapWwd->planes[i].objects[j].name;
            delete[] wapWwd->planes[i].objects[j].logic;
            delete[] wapWwd->planes[i].objects[j].imageSet;
            delete[] wapWwd->planes[i].objects[j].sound;
        }

        // Delete plane's objects
        delete[] wapWwd->planes[i].objects;
    }
    // Delete planes itselves
    delete[] wapWwd->planes;

    // Delete tile descriptions
    delete[] wapWwd->tileDescriptions;

    // Delete WAP_WWD
    delete wapWwd;
    wapWwd = NULL;
}