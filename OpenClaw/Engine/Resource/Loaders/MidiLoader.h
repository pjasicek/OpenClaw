#ifndef MIDILOADER_H_
#define MIDILOADER_H_

#include <Tinyxml/tinyxml.h>
#include "../ResourceCache.h"

class MidiResourceExtraData : public IResourceExtraData
{
public:
    virtual ~MidiResourceExtraData();

    virtual std::string VToString() { return "MidiResourceExtraData"; }
    void LoadMidiFile(char* rawBuffer, uint32 size);
    shared_ptr<MidiFile> GetMidiFile() { return m_pMidiFile; }

private:
    shared_ptr<MidiFile> m_pMidiFile;
};

class MidiResourceLoader : public IResourceLoader
{
public:
    virtual std::string VGetPattern() { return "*.xmi"; }
    virtual bool VUseRawFile() { return false; }
    virtual bool VDiscardRawBufferAfterLoad() { return true; }
    virtual uint32 VGetLoadedResourceSize(char* rawBuffer, uint32 rawSize);
    virtual bool VLoadResource(char* rawBuffer, uint32 rawSize, std::shared_ptr<ResourceHandle> handle);

    static shared_ptr<MidiFile> LoadAndReturnMidiFile(const char* resourceString);
    static std::shared_ptr<MidiResourceLoader> Create();
};

#endif
