#include "LocalAmbientSoundComponent.h"
#include "../../GameApp/BaseGameApp.h"
#include "../../GameApp/BaseGameLogic.h"
#include "PositionComponent.h"
#include "../../UserInterface/HumanView.h"
#include "../../Audio/Audio.h"
#include "../../Resource/Loaders/WavLoader.h"

const char* LocalAmbientSoundComponent::g_Name = "LocalAmbientSoundComponent";

LocalAmbientSoundComponent::LocalAmbientSoundComponent()
    :
    m_ActorsInTriggerArea(0),
    m_pActorInArea(NULL),
    m_DiagonalLength(0.0),
    m_SoundChannel(-1)
{

}

bool LocalAmbientSoundComponent::VInit(TiXmlElement* pData)
{
    assert(pData != NULL);

    m_Properties.LoadFromXml(pData, true);

    m_DiagonalLength = m_Properties.soundAreaSize.Length() / 2;

    return true;
}

TiXmlElement* LocalAmbientSoundComponent::VGenerateXml()
{
    return m_Properties.ToXml();
}

void LocalAmbientSoundComponent::VPostInit()
{
    shared_ptr<TriggerComponent> pTriggerComponent =
        MakeStrongPtr(m_pOwner->GetComponent<TriggerComponent>());
    assert(pTriggerComponent != nullptr);

    pTriggerComponent->AddObserver(this);
}

void LocalAmbientSoundComponent::VPostPostInit()
{
    assert(!m_Properties.soundAreaSize.IsZeroXY());
    if (!m_Properties.soundAreaSize.IsZeroXY())
    {
        ActorFixtureDef fixtureDef;
        fixtureDef.collisionShape = "Rectangle";
        fixtureDef.fixtureType = FixtureType_Trigger;
        fixtureDef.size = m_Properties.soundAreaSize;
        fixtureDef.offset = m_Properties.soundAreaOffset;
        fixtureDef.collisionMask = CollisionFlag_Controller | CollisionFlag_InvisibleController;
        fixtureDef.collisionFlag = CollisionFlag_Trigger;
        fixtureDef.isSensor = true;

        g_pApp->GetGameLogic()->VGetGamePhysics()->VAddActorFixtureToBody(m_pOwner->GetGUID(), &fixtureDef);
    }
}

void LocalAmbientSoundComponent::VUpdate(uint32 msDiff)
{
    if (m_ActorsInTriggerArea > 0)
    {
        UpdateAmbientSound();
    }
}

void LocalAmbientSoundComponent::VOnActorEnteredTrigger(Actor* pActorWhoEntered, FixtureType triggerType)
{
    m_ActorsInTriggerArea++;
    m_pActorInArea = pActorWhoEntered;
    assert(m_ActorsInTriggerArea == 1);

    PlayAmbientSound();
}

void LocalAmbientSoundComponent::VOnActorLeftTrigger(Actor* pActorWhoLeft, FixtureType triggerType)
{
    m_ActorsInTriggerArea--;
    m_pActorInArea = NULL;
    assert(m_ActorsInTriggerArea == 0);

    StopAmbientSound();
}

void LocalAmbientSoundComponent::PlayAmbientSound()
{
    assert(m_SoundChannel == -1);
    assert(m_pActorInArea != NULL);

#ifndef __EMSCRIPTEN__
    m_SoundChannel = Mix_GroupAvailable(1);
    assert(m_SoundChannel != -1 && "Could not get a channel from channel group");
#else
    // TODO: [EMSCRIPTEN] Try to implement Mix_Group* functions
    m_SoundChannel = -1;
#endif

    shared_ptr<Mix_Chunk> pSound = WavResourceLoader::LoadAndReturnSound(m_Properties.sound.c_str());
    assert(pSound != nullptr);

#ifndef __EMSCRIPTEN__
    int globalVolume = (int)((((float)g_pApp->GetAudio()->GetSoundVolume()) / 100.0f) * (float)MIX_MAX_VOLUME);
    int chunkVolume = (int)((((float)m_Properties.volume) / 100.0f) * (float)globalVolume);

    Mix_VolumeChunk(pSound.get(), chunkVolume);
#else
    // TODO: [EMSCRIPTEN] Try to implement Mix_VolumeChunk
#endif

    m_SoundChannel = Mix_PlayChannel(m_SoundChannel, pSound.get(), -1);

    // Set positional properties
    UpdateAmbientSound();
}

void LocalAmbientSoundComponent::StopAmbientSound()
{
    assert(m_SoundChannel != -1);

    Mix_HaltChannel(m_SoundChannel);
    m_SoundChannel = -1;
}

void LocalAmbientSoundComponent::UpdateAmbientSound()
{
#ifdef __EMSCRIPTEN__
    // TODO: [EMSCRIPTEN] Try to implement Mix_SetDistance
    return;
#endif
    assert(m_SoundChannel != -1);
    assert(m_pActorInArea != NULL);

    Point soundDistanceDelta =
        m_pOwner->GetPositionComponent()->GetPosition() - m_pActorInArea->GetPositionComponent()->GetPosition();
    double distance = soundDistanceDelta.Length();
    double distanceRatio = distance / m_DiagonalLength;

    int sdlDistance = std::min(distanceRatio * 255, (double)255);

    Mix_SetDistance(m_SoundChannel, sdlDistance);

    /*LOG("Distance ratio: " + ToStr(distanceRatio));
    LOG("Distance: " + ToStr(distance));*/
}