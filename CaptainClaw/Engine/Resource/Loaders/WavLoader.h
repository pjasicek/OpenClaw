#ifndef WAVLOADER_H_
#define WAVLOADER_H_

#include <Tinyxml/tinyxml.h>
#include "../ResourceCache.h"
#include <SDL2/SDL_mixer.h>
#include <tinyxml.h>

class WavResourceExtraData : public IResourceExtraData
{
public:
    virtual ~WavResourceExtraData();

    virtual std::string VToString() { return "WavResourceExtraData"; }
    void LoadWavSound(char* rawBuffer, uint32 size);
    shared_ptr<Mix_Chunk> GetSound() { return _sound; }

private:
    shared_ptr<Mix_Chunk> _sound;
};

class WavResourceLoader : public IResourceLoader
{
public:
    virtual std::string VGetPattern() { return "*.wav"; }
    virtual bool VUseRawFile() { return false; }
    virtual bool VDiscardRawBufferAfterLoad() { return true; }
    virtual uint32 VGetLoadedResourceSize(char* rawBuffer, uint32 rawSize);
    virtual bool VLoadResource(char* rawBuffer, uint32 rawSize, std::shared_ptr<ResourceHandle> handle);

    static shared_ptr<Mix_Chunk> LoadAndReturnSound(const char* resourceString);
    static std::shared_ptr<WavResourceLoader> Create();
};

#endif
