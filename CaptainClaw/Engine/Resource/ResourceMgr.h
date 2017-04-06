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
    virtual std::shared_ptr<ResourceHandle> VGetHandle(Resource* r, const std::string& resCacheName = "") = 0;
    virtual int32 VPreload(const std::string pattern, void(*progressCallback)(int32, bool &), const std::string& resCacheName = "") = 0;
    virtual std::vector<std::string> VMatch(const std::string pattern, const std::string& resCacheName = "") = 0;
    virtual std::vector<std::string> VGetAllFilesInDirectory(const char* directoryPath, const std::string& resCacheName = "") = 0;
    virtual void VFlush(const std::string& resCacheName = "") = 0;
};

typedef std::vector<ResourceCache*> ResourceCacheList;
class ResourceMgrImpl : public IResourceMgr
{
public:
    virtual ~ResourceMgrImpl();

    virtual void VAddResourceCache(ResourceCache* pCache);
    virtual std::shared_ptr<ResourceHandle> VGetHandle(Resource* r, const std::string& resCacheName = "");
    virtual int32 VPreload(const std::string pattern, void(*progressCallback)(int32, bool &), const std::string& resCacheName = "");
    virtual std::vector<std::string> VMatch(const std::string pattern, const std::string& resCacheName = "");
    virtual std::vector<std::string> VGetAllFilesInDirectory(const char* directoryPath, const std::string& resCacheName = "");
    virtual void VFlush(const std::string& resCacheName = "");

private:
    ResourceCache* GetResourceCacheFromName(const std::string& resCacheName);

    ResourceCacheList m_ResourceCacheList;
};

#endif