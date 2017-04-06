#include "PngLoader.h"

#include "../../Graphics2D/Image.h"
#include "../../GameApp/BaseGameApp.h"
#include "../ResourceMgr.h"

//=================================================================================================
// class PngResourceExtraData
//
//     This class implements the IResourceExtraData
//

void PngResourceExtraData::LoadImage(char* rawBuffer, uint32 size)
{
    if (m_pImage == nullptr)
    {
        m_pImage.reset(Image::CreatePngImage(rawBuffer, size, g_pApp->GetRenderer()));
    }
}

//=================================================================================================
// class PngResourceLoader
//
//     This class implements the IResourceLoader interface with PNG file loading
//

shared_ptr<Image> PngResourceLoader::LoadAndReturnImage(const char* resourceString)
{
    Resource resource(resourceString);

    shared_ptr<ResourceHandle> handle = g_pApp->GetResourceMgr()->VGetHandle(&resource, CUSTOM_RESOURCE);
    assert(handle != nullptr);

    shared_ptr<PngResourceExtraData> extraData = std::static_pointer_cast<PngResourceExtraData>(handle->GetExtraData());
    if (!extraData)
    {
        extraData = shared_ptr<PngResourceExtraData>(new PngResourceExtraData());
        extraData->LoadImage(handle->GetDataBuffer(), handle->GetSize());

        if (!extraData->GetImage())
        {
            LOG_ERROR(extraData->VToString() + ": GetImage() returned nullptr. Check if PngResourceLoader is registered.");
            return nullptr;
        }

        handle->SetExtraData(extraData);
    }

    return extraData->GetImage();
}

shared_ptr<PngResourceLoader> PngResourceLoader::Create()
{
    return shared_ptr<PngResourceLoader>(new PngResourceLoader());
};