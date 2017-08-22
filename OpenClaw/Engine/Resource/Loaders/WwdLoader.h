#ifndef WWDLOADER_H_
#define WWDLOADER_H_

#include <libwap.h>
#include <tinyxml.h>
#include "../ResourceCache.h"

class WwdResourceExtraData : public IResourceExtraData
{
public:
    virtual ~WwdResourceExtraData();

    virtual std::string VToString() { return "WwdResourceExtraData"; }
    void LoadWwd(char* rawBuffer, uint32 size);
    WapWwd* GetWwd() { return _wapWorldLevel; }

private:
    WapWwd* _wapWorldLevel;
};

class WwdResourceLoader : public IResourceLoader
{
public:
    virtual std::string VGetPattern() { return "*.wwd"; }
    virtual bool VUseRawFile() { return false; }
    virtual bool VDiscardRawBufferAfterLoad() { return true; }
    virtual uint32 VGetLoadedResourceSize(char* rawBuffer, uint32 rawSize) { return rawSize; }
    virtual bool VLoadResource(char* rawBuffer, uint32 rawSize, std::shared_ptr<ResourceHandle> handle);

    static WapWwd* LoadAndReturnWwd(const char* resourceString);
    static std::shared_ptr<WwdResourceLoader> Create();
};

#endif
