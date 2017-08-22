#ifndef DEFAULTLOADER_H_
#define DEFAULTLOADER_H_

#include "../ResourceCache.h"

class DefaultResourceLoader : public IResourceLoader
{
public:
    virtual std::string VGetPattern() { return "*"; }
    virtual bool VUseRawFile() { return true; }
    virtual bool VDiscardRawBufferAfterLoad() { return true; }
    virtual uint32 VGetLoadedResourceSize(char* rawBuffer, uint32 rawSize) { return rawSize; }
    virtual bool VLoadResource(char* buffer, uint32 rawSize, std::shared_ptr<ResourceHandle> handle) { return true; }

    static std::shared_ptr<DefaultResourceLoader> Create() { return shared_ptr<DefaultResourceLoader>(new DefaultResourceLoader()); }
};

#endif