#ifndef ANILOADER_H_
#define ANILOADER_H_

#include <libwap.h>
#include <Tinyxml\tinyxml.h>
#include "../ResourceCache.h"

class Animation;
class AniResourceExtraData : public IResourceExtraData
{
public:
    virtual ~AniResourceExtraData();

    virtual std::string VToString() { return "AniResourceExtraData"; }
    void LoadAni(char* rawBuffer, uint32 size);
    WapAni* GetAni() { return _ani; }

private:
    WapAni* _ani;
};

class AniResourceLoader : public IResourceLoader
{
public:
    virtual std::string VGetPattern() { return "*.ani"; }
    virtual bool VUseRawFile() { return false; }
    virtual bool VDiscardRawBufferAfterLoad() { return true; }
    virtual uint32 VGetLoadedResourceSize(char* rawBuffer, uint32 rawSize) { return rawSize; }
    virtual bool VLoadResource(char* rawBuffer, uint32 rawSize, std::shared_ptr<ResourceHandle> handle);

    static WapAni* LoadAndReturnAni(const char* resourceString);
    static std::shared_ptr<AniResourceLoader> Create();
};

#endif