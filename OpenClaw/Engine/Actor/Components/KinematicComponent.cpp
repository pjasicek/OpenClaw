#include "KinematicComponent.h"
#include "../../GameApp/BaseGameApp.h"
#include "../../GameApp/BaseGameLogic.h"

#include "../../Graphics2D/Image.h"
#include "RenderComponent.h"
#include "PositionComponent.h"
#include "PhysicsComponent.h"
#include "../../Physics/ClawPhysics.h"

#include "../../Events/EventMgr.h"
#include "../../Events/Events.h"

const char* KinematicComponent::g_Name = "KinematicComponent";

KinematicComponent::KinematicComponent()
    :
    m_bIsTriggered(false),
    m_bIsDone(false),
    m_bCheckCarriedBodies(false),
    m_TimeSinceLastCarriedBodiesCheck(0)
{
    IEventMgr::Get()->VAddListener(MakeDelegate(this, &KinematicComponent::ClawDiedDelegate), EventData_Claw_Died::sk_EventType);
}

KinematicComponent::~KinematicComponent()
{
    IEventMgr::Get()->VRemoveListener(MakeDelegate(this, &KinematicComponent::ClawDiedDelegate), EventData_Claw_Died::sk_EventType);
}

bool KinematicComponent::VInit(TiXmlElement* pData)
{
    assert(pData != NULL);

    m_pPhysics = g_pApp->GetGameLogic()->VGetGamePhysics();
    if (!m_pPhysics)
    {
        LOG_WARNING("Attemtping to create physics component without valid physics");
        return false;
    }

    DO_AND_CHECK(SetPointIfDefined(&m_Properties.speed, pData->FirstChildElement("Speed"), "x", "y"));
    DO_AND_CHECK(SetPointIfDefined(&m_Properties.minPosition, pData->FirstChildElement("MinPosition"), "x", "y"));
    DO_AND_CHECK(SetPointIfDefined(&m_Properties.maxPosition, pData->FirstChildElement("MaxPosition"), "x", "y"));
    ParseValueFromXmlElem(&m_Properties.hasTriggerBehaviour, pData->FirstChildElement("HasTriggerBehaviour"));
    ParseValueFromXmlElem(&m_Properties.hasStartBehaviour, pData->FirstChildElement("HasStartBehaviour"));
    ParseValueFromXmlElem(&m_Properties.hasStopBehaviour, pData->FirstChildElement("HasStopBehaviour"));
    ParseValueFromXmlElem(&m_Properties.hasOneWayBehaviour, pData->FirstChildElement("HasOneWayBehaviour"));

    double platformSpeedModifier = g_pApp->GetGlobalOptions()->platformSpeedModifier;
    m_Speed = Point(m_Properties.speed.x * platformSpeedModifier, m_Properties.speed.y * platformSpeedModifier);

    return true;
}

void KinematicComponent::VPostInit()
{
    shared_ptr<PositionComponent> pPositionComponent = m_pOwner->GetPositionComponent();
    assert(pPositionComponent);

    m_pPositionComponent = pPositionComponent.get();

    m_InitialPosition = m_pPositionComponent->GetPosition();
    m_LastPosition = m_pPositionComponent->GetPosition();
    m_CurrentSpeed = m_Speed;

    if (m_Properties.hasTriggerBehaviour || m_Properties.hasStartBehaviour)
    {
        m_CurrentSpeed = Point(0, 0);

        // Precalculate initial speed
        Point positionDelta = m_Properties.maxPosition - m_Properties.minPosition;
        Point middlePositionDelta(positionDelta.x / 2.0, positionDelta.y / 2.0);
        Point middleTrajectoryPosition = m_Properties.minPosition + middlePositionDelta;

        m_LastSpeed = m_Speed;
        if (m_pPositionComponent->GetPosition().GetX() > middleTrajectoryPosition.x)
        {
            m_LastSpeed.SetX(m_Speed.x * -1.0);
        }
        if (m_pPositionComponent->GetPosition().GetY() > middleTrajectoryPosition.y)
        {
            m_LastSpeed.SetY(m_Speed.y * -1.0);
        }

        m_InitialSpeed = m_LastSpeed;
    }
}

TiXmlElement* KinematicComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    //

    return baseElement;
}

void KinematicComponent::VUpdate(uint32 msDiff)
{
    if (m_bIsDone)
    {
        return;
    }

    bool directionChanged = false;
    if (fabs(m_Speed.x) > DBL_EPSILON)
    {
        if ((m_CurrentSpeed.x < -1.0 * DBL_EPSILON) && m_pPositionComponent->GetX() < m_Properties.minPosition.x)
        {
            m_CurrentSpeed.SetX(m_CurrentSpeed.x * -1.0);
            directionChanged = true;
        }
        else if ((m_CurrentSpeed.x > DBL_EPSILON) && m_pPositionComponent->GetX() > m_Properties.maxPosition.x)
        {
            m_CurrentSpeed.SetX(m_CurrentSpeed.x * -1.0);
            directionChanged = true;
        }
    }

    if (fabs(m_Speed.y) > DBL_EPSILON)
    {
        if (directionChanged)
        {
            m_CurrentSpeed = Point(m_CurrentSpeed.x, m_CurrentSpeed.y * -1.0);
        }
        else if ((m_CurrentSpeed.y < -1.0 * DBL_EPSILON) && m_pPositionComponent->GetY() < m_Properties.minPosition.y)
        {
            m_CurrentSpeed = Point(m_CurrentSpeed.x, m_CurrentSpeed.y * -1.0);
            directionChanged = true;
        }
        else if ((m_CurrentSpeed.y > DBL_EPSILON) && m_pPositionComponent->GetY() > m_Properties.maxPosition.y)
        {
            m_CurrentSpeed = Point(m_CurrentSpeed.x, m_CurrentSpeed.y * -1.0);
            directionChanged = true;
        }
    }

    if (!m_CurrentSpeed.IsZeroXY())
    {
        m_LastSpeed = m_CurrentSpeed;
    }

    if (m_Properties.hasOneWayBehaviour && directionChanged)
    {
        m_bIsDone = true;
        Point zeroSpeed(0, 0);
        m_pPhysics->VSetLinearSpeedEx(m_pOwner->GetGUID(), zeroSpeed);
        return;
    }

    // This is to get aroung Box2D's imperfections
    // When elevator is going down and Claw stands on it, he gets continuously removed and added making elevator 
    // go slow
    if (m_bCheckCarriedBodies)
    {
        m_TimeSinceLastCarriedBodiesCheck += msDiff;
        if (m_TimeSinceLastCarriedBodiesCheck > 100)
        {
            m_bCheckCarriedBodies = false;
            m_TimeSinceLastCarriedBodiesCheck = 0;

            if (m_Properties.hasStartBehaviour)
            {
                if (m_CarriedBodiesList.empty())
                {
                    
                    m_CurrentSpeed = Point(0, 0);
                }
                else
                {
                    m_CurrentSpeed = m_LastSpeed;
                }
            }
        }
    }

    if (m_Properties.hasStartBehaviour)
    {
        //LOG("Speed: " + m_CurrentSpeed.ToString());
    }

    m_pPhysics->VSetLinearSpeedEx(m_pOwner->GetGUID(), m_CurrentSpeed);
}

void KinematicComponent::RemoveCarriedBody(b2Body* pBody)
{
    for (auto iter = m_CarriedBodiesList.begin(); iter != m_CarriedBodiesList.end(); ++iter)
    {
        if ((*iter) == pBody)
        {
            if (m_Properties.hasStartBehaviour)
            {
                //LOG("REMOVING");
                // TODO: Fix bug with start elevator when it moves in the same direction
                //m_Speed = m_CurrentSpeed;
                //m_CurrentSpeed = Point(0, 0);
                m_bCheckCarriedBodies = true;
                m_TimeSinceLastCarriedBodiesCheck = 0;
            }

            m_CarriedBodiesList.erase(iter);
            return;
        }
    }
}

void KinematicComponent::AddCarriedBody(b2Body* pBody)
{
    if (!m_bIsDone && 
        ((m_Properties.hasTriggerBehaviour && !m_bIsTriggered) 
        || m_Properties.hasStartBehaviour))
    {
        m_bIsTriggered = true;
        m_bCheckCarriedBodies = true;
        m_TimeSinceLastCarriedBodiesCheck = 0;
        m_CurrentSpeed = m_LastSpeed;
    }
    m_CarriedBodiesList.push_back(pBody);
}

void KinematicComponent::OnMoved(Point newPosition)
{
    /*if (m_IsStartElevator && m_CarriedBodiesList.empty())
    {
        return;
    }
    if (m_IsTriggerElevator && !m_IsTriggered)
    {
        return;
    }*/

    // Only necessary if it is moving horizontally and if it carries some bodies with it
    if (/*fabs(m_CurrentSpeed.x) < DBL_EPSILON ||*/ m_CarriedBodiesList.empty())
    {
        m_LastPosition = newPosition;
        return;
    }

    if (fabs(m_Speed.x) < DBL_EPSILON)
    {
        return;
    }

    Point positionDelta = Point(newPosition.x - m_LastPosition.x, newPosition.y - m_LastPosition.y);
    //LOG(ToStr(positionDelta.x) + " " + ToStr(positionDelta.y));

    std::vector<b2Body*> uniqueCarriedBodies = m_CarriedBodiesList;
    std::sort(uniqueCarriedBodies.begin(), uniqueCarriedBodies.end());
    uniqueCarriedBodies.erase(std::unique(uniqueCarriedBodies.begin(), uniqueCarriedBodies.end()), uniqueCarriedBodies.end());
    for (b2Body* pCarriedBody : uniqueCarriedBodies)
    {
        if (pCarriedBody->GetType() == b2_dynamicBody)
        {
            Actor* pActor = (Actor*)pCarriedBody->GetUserData();
            assert(pActor);
            
            shared_ptr<PhysicsComponent> pPhysicsComponent = pActor->GetPhysicsComponent();

            // Provide sanity check
            if (pPhysicsComponent && pPhysicsComponent->GetNumFootContacts() == 0)
            {
                RemoveCarriedBody(pCarriedBody);
                continue;
            }

            if (pPhysicsComponent)
            {
                pPhysicsComponent->SetExternalSourceSpeed(m_CurrentSpeed);
            }

            /*if (pPhysicsComponent && pPhysicsComponent->GetVelocity().y < -4)
            {
                continue;
            }
            if (pPhysicsComponent && fabs(pPhysicsComponent->GetVelocity().x) > DBL_EPSILON)
            {
                continue;
            }

            shared_ptr<PositionComponent> pPositionComponent =
                MakeStrongPtr(pActor->GetComponent<PositionComponent>(PositionComponent::g_Name));
            Point currentPos = pPositionComponent->GetPosition();*/
            //pPositionComponent->SetPosition(currentPos.x + positionDelta.x, currentPos.y + positionDelta.y);
            
            //m_pPhysics->VSetPosition(pActor->GetGUID(), Point(currentPos.x + positionDelta.x, currentPos.y + positionDelta.y));
        }
    }

    m_LastPosition = newPosition;
}

// After claw dies, set elevators to default position
void KinematicComponent::ClawDiedDelegate(IEventDataPtr pEventData)
{
    if (m_Properties.hasTriggerBehaviour)
    {
        m_bIsTriggered = false;
        m_CurrentSpeed = Point(0, 0);
        
    }
    if (m_Properties.hasStartBehaviour || 
        m_Properties.hasTriggerBehaviour || 
        m_Properties.hasStopBehaviour)
    {
        m_CurrentSpeed = Point(0.0, 0.0);
        m_LastSpeed = m_InitialSpeed;
    }

    m_CarriedBodiesList.clear();
    m_bCheckCarriedBodies = false;

    m_bIsDone = false;
    IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Teleport_Actor(m_pOwner->GetGUID(), m_InitialPosition)));
    /*m_pPositionComponent->SetPosition(m_InitialPosition);
    m_pPhysics->VSetPosition(m_pOwner->GetGUID(), m_InitialPosition);*/
}