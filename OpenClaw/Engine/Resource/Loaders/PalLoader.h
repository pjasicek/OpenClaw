#ifndef PALLOADER_H_
#define PALLOADER_H_

#include <libwap.h>
#include <tinyxml.h>
#include "../ResourceCache.h"

class PalResourceExtraData : public IResourceExtraData
{
public:
    virtual ~PalResourceExtraData();

    virtual std::string VToString() { return "PalResourceExtraData"; }
    void LoadPal(char* rawBuffer, uint32 size);
    WapPal* GetPalette() { return _palette; }

private:
    WapPal* _palette;
};

class PalResourceLoader : public IResourceLoader
{
public:
    virtual std::string VGetPattern() { return "*.pal"; }
    virtual bool VUseRawFile() { return false; }
    virtual bool VDiscardRawBufferAfterLoad() { return true; }
    virtual uint32 VGetLoadedResourceSize(char* rawBuffer, uint32 rawSize) { return rawSize; }
    virtual bool VLoadResource(char* rawBuffer, uint32 rawSize, std::shared_ptr<ResourceHandle> handle);

    static WapPal* LoadAndReturnPal(const char* resourceString);
    static std::shared_ptr<PalResourceLoader> Create();
};

#endif