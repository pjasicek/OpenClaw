#ifndef SOUNDCOMPONENT_H_
#define SOUNDCOMPONENT_H_

#include <SDL2/SDL_mixer.h>
#include "../../SharedDefines.h"
#include "../ActorComponent.h"

class SoundComponent : public ActorComponent
{
public:
    SoundComponent() { }
    virtual ~SoundComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;

    inline bool HasSound(std::string soundName) { return (_soundMap.count(soundName) > 0); }
    weak_ptr<Mix_Chunk> GetSound(std::string soundName);

private:
    std::map<std::string, shared_ptr<Mix_Chunk>> _soundMap;
};

#endif
