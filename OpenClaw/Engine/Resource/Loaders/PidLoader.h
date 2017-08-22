#ifndef PIDLOADER_H_
#define PIDLOADER_H_

#include <libwap.h>
#include <tinyxml.h>
#include "../ResourceCache.h"

class Image;
class PidResourceExtraData : public IResourceExtraData
{
public:
    PidResourceExtraData() { _pid = NULL; _image = nullptr; }
    virtual ~PidResourceExtraData();

    virtual std::string VToString() { return "PidResourceExtraData"; }
    void LoadPid(char* rawBuffer, uint32 size, WapPal* palette, const char* resourceString);
    void LoadImage(char* rawBuffer, uint32 size, WapPal* palette, const char* resourceString);
    WapPid* GetPid() { return _pid; }
    shared_ptr<Image> GetImage() { return _image; }

private:
    WapPid* _pid;
    // Use shared_ptr here so that objects that are using it can dictate its lifetime
    shared_ptr<Image> _image;
};

class PidResourceLoader : public IResourceLoader
{
public:
    virtual std::string VGetPattern() { return "*.pid"; }
    virtual bool VUseRawFile() { return true; }
    virtual bool VDiscardRawBufferAfterLoad() { return true; }
    virtual uint32 VGetLoadedResourceSize(char* rawBuffer, uint32 rawSize) { return rawSize; }
    virtual bool VLoadResource(char* rawBuffer, uint32 rawSize, std::shared_ptr<ResourceHandle> handle) { return true; }

    static WapPid* LoadAndReturnPid(const char* resourceString, WapPal* palette);
    static shared_ptr<Image> LoadAndReturnImage(const char* resourceString, WapPal* palette);
    static std::shared_ptr<PidResourceLoader> Create();
};

#endif