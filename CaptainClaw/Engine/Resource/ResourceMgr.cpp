#include "ResourceMgr.h"
#include "ResourceCache.h"

ResourceMgrImpl::~ResourceMgrImpl()
{

}

void ResourceMgrImpl::VAddResourceCache(shared_ptr<ResourceCache> pCache)
{
	m_ResourceCacheMap.insert(std::make_pair(pCache->GetName(), pCache));
}

std::shared_ptr<ResourceHandle> ResourceMgrImpl::VGetHandle(Resource* r, std::string resCacheName)
{
	assert(!m_ResourceCacheMap.empty());

	// Find res cache by name
	if (!resCacheName.empty())
	{
		auto findIt = m_ResourceCacheMap.find(resCacheName);
		assert(findIt != m_ResourceCacheMap.end());

		return findIt->second->GetHandle(r);
	}
	else // Find in all available res caches
	{
		for (auto iter : m_ResourceCacheMap)
		{
			if (std::shared_ptr<ResourceHandle> pHandle = iter.second->GetHandle(r))
			{
				pHandle;
			}
		}
	}

	return nullptr;
}

int32 ResourceMgrImpl::VPreload(const std::string pattern, void(*progressCallback)(int32, bool &), std::string resCacheName)
{
	assert(!m_ResourceCacheMap.empty());

	int32 totalLoaded = 0;

	// Preload res cache by name
	if (!resCacheName.empty())
	{
		auto findIt = m_ResourceCacheMap.find(resCacheName);
		assert(findIt != m_ResourceCacheMap.end());

		totalLoaded = findIt->second->Preload(pattern, progressCallback);
	}
	else // Preload all available res caches
	{
		for (auto iter : m_ResourceCacheMap)
		{
			totalLoaded += iter.second->Preload(pattern, progressCallback);
		}
	}

	return totalLoaded;
}

std::vector<std::string> ResourceMgrImpl::VMatch(const std::string pattern, std::string resCacheName)
{
	assert(!m_ResourceCacheMap.empty());

	std::vector<std::string> matchedStrings;

	if (!resCacheName.empty())
	{
		auto findIt = m_ResourceCacheMap.find(resCacheName);
		assert(findIt != m_ResourceCacheMap.end());

		matchedStrings = findIt->second->Match(pattern);
	}
	else
	{
		for (auto iter : m_ResourceCacheMap)
		{
			std::vector<std::string> matched = iter.second->Match(pattern);
			matchedStrings.insert(matchedStrings.end(), matched.begin(), matched.end());
		}
	}

	return matchedStrings;
}

std::vector<std::string> ResourceMgrImpl::VGetAllFilesInDirectory(const char* directoryPath, std::string resCacheName)
{
	assert(!m_ResourceCacheMap.empty());

	std::vector<std::string> allFiles;

	if (!resCacheName.empty())
	{
		auto findIt = m_ResourceCacheMap.find(resCacheName);
		assert(findIt != m_ResourceCacheMap.end());

		allFiles = findIt->second->GetAllFilesInDirectory(directoryPath);
	}
	else
	{
		for (auto iter : m_ResourceCacheMap)
		{
			std::vector<std::string> files = iter.second->GetAllFilesInDirectory(directoryPath);
			allFiles.insert(allFiles.end(), files.begin(), files.end());
		}
	}

	return allFiles;
}

void ResourceMgrImpl::VFlush(std::string resCacheName)
{
	assert(!m_ResourceCacheMap.empty());

	std::vector<std::string> allFiles;

	if (!resCacheName.empty())
	{
		auto findIt = m_ResourceCacheMap.find(resCacheName);
		assert(findIt != m_ResourceCacheMap.end());

		findIt->second->Flush();
	}
	else
	{
		for (auto iter : m_ResourceCacheMap)
		{
			iter.second->Flush();
		}
	}
}