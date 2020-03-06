#ifndef XMLLOADER_H_
#define XMLLOADER_H_

#include <tinyxml.h>
#include "../ResourceCache.h"

class XmlResourceExtraData : public IResourceExtraData
{
public:
    virtual std::string VToString() { return "XmlResourceExtraData"; }
    void ParseXml(char* rawBuffer);
    TiXmlElement* GetRoot();

private:
    TiXmlDocument _xmlDocument;
};

class XmlResourceLoader : public IResourceLoader
{
public:
    virtual std::string VGetPattern() { return "*.xml"; }
    virtual bool VUseRawFile() { return false; }
    virtual bool VDiscardRawBufferAfterLoad() { return true; }
    virtual uint32 VGetLoadedResourceSize(char* rawBuffer, uint32 rawSize) { return rawSize; }
    virtual bool VLoadResource(char* rawBuffer, uint32 rawSize, std::shared_ptr<ResourceHandle> handle);

    // May produce memory leak!!!
    static TiXmlElement* LoadAndReturnRootXmlElement(const char* resourceString, bool fromLocalFile = false);
    static std::shared_ptr<XmlResourceLoader> Create();
};

#endif