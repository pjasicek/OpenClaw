#ifndef RESOURCECACHE_H_
#define RESOURCECACHE_H_

#include <list>
#include <map>
#include <vector>

#include <stdlib.h>
#include <libwap.h>
#include "../SharedDefines.h"
#include "ZipFile.h"

class Resource
{
public:
    Resource(const std::string &name);

    inline std::string GetName() { return _name; }

protected:
    std::string _name;
};

//-------------------------------------------------------------------------------------------------
// Interfaces
//-------------------------------------------------------------------------------------------------

class IResourceExtraData
{
public:
    virtual std::string VToString() = 0;
};

// This would work well in a perfect universe witihout any code repetition but not all
// resources have this simple LoadAndReturn routine
/*template <typename T>
class IResourceExtraData
{
public:
    virtual std::string VToString() = 0;
    T* GetExtraData() { return _extraData; }
    static T* LoadAndReturnExtraData(const char* resourceString)
    {
        Resource resource(resourceString);

        std::shared_ptr<ResourceHandle> handle = g_pApp->GetResourceCache()->GetHandle(&resource));
        std::shared_ptr<T> extraData = std::static_pointer_cast<T>(handle->GetExtraData());

        if (!extraData)
        {
            LOG_ERROR("Could not cast type to: " + VToString() + ". Check if proper resource loader is registered.");
            return NULL;
        }

        return extraData->GetExtraData();
    }

protected:
    T* _extraData;
};*/

class IResourceFile
{
public:
    virtual bool VOpen() = 0;
    virtual std::string VGetName() const = 0;
    virtual int32 VGetRawResourceSize(Resource* r) = 0;
    virtual int32 VGetRawResource(Resource* r, char* outBuffer) = 0;
    virtual int32 VGetNumResources() const = 0;
    virtual std::string VGetResourceName(int32 num) const = 0;
    virtual bool VIsUsingDevelopmentDIrectories() const = 0;
    virtual std::vector<std::string> GetAllFilesInDirectory(const char* directoryPath) = 0;
    virtual ~IResourceFile() { }
};

class ResourceHandle;
class IResourceLoader
{
public:
    virtual std::string VGetPattern() = 0;
    virtual bool VUseRawFile() = 0;
    virtual bool VDiscardRawBufferAfterLoad() = 0;
    virtual bool VAddNullZero() { return false; }
    virtual uint32 VGetLoadedResourceSize(char* rawBuffer, uint32 rawSize) = 0;
    virtual bool VLoadResource(char* buffer, uint32 rawSize, std::shared_ptr<ResourceHandle> handle) = 0;
};

//-------------------------------------------------------------------------------------------------

class ResourceRezArchive : public IResourceFile
{
public:
    ResourceRezArchive(const std::string rezArchiveFileName);
    virtual ~ResourceRezArchive();

    // Interface
    virtual bool VOpen();
    virtual std::string VGetName() const { return _rezArchiveFileName; }
    virtual int32 VGetRawResourceSize(Resource* r);
    virtual int32 VGetRawResource(Resource* r, char* outBuffer);
    virtual int32 VGetNumResources() const;
    virtual std::string VGetResourceName(int32 num) const;
    virtual bool VIsUsingDevelopmentDIrectories() const { return false; }
    virtual std::vector<std::string> GetAllFilesInDirectory(const char* directoryPath);

private:
    RezArchive* _rezArchive;
    std::string _rezArchiveFileName;
};

class ResourceZipArchive : public IResourceFile
{
public:
    ResourceZipArchive(const std::string resFileName) { m_pZipFile = NULL; m_FileName = resFileName; }
    virtual ~ResourceZipArchive();

    virtual bool VOpen();
    virtual std::string VGetName() const { return m_FileName; }
    virtual int VGetRawResourceSize(Resource* r);
    virtual int VGetRawResource(Resource* r, char *buffer);
    virtual int VGetNumResources() const;
    virtual std::string VGetResourceName(int num) const;
    virtual bool VIsUsingDevelopmentDIrectories() const { return false; }
    virtual std::vector<std::string> GetAllFilesInDirectory(const char* directoryPath);

private:
    ZipFile *m_pZipFile;
    std::string m_FileName;
};

//------------------------------------------------------------------------------------------------

class ResourceCache;
class ResourceHandle
{
public:
    ResourceHandle(Resource& resource, char* buffer, uint32 size, ResourceCache* resCache);
    virtual ~ResourceHandle();

    const std::string GetName() { return _resource.GetName(); }
    uint32 GetSize() const { return _size; }
    char* GetDataBuffer() const { return _buffer; }
    char* GetWritableBuffer() { return _buffer; }

    std::shared_ptr<IResourceExtraData> GetExtraData() { return _extraData; }
    void SetExtraData(std::shared_ptr<IResourceExtraData> extraData) { _extraData = extraData; }

protected:
    Resource _resource;
    char* _buffer;
    uint32 _size;
    std::shared_ptr<IResourceExtraData> _extraData;
    ResourceCache* _resourceCache;

private:
};

typedef std::list<std::shared_ptr<ResourceHandle>> ResourceHandleList;
typedef std::list<std::shared_ptr<IResourceLoader>> ResourceLoaderList;
typedef std::map<std::string, std::shared_ptr<ResourceHandle>> ResourceHandleMap;

class ResourceCache
{
public:
    ResourceCache(const uint32 sizeInMb, IResourceFile* resourceFile, std::string name = "");
    virtual ~ResourceCache();

    bool Init();

    std::string GetName() { return m_Name; }

    void RegisterLoader(std::shared_ptr<IResourceLoader> loader);

    std::shared_ptr<ResourceHandle> GetHandle(Resource* r);

    int32 Preload(const std::string pattern, void(*progressCallback)(int32, bool &));
    std::vector<std::string> Match(const std::string pattern);
    std::vector<std::string> GetAllFilesInDirectory(const char* directoryPath);

    void Flush();

    bool IsUsingDevelopmentDirectories() { assert(_resourceFile != NULL); return _resourceFile->VIsUsingDevelopmentDIrectories(); }

    void MemoryHasBeenFreed(uint32 size);

protected:
    bool MakeRoom(uint32 size);
    char* Allocate(uint32 size);
    void Free(std::shared_ptr<ResourceHandle> gonner);

    std::shared_ptr<ResourceHandle> Load(Resource* r);
    std::shared_ptr<ResourceHandle> Find(Resource* r);
    void Update(std::shared_ptr<ResourceHandle> handle);

    void FreeOneResource();

private:
    std::string m_Name;
    IResourceFile* _resourceFile;

    uint64 _cacheSize;
    uint64 _allocated;

    ResourceHandleList _lruList;
    ResourceLoaderList _resourceLoaderList;
    ResourceHandleMap _resourceMap;
};

#endif