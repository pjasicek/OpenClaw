#include "WwdLoader.h"

#include "../../GameApp/BaseGameApp.h"
//#include "../../Converters.h"
#include "../../Interfaces.h"

//=================================================================================================
// class WwdResourceExtraData
//
//     This class implements the IResourceExtraData
//
//=================================================================================================

WwdResourceExtraData::~WwdResourceExtraData()
{
    if (_wapWorldLevel != NULL)
    {
        WAP_WwdDestroy(_wapWorldLevel);
    }
}

void WwdResourceExtraData::LoadWwd(char* rawBuffer, uint32 size)
{
    _wapWorldLevel = WAP_WwdLoadFromData(rawBuffer, size);
}

//=================================================================================================
// class WwdResourceLoader
//
//     This class implements the IResourceLoader interface with WWD (Wap World level) file loading
//
//=================================================================================================

bool WwdResourceLoader::VLoadResource(char* rawBuffer, uint32 rawSize, std::shared_ptr<ResourceHandle> handle)
{
    if (rawSize <= 0 || rawBuffer == NULL)
    {
        LOG_ERROR("Received invalid rawBuffer or its size");
        return false;
    }

    shared_ptr<WwdResourceExtraData> extraData = shared_ptr<WwdResourceExtraData>(new WwdResourceExtraData());
    extraData->LoadWwd(rawBuffer, rawSize);

    handle->SetExtraData(extraData);

    return true;
}

WapWwd* WwdResourceLoader::LoadAndReturnWwd(const char* resourceString)
{
    Resource resource(resourceString);

    shared_ptr<ResourceHandle> wwdHandle = g_pApp->GetResourceCache()->GetHandle(&resource);
    shared_ptr<WwdResourceExtraData> extraData = std::static_pointer_cast<WwdResourceExtraData>(wwdHandle->GetExtraData());

    if (!extraData)
    {
        LOG_ERROR("Could not cast type to WwdResourceExtraData. Check if WwdResourceLoader is registered.");
        return NULL;
    }

    return extraData->GetWwd();
}

std::shared_ptr<WwdResourceLoader> WwdResourceLoader::Create()
{
    return shared_ptr<WwdResourceLoader>(new WwdResourceLoader());
}