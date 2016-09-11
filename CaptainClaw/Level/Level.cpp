#include <libwap.h>

#include <algorithm>

#include "Level.h"
#include "../Util/Util.h"
#include "../Game.h"
#include "../Image.h"
#include "Plane.h"

#include <iostream>
using namespace std;

Level::Level(LoadLevelInfo* loadLevelInfo, Game* game)
{
    assert(loadLevelInfo != NULL);
    assert(game != NULL);
    assert((loadLevelInfo->levelNumber > 0) && 
        (loadLevelInfo->levelNumber <= MAXIMUM_LEVEL));

    _game = game;

    _levelNumber = loadLevelInfo->levelNumber;

    RezArchive* clawRezArchive = game->GetRezArchive();
    SDL_Renderer* renderer = game->GetRenderer();
    LevelResourcePaths paths = GetResourcePaths(loadLevelInfo->levelNumber);

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Loading level %u ...", loadLevelInfo->levelNumber);

    WapWwd* wwd = WAP_WwdLoadFromRezArchive(clawRezArchive, paths.wwdPath.c_str());
    WapPal* palette = WAP_PalLoadFromRezArchive(clawRezArchive, paths.palettePath.c_str());

    assert(wwd != NULL);
    assert(palette != NULL);

    _levelName = std::string(wwd->properties.levelName);

    //---------- Tile prototypes

    char rezDirDelim = WAP_GetDirectorySeparator();
    std::string tileBaseDirPath(wwd->properties.imageDirectoryPath);
    std::replace(tileBaseDirPath.begin(), tileBaseDirPath.end(), '\\', rezDirDelim);
    cout << "path: " << tileBaseDirPath << endl;

    RezDirectory* tileBaseDir = WAP_GetRezDirectoryFromRezArchive(clawRezArchive, tileBaseDirPath.c_str());
    assert(tileBaseDir != NULL);

    uint32_t tileDescCount = wwd->tileDescriptionsCount;
    for (uint32_t tileId = 0; tileId <= tileDescCount; tileId++)
    {
        std::string tileIdStr = Util::ConvertToThreeDigitsString(tileId);
        std::string tileFileName = tileIdStr + ".PID";
        // This is hack, monolith made some mistake in lvl1
        if (tileId == 74)
        {
            tileFileName.erase(0, 1);
        }

        RezFile* tileFile = FindTileFile(tileBaseDir, tileFileName);
        if (tileFile)
        {
            WapPid* tilePid = WAP_PidLoadFromRezFile(tileFile, palette);
            assert(tilePid != NULL);

            TilePrototype* tilePrototype = CreateTilePrototype(&wwd->tileDescriptions[tileId], tileId, tilePid, renderer);
            assert(tilePrototype != NULL);

            _tilePrototypeMap.insert(std::pair<int32_t, TilePrototype*>(tileId, tilePrototype));
        }
    }

    for (uint32_t planeIdx = 0; planeIdx < wwd->planesCount; planeIdx++)
    {
        Plane* plane = new Plane(this, &wwd->planes[planeIdx], planeIdx, renderer);
        _planesVector.push_back(plane);
    }

    // Create camera
    SDL_Window* window = game->GetWindow();
    _camera = new Camera(window, renderer);

    _camera->SetPosition(wwd->properties.startX, wwd->properties.startY);
}

Level::~Level()
{

}

void Level::Update(uint32_t msDiff)
{
    //START_QUERY_PERFORMANCE_TIMER
    SDL_Renderer* renderer = _game->GetRenderer();

    for (Plane* plane : _planesVector)
    {
        plane->Render(renderer, _camera);
    }
    //END_QUERY_PERFORMANCE_TIMER
}

TilePrototype* Level::CreateTilePrototype(WwdTileDescription* tileDescription, uint32_t tileId, WapPid* pid, SDL_Renderer* renderer)
{
    TilePrototype* tilePrototype = new TilePrototype;
    tilePrototype->id = tileId;
    tilePrototype->width = tileDescription->width;
    tilePrototype->height = tileDescription->height;
    //Util::ParseCollisionRectanglesFromTile(tilePrototype, tileDescription, tileId);
    tilePrototype->texture = Image::GetTextureFromPid(pid, renderer);

    return tilePrototype;
}

RezFile* Level::FindTileFile(RezDirectory* tileDirectory, std::string fileName)
{
    assert(tileDirectory != NULL);
    assert(tileDirectory->directoryContents != NULL);
    assert(tileDirectory->directoryContents->rezDirectoriesCount > 0);

    // Search in every tile folder
    uint32_t tileSubdirs = tileDirectory->directoryContents->rezDirectoriesCount;
    
    for (uint32_t subdirIdx = 0; subdirIdx < tileSubdirs; subdirIdx++)
    {
        std::string tilePath = tileDirectory->directoryContents->rezDirectories[subdirIdx]->name + std::string("/") + fileName;
        RezFile* rezFile = WAP_GetRezFileFromRezDirectory(tileDirectory, tilePath.c_str());
        if (rezFile != NULL)
        {
            return rezFile;
        }
    }

    return NULL;
}

LevelResourcePaths Level::GetResourcePaths(uint32_t levelNumber)
{
    LevelResourcePaths paths;
    paths.wwdPath = "LEVEL" + std::to_string(levelNumber) + "/WORLDS/WORLD.WWD";
    paths.palettePath = "LEVEL" + std::to_string(levelNumber) + "/PALETTES/MAIN.PAL";
    paths.levelImagesPath = "LEVEL" + std::to_string(levelNumber) + "/IMAGES";
    paths.levelAnimationsPath = "LEVEL" + std::to_string(levelNumber) + "/ANIS";
    paths.levelSoundsPath = "LEVEL" + std::to_string(levelNumber) + "/SOUNDS";
    paths.levelMusicPath = "LEVEL" + std::to_string(levelNumber) + "/MUSIC";
    paths.levelTilesPath = "LEVEL" + std::to_string(levelNumber) + "/TILES";

    return paths;
}