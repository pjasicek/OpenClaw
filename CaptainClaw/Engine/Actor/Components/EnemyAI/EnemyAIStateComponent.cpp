#include "EnemyAIStateComponent.h"

#include "EnemyAIComponent.h"
#include "../AnimationComponent.h"
#include "../PhysicsComponent.h"
#include "../PositionComponent.h"
#include "../RenderComponent.h"

#include "../../../GameApp/BaseGameLogic.h"
#include "../../../GameApp/BaseGameApp.h"
#include "../../../Physics/ClawPhysics.h"
#include "../ControllableComponent.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"

#include <time.h>

const char* BaseEnemyAIStateComponent::g_Name = "BaseEnemyAIStateComponent";
const char* TakeDamageAIStateComponent::g_Name = "TakeDamageAIStateComponent";
const char* PatrolEnemyAIStateComponent::g_Name = "PatrolEnemyAIStateComponent";
const char* BaseAttackAIStateComponent::g_Name = "BaseAttackAIStateComponent";
const char* MeleeAttackAIStateComponent::g_Name = "MeleeAttackAIStateComponent";
const char* DuckMeleeAttackAIStateComponent::g_Name = "DuckMeleeAttackAIStateComponent";
const char* RangedAttackAIStateComponent::g_Name = "RangedAttackAIStateComponent";
const char* DuckRangedAttackAIStateComponent::g_Name = "DuckRangedAttackAIStateComponent";

#define MAX_STATE_PRIORITY INT32_MAX
#define MIN_STATE_PRIORITY INT32_MIN

// TODO: Returning pointer like this is inconsistent...
static shared_ptr<EnemyAttackAction> XmlToEnemyAttackActionPtr(TiXmlElement* pElem)
{
    assert(pElem != NULL);

    shared_ptr<EnemyAttackAction> pAttackAction(new EnemyAttackAction);

    if (TiXmlElement* pAnimationElem = pElem->FirstChildElement("Animation"))
    {
        pAttackAction->animation = pAnimationElem->GetText();
    }
    if (TiXmlElement* pAnimAttackFrameIdxElem = pElem->FirstChildElement("AttackAnimFrameIdx"))
    {
        pAttackAction->attackAnimFrameIdx = std::stoi(pAnimAttackFrameIdxElem->GetText());
    }
    if (TiXmlElement* pAttackTypeElem = pElem->FirstChildElement("AttackType"))
    {
        pAttackAction->attackDamageType = DamageType(std::stoi(pAttackTypeElem->GetText()));
    }
    if (TiXmlElement* pAttackFxImageSetElem = pElem->FirstChildElement("AttackFxImageSet"))
    {
        pAttackAction->attackFxImageSet = pAttackFxImageSetElem->GetText();
    }
    if (TiXmlElement* pAttackOffsetElem = pElem->FirstChildElement("AttackSpawnPositionOffset"))
    {
        pAttackOffsetElem->Attribute("x", &pAttackAction->attackSpawnPositionOffset.x);
        pAttackOffsetElem->Attribute("y", &pAttackAction->attackSpawnPositionOffset.y);
    }
    if (TiXmlElement* pAttackAreaSizeElem = pElem->FirstChildElement("AttackAreaSize"))
    {
        pAttackAreaSizeElem->Attribute("width", &pAttackAction->attackAreaSize.x);
        pAttackAreaSizeElem->Attribute("height", &pAttackAction->attackAreaSize.y);
    }
    if (TiXmlElement* pDamageElem = pElem->FirstChildElement("Damage"))
    {
        pAttackAction->damage = std::stoi(pDamageElem->GetText());
    }

    pAttackAction->agroSensorFixture = ActorTemplates::XmlToActorFixtureDef(pElem->FirstChildElement("AgroSensorFixture"));

    return pAttackAction;
}

//=====================================================================================================================
// BaseEnemyAIStateComponent
//=====================================================================================================================

BaseEnemyAIStateComponent::BaseEnemyAIStateComponent(std::string stateName)
    :
    m_IsActive(false),
    m_StatePriority(-1),
    m_StateName(stateName),
    m_pPhysicsComponent(NULL),
    m_pPositionComponent(NULL),
    m_pAnimationComponent(NULL),
    m_pEnemyAIComponent(NULL),
    m_pRenderComponent(NULL)
{

}

bool BaseEnemyAIStateComponent::VInit(TiXmlElement* pData)
{
    assert(pData);

    assert(ParseValueFromXmlElem(&m_StatePriority, pData->FirstChildElement("StatePriority")));

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

    assert(m_StatePriority >= 0);

    m_pEnemyAIComponent->RegisterState(m_StateName, this);
}

//=====================================================================================================================
// TakingDamageAIStateComponent
//=====================================================================================================================

TakeDamageAIStateComponent::TakeDamageAIStateComponent()
    : BaseEnemyAIStateComponent("TakeDamageState")
{

}

bool TakeDamageAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    for (TiXmlElement* pElem = pData->FirstChildElement("TakeDamageAnimation");
        pElem != NULL; 
        pElem = pElem->NextSiblingElement("TakeDamageAnimation"))
    {
        m_TakeDamageAnimations.push_back(pElem->GetText());
    }

    assert(m_TakeDamageAnimations.size() > 0);

    return true;
}

void TakeDamageAIStateComponent::VPostInit()
{
    BaseEnemyAIStateComponent::VPostInit();

    m_pAnimationComponent->AddObserver(this);
}

void TakeDamageAIStateComponent::VUpdate(uint32 msDiff)
{

}

void TakeDamageAIStateComponent::VOnStateEnter()
{
    int randomAnimIdx = Util::GetRandomNumber(0, m_TakeDamageAnimations.size() - 1);
    std::string takeDamageAnim = m_TakeDamageAnimations[randomAnimIdx];
    m_pAnimationComponent->SetAnimation(takeDamageAnim);

    // When taking damage, nothing can interrupt him
    m_StatePriority = MAX_STATE_PRIORITY;

    m_IsActive = true;
}

void TakeDamageAIStateComponent::VOnStateLeave()
{
    m_IsActive = false;
    m_StatePriority = MIN_STATE_PRIORITY;
}

void TakeDamageAIStateComponent::VOnAnimationAtLastFrame(Animation* pAnimation)
{
    if (!m_IsActive)
    {
        return;
    }

    // When done taking damage, everything takes precedence
    m_StatePriority = MIN_STATE_PRIORITY;

    m_pEnemyAIComponent->EnterBestState(true);
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
    m_bInitialized(false),
    m_bRetainDirection(false),
    m_IsAlwaysIdle(false)
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
    if (TiXmlElement* pElem = pData->FirstChildElement("RetainDirection"))
    {
        m_bRetainDirection = std::string(pElem->GetText()) == "true";
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

    Point noSpeed(0, 0);
    m_pPhysics->VSetLinearSpeed(_owner->GetGUID(), noSpeed);
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

        return;
    }

    // If this unit does not have room to move, just let it be idle
    if (m_IsAlwaysIdle)
    {
        if (m_pIdleAction && !m_pIdleAction->isActive)
        {
            CommenceIdleBehaviour();
        }
        
        return;
    }

    // Only makes sense to check for stuff when walking
    if (m_pWalkAction->isActive)
    {
        if (fabs(m_pPhysics->VGetVelocity(_owner->GetGUID()).x) < 0.1)
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
    //CommenceIdleBehaviour();
    ChangeDirection(m_Direction);
}

void PatrolEnemyAIStateComponent::VOnStateLeave()
{
    m_IsActive = false;

    // Since this state set speed to this actor, we need to stop it moving
    m_pPhysics->VSetLinearSpeed(_owner->GetGUID(), Point(0.0, 0.0));
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
        Point toPoint = Point(fromPoint.x, fromPoint.y + height / 2 + height / 2);

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
    SDL_Rect aabb = m_pPhysics->VGetAABB(_owner->GetGUID(), true);

    Point center = m_pPositionComponent->GetPosition();

    Point toLeftRay = Point(center.x - 10000, center.y);
    Point toRightRay = Point(center.x + 10000, center.y);

    RaycastResult raycastResultLeft = m_pPhysics->VRayCast(center, toLeftRay, CollisionFlag_Solid);
    RaycastResult raycastResultRight = m_pPhysics->VRayCast(center, toRightRay, CollisionFlag_Solid);

    if (!raycastResultLeft.foundIntersection)
    {
        LOG_ERROR("Did not find raycastResultLeft intersection for actor: " + _owner->GetName() +
            " with position: " + _owner->GetPositionComponent()->GetPosition().ToString());
        // Dummy large value, should be sufficient
        raycastResultLeft.deltaX = center.x - 10000;
    }
    if (!raycastResultRight.foundIntersection)
    {
        LOG_ERROR("Did not find raycastResultRight intersection for actor: " + _owner->GetName() +
            " with position: " + _owner->GetPositionComponent()->GetPosition().ToString());
        // Dummy large value, should be sufficient
        raycastResultLeft.deltaX = center.x + 10000;
    }

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

    m_LeftPatrolBorder += 10;
    m_RightPatrolBorder -= 10;

    // Not enough room for the unit to move, dont force it, it would fall
    if (m_LeftPatrolBorder >= m_RightPatrolBorder)
    {
        m_IsAlwaysIdle = true;
        m_bRetainDirection = true;
    }

    // Move them into the middle of their patrol path at the beginning
    // Doing not so caused some bugs with level resets
    Point spawnPosition((m_LeftPatrolBorder + m_RightPatrolBorder) / 2, m_pPositionComponent->GetY());
    m_pPhysics->VSetPosition(_owner->GetGUID(), spawnPosition);
    m_pPositionComponent->SetPosition(spawnPosition);

    /*shared_ptr<EventData_Move_Actor> pEvent(new EventData_Move_Actor(_owner->GetGUID(), spawnPosition));
    IEventMgr::Get()->VQueueEvent(pEvent);*/

    assert(m_LeftPatrolBorder > 0);
    assert(m_RightPatrolBorder > 0);
    //assert(m_RightPatrolBorder > m_LeftPatrolBorder);
}

void PatrolEnemyAIStateComponent::ChangeDirection(Direction newDirection)
{
    m_Direction = newDirection;
    if (!m_bRetainDirection)
    {
        m_pRenderComponent->SetMirrored(m_Direction == Direction_Right);
    }

    m_pWalkAction->isActive = true;
    if (m_pIdleAction)
    {
        m_pIdleAction->isActive = false;
    }
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

//=====================================================================================================================
// BaseAttackAIStateComponent
//=====================================================================================================================

BaseAttackAIStateComponent::BaseAttackAIStateComponent(std::string stateName)
    :
    BaseEnemyAIStateComponent(stateName)
{

}

BaseAttackAIStateComponent::~BaseAttackAIStateComponent()
{

}

bool BaseAttackAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    assert(pData);

    if (TiXmlElement* pMeleeAttacksElem = pData->FirstChildElement("Attacks"))
    {
        for (TiXmlElement* pElem = pMeleeAttacksElem->FirstChildElement("AttackAction");
            pElem != NULL;
            pElem = pElem->NextSiblingElement("AttackAction"))
        {
            shared_ptr<EnemyAttackAction> pAttackAction = XmlToEnemyAttackActionPtr(pElem);

            assert(pAttackAction->animation != "");
            assert(pAttackAction->damage > 0);

            m_AttackActions.push_back(pAttackAction);
        }
    }

    assert(!m_AttackActions.empty());
    assert(m_AttackActions.size() == 1 && "Only supporting one attack action per state component");

    return true;
}

void BaseAttackAIStateComponent::VPostInit()
{
    BaseEnemyAIStateComponent::VPostInit();

    m_pAnimationComponent->AddObserver(this);
}

void BaseAttackAIStateComponent::VPostPostInit()
{
    for (shared_ptr<EnemyAttackAction> pAttackAction : m_AttackActions)
    {
        g_pApp->GetGameLogic()->VGetGamePhysics()->VAddActorFixtureToBody(
            _owner->GetGUID(),
            &pAttackAction->agroSensorFixture);
    }
}

void BaseAttackAIStateComponent::VOnStateEnter()
{
    m_IsActive = true;
    VExecuteAttack();
}

void BaseAttackAIStateComponent::VOnStateLeave()
{
    m_IsActive = false;
}

void BaseAttackAIStateComponent::VExecuteAttack()
{
    if (!m_IsActive)
    {
        return;
    }

    Point closestEnemyOffset = FindClosestHostileActorOffset();
    //LOG("X Offset: " + ToStr(closestEnemyOffset.x));
    if (closestEnemyOffset.x < 0)
    {
        m_pRenderComponent->SetMirrored(false);
    }
    else
    {
        m_pRenderComponent->SetMirrored(true);
    }

    srand((long)this + time(NULL));

    // TODO: Pick randomly melee action ?

    m_pAnimationComponent->SetAnimation(m_AttackActions[0]->animation);
}

void BaseAttackAIStateComponent::VOnAnimationLooped(Animation* pAnimation)
{
    if (!m_IsActive)
    {
        return;
    }

    // TODO: Support melee actions consiting of multiple animations ?
    m_pEnemyAIComponent->EnterBestState(true);
    VExecuteAttack();
}

void BaseAttackAIStateComponent::VOnAnimationFrameChanged(
    Animation* pAnimation,
    AnimationFrame* pLastFrame,
    AnimationFrame* pNewFrame)
{
    if (!m_IsActive)
    {
        return;
    }

    // Could be leftover from past state which changed just now
    if (m_pAnimationComponent->GetCurrentAnimation()->GetCurrentAnimationFrame() != pNewFrame)
    {
        return;
    }

    if (m_AttackActions[0]->attackAnimFrameIdx == pNewFrame->idx)
    {
        std::shared_ptr<EnemyAttackAction> pAttack = m_AttackActions[0];

        Direction dir = Direction_Left;
        Point offset = pAttack->attackSpawnPositionOffset;
        if (m_pRenderComponent->IsMirrored())
        {
            dir = Direction_Right;
            offset = Point(offset.x * -1.0, offset.y);
        }

        VOnAttackFrame(pAttack, dir, offset);
    }
}

void BaseAttackAIStateComponent::OnEnemyEnterAgroRange(Actor* pEnemy)
{
    m_EnemyAgroList.push_back(pEnemy);
    m_pEnemyAIComponent->EnterBestState(false);
}

void BaseAttackAIStateComponent::OnEnemyLeftAgroRange(Actor* pEnemy)
{
    for (auto iter = m_EnemyAgroList.begin(); iter != m_EnemyAgroList.end(); ++iter)
    {
        if ((*iter) == pEnemy)
        {
            m_EnemyAgroList.erase(iter);
            return;
        }
    }

    LOG_WARNING("Could not remove enemy - no such actor found");
}

Actor* BaseAttackAIStateComponent::FindClosestHostileActor()
{
    if (m_EnemyAgroList.empty())
    {
        return NULL;
    }

    Point closest(DBL_MAX, DBL_MAX);
    Actor* pClosestEnemy = NULL;

    for (Actor* pHostileActor : m_EnemyAgroList)
    {
        assert(pHostileActor != NULL);

        shared_ptr<PositionComponent> pHostileActorPositionComponent =
            MakeStrongPtr(pHostileActor->GetComponent<PositionComponent>(PositionComponent::g_Name));
        assert(pHostileActorPositionComponent);

        Point positionDiff = pHostileActorPositionComponent->GetPosition() - m_pPositionComponent->GetPosition();
        if (positionDiff.Length() < (m_pPositionComponent->GetPosition() - closest).Length())
        {
            closest = positionDiff;
            pClosestEnemy = pHostileActor;
        }
    }

    assert(std::fabs(closest.x) < DBL_MAX && std::fabs(closest.y) < DBL_MAX);
    assert(pClosestEnemy != NULL);

    return pClosestEnemy;
}

Point BaseAttackAIStateComponent::FindClosestHostileActorOffset()
{
    Point closest(DBL_MAX, DBL_MAX);

    Actor* pClosestEnemy = FindClosestHostileActor();
    if (pClosestEnemy == NULL)
    {
        return closest;
    }

    shared_ptr<PositionComponent> pHostileActorPositionComponent =
        MakeStrongPtr(pClosestEnemy->GetComponent<PositionComponent>(PositionComponent::g_Name));
    assert(pHostileActorPositionComponent);

    return pHostileActorPositionComponent->GetPosition() - m_pPositionComponent->GetPosition();
}

//=====================================================================================================================
// MeleeAttackAIStateComponent
//=====================================================================================================================

MeleeAttackAIStateComponent::MeleeAttackAIStateComponent()
    :
    BaseAttackAIStateComponent("MeleeAttackState")
{

}

MeleeAttackAIStateComponent::~MeleeAttackAIStateComponent()
{

}

bool MeleeAttackAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    assert(pData);

    if (!BaseAttackAIStateComponent::VDelegateInit(pData))
    {
        return false;
    }

    return true;
}

void MeleeAttackAIStateComponent::VOnAttackFrame(std::shared_ptr<EnemyAttackAction> pAttack, Direction dir, const Point& offset)
{
    ActorTemplates::CreateAreaDamage(
        m_pPositionComponent->GetPosition() + offset,
        pAttack->attackAreaSize,
        pAttack->damage,
        CollisionFlag_EnemyAIAttack,
        "Rectangle",
        DamageType_MeleeAttack,
        dir);

    // Play melee attack sound
    Util::PlayRandomSoundFromList(m_pEnemyAIComponent->GetMeleeAttackSounds());
}

//=====================================================================================================================
// DuckMeleeAttackAIStateComponent
//=====================================================================================================================

DuckMeleeAttackAIStateComponent::DuckMeleeAttackAIStateComponent()
    :
    BaseAttackAIStateComponent("DuckMeleeAttackState")
{

}

DuckMeleeAttackAIStateComponent::~DuckMeleeAttackAIStateComponent()
{

}

bool DuckMeleeAttackAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    assert(pData);

    if (!BaseAttackAIStateComponent::VDelegateInit(pData))
    {
        return false;
    }

    return true;
}

void DuckMeleeAttackAIStateComponent::VOnAttackFrame(std::shared_ptr<EnemyAttackAction> pAttack, Direction dir, const Point& offset)
{
    ActorTemplates::CreateAreaDamage(
        m_pPositionComponent->GetPosition() + offset,
        pAttack->attackAreaSize,
        pAttack->damage,
        CollisionFlag_EnemyAIAttack,
        "Rectangle",
        DamageType_MeleeAttack,
        dir);

    // Play melee attack sound
    Util::PlayRandomSoundFromList(m_pEnemyAIComponent->GetMeleeAttackSounds());
}

//=====================================================================================================================
// RangedAttackAIStateComponent
//=====================================================================================================================

RangedAttackAIStateComponent::RangedAttackAIStateComponent()
    :
    BaseAttackAIStateComponent("RangedAttackState")
{

}

RangedAttackAIStateComponent::~RangedAttackAIStateComponent()
{

}

bool RangedAttackAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    assert(pData);

    if (!BaseAttackAIStateComponent::VDelegateInit(pData))
    {
        return false;
    }

    return true;
}

void RangedAttackAIStateComponent::VOnAttackFrame(std::shared_ptr<EnemyAttackAction> pAttack, Direction dir, const Point& offset)
{
    ActorTemplates::CreateProjectile(
        pAttack->attackFxImageSet,
        pAttack->damage,
        pAttack->attackDamageType,
        dir,
        m_pPositionComponent->GetPosition() + offset,
        CollisionFlag_EnemyAIProjectile,
        (CollisionFlag_Controller | CollisionFlag_Solid));

    // Play ranged attack sound
    Util::PlayRandomSoundFromList(m_pEnemyAIComponent->GetRangedAttackSounds());
}

//=====================================================================================================================
// DuckRangedAttackAIStateComponent
//=====================================================================================================================

DuckRangedAttackAIStateComponent::DuckRangedAttackAIStateComponent()
    :
    BaseAttackAIStateComponent("DuckRangedAttackState")
{

}

DuckRangedAttackAIStateComponent::~DuckRangedAttackAIStateComponent()
{

}

bool DuckRangedAttackAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    assert(pData);

    if (!BaseAttackAIStateComponent::VDelegateInit(pData))
    {
        return false;
    }

    return true;
}

void DuckRangedAttackAIStateComponent::VOnAttackFrame(std::shared_ptr<EnemyAttackAction> pAttack, Direction dir, const Point& offset)
{
    ActorTemplates::CreateProjectile(
        pAttack->attackFxImageSet,
        pAttack->damage,
        pAttack->attackDamageType,
        dir,
        m_pPositionComponent->GetPosition() + offset,
        CollisionFlag_EnemyAIProjectile,
        (CollisionFlag_Controller | CollisionFlag_Solid));

    // Play ranged attack sound
    Util::PlayRandomSoundFromList(m_pEnemyAIComponent->GetRangedAttackSounds());
}

bool DuckRangedAttackAIStateComponent::VCanEnter()
{
    if (m_EnemyAgroList.empty())
    {
        return false;
    }

    Actor* pClosestEnemy = FindClosestHostileActor();
    if (pClosestEnemy == NULL)
    {
        return false;
    }

    // TODO: This assumes that the only hostile actor can be Claw.
    shared_ptr<ClawControllableComponent> pClawComponent =
        MakeStrongPtr(pClosestEnemy->GetComponent<ClawControllableComponent>(ClawControllableComponent::g_Name));
    assert(pClawComponent != nullptr);

    if (pClawComponent->IsDucking())
    {
        return true;
    }

    return false;
}