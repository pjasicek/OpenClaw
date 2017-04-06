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

    virtual void VAddResourceCache(shared_ptr<ResourceCache> pCache) = 0;
    virtual std::shared_ptr<ResourceHandle> VGetHandle(Resource* r, std::string resCacheName = "") = 0;
    virtual int32 VPreload(const std::string pattern, void(*progressCallback)(int32, bool &), std::string resCacheName = "") = 0;
    virtual std::vector<std::string> VMatch(const std::string pattern, std::string resCacheName = "") = 0;
    virtual std::vector<std::string> VGetAllFilesInDirectory(const char* directoryPath, std::string resCacheName = "") = 0;
    virtual void VFlush(std::string resCacheName = "") = 0;
};

typedef std::map<std::string, shared_ptr<ResourceCache>> ResourceCacheMap;
class ResourceMgrImpl
{
public:
    virtual ~ResourceMgrImpl();

    virtual void VAddResourceCache(shared_ptr<ResourceCache> pCache);
    virtual std::shared_ptr<ResourceHandle> VGetHandle(Resource* r, std::string resCacheName = "");
    virtual int32 VPreload(const std::string pattern, void(*progressCallback)(int32, bool &), std::string resCacheName = "");
    virtual std::vector<std::string> VMatch(const std::string pattern, std::string resCacheName = "");
    virtual std::vector<std::string> VGetAllFilesInDirectory(const char* directoryPath, std::string resCacheName = "");
    virtual void VFlush(std::string resCacheName = "");

private:
    ResourceCacheMap m_ResourceCacheMap;
};

#endif