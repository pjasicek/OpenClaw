#include "DestroyableComponent.h"
#include "../../GameApp/BaseGameApp.h"
#include "../../GameApp/BaseGameLogic.h"

#include "../../Events/EventMgr.h"
#include "../../Events/Events.h"

const char* DestroyableComponent::g_Name = "DestroyableComponent";

DestroyableComponent::DestroyableComponent()
    :
    m_DeleteOnDestruction(true)
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

    if (TiXmlElement* pElem = pData->FirstChildElement("DeleteOnDestruction"))
    {
        m_DeleteOnDestruction = std::string(pElem->GetText()) == "true";
    }
    
    for (TiXmlElement* pElem = pData->FirstChildElement("DeathSound");
        pElem; pElem = pElem->NextSiblingElement("DeathSound"))
    {
        m_PossibleDestructionSounds.push_back(pElem->GetText());
    }

    return true;
}

void DestroyableComponent::VPostInit()
{
    if (m_PossibleDestructionSounds.empty())
    {
        //LOG_WARNING("Destruction component has no death sounds. Actor: " + _owner->GetName());
    }

    shared_ptr<HealthComponent> pHealthComponent =
        MakeStrongPtr(_owner->GetComponent<HealthComponent>(HealthComponent::g_Name));
    if (pHealthComponent)
    {
        pHealthComponent->AddObserver(this);
    }

    shared_ptr<AnimationComponent> pAnimationComponent =
        MakeStrongPtr(_owner->GetComponent<AnimationComponent>(AnimationComponent::g_Name));
    if (pAnimationComponent)
    {
        pAnimationComponent->AddObserver(this);
    }
}

TiXmlElement* DestroyableComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    //

    return baseElement;
}

void DestroyableComponent::VOnHealthBelowZero()
{
    if (!m_PossibleDestructionSounds.empty())
    {
        srand((long)this + (long)&m_PossibleDestructionSounds);
        int soundToPlayIdx = rand() % m_PossibleDestructionSounds.size();

        // Play this sound
    }

    shared_ptr<AnimationComponent> pAnimationComponent =
        MakeStrongPtr(_owner->GetComponent<AnimationComponent>(AnimationComponent::g_Name));
    if (pAnimationComponent)
    {
        pAnimationComponent->ResumeAnimation();
    }

    m_pPhysics->VRemoveActor(_owner->GetGUID());
}

void DestroyableComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    if (pAnimation->IsAtLastAnimFrame())
    {
        if (m_DeleteOnDestruction)
        {
            shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(_owner->GetGUID()));
            IEventMgr::Get()->VQueueEvent(pEvent);
        }
        else
        {
            pAnimation->Pause();
        }
    }
}