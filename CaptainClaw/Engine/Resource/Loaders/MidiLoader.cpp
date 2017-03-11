#include "MidiLoader.h"

#include "../../GameApp/BaseGameApp.h"

//=================================================================================================
// class MidiResourceExtraData
//
//     This class implements the IResourceExtraData
//

// Custom deallocator libwap stuff
void DeleteMidiFile(MidiFile* pMidiFile)
{
    WAP_MidiDestroy(pMidiFile);
}

MidiResourceExtraData::~MidiResourceExtraData()
{
    // Sound takes care of its own destruction
}

void MidiResourceExtraData::LoadMidiFile(char* rawBuffer, uint32 size)
{
    MidiFile* pMidiFile = WAP_XmiToMidiFromData(rawBuffer, size);
    // TODO: After testing comment this assert
    assert(pMidiFile != NULL && "Failed to load MidiFile");

    m_pMidiFile = shared_ptr<MidiFile>(pMidiFile, DeleteMidiFile);
    if (m_pMidiFile == nullptr)
    {
        LOG_ERROR("Failed to load MidiFile");
    }
}

//=================================================================================================
// class MidiResourceLoader
//
//     This class implements the IResourceLoader interface with Midi "file handle"
//

bool MidiResourceLoader::VLoadResource(char* rawBuffer, uint32 rawSize, std::shared_ptr<ResourceHandle> handle)
{
    if (rawSize <= 0 || rawBuffer == NULL)
    {
        LOG_ERROR("Received invalid rawBuffer or its size");
        return false;
    }

    shared_ptr<MidiResourceExtraData> extraData = shared_ptr<MidiResourceExtraData>(new MidiResourceExtraData());
    extraData->LoadMidiFile(rawBuffer, rawSize);

    if (extraData->GetMidiFile() == NULL)
    {
        LOG_ERROR("Failed to load MidiFile.");
    }

    handle->SetExtraData(extraData);

    return true;
}

uint32 MidiResourceLoader::VGetLoadedResourceSize(char* rawBuffer, uint32 rawSize)
{
    // It is how it is - thats how Mix_Chunk consumes it.
    return rawSize;
}

shared_ptr<MidiFile> MidiResourceLoader::LoadAndReturnMidiFile(const char* resourceString)
{
    Resource resource(resourceString);

    shared_ptr<ResourceHandle> handle = g_pApp->GetResourceCache()->GetHandle(&resource);
    shared_ptr<MidiResourceExtraData> extraData = std::static_pointer_cast<MidiResourceExtraData>(handle->GetExtraData());

    if (!extraData)
    {
        LOG_ERROR("Could not cast type to MidiResourceExtraData. Check if MidiResourceLoader is registered.");
        return NULL;
    }

    return extraData->GetMidiFile();
}

std::shared_ptr<MidiResourceLoader> MidiResourceLoader::Create()
{
    return shared_ptr<MidiResourceLoader>(new MidiResourceLoader());
}