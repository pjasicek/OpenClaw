#include "DestroyableComponent.h"
#include "../../GameApp/BaseGameApp.h"
#include "../../GameApp/BaseGameLogic.h"

#include "../../Events/EventMgr.h"
#include "../../Events/Events.h"

#include "RenderComponent.h"

const char* DestroyableComponent::g_Name = "DestroyableComponent";

DestroyableComponent::DestroyableComponent()
    :
    m_DeleteDelay(0),
    m_bDeleteImmediately(false),
    m_bBlinkOnDestruction(false),
    m_bDeleteOnDestruction(true),
    m_bRemoveFromPhysics(true),
    m_bIsDead(false),
    m_DeleteDelayTimeLeft(0)
{ }

DestroyableComponent::~DestroyableComponent()
{

}

bool DestroyableComponent::VInit(TiXmlElement* pData)
{
    assert(pData != NULL);

    m_pPhysics = g_pApp->GetGameLogic()->VGetGamePhysics();
    if (!m_pPhysics)
    {
        LOG_WARNING("Attemtping to create destructible component without valid physics");
        return false;
    }

    ParseValueFromXmlElem(&m_DeleteDelay, pData->FirstChildElement("DeleteDelay"));
    ParseValueFromXmlElem(&m_bDeleteImmediately, pData->FirstChildElement("DeleteImmediately"));
    ParseValueFromXmlElem(&m_bBlinkOnDestruction, pData->FirstChildElement("BlinkOnDestruction"));
    ParseValueFromXmlElem(&m_bDeleteOnDestruction, pData->FirstChildElement("DeleteOnDestruction"));
    ParseValueFromXmlElem(&m_bRemoveFromPhysics, pData->FirstChildElement("RemoveFromPhysics"));
    ParseValueFromXmlElem(&m_DeathAnimationName, pData->FirstChildElement("DeathAnimationName"));

    for (TiXmlElement* pElem = pData->FirstChildElement("DeathSound");
        pElem; 
        pElem = pElem->NextSiblingElement("DeathSound"))
    {
        m_PossibleDestructionSounds.push_back(pElem->GetText());
    }

    m_DeleteDelayTimeLeft = m_DeleteDelay;

    return true;
}

void DestroyableComponent::VPostInit()
{
    if (m_PossibleDestructionSounds.empty())
    {
        //LOG_WARNING("Destruction component has no death sounds. Actor: " + m_pOwner->GetName());
    }

    shared_ptr<HealthComponent> pHealthComponent =
        MakeStrongPtr(m_pOwner->GetComponent<HealthComponent>(HealthComponent::g_Name));
    if (pHealthComponent)
    {
        pHealthComponent->AddObserver(this);
    }

    shared_ptr<AnimationComponent> pAnimationComponent =
        MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>(AnimationComponent::g_Name));
    if (pAnimationComponent)
    {
        pAnimationComponent->AddObserver(this);
    }

    m_pRenderComponent = MakeStrongPtr(m_pOwner->GetComponent<ActorRenderComponent>()).get();
    assert(m_pRenderComponent != NULL);
}

TiXmlElement* DestroyableComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    //

    return baseElement;
}

void DestroyableComponent::VUpdate(uint32 msDiff)
{
    if (m_bIsDead && m_bBlinkOnDestruction)
    {
        assert(m_DeleteDelay > 0 && "This should blink but no delete delay ?");

        m_DeleteDelayTimeLeft -= msDiff;

        shared_ptr<AnimationComponent> pAnimationComponent =
            MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>());

        int cycleState = m_DeleteDelayTimeLeft / 350;
        if (cycleState % 2 == 0)
        {
            m_pRenderComponent->SetVisible(true);
            if (pAnimationComponent)
            {
                pAnimationComponent->ResumeAnimation();
            }
        }
        else
        {
            m_pRenderComponent->SetVisible(false);
            if (pAnimationComponent)
            {
                pAnimationComponent->PauseAnimation();
            }
        }

        if (m_DeleteDelayTimeLeft <= 0)
        {
            m_pPhysics->VRemoveActor(m_pOwner->GetGUID());
            DeleteActor();
        }
    }
}

void DestroyableComponent::VOnHealthBelowZero(DamageType damageType, int sourceActorId)
{
    m_bIsDead = true;

    if (m_bRemoveFromPhysics && m_DeleteDelay == 0)
    {
        m_pPhysics->VRemoveActor(m_pOwner->GetGUID());
    }

    if (m_DeleteDelay > 0)
    {
        Point nullSpeed(0, 0);
        m_pPhysics->VSetLinearSpeed(m_pOwner->GetGUID(), nullSpeed);
    }

    if (!m_PossibleDestructionSounds.empty())
    {
        // Pick random death sound
        srand((long)this + (long)&m_PossibleDestructionSounds);
        int soundToPlayIdx = rand() % m_PossibleDestructionSounds.size();

        // And play it
        SoundInfo soundInfo(m_PossibleDestructionSounds[soundToPlayIdx]);
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));
    }

    if (!m_DeathAnimationName.empty())
    {
        shared_ptr<AnimationComponent> pAnimationComponent =
            MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>(AnimationComponent::g_Name));
        if (pAnimationComponent)
        {
            if (m_DeathAnimationName == "DEFAULT")
            {
                pAnimationComponent->ResumeAnimation();
            }
            else
            {
                pAnimationComponent->SetAnimation(m_DeathAnimationName);
                pAnimationComponent->ResumeAnimation();
            }
        }
    }

    if (m_bDeleteImmediately)
    {
        DeleteActor();
    }
}

void DestroyableComponent::VOnAnimationAtLastFrame(Animation* pAnimation)
{
    if (!m_bIsDead)
    {
        return;
    }

    if (m_DeleteDelay != 0)
    {
        return;
    }

    if (m_bDeleteOnDestruction)
    {
        DeleteActor();
    }
    else
    {
        pAnimation->Pause();
    }
}

void DestroyableComponent::DeleteActor()
{
    shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(m_pOwner->GetGUID()));
    IEventMgr::Get()->VQueueEvent(pEvent);
}