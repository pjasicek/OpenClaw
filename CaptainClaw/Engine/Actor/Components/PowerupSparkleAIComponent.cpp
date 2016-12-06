#include "PowerupSparkleAIComponent.h"
#include "PositionComponent.h"
#include "../../Events/EventMgr.h"
#include "../../Events/Events.h"

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
        MakeStrongPtr(_owner->GetComponent<AnimationComponent>(AnimationComponent::g_Name));
    assert(pAnimationComponent && pAnimationComponent->GetCurrentAnimation());
    pAnimationComponent->AddObserver(this);

    srand((int)this);
    pAnimationComponent->SetDelay(rand() % 1000);

    m_pPositonComponent = MakeStrongPtr(_owner->GetComponent<PositionComponent>(PositionComponent::g_Name)).get();
    assert(m_pPositonComponent);
}

TiXmlElement* PowerupSparkleAIComponent::VGenerateXml()
{
    return NULL;

    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    return baseElement;
}

void PowerupSparkleAIComponent::VOnAnimationLooped(Animation* pAnimation)
{
    assert(m_pPositonComponent);
    assert(m_pTargetPositionComponent);

    Point targetPos = m_pTargetPositionComponent->GetPosition();
    srand((int)this + (int)m_pPositonComponent->GetX() + (int)m_pPositonComponent->GetY() + time(NULL));
    m_pPositonComponent->SetX(targetPos.x - m_TargetSize.x / 2 + rand() % (int)m_TargetSize.x);
    m_pPositonComponent->SetY(targetPos.y - m_TargetSize.y / 2  + rand() % (int)m_TargetSize.y);

    shared_ptr<EventData_Move_Actor> pEvent(new EventData_Move_Actor(_owner->GetGUID(), m_pPositonComponent->GetPosition()));
    IEventMgr::Get()->VTriggerEvent(pEvent);
}

void PowerupSparkleAIComponent::SetTargetPositionComponent(PositionComponent* pTarget)
{
    assert(pTarget);
    m_pTargetPositionComponent = pTarget;
}