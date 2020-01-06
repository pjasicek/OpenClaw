#ifndef LIBWAP_H_
#define LIBWAP_H_

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// change this condition if you want to use an external dll file in runtime
//#ifdef _WIN32
#if 0
#ifdef LIBWAP_EXPORTS
#define LIBWAP_API __declspec(dllexport)
#else
#define LIBWAP_API __declspec(dllimport)
#endif //LIBWAP_EXPORTS
#else
#define LIBWAP_API
#endif

#include <stdint.h>
#include <stdlib.h>

/***************************************************************/
/********************* REZ FORMAT ******************************/
/***************************************************************/

typedef struct RezArchive RezArchive;
typedef struct RezDirectory RezDirectory;

typedef struct
{
    char* fullPathAndName;
    char* name;
    char extension[4];
    uint32_t size;
    uint32_t offset;
    uint32_t dateAndTime;
    uint32_t fileId;

    RezDirectory* parent;
    RezArchive* owner;
} RezFile;

typedef struct RezDirectoryContents RezDirectoryContents;

typedef struct RezDirectory
{
    RezDirectoryContents* directoryContents;
    char* name;
    uint32_t dateAndTime;
    uint32_t size;
    uint32_t offset;

    RezDirectory* parent;
} RezDirectory;

typedef struct RezDirectoryContents
{
    RezDirectory** rezDirectories;
    uint16_t rezDirectoriesCount;

    RezFile** rezFiles;
    uint16_t rezFilesCount;
} RezDirectoryContents;

typedef struct RezArchive
{
    RezDirectory* rootDirectory;
    char header[127];
    uint32_t version;
} RezArchive;

/**
 * @brief Loads file structure of REZ format archive
 * @note For destroying use supplied function WAP_DestroyRezArchive
 *
 * @param rezFilePath Path to REZ archive
 * @return Returns pointer to RezArchive struct or NULL upon failure
 */
LIBWAP_API RezArchive* WAP_LoadRezArchive(const char* rezFilePath);

/**
 * @brief Destroys RezArchive structure and thus frees memory
 *
 * @param rezArchive Pointer to REZ archive to be destroyed
 */
LIBWAP_API void WAP_DestroyRezArchive(RezArchive* rezArchive);

/**
 * @brief Sets directory separator to be used when traversing REZ directories
 * @note Default separator is '/'
 * @note Use this function only if you know what you are doing
 *
 * @param separator Directory separator to be used 
 */
LIBWAP_API void WAP_SetDirectorySeparator(uint8_t separator);

/**
 * @brief Gets directory separator which is used for traversing REZ directories
 * @note Default separator is '/'
 * @note Use this function only if you know what you are doing
 *
 * @return Returns currently used directory separator
 */
LIBWAP_API uint8_t WAP_GetDirectorySeparator();

/**
 * @brief Gets file content (data buffer) from given RezFile
 * @note All REZ file datas allocated by this function are automatically freed upon destroying RezArchive
 *
 * @param rezFile Given pointer to RezFile structure
 * @return Pointer to RezFile structure or NULL upon failure
 */
LIBWAP_API char* WAP_GetRezFileData(RezFile* rezFile);

/**
 * @brief Frees data buffer allocated by WAP_GetRezFileData function
 * @note All REZ file datas allocated by this function are automatically freed upon destroying RezArchive
 *
 * @param rezFile Pointer to RezFile structure
 */
LIBWAP_API void WAP_FreeFileData(RezFile* rezFile);

/**
 * @brief Gets RezFile from given RezArchive and path to the RezFile
 * @note if rezFilePath is NULL or empty string (""), root directory is returned
 * @usage RezFile* rezFile = WAP_GetRezFileFromRezArchive(rezArchive, "CLAW/IMAGES/001.PID");
 *
 * @param rezArchive REZ archive in which the search is done
 * @param rezFilepath Full path from RezArchive root directory to file, e.g. LEVEL1/WORLDS/WORLD.WWD
 * @return Pointer to RezFile structure or NULL upon failure
 */
LIBWAP_API RezFile* WAP_GetRezFileFromRezArchive(RezArchive* rezArchive, const char* rezFilePath);

/**
 * @brief Gets RezFile from given RezDirectory and path to the RezFile
 * @usage RezFile* rezFile = WAP_GetRezFileFromRezDirectory(rezDirectory, "FOLDER/SUBFOLDER/FILE.EXT");
 *
 * @param rezDirectory REZ directory in which the search is done
 * @param rezFilepath Full path from RezDirectory to file, e.g. WORLDS/WORLD.WWD
 * @return Pointer to RezFile structure or NULL upon failure
 */
LIBWAP_API RezFile* WAP_GetRezFileFromRezDirectory(RezDirectory* rezDirectory, const char* rezFilePath);

/**
 * @brief Gets RezDirectory from given RezArchive and path to the searched RezDirectory
 * @usage RezDirectory* rezDirectory = WAP_GetRezFileFromRezArchive(rezArchive, "CLAW/IMAGES");
 *
 * @param rezArchive REZ directory in which the search is done
 * @param rezDirectoryPath Full path from RezArchive root directory to file
 * @return Pointer to RezDirectory structure or NULL upon failure
 */
LIBWAP_API RezDirectory* WAP_GetRezDirectoryFromRezArchive(RezArchive* rezArchive, const char* rezDirectoryPath);

/**
 * @brief Gets RezDirectory from given RezDirectory and path to the searched RezDirectory
 * @usage RezDirectory* rezDirectory = WAP_GetRezFileFromRezArchive(rezArchive, "FOLDER/SUBFOLDER");
 *
 * @param rezDirectory REZ directory in which the search is done
 * @param rezDirectoryPath Full path from RezDirectory to file
 * @return Pointer to RezDirectory structure or NULL upon failure
 */
LIBWAP_API RezDirectory* WAP_GetRezDirectoryFromRezDirectory(RezDirectory* rezDirectory, const char* rezDirectoryPath);

LIBWAP_API RezFile* WAP_GetRezFileFromFileIdx(RezArchive* rezArchive, uint32_t rezFileIdx);
LIBWAP_API uint32_t WAP_GetRezFilesCount(RezArchive* rezArchive);

/***************************************************************/
/********************* WWD FORMAT ******************************/
/***************************************************************/

typedef enum 
{
    WAP_WWD_FLAG_USE_Z_COORDS = 1 << 0,
    WAP_WWD_FLAG_COMPRESS = 1 << 1,
} WwdFlags;

typedef enum 
{
    WAP_PLANE_FLAG_MAIN_PLANE = 1 << 0,
    WAP_PLANE_FLAG_NO_DRAW = 1 << 1,
    WAP_PLANE_FLAG_X_WRAPPING = 1 << 2,
    WAP_PLANE_FLAG_Y_WRAPPING = 1 << 3,
    WAP_PLANE_FLAG_AUTO_TILE_SIZE = 1 << 4,
} WwdPlaneFlags;

typedef enum 
{
    WAP_OBJECT_ADD_FLAG_DIFFICULT = 1 << 0,
    WAP_OBJECT_ADD_FLAG_EYE_CANDY = 1 << 1,
    WAP_OBJECT_ADD_FLAG_HIGH_DETAIL = 1 << 2,
    WAP_OBJECT_ADD_FLAG_MULTIPLAYER = 1 << 3,
    WAP_OBJECT_ADD_FLAG_EXTRA_MEMORY = 1 << 4,
    WAP_OBJECT_ADD_FLAG_FAST_CPU = 1 << 5,
} WwdObjectAddFlags;

typedef enum 
{
    WAP_OBJECT_DRAW_FLAG_NO_DRAW = 1 << 0,
    WAP_OBJECT_DRAW_FLAG_MIRROR = 1 << 1,
    WAP_OBJECT_DRAW_FLAG_INVERT = 1 << 2,
    WAP_OBJECT_DRAW_FLAG_FLASH = 1 << 3,
} WwdObjectDrawFlags;

typedef enum 
{
    WAP_OBJECT_DYNAMIC_FLAG_NO_HIT = 1 << 0,
    WAP_OBJECT_DYNAMIC_FLAG_ALWAYS_ACTIVE = 1 << 1,
    WAP_OBJECT_DYNAMIC_FLAG_SAFE = 1 << 2,
    WAP_OBJECT_DYNAMIC_FLAG_AUTO_HIT_DAMAGE = 1 << 3,
} WwdObjectDynamicFlags;

typedef enum 
{
    WAP_OBJECT_USER_FLAG_1 = 1 << 0,
    WAP_OBJECT_USER_FLAG_2 = 1 << 1,
    WAP_OBJECT_USER_FLAG_3 = 1 << 2,
    WAP_OBJECT_USER_FLAG_4 = 1 << 3,
    WAP_OBJECT_USER_FLAG_5 = 1 << 4,
    WAP_OBJECT_USER_FLAG_6 = 1 << 5,
    WAP_OBJECT_USER_FLAG_7 = 1 << 6,
    WAP_OBJECT_USER_FLAG_8 = 1 << 7,
    WAP_OBJECT_USER_FLAG_9 = 1 << 8,
    WAP_OBJECT_USER_FLAG_10 = 1 << 9,
    WAP_OBJECT_USER_FLAG_11 = 1 << 10,
    WAP_OBJECT_USER_FLAG_12 = 1 << 11,
} WwdObjectUserFlags;

typedef enum 
{
    WAP_OBJECT_TYPE_GENERIC = 1 << 0,
    WAP_OBJECT_TYPE_PLAYER = 1 << 1,
    WAP_OBJECT_TYPE_ENEMY = 1 << 2,
    WAP_OBJECT_TYPE_POWERUP = 1 << 3,
    WAP_OBJECT_TYPE_SHOT = 1 << 4,
    WAP_OBJECT_TYPE_PSHOT = 1 << 5,
    WAP_OBJECT_TYPE_ESHOT = 1 << 6,
    WAP_OBJECT_TYPE_SPECIAL = 1 << 7,
    WAP_OBJECT_TYPE_USER1 = 1 << 8,
    WAP_OBJECT_TYPE_USER2 = 1 << 9,
    WAP_OBJECT_TYPE_USER3 = 1 << 10,
    WAP_OBJECT_TYPE_USER4 = 1 << 11,
} WwdObjectTypeFlags;

typedef enum 
{
    WAP_TILE_TYPE_SINGLE = 1,
    WAP_TILE_TYPE_DOUBLE = 2,
} WwdTileTypeFlags;

typedef enum 
{
    WAP_TILE_ATTRIBUTE_CLEAR,
    WAP_TILE_ATTRIBUTE_SOLID = 1,
    WAP_TILE_ATTRIBUTE_GROUND = 2,
    WAP_TILE_ATTRIBUTE_CLIMB = 3,
    WAP_TILE_ATTRIBUTE_DEATH = 4,
} WwdTileAttributeFlags;

typedef struct 
{
    uint32_t left;
    uint32_t top;
    uint32_t right;
    uint32_t bottom;
} WwdRect;

typedef struct 
{
    uint32_t signature;
    uint32_t null0;
    uint32_t flags; /* WAP_PLANE_FLAG_ flags */
    uint32_t null1;
    char name[64]; /* WapWorld expects this char buffer to be null-terminated */
    uint32_t pixelWidth;
    uint32_t pixelHeight;
    uint32_t tilePixelWidth;  /* tile's width in pixels */
    uint32_t tilePixelHeight; /* tile's height in pixels */
    uint32_t tilesOnAxisX;
    uint32_t tilesOnAxisY;
    uint32_t null2;
    uint32_t null3;
    int32_t movementPercentX;
    int32_t movementPercentY;
    uint32_t fillColor;
    uint32_t imageSetsCount;
    uint32_t objectsCount;
    uint32_t tilesOffset;
    uint32_t imageSetsOffset;
    uint32_t objectsOffset;
    int32_t coordZ;
    uint32_t null4;
    uint32_t null5;
    uint32_t null6;
} WwdPlaneProperties;

typedef struct 
{
    uint32_t type;          /* WAP_TILE_TYPE_ single value */
    uint32_t unk0;          /* unknown value */
    uint32_t width;         /* in pixels */
    uint32_t height;        /* in pixels */
    uint32_t insideAttrib; /* WAP_TILE_ATTRIBUTE_ */
    /* outside_attrib and rect only if type == WAP_TILE_TYPE_DOUBLE */
    uint32_t outsideAttrib; /* WAP_TILE_ATTRIBUTE_ */
    WwdRect rect;
} WwdTileDescription;


typedef struct 
{
    int32_t id; /* Any value is accepted by WapWorld, but a good id should be
                positive and unique. */
    uint32_t nameLength;
    uint32_t logicLength;
    uint32_t imageSetLength;
    uint32_t soundLength;
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t i;
    uint32_t addFlags;     /* WAP_OBJECT_ADD_FLAG_ flags */
    uint32_t dynamicFlags; /* WAP_OBJECT_DYNAMIC_FLAG_ flags */
    uint32_t drawFlags;    /* WAP_OBJECT_DRAW_FLAG_ flags */
    uint32_t userFlags;    /* WAP_OBJECT_USER_FLAG_ flags */
    int32_t score;
    int32_t points;
    int32_t powerup;
    int32_t damage;
    int32_t smarts;
    int32_t health;
    WwdRect moveRect;
    WwdRect hitRect;
    WwdRect attackRect;
    WwdRect clipRect;
    WwdRect userRect1;
    WwdRect userRect2;
    int32_t userValue1;
    int32_t userValue2;
    int32_t userValue3;
    int32_t userValue4;
    int32_t userValue5;
    int32_t userValue6;
    int32_t userValue7;
    int32_t userValue8;
    int32_t minX;
    int32_t minY;
    int32_t maxX;
    int32_t maxY;
    int32_t speedX;
    int32_t speedY;
    int32_t tweakX;
    int32_t tweakY;
    int32_t counter;
    int32_t speed;
    int32_t width;
    int32_t height;
    int32_t direction;
    int32_t faceDir;
    int32_t timeDelay;
    int32_t frameDelay;
    uint32_t objectType;    /* WAP_OBJECT_TYPE_ single value */
    uint32_t hitTypeFlags; /* WAP_OBJECT_TYPE_ flags */
    int32_t moveResX;
    int32_t moveResY;
    char* name;
    char* logic;
    char* imageSet;
    char* sound;
} WwdObject;

typedef struct 
{
    uint32_t tilesWide;
    uint32_t tilesHigh;
    WwdPlaneProperties properties;

    int32_t* tiles;
    uint32_t tilesCount;

    char** imageSets;
    uint32_t imageSetsCount;

    WwdObject* objects;
    uint32_t objectsCount;
} WwdPlane;

typedef struct 
{
    uint32_t wwdSignature;
    uint32_t null0;
    uint32_t flags; /* WAP_WWD_FLAG_ flags  */
    uint32_t null1;
    /* WapWorld expects all these char buffers to be null-terminated */
    char levelName[64];
    char author[64];
    char birth[64];
    char rezFile[256];
    char imageDirectoryPath[128];
    char rezPalettePath[128];
    int32_t startX;
    int32_t startY;
    uint32_t null2;
    uint32_t numPlanes;
    uint32_t planesOffset;
    uint32_t tileDescriptionsOffset;
    uint32_t mainBlockLength;
    uint32_t checksum;
    uint32_t null3;
    char launchApp[128];
    char imageSet1[128];
    char imageSet2[128];
    char imageSet3[128];
    char imageSet4[128];
    char prefix1[32];
    char prefix2[32];
    char prefix3[32];
    char prefix4[32];
} WwdProperties;

typedef struct
{
    WwdProperties properties;

    WwdPlane* planes;
    uint32_t planesCount;

    WwdTileDescription* tileDescriptions;
    uint32_t tileDescriptionsCount;
} WapWwd;

/**
 * @brief Loads WWD file (= file describing level) from given data buffer
 *
 * @param data Data buffer
 * @param length Data buffer length/size
 * @return Pointer to structure describing loaded level or NULL upon failure
 */
LIBWAP_API WapWwd* WAP_WwdLoadFromData(char* data, uint32_t length);

/**
 * @brief Loads WWD file (= file describing level) from given path to WWD file
 *
 * @param wwdFilePath Filesystem path to WWD file
 * @return Pointer to structure describing loaded level or NULL upon failure
 */
LIBWAP_API WapWwd* WAP_WwdLoadFromFile(char* wwdFilePath);

/**
 * @brief Loads WWD file (= file describing level) from given REZ file
 *
 * @param rezFile Pointer to REZ file description structure
 * @return Pointer to structure describing loaded level or NULL upon failure
 */
LIBWAP_API WapWwd* WAP_WwdLoadFromRezFile(RezFile* rezFile);

/**
 * @brief Loads WWD file (= file describing level) from given path to REZ archive file and full path to WWD file
 * @usage WapWwd* wapWwd = WAP_WwdLoadFromRezArchive(rezArchive, "LEVEL1/WORLDS/WORLD.WWD");
 *
 * @param rezArchive Pointer to REZ archive file structure
 * @param wwdFilePath Full path to WWD file from within REZ archive root directory
 * @return Pointer to structure describing loaded level or NULL upon failure
 */
LIBWAP_API WapWwd* WAP_WwdLoadFromRezArchive(RezArchive* rezArchive, const char* wwdFilePath);

/**
 * @brief Destroys and frees loaded WWD file
 *
 * @param wapWwd Pointer to WWD structure
 */
LIBWAP_API void WAP_WwdDestroy(WapWwd* wapWwd);


/***************************************************************/
/********************* ANI FORMAT ******************************/
/***************************************************************/

typedef struct
{
    uint16_t triggeredEventFlag;
    uint16_t unk0;
    uint16_t unk1;
    uint16_t unk2;
    uint16_t imageFileId;
    uint16_t duration;
    uint16_t unk3;
    uint16_t unk4;
    uint16_t unk5;
    uint8_t unk6;
    uint8_t unk7;
    char* eventFilePath;
} AniAnimationFrame;

typedef struct
{
    uint32_t signature;
    uint32_t unk0;
    uint32_t unk1;
    uint32_t animationFramesCount;
    uint32_t imageSetPathLength;
    char* imageSetPath;
    uint32_t unk2;
    uint32_t unk3;
    uint32_t unk4;

    AniAnimationFrame* animationFrames;
} WapAni;


/**
 * @brief Loads ANI file (= 2D animation description file) from given data buffer
 *
 * @param data Data buffer
 * @param length Data buffer length/size
 * @return Pointer to structure describing loaded 2D Animation or NULL upon failure
 */
LIBWAP_API WapAni* WAP_AniLoadFromData(char* data, size_t size);

/**
 * @brief Loads ANI file (= 2D animation description file) from given path to WWD file
 *
 * @param aniFilePath Filesystem path to ANI file
 * @return Pointer to structure describing loaded 2D Animation or NULL upon failure
 */
LIBWAP_API WapAni* WAP_AniLoadFromFile(char* aniFilePath);

/**
 * @brief Loads ANI file (= 2D animation description file) from REZ archive and its location within REZ archive
 *
 * @param rezFile Pointer to REZ file description structure
 * @return Pointer to structure describing loaded 2D Animation or NULL upon failure
 */
LIBWAP_API WapAni* WAP_AniLoadFromRezFile(RezFile* rezFile);

/**
 * @brief Loads ANI file (= 2D animation description file) from REZ archive and its location within REZ archive
 * @usage WapAni* wapAni = WAP_WwdLoadFromRezArchive(rezArchive, "CLAW/ANIS/DUCKPISTOL.ANI");
 *
 * @param rezArchive Pointer to REZ archive file structure
 * @param aniFilePath Full path to ANI file from within REZ archive root directory
 * @return Pointer to structure describing loaded 2D Animation or NULL upon failure
 */
LIBWAP_API WapAni* WAP_AniLoadFromRezArchive(RezArchive* rezArchive, const char* aniFilePath);

/**
 * @brief Destroys and frees loaded ANI file
 *
 * @param wapAni Pointer to ANI structure
 */
LIBWAP_API void WAP_AniDestroy(WapAni* wapAni);


/***************************************************************/
/********************* XMI FORMAT ******************************/
/***************************************************************/

typedef struct
{
    char* data;
    size_t size;
} MidiFile;

/**
 * @brief Converts XMI music file format data to MIDI music file format data from given XMI data and length
 *
 * @param xmiData XMI music data buffer
 * @param xmiLength XMI music data length
 * @return Pointer to converted MIDI file structure or NULL upon failure
 */
LIBWAP_API MidiFile* WAP_XmiToMidiFromData(char* xmiData, size_t xmiLength);

/**
 * @brief Converts XMI music file format data to MIDI music file format data from given filesystem path to XMI file
 *
 * @param xmiFilePath Filesystem path to XMI file
 * @return Pointer to converted MIDI file structure  or NULL upon failure
 */
LIBWAP_API MidiFile* WAP_XmiToMidiFromFile(const char* xmiFilePath);

/**
 * @brief Converts XMI music file format data to MIDI music file format data from given pointer to RezFile
 *
 * @param rezFile Pointer to REZ file description structure
 * @return Pointer to converted MIDI file structure  or NULL upon failure
 */
LIBWAP_API MidiFile* WAP_XmiToMidiFromRezFile(RezFile* rezFile);

/**
 * @brief Converts XMI music file format data to MIDI music file format data from given pointer to RezArchive and its path within RezArchive
 * 
 * @param rezArchive Pointer to REZ archive file structure
 * @param xmiFilePath Path to XMI file from within given RezArchive
 * @return Pointer to converted MIDI file structure  or NULL upon failure
 */
LIBWAP_API MidiFile* WAP_XmiToMidiFromRezArchive(RezArchive* rezArchive, char* xmiFilePath);

/**
 * @brief Destroys and frees loaded MIDI file
 *
 * @param midiFile Pointer to MIDI file structure
 */
LIBWAP_API void WAP_MidiDestroy(MidiFile* midiFile);


/***************************************************************/
/********************* PAL FORMAT ******************************/
/***************************************************************/

const uint32_t WAP_PALETTE_SIZE_BYTES = 768;
// Only RGB are stored, not alpha
const uint32_t WAP_COLORS_IN_PALETTE = WAP_PALETTE_SIZE_BYTES / 3;

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} WAP_ColorRGBA;

// WAP palette is 8bit - 256 colors
typedef struct
{
    WAP_ColorRGBA colors[256];
} WapPal;

/**
 * @brief Loads PAL color palette from given data buffer
 *
 * @param data PAL data buffer
 * @param size PAL data length
 * @return Pointer to RGBA Color palette
 */
LIBWAP_API WapPal* WAP_PalLoadFromData(char* data, size_t size);

/**
 * @brief Loads PAL color palette from filesystem's file path
 *
 * @param palFilePath Filesystem path to PAL file
 * @return Pointer to RGBA Color palette
 */
LIBWAP_API WapPal* WAP_PalLoadFromFile(const char* palFilePath);

/**
 * @brief Loads PAL color palette from given pointer to RezFile
 *
 * @param Pointer to REZ file description structure
 * @return Pointer to RGBA Color palette
 */
LIBWAP_API WapPal* WAP_PalLoadFromRezFile(RezFile* rezFile);

/**
 * @brief Loads PAL color palette from given pointer to RezArchive and path to PAL file from root
 *
 * @param Pointer to REZ file description structure
 * @param palRezPath Path to PAL file from within given RezArchive root directory
 * @return Pointer to RGBA Color palette
 */
LIBWAP_API WapPal* WAP_PalLoadFromRezArchive(RezArchive* rezArchive, const char* palRezPath);

/**
* @brief Destroys and frees loaded PAL file
*
* @param midiFile Pointer to PAL palette
*/
LIBWAP_API void WAP_PalDestroy(WapPal* wapPal);

/***************************************************************/
/********************* PID FORMAT ******************************/
/***************************************************************/

enum
{
    WAP_PID_FLAG_TRANSPARENCY = 1 << 0,
    WAP_PID_FLAG_VIDEO_MEMORY = 1 << 1,
    WAP_PID_FLAG_SYSTEM_MEMORY = 1 << 2,
    WAP_PID_FLAG_MIRROR = 1 << 3,
    WAP_PID_FLAG_INVERT = 1 << 4,
    WAP_PID_FLAG_COMPRESSION = 1 << 5,
    WAP_PID_FLAG_LIGHTS = 1 << 6,
    WAP_PID_FLAG_EMBEDDED_PALETTE = 1 << 7,
};

typedef struct
{
    uint32_t fileDesc;
    uint32_t flags;
    uint32_t width;
    uint32_t height;
    int32_t offsetX;
    int32_t offsetY;
    uint32_t unk0;
    uint32_t unk1;

    WAP_ColorRGBA* colors; 
    uint32_t colorsCount; //< Count of colors calculated as width*height
} WapPid;

/**
 * @brief Loads PID file (= 2D image format) from given data buffer
 * @note If PID has embedded palette, embedded palette always takes preference
 *
 * @param data PID data buffer
 * @param size PID data length
 * @param palette Color palette to be used when decoding PID image. Pass NULL if you want to use embedded palette.
 * @return Pointer to PID file structure or NULL upon failure
 */
LIBWAP_API WapPid* WAP_PidLoadFromData(char* data, size_t size, WapPal* palette);

/**
 * @brief Loads PID file (= 2D image format) from filesystem's file path
 * @note If PID has embedded palette, embedded palette always takes preference
 *
 * @param pidFilePath Filesystem path to PID file
 * @param palette Color palette to be used when decoding PID image. Pass NULL if you want to use embedded palette.
 * @returnPointer to PID file structure or NULL upon failure
 */
LIBWAP_API WapPid* WAP_PidLoadFromFile(const char* pidFilePath, WapPal* palette);

/**
 * @brief Loads PID file (= 2D image format) from given pointer to RezFile
 * @note If PID has embedded palette, embedded palette always takes preference
 *
 * @param Pointer to REZ file description structure
 * @param palette Color palette to be used when decoding PID image. Pass NULL if you want to use embedded palette.
 * @return Pointer to PID file structure or NULL upon failure
 */
LIBWAP_API WapPid* WAP_PidLoadFromRezFile(RezFile* rezFile, WapPal* palette);

/**
 * @brief Loads PID file (= 2D image format) from given pointer to RezArchive and path to PID file from root
 * @note If PID has embedded palette, embedded palette always takes preference
 *
 * @param Pointer to REZ file description structure
 * @param pidRezPath Path to PID file from within given RezArchive root directory
 * @param palette Color palette to be used when decoding PID image. Pass NULL if you want to use embedded palette.
 * @return Pointer to PID file structure or NULL upon failure
 */
LIBWAP_API WapPid* WAP_PidLoadFromRezArchive(RezArchive* rezArchive, const char* pidRezPath, WapPal* palette);

/**
* @brief Destroys and frees loaded PID image file
*
* @param midiFile Pointer to PID image file
*/
LIBWAP_API void WAP_PidDestroy(WapPid* wapPid);


#ifdef __cplusplus
}
#endif

#endif //LIBWAP_H_
