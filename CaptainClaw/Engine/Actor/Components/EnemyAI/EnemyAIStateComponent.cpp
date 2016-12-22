#include "EnemyAIStateComponent.h"

#include "EnemyAIComponent.h"
#include "../AnimationComponent.h"
#include "../PhysicsComponent.h"
#include "../PositionComponent.h"
#include "../RenderComponent.h"

#include "../../../GameApp/BaseGameLogic.h"
#include "../../../GameApp/BaseGameApp.h"
#include "../../../Physics/ClawPhysics.h"

const char* BaseEnemyAIStateComponent::g_Name = "BaseEnemyAIStateComponent";
const char* PatrolEnemyAIStateComponent::g_Name = "PatrolEnemyAIStateComponent";

//=====================================================================================================================
// BaseEnemyAIStateComponent
//=====================================================================================================================

bool BaseEnemyAIStateComponent::VInit(TiXmlElement* pData)
{
    assert(pData);



    return VDelegateInit(pData);
}

void BaseEnemyAIStateComponent::VPostInit()
{
    m_pAnimationComponent = 
        MakeStrongPtr(_owner->GetComponent<AnimationComponent>(AnimationComponent::g_Name)).get();
    m_pPhysicsComponent =
        MakeStrongPtr(_owner->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name)).get();
    m_pPositionComponent =
        MakeStrongPtr(_owner->GetComponent<PositionComponent>(PositionComponent::g_Name)).get();
    m_pEnemyAIComponent = 
        MakeStrongPtr(_owner->GetComponent<EnemyAIComponent>(EnemyAIComponent::g_Name)).get();
    m_pRenderComponent =
        MakeStrongPtr(_owner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name)).get();

    assert(m_pAnimationComponent);
    assert(m_pPhysicsComponent);
    assert(m_pPositionComponent);
    assert(m_pEnemyAIComponent);
    assert(m_pRenderComponent);

    m_pEnemyAIComponent->RegisterState(m_StateName, this);
}

//=====================================================================================================================
// PatrolEnemyAIStateComponent
//=====================================================================================================================

static Direction ToOppositeDirection(Direction oldDirection)
{
    if (oldDirection == Direction_Left)
    {
        return Direction_Right;
    }

    return Direction_Left;
}

PatrolEnemyAIStateComponent::PatrolEnemyAIStateComponent()
    :
    m_LeftPatrolBorder(0),
    m_RightPatrolBorder(0),
    m_Direction(Direction_Right),
    //m_pCurrentAction(NULL),
    BaseEnemyAIStateComponent("PatrolState"),
    m_PatrolSpeed(0.0),
    m_bInitialized(false)
{

}

PatrolEnemyAIStateComponent::~PatrolEnemyAIStateComponent()
{

}

bool PatrolEnemyAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    assert(pData);

    m_pPhysics = g_pApp->GetGameLogic()->VGetGamePhysics();
    assert(m_pPhysics);

    if (TiXmlElement* pElem = pData->FirstChildElement("PatrolSpeed"))
    {
        m_PatrolSpeed = std::stod(pElem->GetText());
    }
    if (TiXmlElement* pElem = pData->FirstChildElement("LeftPatrolBorder"))
    {
        m_LeftPatrolBorder = std::stoi(pElem->GetText());
    }
    if (TiXmlElement* pElem = pData->FirstChildElement("RightPatrolBorder"))
    {
        m_RightPatrolBorder = std::stoi(pElem->GetText());
    }
    if (TiXmlElement* pElem = pData->FirstChildElement("WalkAction"))
    {
        m_pWalkAction.reset(new EnemyAIAction);
        m_pWalkAction->actionName = "WalkAction";

        if (TiXmlElement* pAnimElem = pElem->FirstChildElement("Animation"))
        {
            m_pWalkAction->animations.push_back(pAnimElem->GetText());
        }
    }
    if (TiXmlElement* pElem = pData->FirstChildElement("IdleAction"))
    {
        m_pIdleAction.reset(new EnemyAIAction);
        m_pIdleAction->actionName = "IdleAction";

        if (TiXmlElement* pAnimDelayElem = pElem->FirstChildElement("AnimationDelay"))
        {
            m_pIdleAction->animDelay = std::stoi(pAnimDelayElem->GetText());
        }

        for (TiXmlElement* pAnimElem = pElem->FirstChildElement("Animation");
            pAnimElem != NULL; pAnimElem = pAnimElem->NextSiblingElement("Animation"))
        {
            m_pIdleAction->animations.push_back(pAnimElem->GetText());
        }
    }

    /*m_LeftPatrolBorder = 6330;
    m_RightPatrolBorder = 6550;*/
    
    assert(fabs(m_PatrolSpeed) > DBL_EPSILON);

    assert(m_pWalkAction != nullptr);
    assert(!m_pWalkAction->animations.empty());
    if (m_pIdleAction == nullptr)
    {
        LOG_WARNING("No idle action");
    }
    else
    {
        assert(!m_pIdleAction->animations.empty());
    }

    return true;
}

void PatrolEnemyAIStateComponent::VPostInit()
{
    BaseEnemyAIStateComponent::VPostInit();

    m_pAnimationComponent->AddObserver(this);
}

void PatrolEnemyAIStateComponent::VUpdate(uint32 msDiff)
{
    if (!m_IsActive)
    {
        return;
    }

    // Has to be here because in VPostInit there is no guarantee that
    // PhysicsComponent is already initialized
    if (!m_bInitialized)
    {
        CalculatePatrolBorders();

        m_bInitialized = true;
    }

    // Only makes sense to check for stuff when walking
    if (m_pWalkAction->isActive)
    {
        if (fabs(m_pPhysics->VGetVelocity(_owner->GetGUID()).x) < DBL_EPSILON)
        {
            CommenceIdleBehaviour();
        }
        else if ((m_pPositionComponent->GetX() <= m_LeftPatrolBorder && m_Direction == Direction_Left) ||
            (m_pPositionComponent->GetX() >= m_RightPatrolBorder && m_Direction == Direction_Right))
        {
            CommenceIdleBehaviour();
        }
    }
}

void PatrolEnemyAIStateComponent::VOnStateEnter()
{
    m_IsActive = true;
    ChangeDirection(m_Direction);
}

void PatrolEnemyAIStateComponent::VOnStateLeave()
{
    m_IsActive = false;
}

void PatrolEnemyAIStateComponent::VOnAnimationLooped(Animation* pAnimation)
{
    if (!m_IsActive)
    { 
        return;
    }

    if (m_pIdleAction && m_pIdleAction->isActive)
    {
        if (m_pIdleAction->IsAtLastAnimation())
        {
            ChangeDirection(ToOppositeDirection(m_Direction));
        }
        else
        {
            m_pIdleAction->activeAnimIdx++;
            m_pAnimationComponent->SetAnimation(m_pIdleAction->animations[m_pIdleAction->activeAnimIdx]);
            m_pAnimationComponent->GetCurrentAnimation()->SetDelay(m_pIdleAction->animDelay);
        }
    }
}

double PatrolEnemyAIStateComponent::FindClosestHole(Point center, int height, float maxSearchDistance)
{
    double leftDelta = 0.0;
    for (leftDelta = 0.0; leftDelta < fabs(maxSearchDistance); leftDelta += 1.0)
    {
        if (maxSearchDistance < 0)
        {
            leftDelta *= -1.0;
        }

        Point fromPoint = Point(center.x + leftDelta, center.y);
        Point toPoint = Point(fromPoint.x, fromPoint.y + height / 2 + height / 4);

        RaycastResult raycastResultDown = m_pPhysics->VRayCast(fromPoint, toPoint, (CollisionFlag_Solid | CollisionFlag_Ground));
        if (!raycastResultDown.foundIntersection)
        {
            return leftDelta;
        }

        leftDelta = fabs(leftDelta);
    }

    return 0.0;
}

void PatrolEnemyAIStateComponent::CalculatePatrolBorders()
{
    SDL_Rect aabb = m_pPhysics->VGetAABB(_owner->GetGUID());

    Point center = m_pPositionComponent->GetPosition();

    Point toLeftRay = Point(center.x - 10000, center.y);
    Point toRightRay = Point(center.x + 10000, center.y);

    RaycastResult raycastResultLeft = m_pPhysics->VRayCast(center, toLeftRay, CollisionFlag_Solid);
    RaycastResult raycastResultRight = m_pPhysics->VRayCast(center, toRightRay, CollisionFlag_Solid);

    assert(raycastResultLeft.foundIntersection);
    assert(raycastResultRight.foundIntersection);

    double patrolLeftBorder = 0.0;
    double patrolRightBorder = 0.0;

    double leftDelta = FindClosestHole(center, aabb.h, raycastResultLeft.deltaX);
    if (fabs(leftDelta) < DBL_EPSILON)
    {
        patrolLeftBorder = center.x + raycastResultLeft.deltaX;
    }
    else
    {
        patrolLeftBorder = center.x + leftDelta;
    }

    double rightDelta = FindClosestHole(center, aabb.h, raycastResultRight.deltaX);
    if (fabs(rightDelta) < DBL_EPSILON)
    {
        patrolRightBorder = center.x + raycastResultRight.deltaX;
    }
    else
    {
        patrolRightBorder = center.x + rightDelta;
    }

    if (m_LeftPatrolBorder == 0 || m_LeftPatrolBorder < (int)patrolLeftBorder)
    {
        m_LeftPatrolBorder = (int)patrolLeftBorder + 25;
    }

    if (m_RightPatrolBorder == 0 || m_RightPatrolBorder > (int)patrolRightBorder)
    {
        m_RightPatrolBorder = (int)patrolRightBorder - 25;
    }

    // Some min and max x's were stupidly preset
    if (m_RightPatrolBorder < m_LeftPatrolBorder)
    {
        m_RightPatrolBorder = (int)patrolRightBorder - 25;
    }

    assert(m_LeftPatrolBorder > 0);
    assert(m_RightPatrolBorder > 0);
    assert(m_RightPatrolBorder > m_LeftPatrolBorder);
}

void PatrolEnemyAIStateComponent::ChangeDirection(Direction newDirection)
{
    m_Direction = newDirection;
    m_pRenderComponent->SetMirrored(m_Direction == Direction_Right);

    m_pWalkAction->isActive = true;
    m_pIdleAction->isActive = false;
    m_pAnimationComponent->SetAnimation(m_pWalkAction->animations[0]);

    if (m_Direction == Direction_Left)
    {
        m_pPhysics->VSetLinearSpeed(_owner->GetGUID(), Point(-1.0 * m_PatrolSpeed, 0.0));
    }
    else
    {
        m_pPhysics->VSetLinearSpeed(_owner->GetGUID(), Point(m_PatrolSpeed, 0.0));
    }
}

void PatrolEnemyAIStateComponent::CommenceIdleBehaviour()
{
    if (m_pIdleAction)
    {
        m_pWalkAction->isActive = false;
        m_pIdleAction->isActive = true;
        m_pIdleAction->activeAnimIdx = 0;
        m_pAnimationComponent->SetAnimation(m_pIdleAction->animations[0]);
        m_pAnimationComponent->GetCurrentAnimation()->SetDelay(m_pIdleAction->animDelay);
        m_pPhysics->VSetLinearSpeed(_owner->GetGUID(), Point(0.0, 0.0));
    }
    else
    {
        ChangeDirection(ToOppositeDirection(m_Direction));
    }
}