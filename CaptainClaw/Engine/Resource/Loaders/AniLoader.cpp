#include <stdlib.h>
#include "AniLoader.h"
#include "ResourceCorrection.h"

#include "../../GameApp/BaseGameApp.h"

//=================================================================================================
// class AniResourceExtraData
//
//     This class implements the IResourceExtraData
//

AniResourceExtraData::~AniResourceExtraData()
{
    if (_ani != NULL)
    {
        WAP_AniDestroy(_ani);
    }
}

void AniResourceExtraData::LoadAni(char* rawBuffer, uint32 size, const char* resourceString)
{
    _ani = WAP_AniLoadFromData(rawBuffer, size);
    OnAniLoaded(resourceString, _ani);
}

//=================================================================================================
// class AniResourceLoader
//
//     This class implements the IResourceLoader interface with ANI animation desc format
//

bool AniResourceLoader::VLoadResource(char* rawBuffer, uint32 rawSize, std::shared_ptr<ResourceHandle> handle)
{
    if (rawSize <= 0 || rawBuffer == NULL)
    {
        LOG_ERROR("Received invalid rawBuffer or its size");
        return false;
    }

    shared_ptr<AniResourceExtraData> extraData = shared_ptr<AniResourceExtraData>(new AniResourceExtraData());
    extraData->LoadAni(rawBuffer, rawSize, handle->GetName().c_str());

    handle->SetExtraData(extraData);

    return true;
}

WapAni* AniResourceLoader::LoadAndReturnAni(const char* resourceString)
{
    Resource resource(resourceString);

    shared_ptr<ResourceHandle> handle = g_pApp->GetResourceCache()->GetHandle(&resource);
    shared_ptr<AniResourceExtraData> extraData = std::static_pointer_cast<AniResourceExtraData>(handle->GetExtraData());

    if (!extraData)
    {
        LOG_ERROR("Could not cast type to AniResourceExtraData. Check if AniResourceLoader is registered. Resource: " + ToStr(resourceString));
        return NULL;
    }

    return extraData->GetAni();
}

std::shared_ptr<AniResourceLoader> AniResourceLoader::Create()
{
    return shared_ptr<AniResourceLoader>(new AniResourceLoader());
}