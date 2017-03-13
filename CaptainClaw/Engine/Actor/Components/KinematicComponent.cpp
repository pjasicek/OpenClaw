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
    m_Size(Point(0, 0)),
    m_Speed(Point(0, 0)),
    m_CurrentSpeed(Point(0, 0)),
    m_MinPosition(Point(0, 0)),
    m_MaxPosition(Point(0, 0)),
    m_LastPosition(Point(0, 0)),
    m_IsStartElevator(false),
    m_IsTriggerElevator(false),
    m_IsTriggered(true)
{
    IEventMgr::Get()->VAddListener(MakeDelegate(this, &KinematicComponent::ClawDiedDelegate), EventData_Claw_Died::sk_EventType);
}

KinematicComponent::~KinematicComponent()
{
    IEventMgr::Get()->VRemoveListener(MakeDelegate(this, &KinematicComponent::ClawDiedDelegate), EventData_Claw_Died::sk_EventType);
}

bool KinematicComponent::VInit(TiXmlElement* data)
{
    assert(data != NULL);

    m_pPhysics = g_pApp->GetGameLogic()->VGetGamePhysics();
    if (!m_pPhysics)
    {
        LOG_WARNING("Attemtping to create physics component without valid physics");
        return false;
    }

    if (TiXmlElement* pElem = data->FirstChildElement("Size"))
    {
        pElem->Attribute("width", &m_Size.x);
        pElem->Attribute("height", &m_Size.y);
    }
    if (TiXmlElement* pElem = data->FirstChildElement("Speed"))
    {
        pElem->Attribute("x", &m_Speed.x);
        pElem->Attribute("y", &m_Speed.y);
        m_Speed = Point(m_Speed.x / 50, m_Speed.y / 50);
    }
    if (TiXmlElement* pElem = data->FirstChildElement("MinPosition"))
    {
        pElem->Attribute("x", &m_MinPosition.x);
        pElem->Attribute("y", &m_MinPosition.y);
    }
    if (TiXmlElement* pElem = data->FirstChildElement("MaxPosition"))
    {
        pElem->Attribute("x", &m_MaxPosition.x);
        pElem->Attribute("y", &m_MaxPosition.y);
    }
    if (TiXmlElement* pElem = data->FirstChildElement("StartBehaviour"))
    {
        m_IsStartElevator = std::string(pElem->GetText()) == "true";
    }
    if (TiXmlElement* pElem = data->FirstChildElement("TriggeredBehaviour"))
    {
        m_IsTriggerElevator = std::string(pElem->GetText()) == "true";
        m_IsTriggered = false;
    }


    return true;
}

void KinematicComponent::VPostInit()
{
    // Set size from current image if necessary
    if (fabs(m_Size.x) < DBL_EPSILON || fabs(m_Size.y) < DBL_EPSILON)
    {
        shared_ptr<ActorRenderComponent> pRenderComponent =
            MakeStrongPtr(_owner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name));
        assert(pRenderComponent);

        shared_ptr<Image> pImage = MakeStrongPtr(pRenderComponent->GetCurrentImage());

        m_Size.x = pImage->GetWidth();
        m_Size.y = pImage->GetHeight();
    }

    shared_ptr<PositionComponent> pPositionComponent =
        MakeStrongPtr(_owner->GetComponent<PositionComponent>(PositionComponent::g_Name));
    assert(pPositionComponent);

    m_pPositionComponent = pPositionComponent.get();
    
    /*if (m_pPositionComponent->GetX() < m_MinPosition.x)
    {
        LOG("SETTING");
        m_pPositionComponent->SetX(m_MinPosition.x);
    }*/

    m_InitialPosition = m_pPositionComponent->GetPosition();
    m_LastPosition = m_pPositionComponent->GetPosition();
    m_CurrentSpeed = m_Speed;

    if (m_IsTriggerElevator || m_IsStartElevator)
    {
        m_CurrentSpeed = Point(0, 0);
    }

    m_pPhysics->VAddKinematicBody(_owner);
}

TiXmlElement* KinematicComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    //

    return baseElement;
}

void KinematicComponent::VUpdate(uint32 msDiff)
{
    /*if (m_pPositionComponent->GetX() < m_MinPosition.x ||
        m_pPositionComponent->GetX() > m_MaxPosition.x )
    {
        m_CurrentSpeed = Point(m_CurrentSpeed.x * -1.0, m_CurrentSpeed.y * -1.0);
    }*/
    bool directionChanged = false;
    if (m_Speed.x > 0)
    {
        if (m_pPositionComponent->GetX() < m_MinPosition.x)
        {
            m_pPositionComponent->SetX(m_MinPosition.x);
            m_pPhysics->VSetPosition(_owner->GetGUID(), m_pPositionComponent->GetPosition());
            m_CurrentSpeed = Point(m_CurrentSpeed.x * -1.0, m_CurrentSpeed.y);
            directionChanged = true;
        }
        else if (m_pPositionComponent->GetX() > m_MaxPosition.x)
        {
            m_pPositionComponent->SetX(m_MaxPosition.x);
            m_pPhysics->VSetPosition(_owner->GetGUID(), m_pPositionComponent->GetPosition());
            m_CurrentSpeed = Point(m_CurrentSpeed.x * -1.0, m_CurrentSpeed.y);
            directionChanged = true;
        }
    }

    if (m_Speed.y > 0)
    {
        if (directionChanged)
        {
            m_CurrentSpeed = Point(m_CurrentSpeed.x, m_CurrentSpeed.y * -1.0);
        }
        else if (m_pPositionComponent->GetY() < m_MinPosition.y)
        {
            m_pPositionComponent->SetY(m_MinPosition.y);
            m_pPhysics->VSetPosition(_owner->GetGUID(), m_pPositionComponent->GetPosition());
            m_CurrentSpeed = Point(m_CurrentSpeed.x, m_CurrentSpeed.y * -1.0);
        }
        else if (m_pPositionComponent->GetY() > m_MaxPosition.y)
        {
            m_pPositionComponent->SetY(m_MaxPosition.y);
            m_pPhysics->VSetPosition(_owner->GetGUID(), m_pPositionComponent->GetPosition());
            m_CurrentSpeed = Point(m_CurrentSpeed.x, m_CurrentSpeed.y * -1.0);
        }
    }

    // Move

    /*LOG(ToStr(m_pPositionComponent->GetX()) + " - " + ToStr(m_pPositionComponent->GetY()));
    LOG(ToStr(m_MinPosition.x) + " - " + ToStr(m_MinPosition.y));*/

    m_pPhysics->VSetLinearSpeedEx(_owner->GetGUID(), m_CurrentSpeed);
}

void KinematicComponent::RemoveCarriedBody(b2Body* pBody)
{
    for (auto iter = m_CarriedBodiesList.begin(); iter != m_CarriedBodiesList.end(); ++iter)
    {
        if ((*iter) == pBody)
        {
            if (m_IsStartElevator)
            {
                //LOG("REMOVING");
                // TODO: Fix bug with start elevator when it moves in the same direction
                //m_Speed = m_CurrentSpeed;
                m_CurrentSpeed = Point(0, 0);
            }

            m_CarriedBodiesList.erase(iter);
            return;
        }
    }
}

void KinematicComponent::AddCarriedBody(b2Body* pBody)
{
    if ((m_IsTriggerElevator && !m_IsTriggered) || m_IsStartElevator)
    {
        m_IsTriggered = true;
        m_CurrentSpeed = m_Speed;
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
            
            shared_ptr<PhysicsComponent> pPhysicsComponent =
                MakeStrongPtr(pActor->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));

            // Provide sanity check
            if (pPhysicsComponent && pPhysicsComponent->GetNumFootContacts() == 0)
            {
                RemoveCarriedBody(pCarriedBody);
                continue;
            }

            if (pPhysicsComponent && pPhysicsComponent->GetVelocity().y < -4)
            {
                continue;
            }
            if (pPhysicsComponent && fabs(pPhysicsComponent->GetVelocity().x) > DBL_EPSILON)
            {
                continue;
            }

            shared_ptr<PositionComponent> pPositionComponent =
                MakeStrongPtr(pActor->GetComponent<PositionComponent>(PositionComponent::g_Name));
            Point currentPos = pPositionComponent->GetPosition();
            //pPositionComponent->SetPosition(currentPos.x + positionDelta.x, currentPos.y + positionDelta.y);
            
            m_pPhysics->VSetPosition(pActor->GetGUID(), Point(currentPos.x + positionDelta.x, currentPos.y + positionDelta.y));
        }
    }

    m_LastPosition = newPosition;
}

// After claw dies, set elevators to default position
void KinematicComponent::ClawDiedDelegate(IEventDataPtr pEventData)
{
    if (m_IsTriggerElevator)
    {
        m_IsTriggered = false;
        m_CurrentSpeed = Point(0, 0);
    }

    m_pPositionComponent->SetPosition(m_InitialPosition);
    m_pPhysics->VSetPosition(_owner->GetGUID(), m_InitialPosition);
}