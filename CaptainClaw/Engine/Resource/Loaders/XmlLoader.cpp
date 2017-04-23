#include "../../GameApp/BaseGameApp.h"

#include "XmlLoader.h"
#include "../ResourceMgr.h"

//=================================================================================================
// class XmlResourceExtraData
//
//     This class implements the IResourceExtraData
//

void XmlResourceExtraData::ParseXml(char* rawBuffer)
{
    _xmlDocument.Parse(rawBuffer);
}

TiXmlElement* XmlResourceExtraData::GetRoot()
{
    return _xmlDocument.RootElement();
}

//=================================================================================================
// class XmlResourceLoader
//
//     This class implements the IResourceLoader interface with XML document loading
//

bool XmlResourceLoader::VLoadResource(char* rawBuffer, uint32 rawSize, std::shared_ptr<ResourceHandle> handle)
{
    if (rawSize <= 0 || rawBuffer == NULL)
    {
        LOG_ERROR("Received invalid rawBuffer or its size");
        return false;
    }

    shared_ptr<XmlResourceExtraData> extraData = shared_ptr<XmlResourceExtraData>(new XmlResourceExtraData());
    extraData->ParseXml(rawBuffer);

    handle->SetExtraData(extraData);

    return true;
}

TiXmlElement* XmlResourceLoader::LoadAndReturnRootXmlElement(const char* resourceString, bool fromLocalFile)
{
    if (fromLocalFile)
    {
        // In this case caller is responsible for freeing the resource
        TiXmlDocument* doc = new TiXmlDocument(resourceString);
        doc->LoadFile();
        if (doc->Error())
        {
            LOG_ERROR("Could not load XML document: " + std::string(resourceString) + ". Error: " + doc->ErrorDesc());
            return NULL;
        }
        
        return doc->RootElement();
    }
    else
    {
        Resource resource(resourceString);

        // XMLs are only located in my own archive
        shared_ptr<ResourceHandle> pHandle = g_pApp->GetResourceMgr()->VGetHandle(&resource, CUSTOM_RESOURCE);
        shared_ptr<XmlResourceExtraData> extraData = std::static_pointer_cast<XmlResourceExtraData>(pHandle->GetExtraData());

        if (!extraData)
        {
            LOG_ERROR("Could not cast type to WwdResourceExtraData. Check if WwdResourceLoader is registered.");
            return NULL;
        }

        return extraData->GetRoot();
    }
}

std::shared_ptr<XmlResourceLoader> XmlResourceLoader::Create()
{
    return shared_ptr<XmlResourceLoader>(new XmlResourceLoader());
}