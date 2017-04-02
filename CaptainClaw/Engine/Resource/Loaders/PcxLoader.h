#ifndef __PCX_LOADER_H__
#define __PCX_LOADER_H__

#include <libwap.h>
#include <tinyxml.h>
#include "../ResourceCache.h"

class Image;
class PcxResourceExtraData : public IResourceExtraData
{
public:
    PcxResourceExtraData() { m_pImage = nullptr; }

    virtual std::string VToString() { return "PcxResourceExtraData"; }
    void LoadImage(char* rawBuffer, uint32 size, bool useColorKey = false, SDL_Color colorKey = { 0, 0, 0, 0 });
    shared_ptr<Image> GetImage() { return m_pImage; }

private:
    shared_ptr<Image> m_pImage;
};

class PcxResourceLoader : public IResourceLoader
{
public:
    virtual std::string VGetPattern() { return "*.pcx"; }
    virtual bool VUseRawFile() { return true; }
    virtual bool VDiscardRawBufferAfterLoad() { return true; }
    virtual uint32 VGetLoadedResourceSize(char* rawBuffer, uint32 rawSize) { return rawSize; }
    virtual bool VLoadResource(char* rawBuffer, uint32 rawSize, std::shared_ptr<ResourceHandle> handle) { return true; }

    static shared_ptr<Image> LoadAndReturnImage(const char* resourceString, bool useColorKey = false, SDL_Color colorKey = { 0, 0, 0, 0 });
    static std::shared_ptr<PcxResourceLoader> Create();
};

#endif