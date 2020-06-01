#include "PathElevatorComponent.h"
#include "PhysicsComponent.h"
#include "PositionComponent.h"

#include "../../GameApp/BaseGameApp.h"
#include "../../GameApp/BaseGameLogic.h"

#include "../../Events/EventMgr.h"
#include "../../Events/Events.h"

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
    assert(m_Properties.elevatorPath.size() >= 1);

    m_pPhysics = g_pApp->GetGameLogic()->VGetGamePhysics();
    assert(m_pPhysics != nullptr);

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
    auto pPC = m_pOwner->GetPositionComponent();
    assert(pPC != nullptr);

    m_LastPosition = pPC->GetPosition();
    m_InitialPosition = pPC->GetPosition();

    int i = 0;
    Point previousStepPosition = pPC->GetPosition();
    for (ElevatorStepDef& elevatorStep : m_Properties.elevatorPath)
    {
        //assert(elevatorStep.direction != Direction_None && "PathElevator waiting not supported at the moment");
        if (elevatorStep.direction != Direction_None)
        {
            Point dirSpeed = CalculateSpeed(1.0, elevatorStep.direction);
            Point deltaStep(dirSpeed.x * elevatorStep.stepDeltaDistance, dirSpeed.y * elevatorStep.stepDeltaDistance);

            elevatorStep.destinationPosition = previousStepPosition + deltaStep;

            previousStepPosition = elevatorStep.destinationPosition;
        }
    }

    m_CurrentStepDef = m_Properties.elevatorPath[m_CurrentStepDefIdx];

    // From px/s to m/s
    m_Properties.speed *= g_pApp->GetGlobalOptions()->platformSpeedModifier;

    m_CurrentSpeed = CalculateSpeed(m_Properties.speed, m_CurrentStepDef.direction);
    m_pPhysics->VSetLinearSpeed(m_pOwner->GetGUID(), m_CurrentSpeed);
}

void PathElevatorComponent::VUpdate(uint32 msDiff)
{
    if (m_CurrentStepDef.isWaiting)
    {
        m_StepTime += msDiff;
        if (m_StepTime >= m_CurrentStepDef.waitMsTime)
        {
            ChangeToNextStep();
        }
    }
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
        case Direction_None: calculatedSpeed.Set(0.0, 0.0); break;
        default: assert(false);
    }

    if (dir != Direction_None)
    {
        assert(!calculatedSpeed.IsZeroXY());
    }

    return calculatedSpeed;
}

void PathElevatorComponent::ChangeToNextStep()
{
    if (m_CurrentStepDefIdx == m_Properties.elevatorPath.size() - 1)
    {
        if (m_Properties.elevatorPath.size() == 1)
        {
            ElevatorStepDef& step = m_Properties.elevatorPath[0];
            m_pPhysics->VSetPosition(m_pOwner->GetGUID(), m_InitialPosition);
            m_pOwner->GetPositionComponent()->SetPosition(m_InitialPosition);
        }

        m_CurrentStepDefIdx = 0;
    }
    else
    {
        m_CurrentStepDefIdx++;
    }

    m_CurrentStepDef = m_Properties.elevatorPath[m_CurrentStepDefIdx];

    if (m_CurrentStepDef.direction == Direction_None)
    {
        m_CurrentSpeed.Set(0.0, 0.0);
        m_pPhysics->VSetLinearSpeed(m_pOwner->GetGUID(), m_CurrentSpeed);
    }
    else
    {
        m_CurrentSpeed = CalculateSpeed(m_Properties.speed, m_CurrentStepDef.direction);
        m_pPhysics->VSetLinearSpeed(m_pOwner->GetGUID(), m_CurrentSpeed);
    }

    m_StepTime = 0;
    m_StepElapsedDistance = 0;
}

bool PathElevatorComponent::ShouldChangeDirection(const Point& newPosition)
{
    // It should change position if it is past its X destination or Y destination
    Point speedDirection = CalculateSpeed(1.0f, m_CurrentStepDef.direction);

    // Elevator is moving Up
    if ((speedDirection.y < (-1.0 * DBL_EPSILON) && (newPosition.y < m_CurrentStepDef.destinationPosition.y)))
    {
        return true;
    }
    // Elevator is moving Down
    else if ((speedDirection.y > DBL_EPSILON) && (newPosition.y > m_CurrentStepDef.destinationPosition.y))
    {
        return true;
    }
    // Elevator is moving Left
    if ((speedDirection.x < (-1.0 * DBL_EPSILON) && (newPosition.x < m_CurrentStepDef.destinationPosition.x)))
    {
        return true;
    }
    // Elevator is moving Right
    else if ((speedDirection.x > DBL_EPSILON) && (newPosition.x > m_CurrentStepDef.destinationPosition.x))
    {
        return true;
    }

    return false;
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
    if (ShouldChangeDirection(newPosition))
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
        }
    }

    m_LastPosition = newPosition;
}
