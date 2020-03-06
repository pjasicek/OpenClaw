#include "ConveyorBeltComponent.h"
#include "../Actor.h"
#include "PhysicsComponent.h"

const char* ConveyorBeltComponent::g_Name = "ConveyorBeltComponent";

ConveyorBeltComponent::ConveyorBeltComponent()
    :
    m_Speed(0.0)
{

}

bool ConveyorBeltComponent::VInit(TiXmlElement* pData)
{
    DO_AND_CHECK(ParseValueFromXmlElem(&m_Speed, pData->FirstChildElement("Speed")));

    return true;
}

void ConveyorBeltComponent::VUpdate(uint32 msDiff)
{
    for (Actor* pStandingActor : m_StandingActorsList)
    {
        PhysicsComponent* pPhysicsComponent = pStandingActor->GetRawComponent<PhysicsComponent>(true);
        Point extSpeed(m_Speed, 0);

        pPhysicsComponent->SetExternalConveyorBeltSpeed(extSpeed);
    }
}

void ConveyorBeltComponent::OnActorBeginContact(Actor* pActor)
{
    m_StandingActorsList.push_back(pActor);
}

void ConveyorBeltComponent::OnActorEndContact(Actor* pActor)
{
    for (auto actorIter = m_StandingActorsList.begin(); actorIter != m_StandingActorsList.end(); ++actorIter)
    {
        if (pActor == (*actorIter))
        {
            m_StandingActorsList.erase(actorIter);
            return;
        }
    }
}