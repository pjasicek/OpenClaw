#include "PowerupSparkleAIComponent.h"
#include "PositionComponent.h"
#include "../../Events/EventMgr.h"
#include "../../Events/Events.h"
#include "../Actor.h"

#include <time.h>

const char* PowerupSparkleAIComponent::g_Name = "PowerupSparkleAIComponent";

// This whole thing feels like a HACK
// This component (e.g. actor) is created in PowerupComponent component

PowerupSparkleAIComponent::PowerupSparkleAIComponent()
    :
    m_TargetSize(Point(40, 110)),
    m_pTargetPositionComponent(NULL),
    m_pPositonComponent(NULL)
{ }

bool PowerupSparkleAIComponent::VInit(TiXmlElement* data)
{
    return true;
}

void PowerupSparkleAIComponent::VPostInit()
{
    shared_ptr<AnimationComponent> pAnimationComponent =
        MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>());
    assert(pAnimationComponent && pAnimationComponent->GetCurrentAnimation());
    pAnimationComponent->AddObserver(this);

    srand((long)this);
    //pAnimationComponent->SetDelay(rand() % 1000);

    int numFrames = pAnimationComponent->GetCurrentAnimation()->GetAnimFramesSize();
    int skipFrames = rand() % numFrames;
    for (int i = 0; i < skipFrames; i++)
    {
        pAnimationComponent->GetCurrentAnimation()->SetNextFrame();
    }

    m_pPositonComponent = m_pOwner->GetPositionComponent().get();
    assert(m_pPositonComponent);
}

void PowerupSparkleAIComponent::VPostPostInit()
{
    
}

TiXmlElement* PowerupSparkleAIComponent::VGenerateXml()
{
    return NULL;

    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    return baseElement;
}

void PowerupSparkleAIComponent::VOnAnimationLooped(Animation* pAnimation)
{
    ChooseNewPosition();
}

void PowerupSparkleAIComponent::SetTargetPositionComponent(PositionComponent* pTarget)
{
    assert(pTarget);
    m_pTargetPositionComponent = pTarget;

    shared_ptr<EventData_Teleport_Actor> pEvent(new EventData_Teleport_Actor(m_pOwner->GetGUID(), m_pTargetPositionComponent->GetPosition()));
    IEventMgr::Get()->VTriggerEvent(pEvent);

    ChooseNewPosition();
}

void PowerupSparkleAIComponent::ChooseNewPosition()
{
    assert(m_pPositonComponent);
    assert(m_pTargetPositionComponent);

    Point targetPos = m_pTargetPositionComponent->GetPosition();
    srand((long)this + (int)m_pPositonComponent->GetX() + (int)m_pPositonComponent->GetY() + time(NULL));
    m_pPositonComponent->SetX(targetPos.x - m_TargetSize.x / 2 + rand() % (int)m_TargetSize.x);
    m_pPositonComponent->SetY(targetPos.y - m_TargetSize.y / 2 + rand() % (int)m_TargetSize.y);

    shared_ptr<EventData_Teleport_Actor> pEvent(new EventData_Teleport_Actor(m_pOwner->GetGUID(), m_pPositonComponent->GetPosition()));
    IEventMgr::Get()->VTriggerEvent(pEvent);
}