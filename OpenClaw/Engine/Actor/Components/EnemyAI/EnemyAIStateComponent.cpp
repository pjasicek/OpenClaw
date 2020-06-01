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
const char* FallAIStateComponent::g_Name = "FallAIStateComponent";
const char* PatrolEnemyAIStateComponent::g_Name = "PatrolEnemyAIStateComponent";
const char* ParryEnemyAIStateComponent::g_Name = "ParryEnemyAIStateComponent";
const char* BaseAttackAIStateComponent::g_Name = "BaseAttackAIStateComponent";
const char* MeleeAttackAIStateComponent::g_Name = "MeleeAttackAIStateComponent";
const char* DuckMeleeAttackAIStateComponent::g_Name = "DuckMeleeAttackAIStateComponent";
const char* RangedAttackAIStateComponent::g_Name = "RangedAttackAIStateComponent";
const char* DuckRangedAttackAIStateComponent::g_Name = "DuckRangedAttackAIStateComponent";
const char* DiveAttackAIStateComponent::g_Name = "DiveAttackAIStateComponent";
const char* BaseBossAIStateComponennt::g_Name = "BaseBossAIStateComponennt";
const char* LaRauxBossAIStateComponent::g_Name = "LaRauxBossAIStateComponent";
const char* KatherineBossAIStateComponent::g_Name = "KatherineBossAIStateComponent";
const char* WolvingtonBossAIStateComponent::g_Name = "WolvingtonBossAIStateComponent";
const char* RollEnemyAIStateComponent::g_Name = "RollEnemyAIStateComponent";

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

    // This should be used, the stuff behind (AttackFxImageSet, AttackAreaSize, ...) is deprecated
    std::string projectileProtoStr;
    if (ParseValueFromXmlElem(&projectileProtoStr, pElem->FirstChildElement("ProjectilePrototype")))
    {
        pAttackAction->projectileProto = StringToEnum_ActorPrototype(projectileProtoStr);
    }

    ParseValueFromXmlElem(&pAttackAction->isMirrored, pElem->FirstChildElement("IsMirrored"));

    pAttackAction->agroSensorFixture = ActorTemplates::XmlToActorFixtureDef(pElem->FirstChildElement("AgroSensorFixture"));

    return pAttackAction;
}

//=====================================================================================================================
// BaseEnemyAIStateComponent
//=====================================================================================================================

BaseEnemyAIStateComponent::BaseEnemyAIStateComponent(const std::string &stateName)
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

    DO_AND_CHECK(ParseValueFromXmlElem(&m_StatePriority, pData->FirstChildElement("StatePriority")));

    return VDelegateInit(pData);
}

void BaseEnemyAIStateComponent::VPostInit()
{
    m_pAnimationComponent = 
        MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>(AnimationComponent::g_Name)).get();
    m_pPhysicsComponent = m_pOwner->GetPhysicsComponent().get();
    m_pPositionComponent = m_pOwner->GetPositionComponent().get();
    m_pEnemyAIComponent = 
        MakeStrongPtr(m_pOwner->GetComponent<EnemyAIComponent>(EnemyAIComponent::g_Name)).get();
    m_pRenderComponent =
        MakeStrongPtr(m_pOwner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name)).get();

    assert(m_pAnimationComponent);
    assert(m_pPhysicsComponent);
    assert(m_pPositionComponent);
    assert(m_pEnemyAIComponent);
    assert(m_pRenderComponent);

    assert(m_StatePriority >= 0);

    m_pEnemyAIComponent->RegisterState(m_StateName, this);
}

bool BaseEnemyAIStateComponent::IsActorWithinLOS(Actor* pActor)
{
    static const double thresholdY = 50.0;

    Point fromPoint = m_pOwner->GetPositionComponent()->GetPosition();
    Point toPoint = pActor->GetPositionComponent()->GetPosition();

    if (abs(fromPoint.y - toPoint.y) > thresholdY)
    {
        return false;
    }

    RaycastResult raycastResult = g_pApp->GetGameLogic()->VGetGamePhysics()->VRayCast(
        fromPoint,
        toPoint,
        CollisionFlag_Solid);
    if (raycastResult.foundIntersection)
    {
        // Vision is obstructed
        return false;
    }

    return true;
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

void TakeDamageAIStateComponent::VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState)
{
    int randomAnimIdx = Util::GetRandomNumber(0, m_TakeDamageAnimations.size() - 1);
    std::string takeDamageAnim = m_TakeDamageAnimations[randomAnimIdx];
    m_pAnimationComponent->SetAnimation(takeDamageAnim);

    // When taking damage, nothing can interrupt him
    m_StatePriority = MAX_STATE_PRIORITY;

    m_IsActive = true;
}

void TakeDamageAIStateComponent::VOnStateLeave(BaseEnemyAIStateComponent* pNextState)
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
// FallAIStateComponent
//=====================================================================================================================

FallAIStateComponent::FallAIStateComponent()
    : 
    m_bAlreadyFell(false),
    BaseEnemyAIStateComponent("FallState")
{

}

bool FallAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    DO_AND_CHECK(ParseValueFromXmlElem(&m_FallAnimation, pData->FirstChildElement("FallAnimation")));

    return true;
}

void FallAIStateComponent::VUpdate(uint32 msDiff)
{
    // This is not sticking to original design, but there is no easy way around it
    // I need to constantly check if this unit is in air to be able to force
    // state transition as soon as it is in air

    if (m_bAlreadyFell)
    {
        return;
    }

    bool bInAir = fabs(m_pPhysicsComponent->GetVelocity().y) > DBL_EPSILON;
    if (!m_IsActive && bInAir)
    {
        m_pEnemyAIComponent->EnterBestState(true);
    }

    if (!m_IsActive)
    {
        return;
    }

    if (!bInAir)
    {
        m_bAlreadyFell = true;
        m_pEnemyAIComponent->EnterBestState(true);
    }
}

bool FallAIStateComponent::VCanEnter()
{
    bool bInAir = fabs(m_pPhysicsComponent->GetVelocity().y) > DBL_EPSILON;

    return bInAir;
}

void FallAIStateComponent::VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState)
{
    m_pAnimationComponent->SetAnimation(m_FallAnimation);

    m_IsActive = true;
}

void FallAIStateComponent::VOnStateLeave(BaseEnemyAIStateComponent* pNextState)
{
    m_IsActive = false;
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
    m_bIsFlying(false),
    m_PatrolSpeed(0.0),
    m_bInitialized(false),
    m_bRetainDirection(false),
    m_IsAlwaysIdle(false),
    m_IdleSpeechSoundMaxDistance(0),
    m_IdleSpeechSoundPlayChance(0),
    m_bRecalculatePatrolBorders(true),
    m_bChaseEnemy(false)
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

    ParseValueFromXmlElem(&m_bIsFlying, pData->FirstChildElement("IsFlying"));
    ParseValueFromXmlElem(&m_IsAlwaysIdle, pData->FirstChildElement("IsAlwaysIdle"));
    ParseValueFromXmlElem(&m_bRecalculatePatrolBorders, pData->FirstChildElement("RecalculatePatrolBorders"));

    if (ParseValueFromXmlElem(&m_bChaseEnemy, pData->FirstChildElement("ChaseEnemyBehaviour")))
    {
        m_ChaseEnemySensorFixtureDef = ActorTemplates::XmlToActorFixtureDef(pData->FirstChildElement("ChaseEnemySensorFixture"));
    }

    if (TiXmlElement* pElem = pData->FirstChildElement("IdleSpeech"))
    {
        for (TiXmlElement* pSoundElem = pElem->FirstChildElement("IdleSpeechSound");
            pSoundElem != NULL;
            pSoundElem = pSoundElem->NextSiblingElement("IdleSpeechSound"))
        {
            m_IdleSoundList.push_back(pSoundElem->GetText());
        }

        DO_AND_CHECK(ParseValueFromXmlElem(&m_IdleSpeechSoundMaxDistance, pElem->FirstChildElement("IdleSpeechSoundMaxDistance")));
        DO_AND_CHECK(ParseValueFromXmlElem(&m_IdleSpeechSoundPlayChance, pElem->FirstChildElement("IdleSpeechSoundPlayChance")));
    }

    /*m_LeftPatrolBorder = 6330;
    m_RightPatrolBorder = 6550;*/
    
    if (!m_IsAlwaysIdle)
    {
        assert(fabs(m_PatrolSpeed) > DBL_EPSILON);
    }

    assert(m_pWalkAction != nullptr);
    assert(!m_pWalkAction->animations.empty());
    if (m_pIdleAction == nullptr)
    {
        //LOG_WARNING("No idle action");
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
    m_pPhysics->VSetLinearSpeed(m_pOwner->GetGUID(), noSpeed);
}

void PatrolEnemyAIStateComponent::VPostPostInit()
{
    if (m_bRecalculatePatrolBorders && !m_IsAlwaysIdle)
    {
        CalculatePatrolBorders();
    }

    if (m_bChaseEnemy)
    {
        m_pPhysics->VAddActorFixtureToBody(m_pOwner->GetGUID(), &m_ChaseEnemySensorFixtureDef);
        m_pOwner->GetRawComponent<TriggerComponent>(true)->AddObserver(this);
    }

    m_bInitialized = true;
}

void PatrolEnemyAIStateComponent::VUpdate(uint32 msDiff)
{
    if (!m_IsActive)
    {
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

    if (m_bChaseEnemy && !m_ChasedActorsList.empty())
    {
        TryChaseEnemy();
    }

    // Only makes sense to check for stuff when walking
    if (m_pWalkAction->isActive)
    {
        if (fabs(m_pPhysics->VGetVelocity(m_pOwner->GetGUID()).x) < 0.1)
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

void PatrolEnemyAIStateComponent::VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState)
{
    m_IsActive = true;
    //CommenceIdleBehaviour();
    TryChaseEnemy();
    ChangeDirection(m_Direction);
}

void PatrolEnemyAIStateComponent::VOnStateLeave(BaseEnemyAIStateComponent* pNextState)
{
    m_IsActive = false;

    // Since this state set speed to this actor, we need to stop it moving
    if (pNextState && pNextState->VGetStateType() != EnemyAIState_Falling)
    {
        m_pPhysics->VSetLinearSpeed(m_pOwner->GetGUID(), Point(0.0, 0.0));
    }
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

double PatrolEnemyAIStateComponent::FindClosestHole(const Point &center, int height, float maxSearchDistance)
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
    SDL_Rect aabb = m_pPhysics->VGetAABB(m_pOwner->GetGUID(), true);

    Point center = m_pPositionComponent->GetPosition();

    Point toLeftRay = Point(center.x - 10000, center.y);
    Point toRightRay = Point(center.x + 10000, center.y);

    RaycastResult raycastResultLeft = m_pPhysics->VRayCast(center, toLeftRay, CollisionFlag_Solid | CollisionFlag_Ground);
    RaycastResult raycastResultRight = m_pPhysics->VRayCast(center, toRightRay, CollisionFlag_Solid | CollisionFlag_Ground);

    if (!raycastResultLeft.foundIntersection)
    {
        LOG_WARNING("Did not find raycastResultLeft intersection for actor: " + m_pOwner->GetName() +
            " with position: " + m_pOwner->GetPositionComponent()->GetPosition().ToString());
        // Dummy large value, should be sufficient
        raycastResultLeft.deltaX = center.x - 10000;
    }
    if (!raycastResultRight.foundIntersection)
    {
        LOG_WARNING("Did not find raycastResultRight intersection for actor: " + m_pOwner->GetName() +
            " with position: " + m_pOwner->GetPositionComponent()->GetPosition().ToString());
        // Dummy large value, should be sufficient
        raycastResultRight.deltaX = center.x + 10000;
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
    /*Point spawnPosition((m_LeftPatrolBorder + m_RightPatrolBorder) / 2, m_pPositionComponent->GetY());
    m_pPhysics->VSetPosition(m_pOwner->GetGUID(), spawnPosition);
    m_pPositionComponent->SetPosition(spawnPosition);*/

    /*shared_ptr<EventData_Move_Actor> pEvent(new EventData_Move_Actor(m_pOwner->GetGUID(), spawnPosition));
    IEventMgr::Get()->VQueueEvent(pEvent);*/

    if (m_LeftPatrolBorder <= 0)
    {
        LOG_ERROR("Invalid left patrol border for actor: " + m_pOwner->GetName() + ". Setting to AlwaysIdle.");
        m_IsAlwaysIdle = true;
        m_bRetainDirection = true;
        //assert(m_LeftPatrolBorder > 0);
    }
    if (m_RightPatrolBorder <= 0)
    {
        LOG_ERROR("Invalid right patrol border for actor: " + m_pOwner->GetName() + ". Setting to AlwaysIdle.");
        m_IsAlwaysIdle = true;
        m_bRetainDirection = true;
        //assert(m_RightPatrolBorder > 0);
    }
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
        m_pPhysics->VSetLinearSpeed(m_pOwner->GetGUID(), Point(-1.0 * m_PatrolSpeed, 0.0));
    }
    else
    {
        m_pPhysics->VSetLinearSpeed(m_pOwner->GetGUID(), Point(m_PatrolSpeed, 0.0));
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
        m_pPhysics->VSetLinearSpeed(m_pOwner->GetGUID(), Point(0.0, 0.0));
        
        // TODO: Try to play idle sound

        // Hack
        StrongActorPtr pClaw = g_pApp->GetGameLogic()->GetClawActor();
        assert(pClaw);

        if ((m_pOwner->GetPositionComponent()->GetPosition() - pClaw->GetPositionComponent()->GetPosition()).Length() < m_IdleSpeechSoundMaxDistance)
        {
            m_pEnemyAIComponent->TryPlaySpeechSound(m_IdleSpeechSoundPlayChance, m_IdleSoundList);
        }
    }
    else
    {
        ChangeDirection(ToOppositeDirection(m_Direction));
    }
}

bool PatrolEnemyAIStateComponent::TryChaseEnemy()
{
    if (!m_IsActive || !m_bChaseEnemy || m_ChasedActorsList.empty())
    {
        return false;
    }

    // Just pick the first one, it can only be Claw
    Actor* pChasedActor = m_ChasedActorsList[0];
    

    if (IsActorWithinLOS(pChasedActor))
    {
        double diffX = m_pPositionComponent->GetX() - pChasedActor->GetPositionComponent()->GetX();
        Direction chaseDirection = Direction_Right;
        if (diffX > DBL_EPSILON)
        {
            chaseDirection = Direction_Left;
        }

        ChangeDirection(chaseDirection);

        return true;
    }

    return false;
}

void PatrolEnemyAIStateComponent::VOnActorEnteredTrigger(Actor* pActorWhoEntered, FixtureType triggerType)
{
    if (triggerType != FixtureType_Trigger_ChaseEnemyAreaSensor || pActorWhoEntered->GetName() != "Claw")
    {
        return;
    }

    m_ChasedActorsList.push_back(pActorWhoEntered);
    
    if (m_ChasedActorsList.size() == 1 && m_IsActive)
    {
        TryChaseEnemy();
    }
}

void PatrolEnemyAIStateComponent::VOnActorLeftTrigger(Actor* pActorWhoLeft, FixtureType triggerType)
{
    if (triggerType != FixtureType_Trigger_ChaseEnemyAreaSensor || pActorWhoLeft->GetName() != "Claw")
    {
        return; 
    }

    for (auto iter = m_ChasedActorsList.begin(); iter != m_ChasedActorsList.end(); ++iter)
    {
        if ((*iter) == pActorWhoLeft)
        {
            m_ChasedActorsList.erase(iter);
            return;
        }
    }
}

//=====================================================================================================================
// ParryEnemyAIStateComponent
//=====================================================================================================================

ParryEnemyAIStateComponent::ParryEnemyAIStateComponent()
    : 
    BaseEnemyAIStateComponent("ParryState")
{

}

bool ParryEnemyAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    if (TiXmlElement* pParrySoundsElem = pData->FirstChildElement("ParrySounds"))
    {
        for (TiXmlElement* pElem = pParrySoundsElem->FirstChildElement("ParrySound");
            pElem != NULL;
            pElem = pElem->NextSiblingElement("ParrySound"))
        {
            m_ParrySoundList.push_back(pElem->GetText());
        }
    }

    for (TiXmlElement* pElem = pData->FirstChildElement("IgnoreParryInAnim");
        pElem != NULL;
        pElem = pElem->NextSiblingElement("IgnoreParryInAnim"))
    {
        IgnoreParryInAnim def;
        ParseValueFromXmlElem(&def.animationName, pElem->FirstChildElement("Animation"));
        ParseValueFromXmlElem(&def.minAnimIdx, pElem->FirstChildElement("RequiredMinAnimIdx"));

        m_IgnoreParryInAnimList.push_back(def);
    }

    if (TiXmlElement* pParryChancesElem = pData->FirstChildElement("ParryChances"))
    {
        for (TiXmlElement* pElem = pParryChancesElem->FirstChildElement("ParryChance");
            pElem != NULL;
            pElem = pElem->NextSiblingElement("ParryChance"))
        {
            std::string damageTypeStr;
            std::string parryChanceStr;
            ParseAttributeFromXmlElem(&damageTypeStr, "DamageType", pElem);
            ParseAttributeFromXmlElem(&parryChanceStr, "Chance", pElem);

            DamageType damageType = StringToDamageTypeEnum(damageTypeStr);
            int parryChance = std::stoi(parryChanceStr);

            m_ParryChanceMap.insert(std::make_pair(damageType, parryChance));
        }
    }

    DO_AND_CHECK(ParseValueFromXmlElem(&m_ParryAnimFrameIdx, pData->FirstChildElement("ParryAnimFrameIdx")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_ParryAnimation, pData->FirstChildElement("ParryAnimation")));

    assert(m_ParrySoundList.size() > 0);
    assert(m_ParryChanceMap.size() > 0);

    return true;
}

void ParryEnemyAIStateComponent::VPostInit()
{
    BaseEnemyAIStateComponent::VPostInit();

    m_pAnimationComponent->AddObserver(this);
}

void ParryEnemyAIStateComponent::VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState)
{
    m_pAnimationComponent->ResetAnimation();
    m_pAnimationComponent->SetAnimation(m_ParryAnimation);

    Util::PlayRandomSoundFromList(m_ParrySoundList);

    // When taking damage, nothing can interrupt him
    m_StatePriority = MAX_STATE_PRIORITY;

    m_IsActive = true;
}

void ParryEnemyAIStateComponent::VOnStateLeave(BaseEnemyAIStateComponent* pNextState)
{
    m_IsActive = false;
    m_StatePriority = MIN_STATE_PRIORITY;
}

void ParryEnemyAIStateComponent::VOnAnimationLooped(Animation* pAnimation)
{
    if (!m_IsActive)
    {
        return;
    }

    m_StatePriority = MIN_STATE_PRIORITY;

    m_pEnemyAIComponent->EnterBestState(true);
}

bool ParryEnemyAIStateComponent::CanParry(DamageType damageType, EnemyAIState currentState)
{
    auto findIt = m_ParryChanceMap.find(damageType);
    if (findIt == m_ParryChanceMap.end())
    {
        return false;
    }

    for (const IgnoreParryInAnim& ignoreParryDef : m_IgnoreParryInAnimList)
    {
        if ((m_pAnimationComponent->GetCurrentAnimationName() == ignoreParryDef.animationName) &&
            (m_pAnimationComponent->GetCurrentAnimation()->GetCurrentAnimationFrame()->idx >= ignoreParryDef.minAnimIdx))
        {
            return false;
        }
    }

    int parryChance = findIt->second;
    int parryRand = Util::GetRandomNumber(1, 100);

    // If unit is capable of parrying and it is already parrying, he should be able to parry again
    if (currentState == EnemyAIState_Parry)
    {
        parryRand = 0;
    }
    /*else if (currentState == EnemyAIState_DuckMeleeAttacking ||
             currentState == EnemyAIState_MeleeAttacking ||
             currentState == EnemyAIState_DuckRangedAttacking ||
             currentState == EnemyAIState_DuckRangedAttacking)
    {
        // Lower chance to parry if enemy is already attacking 
        if (m_pAnimationComponent->GetCurrentAnimation()->GetCurrentAnimationFrame()->idx > 1)
        {
            parryChance /= 2;
        }
    }*/

    return (parryChance >= parryRand);
}

//=====================================================================================================================
// BaseAttackAIStateComponent
//=====================================================================================================================

BaseAttackAIStateComponent::BaseAttackAIStateComponent(const std::string &stateName)
    :
    m_AttackDelay(0),
    m_AttackSpeechSoundPlayChance(0),
    m_CurrentAttackActionIdx(0),
    BaseEnemyAIStateComponent(stateName)
{
    IEventMgr::Get()->VAddListener(MakeDelegate(this, &BaseAttackAIStateComponent::ClawHealthBelowZeroDelegate), EventData_Claw_Health_Below_Zero::sk_EventType);
}

BaseAttackAIStateComponent::~BaseAttackAIStateComponent()
{
    IEventMgr::Get()->VRemoveListener(MakeDelegate(this, &BaseAttackAIStateComponent::ClawHealthBelowZeroDelegate), EventData_Claw_Health_Below_Zero::sk_EventType);
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

            if (pAttackAction->projectileProto == ActorPrototype_None)
            {
                assert(pAttackAction->animation != "");
                assert(pAttackAction->damage > 0);
            }

            m_AttackActions.push_back(pAttackAction);
        }
    }

    ParseValueFromXmlElem(&m_AttackDelay, pData->FirstChildElement("AttackDelay"));

    if (TiXmlElement* pElem = pData->FirstChildElement("AttackSpeech"))
    {
        for (TiXmlElement* pSoundElem = pElem->FirstChildElement("AttackSpeechSound");
            pSoundElem != NULL;
            pSoundElem = pSoundElem->NextSiblingElement("AttackSpeechSound"))
        {
            m_AttackSpeechSoundList.push_back(pSoundElem->GetText());
        }

        DO_AND_CHECK(ParseValueFromXmlElem(&m_AttackSpeechSoundPlayChance, pElem->FirstChildElement("AttackSpeechSoundPlayChance")));
    }

    assert(!m_AttackActions.empty());
    //assert(m_AttackActions.size() == 1 && "Only supporting one attack action per state component");

    return true;
}

void BaseAttackAIStateComponent::VPostInit()
{
    BaseEnemyAIStateComponent::VPostInit();

    m_pAnimationComponent->AddObserver(this);
}

void BaseAttackAIStateComponent::VPostPostInit()
{
    for (const shared_ptr<EnemyAttackAction> &pAttackAction : m_AttackActions)
    {
        g_pApp->GetGameLogic()->VGetGamePhysics()->VAddActorFixtureToBody(
            m_pOwner->GetGUID(),
            &pAttackAction->agroSensorFixture);

        // Hack
        break;
    }
}

void BaseAttackAIStateComponent::VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState)
{
    m_IsActive = true;

    m_CurrentAttackActionIdx = 0;
    VExecuteAttack();
    m_pAnimationComponent->SetDelay(m_AttackDelay);
}

void BaseAttackAIStateComponent::VOnStateLeave(BaseEnemyAIStateComponent* pNextState)
{
    m_IsActive = false;
    m_CurrentAttackActionIdx = 0;
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

    m_pAnimationComponent->SetAnimation(m_AttackActions[m_CurrentAttackActionIdx]->animation);
}

void BaseAttackAIStateComponent::VOnAnimationLooped(Animation* pAnimation)
{
    if (!m_IsActive)
    {
        return;
    }

    // This was the last attack action
    if (m_CurrentAttackActionIdx > (m_AttackActions.size() - 1))
    {
        m_CurrentAttackActionIdx = 0;
        m_pEnemyAIComponent->EnterBestState(true);
        if (m_IsActive)
        {
            VExecuteAttack();
            m_pAnimationComponent->SetDelay(m_AttackDelay);
        }
    }
    else
    {
        VExecuteAttack();
    }
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

    if (m_CurrentAttackActionIdx > (m_AttackActions.size() - 1))
    {
        return;
    }

    if (m_AttackActions[m_CurrentAttackActionIdx]->attackAnimFrameIdx == pNewFrame->idx)
    {
        std::shared_ptr<EnemyAttackAction> pAttack = m_AttackActions[m_CurrentAttackActionIdx];

        Direction dir = Direction_Left;
        Point offset = pAttack->attackSpawnPositionOffset;
        if (m_pRenderComponent->IsMirrored())
        {
            dir = Direction_Right;
            offset = Point(offset.x * -1.0, offset.y);
        }

        VOnAttackFrame(pAttack, dir, offset);

        m_CurrentAttackActionIdx++;
    }
}

void BaseAttackAIStateComponent::OnEnemyEnterAgroRange(Actor* pEnemy)
{
    m_EnemyAgroList.push_back(pEnemy);

    bool wasInAttackState = dynamic_cast<BaseAttackAIStateComponent*>(m_pEnemyAIComponent->GetCurrentState()) != NULL;
    m_pEnemyAIComponent->EnterBestState(false);
    bool isInAttackState = dynamic_cast<BaseAttackAIStateComponent*>(m_pEnemyAIComponent->GetCurrentState()) != NULL;

    if (!wasInAttackState && isInAttackState)
    {
        m_pEnemyAIComponent->TryPlaySpeechSound(m_AttackSpeechSoundPlayChance, m_AttackSpeechSoundList);
    }
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

        shared_ptr<PositionComponent> pHostileActorPositionComponent = pHostileActor->GetPositionComponent();
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

    shared_ptr<PositionComponent> pHostileActorPositionComponent = pClosestEnemy->GetPositionComponent();
    assert(pHostileActorPositionComponent);

    return pHostileActorPositionComponent->GetPosition() - m_pPositionComponent->GetPosition();
}

bool BaseAttackAIStateComponent::VCanEnter()
{
    if (m_EnemyAgroList.empty())
    {
        return false;
    }

    // TODO: Only Claw is the enemy at the moment, if the need to have more enemies arises, this needs to be changed
    // Since the first found actor is picked

    // TODO: Revisit this if enemy ever has more than one attack action per AttackAIStateComponent
    EnemyAttackAction& action = *m_AttackActions[m_CurrentAttackActionIdx];

    // Check if enemy is within line of sight
    Point fromPoint = m_pOwner->GetPositionComponent()->GetPosition() + action.agroSensorFixture.offset;
    Point toPoint(m_EnemyAgroList[0]->GetPositionComponent()->GetPosition().x,
            m_pOwner->GetPositionComponent()->GetPosition().y + action.agroSensorFixture.offset.y);

    RaycastResult raycastResult = g_pApp->GetGameLogic()->VGetGamePhysics()->VRayCast(
        fromPoint, 
        toPoint, 
        CollisionFlag_Solid);
    if (raycastResult.foundIntersection)
    {
        // Vision is obstructed
        return false;
    }

    return true;
}

void BaseAttackAIStateComponent::ClawHealthBelowZeroDelegate(IEventDataPtr pEventData)
{
    m_EnemyAgroList.clear();
    m_pEnemyAIComponent->EnterBestState(true);
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
        dir,
        m_pOwner->GetGUID());

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
        dir,
        m_pOwner->GetGUID());

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
    if (pAttack->projectileProto == ActorPrototype_None)
    {
        ActorTemplates::CreateProjectile(
            pAttack->attackFxImageSet,
            pAttack->damage,
            pAttack->attackDamageType,
            dir,
            m_pPositionComponent->GetPosition() + offset,
            CollisionFlag_EnemyAIProjectile,
            (CollisionFlag_Controller | CollisionFlag_Solid | CollisionFlag_InvisibleController),
            m_pOwner->GetGUID());
    }
    else
    {
        ActorTemplates::CreateActor_Projectile(
            pAttack->projectileProto,
            m_pPositionComponent->GetPosition() + offset,
            dir,
            m_pOwner->GetGUID());
    }

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
    if (pAttack->projectileProto == ActorPrototype_None)
    {
        ActorTemplates::CreateProjectile(
            pAttack->attackFxImageSet,
            pAttack->damage,
            pAttack->attackDamageType,
            dir,
            m_pPositionComponent->GetPosition() + offset,
            CollisionFlag_EnemyAIProjectile,
            (CollisionFlag_Controller | CollisionFlag_Solid | CollisionFlag_InvisibleController),
            m_pOwner->GetGUID());
    }
    else
    {
        ActorTemplates::CreateActor_Projectile(
            pAttack->projectileProto,
            m_pPositionComponent->GetPosition() + offset,
            dir,
            m_pOwner->GetGUID());
    }

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
    if (pClawComponent == nullptr)
    {
        LOG("Closest enemy name: " + pClosestEnemy->GetName());
    }
    assert(pClawComponent != nullptr);

    if (pClawComponent->IsDucking())
    {
        return true;
    }

    return false;
}

//=====================================================================================================================
// DiveAttackAIStateComponent
//=====================================================================================================================

// --- This is a bit hacked together - serves only for Seagull enemies

DiveAttackAIStateComponent::DiveAttackAIStateComponent()
:
    BaseEnemyAIStateComponent("DiveAttackAIStateComponent"),
    m_DiveState(DiveState_None)
{

}

DiveAttackAIStateComponent::~DiveAttackAIStateComponent()
{

}

bool DiveAttackAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    assert(pData);

    DO_AND_CHECK(ParseValueFromXmlElem(&m_DiveSound, pData->FirstChildElement("DiveSound")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_DiveInAnimation, pData->FirstChildElement("DiveInAnimation")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_DiveOutAnimation, pData->FirstChildElement("DiveOutAnimation")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_DiveSpeed, pData->FirstChildElement("DiveSpeed")));

    for (TiXmlElement* pElem = pData->FirstChildElement("DiveAreaSensorFixture");
        pElem != NULL;
        pElem = pElem->NextSiblingElement("DiveAreaSensorFixture"))
    {
        m_DiveAreaSensorFixtureList.push_back(ActorTemplates::XmlToActorFixtureDef(pElem));
    }

    assert(m_DiveAreaSensorFixtureList.size() > 0);

    return true;
}

void DiveAttackAIStateComponent::VPostPostInit()
{
    for (const ActorFixtureDef& diveFixture : m_DiveAreaSensorFixtureList)
    {
        g_pApp->GetGameLogic()->VGetGamePhysics()->VAddActorFixtureToBody(
            m_pOwner->GetGUID(),
            &diveFixture);
    }

    m_InitialPosition = m_pOwner->GetPositionComponent()->GetPosition();
}

void DiveAttackAIStateComponent::VUpdate(uint32 msDiff)
{
    if (!m_IsActive)
    {
        return;
    }

    assert(m_DiveState != DiveState_None);

    Point currentPosition = m_pOwner->GetPositionComponent()->GetPosition();
    if (m_DiveState == DiveState_DivingIn)
    {
        if ((currentPosition.y > m_Destination.y) || fabs(m_pPhysicsComponent->GetVelocity().y) < DBL_EPSILON)
        {
            g_pApp->GetGameLogic()->VGetGamePhysics()->VSetLinearSpeed(m_pOwner->GetGUID(), Point(0, -1.0f * m_DiveSpeed));
            m_pAnimationComponent->SetAnimation(m_DiveOutAnimation);
            m_DiveState = DiveState_DivingOut;
        }
    }
    else
    {
        // On its way up - if already surpassed initial position, leave state
        if (currentPosition.y < m_InitialPosition.y)
        {
            g_pApp->GetGameLogic()->VGetGamePhysics()->VSetLinearSpeed(m_pOwner->GetGUID(), Point(0, 0));
            m_EnemyAgroList.clear();
            m_pEnemyAIComponent->EnterBestState(true);
        }
    }
}

void DiveAttackAIStateComponent::VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState)
{
    assert(!m_EnemyAgroList.empty());

    m_DiveState = DiveState_DivingIn;
    m_Destination = m_EnemyAgroList[0]->GetPositionComponent()->GetPosition();

    Point currentPosition = m_pOwner->GetPositionComponent()->GetPosition();
    Point positionDiff = currentPosition - m_Destination;
    positionDiff.Set(abs(positionDiff.x), abs(positionDiff.y));

    // Y speed is preset - this is the diving speed.
    // X speed is calculated so that the dive ends up in defined location
    float xyDistanceRatio = positionDiff.x / positionDiff.y;
    float xSpeed = xyDistanceRatio * m_DiveSpeed;

    if ((currentPosition.x - m_Destination.x) < 0)
    {
        m_pRenderComponent->SetMirrored(true);
    }
    else
    {
        m_pRenderComponent->SetMirrored(false);
        xSpeed *= -1.0f;
    }

    g_pApp->GetGameLogic()->VGetGamePhysics()->VSetLinearSpeed(m_pOwner->GetGUID(), Point(xSpeed, m_DiveSpeed));

    m_pAnimationComponent->SetAnimation(m_DiveInAnimation);

    SoundInfo soundInfo(m_DiveSound);
    IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
        new EventData_Request_Play_Sound(soundInfo)));

    m_IsActive = true;
}

void DiveAttackAIStateComponent::VOnStateLeave(BaseEnemyAIStateComponent* pNextState)
{
    m_IsActive = false;
    m_DiveState = DiveState_None;
}

bool DiveAttackAIStateComponent::VCanEnter()
{
    if (m_EnemyAgroList.size() > 0)
    {
        Point currentPosition = m_pOwner->GetPositionComponent()->GetPosition();
        Point destPosition = m_EnemyAgroList[0]->GetPositionComponent()->GetPosition();

        // Can attack only in the direction where the enemy is looking
        if (m_pRenderComponent->IsMirrored())
        {
            return (currentPosition.x - destPosition.x) < 0;
        }
        else
        {
            return (currentPosition.x - destPosition.x) > 0;
        }
    }

    return false;
}

void DiveAttackAIStateComponent::OnEnemyEnterAgroRange(Actor* pActor)
{
    m_EnemyAgroList.push_back(pActor);
    m_pEnemyAIComponent->EnterBestState(false);
}

void DiveAttackAIStateComponent::OnEnemyLeftAgroRange(Actor* pActor)
{
    for (auto iter = m_EnemyAgroList.begin(); iter != m_EnemyAgroList.end(); ++iter)
    {
        if ((*iter) == pActor)
        {
            m_EnemyAgroList.erase(iter);
            return;
        }
    }

    //LOG_WARNING("Could not remove enemy - no such actor found");
}

//=====================================================================================================================
// BaseBossAIStateComponennt
//=====================================================================================================================

BaseBossAIStateComponennt::BaseBossAIStateComponennt(std::string stateName)
    :
    BaseEnemyAIStateComponent("BaseBossAIStateComponent"),
    m_bBossFightStarted(false)
{
    IEventMgr::Get()->VAddListener(MakeDelegate(this, &BaseBossAIStateComponennt::ActorEnteredBossAreaDelegate), EventData_Entered_Boss_Area::sk_EventType);
    IEventMgr::Get()->VAddListener(MakeDelegate(this, &BaseBossAIStateComponennt::BossFightStartedDelegate), EventData_Boss_Fight_Started::sk_EventType);
    IEventMgr::Get()->VAddListener(MakeDelegate(this, &BaseBossAIStateComponennt::BossFightEndedDelegate), EventData_Boss_Fight_Ended::sk_EventType);
}

BaseBossAIStateComponennt::~BaseBossAIStateComponennt()
{
    IEventMgr::Get()->VRemoveListener(MakeDelegate(this, &BaseBossAIStateComponennt::ActorEnteredBossAreaDelegate), EventData_Entered_Boss_Area::sk_EventType);
    IEventMgr::Get()->VRemoveListener(MakeDelegate(this, &BaseBossAIStateComponennt::BossFightStartedDelegate), EventData_Boss_Fight_Started::sk_EventType);
    IEventMgr::Get()->VRemoveListener(MakeDelegate(this, &BaseBossAIStateComponennt::BossFightEndedDelegate), EventData_Boss_Fight_Ended::sk_EventType);
}

bool BaseBossAIStateComponennt::VDelegateInit(TiXmlElement* pData)
{
    assert(pData);

    DO_AND_CHECK(ParseValueFromXmlElem(&m_BossDialogAnimation, pData->FirstChildElement("BossDialogAnimation")));

    return true;
}

void BaseBossAIStateComponennt::VPostInit()
{
    BaseEnemyAIStateComponent::VPostInit();

    auto pHC = MakeStrongPtr(m_pOwner->GetComponent<HealthComponent>());
    assert(pHC != nullptr);

    m_pHealthComponent = pHC.get();
    m_pHealthComponent->AddObserver(this);

    m_DefaultPosition = m_pPositionComponent->GetPosition();

    m_pAnimationComponent->SetAnimation(m_BossDialogAnimation);
}

void BaseBossAIStateComponennt::VOnHealthChanged(int32 oldHealth, int32 newHealth, DamageType damageType, Point impactPoint, int sourceActorId)
{
    int maxHealth = m_pHealthComponent->GetMaxHealth();
    float newHealthPercentage = ((float)newHealth / (float)maxHealth) * 100;

    if (m_bBossFightStarted)
    {
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Boss_Health_Changed((int)newHealthPercentage, newHealth)));
    }
}

void BaseBossAIStateComponennt::VOnHealthBelowZero(DamageType damageType, int sourceActorId)
{
    IEventMgr::Get()->VQueueEvent(IEventDataPtr(new EventData_Boss_Fight_Ended(true)));
}

void BaseBossAIStateComponennt::ActorEnteredBossAreaDelegate(IEventDataPtr pEvent)
{
    assert(this && m_pAnimationComponent != NULL);
    m_pAnimationComponent->SetAnimation(m_BossDialogAnimation);

    IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Boss_Health_Changed(100, m_pHealthComponent->GetHealth())));

    VOnActorEnteredBossArea();
}

void BaseBossAIStateComponennt::BossFightStartedDelegate(IEventDataPtr pEvent)
{
    m_bBossFightStarted = true;

    VOnBossFightStarted();
}

void BaseBossAIStateComponennt::BossFightEndedDelegate(IEventDataPtr pEvent)
{
    m_bBossFightStarted = false;

    // Reset health if boss did not die (Claw died) and reset his position
    if (m_pHealthComponent->GetCurrentHealth() > 0)
    {
        m_pEnemyAIComponent->EnterBestState(true);
        m_pHealthComponent->SetMaxHealth();
        IEventMgr::Get()->VQueueEvent(IEventDataPtr(new EventData_Teleport_Actor(m_pOwner->GetGUID(), m_DefaultPosition)));

        m_pAnimationComponent->SetAnimation(m_BossDialogAnimation);

        VOnBossFightEnded(false);
    }
    else
    {
        VOnBossFightEnded(true);
    }
}

//=====================================================================================================================
// LaRauxBossAIStateComponent
//=====================================================================================================================

LaRauxBossAIStateComponent::LaRauxBossAIStateComponent()
    :
    BaseBossAIStateComponennt("LaRauxBossAIStateComponent")
{

}

LaRauxBossAIStateComponent::~LaRauxBossAIStateComponent()
{

}

bool LaRauxBossAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    assert(pData);

    if (!BaseBossAIStateComponennt::VDelegateInit(pData))
    {
        return false;
    }

    return true;
}

void LaRauxBossAIStateComponent::VUpdate(uint32 msDiff)
{

}

void LaRauxBossAIStateComponent::VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState)
{

}

void LaRauxBossAIStateComponent::VOnStateLeave(BaseEnemyAIStateComponent* pNextState)
{

}

void LaRauxBossAIStateComponent::VOnBossFightStarted()
{
    m_pEnemyAIComponent->EnterBestState(true);
}

void LaRauxBossAIStateComponent::VOnBossFightEnded(bool isBossDead)
{
    if (isBossDead)
    {
        SoundInfo soundInfo(SOUND_GAME_AMULET_RISE);
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));

        soundInfo.soundToPlay = SOUND_LEVEL2_BOSS_DEAD;
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));
    }
}

//=====================================================================================================================
// KatherineBossAIStateComponent
//=====================================================================================================================

KatherineBossAIStateComponent::KatherineBossAIStateComponent()
    :
    BaseBossAIStateComponennt("KatherineBossAIStateComponent")
{

}

KatherineBossAIStateComponent::~KatherineBossAIStateComponent()
{

}

bool KatherineBossAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    assert(pData);

    if (!BaseBossAIStateComponennt::VDelegateInit(pData))
    {
        return false;
    }

    return true;
}

void KatherineBossAIStateComponent::VUpdate(uint32 msDiff)
{

}

void KatherineBossAIStateComponent::VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState)
{

}

void KatherineBossAIStateComponent::VOnStateLeave(BaseEnemyAIStateComponent* pNextState)
{

}

void KatherineBossAIStateComponent::VOnBossFightStarted()
{
    m_pEnemyAIComponent->EnterBestState(true);
}

void KatherineBossAIStateComponent::VOnBossFightEnded(bool isBossDead)
{
    if (isBossDead)
    {
        SoundInfo soundInfo(SOUND_GAME_AMULET_RISE);
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));
    }
}

//=====================================================================================================================
// WolvingtonBossAIStateComponent
//=====================================================================================================================

WolvingtonBossAIStateComponent::WolvingtonBossAIStateComponent()
:
BaseBossAIStateComponennt("WolvingtonBossAIStateComponent")
{

}

WolvingtonBossAIStateComponent::~WolvingtonBossAIStateComponent()
{

}

bool WolvingtonBossAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    assert(pData);

    if (!BaseBossAIStateComponennt::VDelegateInit(pData))
    {
        return false;
    }

    return true;
}

void WolvingtonBossAIStateComponent::VOnBossFightStarted()
{
    // To refresh contact list after Claw's death
    g_pApp->GetGameLogic()->VGetGamePhysics()->VActivate(m_pOwner->GetGUID());

    m_pEnemyAIComponent->EnterBestState(true);
}

void WolvingtonBossAIStateComponent::VOnBossFightEnded(bool isBossDead)
{
    if (isBossDead)
    {
        SoundInfo soundInfo(SOUND_GAME_AMULET_RISE);
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(
            new EventData_Request_Play_Sound(soundInfo)));
    }
    else
    {
        // To refresh contact list after Claw's death
        g_pApp->GetGameLogic()->VGetGamePhysics()->VDeactivate(m_pOwner->GetGUID());
    }
}

//=====================================================================================================================
// RollEnemyAIStateComponent
//=====================================================================================================================

RollEnemyAIStateComponent::RollEnemyAIStateComponent()
:
m_RollSpeed(0.0),
m_ReuseTimeLeft(0),
m_pPatrolStateComponent(nullptr),
BaseEnemyAIStateComponent("RollState")
{

}

bool RollEnemyAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    m_pPhysics = g_pApp->GetGameLogic()->VGetGamePhysics();
    assert(m_pPhysics);

    DO_AND_CHECK(ParseValueFromXmlElem(&m_RollSpeed, pData->FirstChildElement("RollSpeed")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_ForwardRollAnimation, pData->FirstChildElement("ForwardRollAnimation")));
    DO_AND_CHECK(ParseValueFromXmlElem(&m_BackwardRollAnimation, pData->FirstChildElement("BackwardRollAnimation")));
    m_RollSensorFixtureDef = ActorTemplates::XmlToActorFixtureDef(pData->FirstChildElement("RollSensorFixtureDef"));

    return true;
}

void RollEnemyAIStateComponent::VPostInit()
{
    BaseEnemyAIStateComponent::VPostInit();

    m_pOwner->GetRawComponent<AnimationComponent>(true)->AddObserver(this);
    m_pOwner->GetRawComponent<TriggerComponent>(true)->AddObserver(this);
    m_pOwner->GetRawComponent<HealthComponent>(true)->AddObserver(this);

    m_pPatrolStateComponent = m_pOwner->GetRawComponent<PatrolEnemyAIStateComponent>(true);
}

void RollEnemyAIStateComponent::VPostPostInit()
{
    BaseEnemyAIStateComponent::VPostPostInit();

    m_pPhysics->VAddActorFixtureToBody(m_pOwner->GetGUID(), &m_RollSensorFixtureDef);
}

void RollEnemyAIStateComponent::VUpdate(uint32 msDiff)
{
    m_ReuseTimeLeft -= msDiff;

    if (m_IsActive)
    {
        if ((m_pPositionComponent->GetX() > m_pPatrolStateComponent->GetRightPatrolBorder() &&
             m_pPhysics->VGetVelocity(m_pOwner->GetGUID()).x > DBL_EPSILON) ||
            (m_pPositionComponent->GetX() < m_pPatrolStateComponent->GetLeftPatrolBorder() &&
             m_pPhysics->VGetVelocity(m_pOwner->GetGUID()).x < -1.0 * DBL_EPSILON))
        {
            Point zeroSpeed(0, 0);
            m_pPhysics->VSetLinearSpeed(m_pOwner->GetGUID(), zeroSpeed);
        }
    }
}

bool RollEnemyAIStateComponent::VCanEnter()
{
    bool bCanEnter = (!m_ActorsInLosList.empty() && m_ReuseTimeLeft <= 0);
    return bCanEnter;
}

void RollEnemyAIStateComponent::VOnStateEnter(BaseEnemyAIStateComponent* pPreviousState)
{
    m_IsActive = true;

    assert(!m_ActorsInLosList.empty());

   
    Direction rollDirection = DetermineRollDirection();

    Point speed(m_RollSpeed, 0);
    if (rollDirection == Direction_Left)
    {
        speed.x *= -1.0;
        m_pAnimationComponent->SetAnimation(m_BackwardRollAnimation);
    }
    else
    {
        m_pAnimationComponent->SetAnimation(m_ForwardRollAnimation);
    }

    m_pPhysics->VSetLinearSpeed(m_pOwner->GetGUID(), speed);
}

void RollEnemyAIStateComponent::VOnStateLeave(BaseEnemyAIStateComponent* pNextState)
{
    m_IsActive = false;

    Point zeroSpeed(0, 0);
    m_pPhysics->VSetLinearSpeed(m_pOwner->GetGUID(), zeroSpeed);
}

void RollEnemyAIStateComponent::VOnActorEnteredTrigger(Actor* pActorWhoEntered, FixtureType triggerType)
{
    if (triggerType != FixtureType_Trigger_RollAreaSensor || pActorWhoEntered->GetName() != "Claw")
    {
        return;
    }

    m_ActorsInLosList.push_back(pActorWhoEntered);
    m_pEnemyAIComponent->EnterBestState(false);
}

void RollEnemyAIStateComponent::VOnActorLeftTrigger(Actor* pActorWhoLeft, FixtureType triggerType)
{
    if (triggerType != FixtureType_Trigger_RollAreaSensor || pActorWhoLeft->GetName() != "Claw")
    {
        return;
    }

    for (auto iter = m_ActorsInLosList.begin(); iter != m_ActorsInLosList.end(); ++iter)
    {
        if ((*iter) == pActorWhoLeft)
        {
            m_ActorsInLosList.erase(iter);
            return;
        }
    }
}

bool RollEnemyAIStateComponent::VCanResistDamage(DamageType damageType, Point impactPoint)
{
    return m_IsActive;
}

void RollEnemyAIStateComponent::VOnAnimationLooped(Animation* pAnimation)
{
    if (!m_IsActive)
    {
        return;
    }

    m_ReuseTimeLeft = 650;

    m_pEnemyAIComponent->EnterBestState(true);
}

Direction RollEnemyAIStateComponent::DetermineRollDirection()
{
    static const int BORDER_DISTANCE_THRESHOLD = 75;
    Direction rollDirection = Direction_Right;

    int leftPatrolBorder = m_pPatrolStateComponent->GetLeftPatrolBorder();
    int rightPatrolBorder = m_pPatrolStateComponent->GetRightPatrolBorder();

    // Determine to where to roll - if he is further from left patrol border, roll to that direction
    // and vice versa

    int leftDelta = abs((int)m_pPositionComponent->GetX() - leftPatrolBorder);
    int rightDelta = abs((int)m_pPositionComponent->GetX() - rightPatrolBorder);

    if (leftDelta < BORDER_DISTANCE_THRESHOLD)
    {
        rollDirection = Direction_Right;
    }
    else if (rightDelta < BORDER_DISTANCE_THRESHOLD)
    {
        rollDirection = Direction_Left;
    }
    else
    {
        if (Util::GetRandomNumber(0, 1) == 0)
        {
            rollDirection = Direction_Right;
        }
        else
        {
            rollDirection = Direction_Left;
        }
    }

    return rollDirection;
}
