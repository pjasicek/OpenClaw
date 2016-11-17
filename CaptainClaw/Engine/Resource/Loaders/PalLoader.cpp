#include "PalLoader.h"

#include "../../GameApp/BaseGameApp.h"

//=================================================================================================
// class PalResourceExtraData
//
//     This class implements the IResourceExtraData
//

PalResourceExtraData::~PalResourceExtraData()
{
    if (_palette != NULL)
    {
        WAP_PalDestroy(_palette);
    }
}

void PalResourceExtraData::LoadPal(char* rawBuffer, uint32 size)
{
    _palette = WAP_PalLoadFromData(rawBuffer, size);
}

//=================================================================================================
// class PalResourceLoader
//
//     This class implements the IResourceLoader interface with PAL (256bit palette) file loading
//

bool PalResourceLoader::VLoadResource(char* rawBuffer, uint32 rawSize, std::shared_ptr<ResourceHandle> handle)
{
    if (rawSize <= 0 || rawBuffer == NULL)
    {
        LOG_ERROR("Received invalid rawBuffer or its size");
        return false;
    }

    shared_ptr<PalResourceExtraData> extraData = shared_ptr<PalResourceExtraData>(new PalResourceExtraData());
    extraData->LoadPal(rawBuffer, rawSize);

    handle->SetExtraData(extraData);

    return true;
}

WapPal* PalResourceLoader::LoadAndReturnPal(const char* resourceString)
{
    Resource resource(resourceString);

    shared_ptr<ResourceHandle> handle = g_pApp->GetResourceCache()->GetHandle(&resource);
    shared_ptr<PalResourceExtraData> extraData = std::static_pointer_cast<PalResourceExtraData>(handle->GetExtraData());

    if (!extraData)
    {
        LOG_ERROR("Could not cast type to PalResourceExtraData. Check if PalResourceLoader is registered.");
        return NULL;
    }

    return extraData->GetPalette();
}

std::shared_ptr<PalResourceLoader> PalResourceLoader::Create()
{
    return shared_ptr<PalResourceLoader>(new PalResourceLoader());
}