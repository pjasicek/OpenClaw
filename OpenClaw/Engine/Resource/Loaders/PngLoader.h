#ifndef __PNG_LOADER_H__
#define __PNG_LOADER_H__

#include <tinyxml.h>
#include "../ResourceCache.h"

class Image;
class PngResourceExtraData : public IResourceExtraData
{
public:
    PngResourceExtraData() { m_pImage = nullptr; }

    virtual std::string VToString() { return "PngResourceExtraData"; }
    void LoadImage(char* rawBuffer, uint32 size);
    shared_ptr<Image> GetImage() { return m_pImage; }

private:
    shared_ptr<Image> m_pImage;
};

class PngResourceLoader : public IResourceLoader
{
public:
    virtual std::string VGetPattern() { return "*.png"; }
    virtual bool VUseRawFile() { return true; }
    virtual bool VDiscardRawBufferAfterLoad() { return true; }
    virtual uint32 VGetLoadedResourceSize(char* rawBuffer, uint32 rawSize) { return rawSize; }
    virtual bool VLoadResource(char* rawBuffer, uint32 rawSize, std::shared_ptr<ResourceHandle> handle) { return true; }

    static shared_ptr<Image> LoadAndReturnImage(const char* resourceString);
    static std::shared_ptr<PngResourceLoader> Create();
};

#endif