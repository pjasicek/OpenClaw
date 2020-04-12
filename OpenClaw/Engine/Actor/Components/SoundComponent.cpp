#include "SoundComponent.h"
#include "../../GameApp/BaseGameApp.h"
#include "../../Resource/Loaders/WavLoader.h"

const char* SoundComponent::g_Name = "SoundComponent";

SoundComponent::~SoundComponent()
{
    _soundMap.clear();
}

bool SoundComponent::VInit(TiXmlElement* data)
{
    assert(data != NULL);

    for (TiXmlElement* animPathElem = data->FirstChildElement("SoundPath");
        animPathElem != NULL; animPathElem = animPathElem->NextSiblingElement("SoundPath"))
    {
        const char* soundsPath = animPathElem->GetText();

        std::vector<std::string> matchingSoundNames =
            g_pApp->GetResourceCache()->Match(soundsPath);

        for (const std::string& soundPath : matchingSoundNames)
        {
            shared_ptr<Mix_Chunk> sound = WavResourceLoader::LoadAndReturnSound(soundPath.c_str());
            if (sound == nullptr)
            {
                LOG_WARNING("Failed to load sound: " + soundPath);
                return false;
            }

            std::string soundNameKey = StripPathAndExtension(soundPath);

            auto result = _soundMap.insert(std::make_pair(soundNameKey, sound));

            // Check if we dont already have the sound loaded
            bool inserted = result.second;
            if (!inserted)
            {
                LOG_WARNING("Trying to load existing sound: " + soundPath);
                continue;
            }
        }
    }

    if (_soundMap.empty())
    {
        LOG_WARNING("Sound map for sound component is empty for actor: " + std::string(data->Parent()->ToElement()->Attribute("Type")));
    }

    return true;
}

TiXmlElement* SoundComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    //

    return baseElement;
}

weak_ptr<Mix_Chunk> SoundComponent::GetSound(std::string soundName)
{
    if (HasSound(soundName))
    {
        return _soundMap[soundName];
    }

    return weak_ptr<Mix_Chunk>();
}