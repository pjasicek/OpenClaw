#define CATCH_CONFIG_MAIN
#include "Catch.hpp"

#include <libwap.h>
#include "TestUtil.h"

TEST_CASE("----- REZ ARCHIVE FILE -----")
{
    SECTION("Loading file with wrong path to REZ archive returns NULL")
    {
        RezArchive* rezArchive = WAP_LoadRezArchive("NonexistandFilePath");

        REQUIRE(rezArchive == NULL);
    }
    
    SECTION("Loading invalid file format returns NULL")
    {
        RezArchive* rezArchive = WAP_LoadRezArchive("INVALID.REZ");

        REQUIRE(rezArchive == NULL);
    }
    
    SECTION("Loading valid REZ file returns valid REZ archive file structure")
    {
        // Official CLAW.REZ file
        RezArchive* rezArchive = WAP_LoadRezArchive("CLAW.REZ");

        // Simple checks
        REQUIRE(rezArchive != NULL);
        REQUIRE(rezArchive->rootDirectory != NULL);
        REQUIRE(rezArchive->rootDirectory->offset == 119321494);
        REQUIRE(rezArchive->rootDirectory->size == 392);
        REQUIRE(rezArchive->rootDirectory->directoryContents != NULL);
        REQUIRE(rezArchive->rootDirectory->directoryContents->rezDirectoriesCount == 17);
        REQUIRE(rezArchive->rootDirectory->directoryContents->rezFilesCount == 0);

        // File specific checks
        RezDirectory* currRezDirectory = rezArchive->rootDirectory;
        currRezDirectory = WAP_GetRezDirectoryFromRezDirectory(currRezDirectory, "LEVEL7");
        REQUIRE(currRezDirectory != NULL);
        REQUIRE(currRezDirectory->directoryContents != NULL);
        REQUIRE(currRezDirectory->directoryContents->rezDirectoriesCount == 8);
        REQUIRE(currRezDirectory->directoryContents->rezFilesCount == 0);

        currRezDirectory = WAP_GetRezDirectoryFromRezDirectory(currRezDirectory, "SCREENS");
        REQUIRE(currRezDirectory != NULL);
        REQUIRE(currRezDirectory->directoryContents != NULL);
        REQUIRE(currRezDirectory->directoryContents->rezDirectoriesCount == 0);
        REQUIRE(currRezDirectory->directoryContents->rezFilesCount == 1);

        RezFile* rezFile = WAP_GetRezFileFromRezDirectory(currRezDirectory, "LOADING.PCX");
        REQUIRE(rezFile != NULL);
        REQUIRE(rezFile->size == 262395);

        WAP_DestroyRezArchive(rezArchive);
    }

    SECTION("Getting invalid directory from valid REZ archive returns NULL")
    {
        // Official CLAW.REZ file
        RezArchive* rezArchive = WAP_LoadRezArchive("CLAW.REZ");

        REQUIRE(rezArchive != NULL);

        RezDirectory* rezDirectory = WAP_GetRezDirectoryFromRezArchive(rezArchive, "NONEXISTANT");
        REQUIRE(rezDirectory == NULL);

        WAP_DestroyRezArchive(rezArchive);
    }

    SECTION("Getting valid directory from invalid REZ archive returns NULL")
    {
        RezArchive* rezArchive = WAP_LoadRezArchive("INVALID.REZ");

        REQUIRE(rezArchive == NULL);

        RezDirectory* rezDirectory = WAP_GetRezDirectoryFromRezArchive(rezArchive, "LEVEL1");
        REQUIRE(rezDirectory == NULL);
    }

    SECTION("Getting valid directory from valid REZ archive returns valid directory structure")
    {
        // Official CLAW.REZ file
        RezArchive* rezArchive = WAP_LoadRezArchive("CLAW.REZ");

        REQUIRE(rezArchive != NULL);

        RezDirectory* rezDirectory = WAP_GetRezDirectoryFromRezArchive(rezArchive, "LEVEL1");
        REQUIRE(rezDirectory != NULL);
        REQUIRE(strcmp(rezDirectory->name, "level1") == 0);
        REQUIRE(rezDirectory->directoryContents != NULL);
        REQUIRE(rezDirectory->directoryContents->rezDirectoriesCount == 8);
        REQUIRE(rezDirectory->directoryContents->rezFilesCount == 0);

        rezDirectory = WAP_GetRezDirectoryFromRezArchive(rezArchive, "STATES/DIALOGS/IMAGES/CONFIGMACROS");
        REQUIRE(rezDirectory != NULL);
        REQUIRE(strcmp(rezDirectory->name, "configmacros") == 0);
        REQUIRE(rezDirectory->directoryContents != NULL);
        REQUIRE(rezDirectory->directoryContents->rezDirectoriesCount == 0);
        REQUIRE(rezDirectory->directoryContents->rezFilesCount == 1);

        rezDirectory = WAP_GetRezDirectoryFromRezArchive(rezArchive, "GAME/IMAGES/GAMEOVERMENU/TIME");
        REQUIRE(rezDirectory != NULL);
        REQUIRE(strcmp(rezDirectory->name, "time") == 0);
        REQUIRE(rezDirectory->directoryContents != NULL);
        REQUIRE(rezDirectory->directoryContents->rezDirectoriesCount == 0);
        REQUIRE(rezDirectory->directoryContents->rezFilesCount == 21);

        // Check special case - getting root directory
        rezDirectory = WAP_GetRezDirectoryFromRezArchive(rezArchive, "");
        REQUIRE(rezDirectory != NULL);

        rezDirectory = WAP_GetRezDirectoryFromRezArchive(rezArchive, NULL   );
        REQUIRE(rezDirectory != NULL);

        WAP_DestroyRezArchive(rezArchive);
    }

    SECTION("Getting valid directory from valid REZ directory returns valid directory structure")
    {
        // Official CLAW.REZ file
        RezArchive* rezArchive = WAP_LoadRezArchive("CLAW.REZ");

        REQUIRE(rezArchive != NULL);

        // Get root directory
        RezDirectory* rezDirectory = WAP_GetRezDirectoryFromRezArchive(rezArchive, NULL);
        REQUIRE(rezDirectory != NULL);

        rezDirectory = WAP_GetRezDirectoryFromRezDirectory(rezDirectory, "STATES/DIALOGS/IMAGES");
        REQUIRE(rezDirectory != NULL);
        REQUIRE(strcmp(rezDirectory->name, "images") == 0);
        REQUIRE(rezDirectory->directoryContents != NULL);
        REQUIRE(rezDirectory->directoryContents->rezDirectoriesCount == 8);
        REQUIRE(rezDirectory->directoryContents->rezFilesCount == 0);

        rezDirectory = WAP_GetRezDirectoryFromRezDirectory(rezDirectory, "MESSAGE");
        REQUIRE(rezDirectory != NULL);
        REQUIRE(strcmp(rezDirectory->name, "message") == 0);
        REQUIRE(rezDirectory->directoryContents != NULL);
        REQUIRE(rezDirectory->directoryContents->rezDirectoriesCount == 0);
        REQUIRE(rezDirectory->directoryContents->rezFilesCount == 1);

        WAP_DestroyRezArchive(rezArchive);
    }

    SECTION("Getting valid file from invalid REZ archive returns NULL")
    {
        RezArchive* rezArchive = WAP_LoadRezArchive("INVALID.REZ");

        REQUIRE(rezArchive == NULL);

        RezFile* rezFile = WAP_GetRezFileFromRezArchive(rezArchive, "CLAW/ANIS/CLIMB.ANI");
        REQUIRE(rezFile == NULL);
    }

    SECTION("Getting invalid file from valid REZ archive returns NULL")
    {
        // Official CLAW.REZ file
        RezArchive* rezArchive = WAP_LoadRezArchive("CLAW.REZ");

        REQUIRE(rezArchive != NULL);

        RezFile* rezFile = WAP_GetRezFileFromRezArchive(rezArchive, "CLAW/ANIS/INVALID.ANI");
        REQUIRE(rezFile == NULL);
    }

    SECTION("Getting valid file from valid REZ archive returns valid file structure")
    {
        // Official CLAW.REZ file
        RezArchive* rezArchive = WAP_LoadRezArchive("CLAW.REZ");

        REQUIRE(rezArchive != NULL);

        RezFile* rezFile = WAP_GetRezFileFromRezArchive(rezArchive, "CLAW/ANIS/CLIMB.ANI");
        REQUIRE(rezFile != NULL);
        REQUIRE(strcmp(rezFile->name, "climb") == 0);
        REQUIRE(strcmp(rezFile->extension, "ani") == 0);
        REQUIRE(rezFile->size == 283);

        
        rezFile = WAP_GetRezFileFromRezArchive(rezArchive, "STATES/DIALOGS/IMAGES/CONFIGMACROS/001_BACK.PID");
        REQUIRE(rezFile != NULL);
        REQUIRE(strcmp(rezFile->name, "001_back") == 0);
        REQUIRE(strcmp(rezFile->extension, "pid") == 0);
        REQUIRE(rezFile->size == 13494);
    }

    SECTION("Getting valid RezFile from valid RezDirectory returns valid file structure")
    {
        // Official CLAW.REZ file
        RezArchive* rezArchive = WAP_LoadRezArchive("CLAW.REZ");

        REQUIRE(rezArchive != NULL);

        RezDirectory* rezDirectory = WAP_GetRezDirectoryFromRezArchive(rezArchive, "/CLAW/ANIS");
        REQUIRE(rezDirectory != NULL);

        RezFile* rezFile = WAP_GetRezFileFromRezDirectory(rezDirectory, "CLIMB.ANI");
        REQUIRE(rezFile != NULL);

        rezDirectory = WAP_GetRezDirectoryFromRezArchive(rezArchive, "CLAW");
        REQUIRE(rezDirectory != NULL);

        rezDirectory = WAP_GetRezDirectoryFromRezArchive(rezArchive, "/CLAW");
        REQUIRE(rezDirectory != NULL);

        RezDirectory* subDir = WAP_GetRezDirectoryFromRezDirectory(rezDirectory, "ANIS");
        REQUIRE(subDir != NULL);

        subDir = WAP_GetRezDirectoryFromRezDirectory(rezDirectory, "/ANIS");
        REQUIRE(subDir != NULL);
    }

    SECTION("Getting file data from invalid REZ file returns NULL")
    {
        // Official CLAW.REZ file
        RezArchive* rezArchive = WAP_LoadRezArchive("CLAW.REZ");

        REQUIRE(rezArchive != NULL);

        RezFile* rezFile = WAP_GetRezFileFromRezArchive(rezArchive, "CLAW/ANIS/INVALID.ANI");
        REQUIRE(rezFile == NULL);

        char* data = WAP_GetRezFileData(rezFile);
        REQUIRE(data == NULL);
    }

    SECTION("Getting file data from valid REZ file returns valid data")
    {
        // Official CLAW.REZ file
        RezArchive* rezArchive = WAP_LoadRezArchive("CLAW.REZ");

        REQUIRE(rezArchive != NULL);

        RezFile* rezFile = WAP_GetRezFileFromRezArchive(rezArchive, "CLAW/ANIS/DUCKPISTOL.ANI");
        REQUIRE(rezFile != NULL);

        char* data = WAP_GetRezFileData(rezFile);
        REQUIRE(data != NULL);

        WapAni* aniFile = WAP_AniLoadFromData(data, rezFile->size);
        REQUIRE(aniFile != NULL);
        REQUIRE(aniFile->animationFramesCount == 11);
        REQUIRE(strcmp(aniFile->imageSetPath, "CLAW_IMAGES") == 0);
        REQUIRE(strlen(aniFile->imageSetPath) == 11);
        REQUIRE(aniFile->imageSetPathLength == 11);
        REQUIRE(aniFile->animationFrames[3].triggeredEventFlag == 2);
        REQUIRE(strcmp(aniFile->animationFrames[3].eventFilePath, "CLAW_GUNSHOT") == 0);
        REQUIRE(aniFile->animationFrames[10].unk6 == 152);
        REQUIRE(aniFile->animationFrames[10].unk7 == 24);

        WAP_FreeFileData(rezFile);
    }

    SECTION("Getting valid directory from valid REZ directory with non-compliant directory separator set returns NULL")
    {
        // Official CLAW.REZ file
        RezArchive* rezArchive = WAP_LoadRezArchive("CLAW.REZ");

        REQUIRE(rezArchive != NULL);

        // Get root directory
        RezDirectory* rezDirectory = WAP_GetRezDirectoryFromRezArchive(rezArchive, NULL);
        REQUIRE(rezDirectory != NULL);

        WAP_SetDirectorySeparator('\\');

        rezDirectory = WAP_GetRezDirectoryFromRezDirectory(rezDirectory, "STATES/DIALOGS/IMAGES");
        REQUIRE(rezDirectory == NULL);

        rezDirectory = WAP_GetRezDirectoryFromRezDirectory(rezDirectory, "MESSAGE");
        REQUIRE(rezDirectory == NULL);

        WAP_DestroyRezArchive(rezArchive);
        WAP_SetDirectorySeparator('/');
    }

    SECTION("Getting valid directory from valid REZ directory with compliant directory separator set returns valid directory")
    {
        // Official CLAW.REZ file
        RezArchive* rezArchive = WAP_LoadRezArchive("CLAW.REZ");

        REQUIRE(rezArchive != NULL);

        WAP_SetDirectorySeparator('\\');

        RezDirectory* rezDirectory = WAP_GetRezDirectoryFromRezArchive(rezArchive, "LEVEL1");
        REQUIRE(rezDirectory != NULL);
        REQUIRE(strcmp(rezDirectory->name, "level1") == 0);
        REQUIRE(rezDirectory->directoryContents != NULL);
        REQUIRE(rezDirectory->directoryContents->rezDirectoriesCount == 8);
        REQUIRE(rezDirectory->directoryContents->rezFilesCount == 0);

        rezDirectory = WAP_GetRezDirectoryFromRezArchive(rezArchive, "STATES\\DIALOGS\\IMAGES\\CONFIGMACROS");
        REQUIRE(rezDirectory != NULL);
        REQUIRE(strcmp(rezDirectory->name, "configmacros") == 0);
        REQUIRE(rezDirectory->directoryContents != NULL);
        REQUIRE(rezDirectory->directoryContents->rezDirectoriesCount == 0);
        REQUIRE(rezDirectory->directoryContents->rezFilesCount == 1);

        rezDirectory = WAP_GetRezDirectoryFromRezArchive(rezArchive, "GAME\\IMAGES\\GAMEOVERMENU\\TIME");
        REQUIRE(rezDirectory != NULL);
        REQUIRE(strcmp(rezDirectory->name, "time") == 0);
        REQUIRE(rezDirectory->directoryContents != NULL);
        REQUIRE(rezDirectory->directoryContents->rezDirectoriesCount == 0);
        REQUIRE(rezDirectory->directoryContents->rezFilesCount == 21);

        // Check special case - getting root directory
        rezDirectory = WAP_GetRezDirectoryFromRezArchive(rezArchive, "");
        REQUIRE(rezDirectory != NULL);

        rezDirectory = WAP_GetRezDirectoryFromRezArchive(rezArchive, NULL);
        REQUIRE(rezDirectory != NULL);

        WAP_DestroyRezArchive(rezArchive);
        WAP_SetDirectorySeparator('/');
    }

    SECTION("Getting count of rez files from rez archive is correct")
    {
        // Official CLAW.REZ file
        RezArchive* rezArchive = WAP_LoadRezArchive("CLAW.REZ");

        REQUIRE(rezArchive != NULL);

        uint32_t rezFilesCount = WAP_GetRezFilesCount(rezArchive);
    }
}

TEST_CASE("----- WWD FILE -----")
{
    SECTION("Loading WWD file with invalid path returns NULL")
    {
        WapWwd* wwdFile;

        wwdFile = WAP_WwdLoadFromFile("NonexistandFilePath");
        REQUIRE(wwdFile == NULL);

        // Offical Claw REZ archive
        RezArchive* rezArchive = WAP_LoadRezArchive("NONEXISTANT.REZ");
        REQUIRE(rezArchive == NULL);

        RezFile* rezFile = WAP_GetRezFileFromRezArchive(rezArchive, "LEVEL1/WORLDS/WORLD.WWD");
        REQUIRE(rezFile == NULL);

        wwdFile = WAP_WwdLoadFromRezFile(rezFile);
        REQUIRE(wwdFile == NULL);

        wwdFile = WAP_WwdLoadFromRezArchive(rezArchive, "LEVEL1/WORLDS/WORLD.WWD");
        REQUIRE(wwdFile == NULL);
    }

    SECTION("Loading WWD from within invalid REZ file returns NULL")
    {
        RezArchive* rezArchive = WAP_LoadRezArchive("INVALID.REZ");
        REQUIRE(rezArchive == NULL);

        RezFile* rezFile = WAP_GetRezFileFromRezArchive(rezArchive, "LEVEL1/WORLDS/INVALID.WWD");
        REQUIRE(rezFile == NULL);

        WapWwd* wwdFile = WAP_WwdLoadFromRezFile(rezFile);
        REQUIRE(wwdFile == NULL);

        wwdFile = WAP_WwdLoadFromRezArchive(rezArchive, "LEVEL1/WORLDS/WORLD.WWD");
        REQUIRE(wwdFile == NULL);
    }

    SECTION("[WAP_WwdLoadFromFile]: Loading valid WWD file from real WWD file path returns valid WWD structure")
    {
        WapWwd* wwdFile = WAP_WwdLoadFromFile("LEVEL1.WWD");

        REQUIRE(wwdFile != NULL);
        REQUIRE(wwdFile->planesCount == 3);
        REQUIRE(wwdFile->properties.wwdSignature == 1524);
        REQUIRE(wwdFile->properties.null0 == 0);
        REQUIRE(wwdFile->properties.null1 == 0);
        REQUIRE(wwdFile->properties.null2 == 0);
        REQUIRE(strcmp(wwdFile->properties.levelName, "Claw - Level 1") == 0);
        REQUIRE(wwdFile->planes[1].objectsCount == 1479);
        REQUIRE(wwdFile->planes[1].properties.tilePixelHeight == 64);
        REQUIRE(strcmp(wwdFile->planes[2].properties.name, "Front") == 0);
        
        WAP_WwdDestroy(wwdFile);
    }

    SECTION("[WAP_WwdLoadFromRezFile]: Loading valid WWD file from within REZ returns valid WWD structure")
    {
        // Offical Claw REZ archive
        RezArchive* rezArchive = WAP_LoadRezArchive("CLAW.REZ");
        REQUIRE(rezArchive != NULL);

        RezFile* rezFile = WAP_GetRezFileFromRezArchive(rezArchive, "LEVEL1/WORLDS/WORLD.WWD");
        REQUIRE(rezFile != NULL);

        WapWwd* wwdFile = WAP_WwdLoadFromRezFile(rezFile);
        REQUIRE(wwdFile != NULL);
        REQUIRE(wwdFile->planesCount == 3);
        REQUIRE(wwdFile->properties.wwdSignature == 1524);
        REQUIRE(wwdFile->properties.null0 == 0);
        REQUIRE(wwdFile->properties.null1 == 0);
        REQUIRE(wwdFile->properties.null2 == 0);
        REQUIRE(strcmp(wwdFile->properties.levelName, "Claw - Level 1") == 0);
        REQUIRE(wwdFile->planes[1].objectsCount == 1479);
        REQUIRE(wwdFile->planes[1].properties.tilePixelHeight == 64);
        REQUIRE(strcmp(wwdFile->planes[2].properties.name, "Front") == 0);

        WAP_WwdDestroy(wwdFile);
    }

    SECTION("[WAP_WwdLoadFromRezArchive]: Loading valid WWD file from within REZ archive returns valid WWD structure")
    {
        // Offical Claw REZ archive
        RezArchive* rezArchive = WAP_LoadRezArchive("CLAW.REZ");
        REQUIRE(rezArchive != NULL);

        WapWwd* wwdFile = WAP_WwdLoadFromRezArchive(rezArchive, "LEVEL1/WORLDS/WORLD.WWD");
        REQUIRE(wwdFile != NULL);
        REQUIRE(wwdFile->planesCount == 3);
        REQUIRE(wwdFile->properties.wwdSignature == 1524);
        REQUIRE(wwdFile->properties.null0 == 0);
        REQUIRE(wwdFile->properties.null1 == 0);
        REQUIRE(wwdFile->properties.null2 == 0);
        REQUIRE(strcmp(wwdFile->properties.levelName, "Claw - Level 1") == 0);
        REQUIRE(wwdFile->planes[1].objectsCount == 1479);
        REQUIRE(wwdFile->planes[1].properties.tilePixelHeight == 64);
        REQUIRE(strcmp(wwdFile->planes[2].properties.name, "Front") == 0);

        WAP_WwdDestroy(wwdFile);
    }
}

TEST_CASE("----- ANI FILE -----")
{
    SECTION("[WAP_AniLoadFromFile]: Loading ANI file with invalid path returns NULL")
    {
        WapAni* aniFile;

        aniFile = WAP_AniLoadFromFile("NonexistandFilePath");
        REQUIRE(aniFile == NULL);
    }

    SECTION("[WAP_AniLoadFromFile]: Loading invalid file format returns NULL")
    {
        WapAni* aniFile;

        aniFile = WAP_AniLoadFromFile("INVALID.REZ");
        REQUIRE(aniFile == NULL);
    }

    SECTION("[WAP_AniLoadFromFile]: Loading valid ANI file returns valid ANI structure")
    {
        WapAni* aniFile;

        aniFile = WAP_AniLoadFromFile("DUCKPISTOL.ANI");
        REQUIRE(aniFile != NULL);
        REQUIRE(aniFile->animationFramesCount == 11);
        REQUIRE(strcmp(aniFile->imageSetPath, "CLAW_IMAGES") == 0);
        REQUIRE(strlen(aniFile->imageSetPath) == 11);
        REQUIRE(aniFile->imageSetPathLength == 11);
        REQUIRE(aniFile->animationFrames[3].triggeredEventFlag == 2);
        REQUIRE(strcmp(aniFile->animationFrames[3].eventFilePath, "CLAW_GUNSHOT") == 0);
        REQUIRE(aniFile->animationFrames[10].unk6 == 152);
        REQUIRE(aniFile->animationFrames[10].unk7 == 24);

        WAP_AniDestroy(aniFile);
    }

    SECTION("[WAP_AniLoadFromRezFile]: Loading valid ANI file returns valid ANI structure")
    {
        // Offical Claw REZ archive
        RezArchive* rezArchive = WAP_LoadRezArchive("CLAW.REZ");
        REQUIRE(rezArchive != NULL);

        RezFile* rezFile = WAP_GetRezFileFromRezArchive(rezArchive, "CLAW/ANIS/DUCKPISTOL.ANI");
        REQUIRE(rezFile != NULL);

        WapAni* aniFile = WAP_AniLoadFromRezFile(rezFile);
        REQUIRE(aniFile != NULL);
        REQUIRE(aniFile->animationFramesCount == 11);
        REQUIRE(strcmp(aniFile->imageSetPath, "CLAW_IMAGES") == 0);
        REQUIRE(strlen(aniFile->imageSetPath) == 11);
        REQUIRE(aniFile->imageSetPathLength == 11);
        REQUIRE(aniFile->animationFrames[3].triggeredEventFlag == 2);
        REQUIRE(strcmp(aniFile->animationFrames[3].eventFilePath, "CLAW_GUNSHOT") == 0);
        REQUIRE(aniFile->animationFrames[10].unk6 == 152);
        REQUIRE(aniFile->animationFrames[10].unk7 == 24);

        WAP_AniDestroy(aniFile);
        WAP_DestroyRezArchive(rezArchive);
    }

    SECTION("[WAP_AniLoadFromRezArchive]: Loading valid ANI file returns valid ANI structure")
    {
        // Offical Claw REZ archive
        RezArchive* rezArchive = WAP_LoadRezArchive("CLAW.REZ");
        REQUIRE(rezArchive != NULL);

        WapAni* aniFile = WAP_AniLoadFromRezArchive(rezArchive, "CLAW/ANIS/DUCKPISTOL.ANI");
        REQUIRE(aniFile != NULL);
        REQUIRE(aniFile->animationFramesCount == 11);
        REQUIRE(strcmp(aniFile->imageSetPath, "CLAW_IMAGES") == 0);
        REQUIRE(strlen(aniFile->imageSetPath) == 11);
        REQUIRE(aniFile->imageSetPathLength == 11);
        REQUIRE(aniFile->animationFrames[3].triggeredEventFlag == 2);
        REQUIRE(strcmp(aniFile->animationFrames[3].eventFilePath, "CLAW_GUNSHOT") == 0);
        REQUIRE(aniFile->animationFrames[10].unk6 == 152);
        REQUIRE(aniFile->animationFrames[10].unk7 == 24);

        WAP_AniDestroy(aniFile);
        WAP_DestroyRezArchive(rezArchive);
    }
}

TEST_CASE("----- XMI FILE -----")
{
    SECTION("[WAP_XmiToMidiFromFile]: Converting XMI file with wrong path to XMI file returns NULL")
    {
        MidiFile* midiFile = NULL;

        midiFile = WAP_XmiToMidiFromFile("NonexistandFilePath");
        REQUIRE(midiFile == NULL);
    }

    SECTION("[WAP_XmiToMidiFromFile]: Converting XMI file from wrong XMI file returns NULL")
    {
        MidiFile* midiFile = NULL;

        midiFile = WAP_XmiToMidiFromFile("INVALID.REZ");
        REQUIRE(midiFile == NULL);
    }

    SECTION("[WAP_XmiToMidiFromFile]: Converting valid XMI file returns valid MIDI file structure")
    {
        MidiFile* midiFile = NULL;

        midiFile = WAP_XmiToMidiFromFile("PLAY.XMI");
        REQUIRE(midiFile != NULL);
        REQUIRE(midiFile->size == 43755);

        WAP_MidiDestroy(midiFile);
    }

    SECTION("[WAP_XmiToMidiFromRezFile]: Converting valid XMI file from valid RezFile returns valid MIDI file structure")
    {
        MidiFile* midiFile = NULL;

        // Offical Claw REZ archive
        RezArchive* rezArchive = WAP_LoadRezArchive("CLAW.REZ");
        REQUIRE(rezArchive != NULL);

        RezFile* rezFile = WAP_GetRezFileFromRezArchive(rezArchive, "/LEVEL1/MUSIC/PLAY.XMI");
        REQUIRE(rezFile != NULL);

        midiFile = WAP_XmiToMidiFromRezFile(rezFile);
        REQUIRE(midiFile != NULL);
        REQUIRE(midiFile->size == 43755);

        WAP_MidiDestroy(midiFile);
    }

    SECTION("[WAP_XmiToMidiFromRezArchive]: Converting XMI file from valid XMI path and valid RezArchive returns valid MIDI file structure")
    {
        MidiFile* midiFile = NULL;

        // Offical Claw REZ archive
        RezArchive* rezArchive = WAP_LoadRezArchive("CLAW.REZ");
        REQUIRE(rezArchive != NULL);

        midiFile = WAP_XmiToMidiFromRezArchive(rezArchive, "LEVEL1/MUSIC/PLAY.XMI");
        REQUIRE(midiFile != NULL);
        REQUIRE(midiFile->size == 43755);

        WAP_MidiDestroy(midiFile);
    }
}

TEST_CASE("----- PAL FILE -----")
{
    SECTION("[WAP_PalLoadFromFile]: Loading PAL from invalid file path returns NULL")
    {
        WapPal* wapPal = NULL;

        wapPal = WAP_PalLoadFromFile("NonexistandFilePath");
        REQUIRE(wapPal == NULL);
    }

    SECTION("[WAP_PalLoadFromFile]: Loading PAL from invalid file returns NULL")
    {
        WapPal* wapPal = NULL;

        wapPal = WAP_PalLoadFromFile("INVALID.REZ");
        REQUIRE(wapPal == NULL);
    }

    SECTION("[WAP_PalLoadFromData]: Loading PAL from valid data returns valid PAL structure")
    {
        WapPal* wapPal = WAP_PalLoadFromData((char*)test_palette, 768);
        REQUIRE(wapPal != NULL);
        
        bool valid = true;
        for (int i = 0; i < 256; i++)
        {
            int arrIdx = i * 3;
            if (wapPal->colors[i].r != test_palette[arrIdx + 0] ||
                wapPal->colors[i].g != test_palette[arrIdx + 1] ||
                wapPal->colors[i].b != test_palette[arrIdx + 2])
            {
                
                valid = false;
                break;
            }
        }

        REQUIRE(valid == true);

        WAP_PalDestroy(wapPal);
    }

    SECTION("[WAP_PalLoadFromRezFile]: Loading PAL from valid RezFile returns valid PAL structure")
    {
        WapPal* wapPal = NULL;

        // Offical Claw REZ archive
        RezArchive* rezArchive = WAP_LoadRezArchive("CLAW.REZ");
        REQUIRE(rezArchive != NULL);

        RezFile* rezFile = WAP_GetRezFileFromRezArchive(rezArchive, "LEVEL1/PALETTES/MAIN.PAL");
        REQUIRE(rezFile != NULL);

        wapPal = WAP_PalLoadFromRezFile(rezFile);
        REQUIRE(wapPal != NULL);

        WAP_PalDestroy(wapPal);
    }

    SECTION("[WAP_PalLoadFromRezArchive]: Loading PAL from valid RezArchive and valid rez archive path returns valid PAL structure")
    {
        WapPal* wapPal = NULL;

        // Offical Claw REZ archive
        RezArchive* rezArchive = WAP_LoadRezArchive("CLAW.REZ");
        REQUIRE(rezArchive != NULL);

        wapPal = WAP_PalLoadFromRezArchive(rezArchive, "LEVEL1/PALETTES/MAIN.PAL");
        REQUIRE(wapPal != NULL);

        WAP_PalDestroy(wapPal);
    }
}