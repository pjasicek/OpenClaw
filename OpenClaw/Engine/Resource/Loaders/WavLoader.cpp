#include "WavLoader.h"

#include "../../GameApp/BaseGameApp.h"

//=================================================================================================
// class WavResourceExtraData
//
//     This class implements the IResourceExtraData
//

// Custom deallocator for SDL stuff
void DeleteMixChunk(Mix_Chunk* sound)
{
    Mix_FreeChunk(sound);
}

WavResourceExtraData::~WavResourceExtraData()
{
    // Sound takes care of its own destruction
}

void WavResourceExtraData::LoadWavSound(char* rawBuffer, uint32 size)
{
    SDL_RWops* soundRwOps = SDL_RWFromMem((void*)rawBuffer, size);
    _sound = shared_ptr<Mix_Chunk>(Mix_LoadWAV_RW(soundRwOps, 1), DeleteMixChunk);
    if (_sound == NULL)
    {
        LOG_ERROR("Failed to load WAV sound");
    }
    //LOG("RawBufferSize = " + ToStr(size) + ", Sound size = " + ToStr(_sound->alen));
}

//=================================================================================================
// class WavResourceLoader
//
//     This class implements the IResourceLoader interface with WAV sound format
//

bool WavResourceLoader::VLoadResource(char* rawBuffer, uint32 rawSize, std::shared_ptr<ResourceHandle> handle)
{
    if (rawSize <= 0 || rawBuffer == NULL)
    {
        LOG_ERROR("Received invalid rawBuffer or its size");
        return false;
    }

    shared_ptr<WavResourceExtraData> extraData = shared_ptr<WavResourceExtraData>(new WavResourceExtraData());
    extraData->LoadWavSound(rawBuffer, rawSize);

    if (extraData->GetSound() == NULL)
    {
        LOG_ERROR("Failed to load sound. Is sound system initialized ?");
    }

    handle->SetExtraData(extraData);

    return true;
}

uint32 WavResourceLoader::VGetLoadedResourceSize(char* rawBuffer, uint32 rawSize)
{
    // TODO: This is inefficent, this resource gets basically loaded twice, once just
    // to find out how much room it takes and second time to actually create it
    SDL_RWops* soundRwOps = SDL_RWFromMem((void*)rawBuffer, rawSize);
    auto pSound = shared_ptr<Mix_Chunk>(Mix_LoadWAV_RW(soundRwOps, 1), DeleteMixChunk);
    return pSound->alen;
}

shared_ptr<Mix_Chunk> WavResourceLoader::LoadAndReturnSound(const char* resourceString)
{
    Resource resource(resourceString);

    shared_ptr<ResourceHandle> handle = g_pApp->GetResourceCache()->GetHandle(&resource);
    shared_ptr<WavResourceExtraData> extraData = std::static_pointer_cast<WavResourceExtraData>(handle->GetExtraData());

    if (!extraData)
    {
        LOG_ERROR("Could not cast type to WavResourceExtraData. Check if WavResourceLoader is registered.");
        return NULL;
    }

    return extraData->GetSound();
}

std::shared_ptr<WavResourceLoader> WavResourceLoader::Create()
{
    return shared_ptr<WavResourceLoader>(new WavResourceLoader());
}