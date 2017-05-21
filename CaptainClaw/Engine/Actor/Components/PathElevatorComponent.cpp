#include "PathElevatorComponent.h"
#include "PhysicsComponent.h"
#include "PositionComponent.h"

#include "../../GameApp/BaseGameApp.h"
#include "../../GameApp/BaseGameLogic.h"

const char* PathElevatorComponent::g_Name = "PathElevatorComponent";

PathElevatorComponent::PathElevatorComponent()
    :
    m_CurrentDirection(Direction_None),
    m_StepTime(0),
    m_StepElapsedDistance(0.0),
    m_CurrentStepDefIdx(0),
    m_pPhysics(nullptr)
{

}

bool PathElevatorComponent::VInit(TiXmlElement* pData)
{
    assert(pData != NULL);

    m_Properties.LoadFromXml(pData, true);
    
    assert(m_Properties.speed > DBL_EPSILON);
    assert(m_Properties.elevatorPath.size() > 2);

    m_pPhysics = g_pApp->GetGameLogic()->VGetGamePhysics();
    assert(m_pPhysics != nullptr);

    m_Properties.speed *= g_pApp->GetGlobalOptions()->platformSpeedModifier;
    m_CurrentStepDef = m_Properties.elevatorPath[m_CurrentStepDefIdx];

    /*LOG("Speed: " + ToStr(m_Properties.speed));
    int i = 0;
    for (auto def : m_Properties.elevatorPath)
    {
        if (!def.isWaiting)
        {
        LOG("[" + ToStr(i++) + "]: " + ToStr((int)def.direction) + " -> " + ToStr(def.stepDeltaDistance));
        }
        else
        {
        LOG("[" + ToStr(i++) + "]: " + ToStr(def.isWaiting) + " -> " + ToStr(def.waitMsTime));
        }
    }*/

    return true;
}

void PathElevatorComponent::VPostInit()
{
    m_CurrentSpeed = CalculateSpeed(m_Properties.speed, m_CurrentStepDef.direction);
    m_pPhysics->VSetLinearSpeed(_owner->GetGUID(), m_CurrentSpeed);

    auto pPC = MakeStrongPtr(_owner->GetComponent<PositionComponent>());
    m_LastPosition = pPC->GetPosition();
}

void PathElevatorComponent::VUpdate(uint32 msDiff)
{

}

double PathElevatorComponent::CalculateElapsedDistance(const Point& lastPosition, const Point& currentPosition, Direction dir)
{

}

Point PathElevatorComponent::CalculateSpeed(double speed, Direction dir)
{
    Point calculatedSpeed;

    switch (dir)
    {
        case Direction_Up: calculatedSpeed.Set(0, -1.0 * speed); break;
        case Direction_Up_Right: calculatedSpeed.Set(speed, -1.0 * speed); break;
        case Direction_Right: calculatedSpeed.Set(speed, 0); break;
        case Direction_Down_Right: calculatedSpeed.Set(speed, speed); break;
        case Direction_Down: calculatedSpeed.Set(0, speed); break;
        case Direction_Down_Left: calculatedSpeed.Set(-1.0 * speed, speed); break;
        case Direction_Left: calculatedSpeed.Set(-1.0 * speed, 0); break;
        case Direction_Up_Left: calculatedSpeed.Set(-1.0 * speed, -1.0 * speed); break;
        default: assert(false);
    }

    assert(!calculatedSpeed.IsZeroXY());

    return calculatedSpeed;
}

void PathElevatorComponent::ChangeToNextStep()
{
    if (m_CurrentStepDefIdx == m_Properties.elevatorPath.size() - 1)
    {
        m_CurrentStepDefIdx = 0;
    }
    else
    {
        m_CurrentStepDefIdx++;
    }

    m_CurrentStepDef = m_Properties.elevatorPath[m_CurrentStepDefIdx];
    m_CurrentSpeed = CalculateSpeed(m_Properties.speed, m_CurrentStepDef.direction);
    m_pPhysics->VSetLinearSpeed(_owner->GetGUID(), m_CurrentSpeed);
    m_StepElapsedDistance = 0;
}

void PathElevatorComponent::AddCarriedBody(b2Body* pBody)
{
    for (auto iter = m_CarriedBodiesList.begin(); iter != m_CarriedBodiesList.end(); ++iter)
    {
        if ((*iter) == pBody)
        {
            m_CarriedBodiesList.erase(iter);
            return;
        }
    }
}

void PathElevatorComponent::RemoveCarriedBody(b2Body* pBody)
{

}

void PathElevatorComponent::OnMoved(Point newPosition)
{
    Point positionDelta = Point(newPosition.x - m_LastPosition.x, newPosition.y - m_LastPosition.y);

    m_StepElapsedDistance += max(fabs(positionDelta.x), fabs(positionDelta.y));
    if (m_StepElapsedDistance >= m_CurrentStepDef.stepDeltaDistance)
    {
        ChangeToNextStep();
    }

    if (m_CarriedBodiesList.empty())
    {
        m_LastPosition = newPosition;
        return;
    }

    if (fabs(m_CurrentSpeed.x) < DBL_EPSILON)
    {
        return;
    }

    
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

            if (pPhysicsComponent)
            {
                pPhysicsComponent->SetExternalSourceSpeed(m_CurrentSpeed);
            }
        }
    }

    m_LastPosition = newPosition;
}