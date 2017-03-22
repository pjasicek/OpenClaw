#include "PcxLoader.h"

#include "../../Graphics2D/Image.h"
#include "../../GameApp/BaseGameApp.h"

//=================================================================================================
// class PcxResourceExtraData
//
//     This class implements the IResourceExtraData
//

void PcxResourceExtraData::LoadImage(char* rawBuffer, uint32 size)
{
    if (m_pImage == nullptr)
    {
        m_pImage.reset(Image::CreatePcxImage(rawBuffer, size, g_pApp->GetRenderer()));
    }
}

//=================================================================================================
// class PcxResourceLoader
//
//     This class implements the IResourceLoader interface with PCX file loading
//

shared_ptr<Image> PcxResourceLoader::LoadAndReturnImage(const char* resourceString)
{
    Resource resource(resourceString);

    shared_ptr<ResourceHandle> handle = g_pApp->GetResourceCache()->GetHandle(&resource);
    shared_ptr<PcxResourceExtraData> extraData = std::static_pointer_cast<PcxResourceExtraData>(handle->GetExtraData());

    if (!extraData)
    {
        extraData = shared_ptr<PcxResourceExtraData>(new PcxResourceExtraData());
        extraData->LoadImage(handle->GetDataBuffer(), handle->GetSize());

        if (!extraData->GetImage())
        {
            LOG_ERROR(extraData->VToString() + ": GetImage() returned nullptr. Check if PidResourceLoader is registered.");
            return nullptr;
        }

        handle->SetExtraData(extraData);
    }

    return extraData->GetImage();
}

shared_ptr<PcxResourceLoader> PcxResourceLoader::Create()
{
    return shared_ptr<PcxResourceLoader>(new PcxResourceLoader());
};