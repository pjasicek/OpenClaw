#include "AreaDamageComponent.h"
#include "../../GameApp/BaseGameApp.h"
#include "../../GameApp/BaseGameLogic.h"

#include "../../Events/EventMgr.h"
#include "../../Events/Events.h"

const char* AreaDamageComponent::g_Name = "AreaDamageComponent";

AreaDamageComponent::AreaDamageComponent()
    :
    m_Damage(50),
    m_Duration(50),
    m_ActiveTime(0),
    m_HitDirection(Direction_None),
    m_DamageType(DamageType_None),
    m_SourceActorId(INVALID_ACTOR_ID)
{ }

bool AreaDamageComponent::VDelegateInit(TiXmlElement* pData)
{
    assert(pData);

    ParseValueFromXmlElem(&m_Duration, pData->FirstChildElement("Duration"));
    ParseValueFromXmlElem(&m_Damage, pData->FirstChildElement("Damage"));
    ParseValueFromXmlElem(&m_SourceActorId, pData->FirstChildElement("SourceActorId"));

    int damageType;
    if (ParseValueFromXmlElem(&damageType, pData->FirstChildElement("DamageType")))
    {
        m_DamageType = DamageType(damageType);
    }

    int directionType;
    if (ParseValueFromXmlElem(&directionType, pData->FirstChildElement("HitDirection")))
    {
        m_HitDirection = Direction(directionType);
    }

    /*std::string directionString;
    ParseValueFromXmlElem(&directionString, pData->FirstChildElement(("HitDirection")));
    if (directionString == "Right")
    {
        m_HitDirection = Direction_Right;
    }
    else if (directionString == "Left")
    {
        m_HitDirection = Direction_Left;
    }
    else
    {
        m_HitDirection = Direction_None;
    }*/

    assert(m_Duration > 0);

    return true;
}

void AreaDamageComponent::VCreateInheritedXmlElements(TiXmlElement* pBaseElement)
{

}

bool AreaDamageComponent::VOnApply(Actor* pActorWhoPickedThis)
{
    shared_ptr<HealthComponent> pHealthComponent =
        MakeStrongPtr(pActorWhoPickedThis->GetComponent<HealthComponent>(HealthComponent::g_Name));
    if (pHealthComponent)
    {
        
        // This is a hacky part
        SDL_Rect areaDamageAABB = g_pApp->GetGameLogic()->VGetGamePhysics()->VGetAABB(m_pOwner->GetGUID(), false);
        SDL_Rect actorAABB = g_pApp->GetGameLogic()->VGetGamePhysics()->VGetAABB(pActorWhoPickedThis->GetGUID(), true);

        SDL_Rect impactRect;
        Point contactPoint;
        if (SDL_IntersectRect(&areaDamageAABB, &actorAABB, &impactRect))
        {
            //                      +----------------+
            //                   +--|--------+       |  <--- ActorWhoGotHit
            //   AreaDamage ---> |  |xxxxxxxx| <--- ImpactRect
            //                   +--|--------+       |
            //                      +----------------+
            if (m_HitDirection == Direction_Right)
            {
                contactPoint.x = impactRect.x;
                contactPoint.y = impactRect.y + impactRect.h / 2;
            }
            else if (m_HitDirection == Direction_Left)
            {
                contactPoint.x = impactRect.x + impactRect.w;
                contactPoint.y = impactRect.y + impactRect.h / 2;
            }
        }
        
        pHealthComponent->AddHealth(-m_Damage, m_DamageType, contactPoint, m_SourceActorId);
    }

    return false;
}

void AreaDamageComponent::VUpdate(uint32 msDiff)
{
    m_ActiveTime += msDiff;

    if (m_ActiveTime >= m_Duration)
    {
        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(m_pOwner->GetGUID()));
        IEventMgr::Get()->VQueueEvent(pEvent);
    }
}