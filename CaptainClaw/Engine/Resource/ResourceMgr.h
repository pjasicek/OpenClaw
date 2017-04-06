#ifndef __RESOURCE_MGR_H__
#define __RESOURCE_MGR_H__

#include "../SharedDefines.h"

class Resource;
class ResourceHandle;
class ResourceCache;
class IResourceMgr
{
public:
    virtual ~IResourceMgr() { }

    virtual void VAddResourceCache(ResourceCache* pCache) = 0;
    virtual std::shared_ptr<ResourceHandle> VGetHandle(Resource* r, std::string resCacheName = "") = 0;
    virtual int32 VPreload(const std::string pattern, void(*progressCallback)(int32, bool &), std::string resCacheName = "") = 0;
    virtual std::vector<std::string> VMatch(const std::string pattern, std::string resCacheName = "") = 0;
    virtual std::vector<std::string> VGetAllFilesInDirectory(const char* directoryPath, std::string resCacheName = "") = 0;
    virtual void VFlush(std::string resCacheName = "") = 0;
};

typedef std::map<std::string, ResourceCache*> ResourceCacheMap;
class ResourceMgrImpl : public IResourceMgr
{
public:
    virtual ~ResourceMgrImpl();

    virtual void VAddResourceCache(ResourceCache* pCache);
    virtual std::shared_ptr<ResourceHandle> VGetHandle(Resource* r, std::string resCacheName = ORIGINAL_RESOURCE);
    virtual int32 VPreload(const std::string pattern, void(*progressCallback)(int32, bool &), std::string resCacheName = ORIGINAL_RESOURCE);
    virtual std::vector<std::string> VMatch(const std::string pattern, std::string resCacheName = ORIGINAL_RESOURCE);
    virtual std::vector<std::string> VGetAllFilesInDirectory(const char* directoryPath, std::string resCacheName = ORIGINAL_RESOURCE);
    virtual void VFlush(std::string resCacheName = ORIGINAL_RESOURCE);

private:
    ResourceCacheMap m_ResourceCacheMap;
};

#endif