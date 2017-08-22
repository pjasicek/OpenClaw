#include "PidLoader.h"

#include "../../Graphics2D/Image.h"
#include "../../GameApp/BaseGameApp.h"
#include "ResourceCorrection.h"

//=================================================================================================
// class PidResourceExtraData
//
//     This class implements the IResourceExtraData
//

PidResourceExtraData::~PidResourceExtraData()
{
    if (_pid != NULL)
    {
        WAP_PidDestroy(_pid);
    }
}

void PidResourceExtraData::LoadPid(char* rawBuffer, uint32 size, WapPal* palette, const char* resourceString)
{
    if (_pid == NULL)
    {
        _pid = WAP_PidLoadFromData(rawBuffer, size, palette);
        OnPidLoaded(resourceString, _pid);
    }
}

void PidResourceExtraData::LoadImage(char* rawBuffer, uint32 size, WapPal* palette, const char* resourceString)
{
    if (_pid == NULL)
    {
        LoadPid(rawBuffer, size, palette, resourceString);
    }
    if (_image == NULL)
    {
        SDL_Renderer* renderer = g_pApp->GetRenderer();
        _image = shared_ptr<Image>(Image::CreateImage(_pid, renderer));
        WAP_PidDestroy(_pid); _pid = NULL;
        //SAFE_DELETE_ARRAY(rawBuffer);
    }
}

//=================================================================================================
// class PidResourceLoader
//
//     This class implements the IResourceLoader interface with PID file loading
//

WapPid* PidResourceLoader::LoadAndReturnPid(const char* resourceString, WapPal* palette)
{
    Resource resource(resourceString);

    shared_ptr<ResourceHandle> handle = g_pApp->GetResourceCache()->GetHandle(&resource);
    shared_ptr<PidResourceExtraData> extraData = std::static_pointer_cast<PidResourceExtraData>(handle->GetExtraData());

    if (!extraData)
    {
        extraData = shared_ptr<PidResourceExtraData>(new PidResourceExtraData());
        extraData->LoadPid(handle->GetDataBuffer(), handle->GetSize(), palette, resourceString);

        if (extraData->GetPid() == NULL)
        {
            LOG_ERROR(extraData->VToString() + ": GetPid() returned NULL. Check if PidResourceLoader is registered.");
            return NULL;
        }

        handle->SetExtraData(extraData);
    }

    return extraData->GetPid();
}

shared_ptr<Image> PidResourceLoader::LoadAndReturnImage(const char* resourceString, WapPal* palette)
{
    Resource resource(resourceString);

    shared_ptr<ResourceHandle> handle = g_pApp->GetResourceCache()->GetHandle(&resource);
    shared_ptr<PidResourceExtraData> extraData = std::static_pointer_cast<PidResourceExtraData>(handle->GetExtraData());

    if (!extraData)
    {
        extraData = shared_ptr<PidResourceExtraData>(new PidResourceExtraData());
        extraData->LoadImage(handle->GetDataBuffer(), handle->GetSize(), palette, resourceString);

        if (!extraData->GetImage())
        {
            LOG_ERROR(extraData->VToString() + ": GetImage() returned nullptr. Check if PidResourceLoader is registered.");
            return NULL;
        }

        handle->SetExtraData(extraData);
    }
    // Extra data could be created within LoadAndReturnPid but not the image
    else if (!extraData->GetImage())
    {
        extraData->LoadImage(handle->GetDataBuffer(), handle->GetSize(), palette, resourceString);

        if (!extraData->GetImage())
        {
            LOG_ERROR(extraData->VToString() + ": GetImage() returned nullptr. Check if PidResourceLoader is registered.");
            return NULL;
        }
    }

    return extraData->GetImage();
}

shared_ptr<PidResourceLoader> PidResourceLoader::Create()
{
    return shared_ptr<PidResourceLoader>(new PidResourceLoader());
};