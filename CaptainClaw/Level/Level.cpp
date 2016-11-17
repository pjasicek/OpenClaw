#include <libwap.h>

#include <algorithm>

#include "Level.h"
#include "../Util/Util.h"
#include "../GameApplication.h"
#include "../Image.h"
#include "../Collision/CollisionWorld.h"
#include "Plane.h"

#include "..\Engine\Resource\Loaders\WwdLoader.h"
#include "..\Engine\Resource\Loaders\PalLoader.h"

#include "../Engine/Actor/Actor.h"
#include "../Engine/Actor/ActorFactory.h"

#include "../Engine/Actor/Components/RenderComponent.h"
#include "../Engine/Actor/Components/PositionComponent.h"

#include "../Image.h"

#include "../Engine/Scene/Scene.h"

#include <iostream>
using namespace std;

Level::Level(LoadLevelInfo* loadLevelInfo)
{
    assert(loadLevelInfo != NULL);
    assert((loadLevelInfo->levelNumber > 0) && 
        (loadLevelInfo->levelNumber <= MAXIMUM_LEVEL));

    _levelNumber = loadLevelInfo->levelNumber;

    //RezArchive* clawRezArchive = sGameApplication->GetRezArchive();
    SDL_Renderer* renderer = sGameApplication->GetRenderer();
    LevelResourcePaths paths = GetResourcePaths(loadLevelInfo->levelNumber);

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Loading level %u ...", loadLevelInfo->levelNumber);

    WapWwd* wwd = WwdResourceLoader::LoadAndReturnWwd(paths.wwdPath.c_str());
    _palette = PalResourceLoader::LoadAndReturnPal(paths.palettePath.c_str());
    sGameApplication->SetCurrentPalette(_palette);

    assert(wwd != NULL);
    assert(_palette != NULL);

    _levelName = std::string(wwd->properties.levelName);

    //---------- Planes

    // Get base directory containing tile subfolders for separate planes, e.g. /LEVEL1/TILES
    char rezDirDelim = WAP_GetDirectorySeparator();
    std::string tileBaseDirPath(wwd->properties.imageDirectoryPath);
    std::replace(tileBaseDirPath.begin(), tileBaseDirPath.end(), '\\', rezDirDelim);

    for (uint32_t planeIdx = 0; planeIdx < wwd->planesCount; planeIdx++)
    {
        Plane* plane = new Plane(this, &wwd->planes[planeIdx], planeIdx, tileBaseDirPath, renderer);
        _planesVector.push_back(plane);

        if (plane->IsMainPlane())
        {
            _mainPlane = plane;
        }
    }

    //--------- Tile collision prototypes

    uint32_t tileDescCount = wwd->tileDescriptionsCount;
    for (uint32_t tileId = 0; tileId < tileDescCount; tileId++)
    {
        TileCollisionPrototype* proto = CreateTilePrototype(&wwd->tileDescriptions[tileId], tileId);
        _tilePrototypeMap.insert(std::pair<int32_t, TileCollisionPrototype*>(tileId, proto));
    }

    //--------- Camera
    SDL_Window* window = sGameApplication->GetWindow();
    _camera = new Camera(window, renderer);
    // Clamp camera
    int32_t worldPixelWidth = _mainPlane->GetPlanePixelHeight();
    int32_t worldPixelHeight = _mainPlane->GetPlanePixelWidth();
    _camera->ClampViewport(worldPixelWidth, worldPixelHeight);

    _camera->SetScale(1.0f, 1.0f);
    //_camera->SetScale(0.2f, 0.2f);
    _camera->SetPositionCentered(wwd->properties.startX, wwd->properties.startY);

    _collisionWorld = new CollisionWorld(this);

    ActorFactory factory;
    

    m_pScene = shared_ptr<Scene>(new Scene(renderer));
    
    m_pCamera.reset(new CameraNode({ _camera->GetX(), _camera->GetY() }, _camera->GetCameraWidth(), _camera->GetCameraHeight()));
    m_pScene->SetCamera(m_pCamera);

    for (int i = 0; i < 10; i++)
    {
        //PROFILE_CPU("ACTOR CREATION");

        StrongActorPtr actor = factory.CreateActor("Actors/Claw.xml", NULL);

        shared_ptr<PositionComponent> posComp =
            Util::MakeStrongPtr(actor->GetComponent<PositionComponent>(PositionComponent::g_Name));
        posComp->SetPosition(posComp->GetX() + i * 6, posComp->GetY() - i * 6);

        shared_ptr<ActorRenderComponent> renderComponent =
            Util::MakeStrongPtr(actor->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name));
        if (!renderComponent)
        {
            exit(1);
        }
        m_pScene->AddChild(actor->GetGUID(), renderComponent->GetScneNodePublicTest());
        _actorMap.insert(make_pair(actor->GetGUID(), actor));
    }
}

Level::~Level()
{
    sGameApplication->SetCurrentPalette(NULL);
}

void Level::Update(uint32_t msDiff)
{
    //START_QUERY_PERFORMANCE_TIMER
    SDL_Renderer* renderer = sGameApplication->GetRenderer();

    for (Plane* plane : _planesVector)
    {
        plane->Render(renderer, _camera);
    }
    
    _collisionWorld->DebugRender(renderer, _camera);
    
    {
        //LOG("Profiling actor update loop");
        START_QUERY_PERFORMANCE_TIMER
        for (auto actorIter : _actorMap)
        {
            
            StrongActorPtr actor = actorIter.second;

            actor->Update(msDiff);
            
            /*shared_ptr<RenderComponent> renderComponent =
                Util::MakeStrongPtr(actor->GetComponent<RenderComponent>(RenderComponent::g_Name));
                shared_ptr<PositionComponent> positionComponent =
                Util::MakeStrongPtr(actor->GetComponent<PositionComponent>(PositionComponent::g_Name));
                if (positionComponent && renderComponent)
                {
                shared_ptr<Image> actorImage = Util::MakeStrongPtr(renderComponent->GetCurrentImage());
                if (actorImage)
                {
                //actorImage->
                SDL_Rect renderRect =
                {

                positionComponent->GetX() - _camera->GetX() + actorImage->GetOffsetX(),
                positionComponent->GetY() - _camera->GetY() + actorImage->GetOffsetY(),
                actorImage->GetWidth(),
                actorImage->GetHeight()
                };
                SDL_RenderCopy(renderer, actorImage->GetTexture(), NULL, &renderRect);
                }
                }*/
        }
        //END_QUERY_PERFORMANCE_TIMER
    }

    
    //LOG("Profiling render loop");
    //START_QUERY_PERFORMANCE_TIMER
    m_pScene->OnUpdate(msDiff);
    m_pScene->OnRender();
    //END_QUERY_PERFORMANCE_TIMER
}

TileCollisionPrototype* Level::CreateTilePrototype(WwdTileDescription* tileDescription, uint32_t tileId)
{
    TileCollisionPrototype* tilePrototype = new TileCollisionPrototype;
    tilePrototype->id = tileId;
    tilePrototype->width = tileDescription->width;
    tilePrototype->height = tileDescription->height;
    Util::ParseCollisionRectanglesFromTile(tilePrototype, tileDescription, tileId);

    return tilePrototype;
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