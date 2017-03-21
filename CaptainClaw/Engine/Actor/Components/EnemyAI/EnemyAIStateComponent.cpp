#include "EnemyAIStateComponent.h"

#include "EnemyAIComponent.h"
#include "../AnimationComponent.h"
#include "../PhysicsComponent.h"
#include "../PositionComponent.h"
#include "../RenderComponent.h"

#include "../../../GameApp/BaseGameLogic.h"
#include "../../../GameApp/BaseGameApp.h"
#include "../../../Physics/ClawPhysics.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"

#include <time.h>

const char* BaseEnemyAIStateComponent::g_Name = "BaseEnemyAIStateComponent";
const char* PatrolEnemyAIStateComponent::g_Name = "PatrolEnemyAIStateComponent";
const char* MeleeAttackAIStateComponent::g_Name = "MeleeAttackAIStateComponent";
const char* RangedAttackAIStateComponent::g_Name = "RangedAttackAIStateComponent";

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

    

    // Disable gravity and stick them to their platforms / grounds
    /*Point center = m_pPositionComponent->GetPosition();

    Point fromPoint = Point(center.x, center.y);
    Point toPoint = Point(center.x, center.y + 1000);

    RaycastResult raycastResultDown = m_pPhysics->VRayCast(fromPoint, toPoint, (CollisionFlag_Solid | CollisionFlag_Ground));
    assert(raycastResultDown.foundIntersection);

    m_pPositionComponent->SetPosition(center.x, center.y + raycastResultDown.deltaY);
    m_pPhysics->VSetPosition(_owner->GetGUID(), m_pPositionComponent->GetPosition());
    m_pPhysics->VSetGravityScale(_owner->GetGUID(), 0.0f);*/
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

    m_LeftPatrolBorder += 10;
    m_RightPatrolBorder -= 10;

    // Not enough room for the unit to move, dont force it, it would fall
    if (m_LeftPatrolBorder >= m_RightPatrolBorder)
    {
        m_IsAlwaysIdle = true;
        m_bRetainDirection = true;
    }

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
// MeleeAttackAIStateComponent
//=====================================================================================================================

MeleeAttackAIStateComponent::MeleeAttackAIStateComponent()
    :
    BaseEnemyAIStateComponent("MeleeAttackState")
{

}

MeleeAttackAIStateComponent::~MeleeAttackAIStateComponent()
{

}

bool MeleeAttackAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    assert(pData);

    if (TiXmlElement* pMeleeAttacksElem = pData->FirstChildElement("Attacks"))
    {
        for (TiXmlElement* pElem = pMeleeAttacksElem->FirstChildElement("AttackAction");
            pElem != NULL; pElem = pElem->NextSiblingElement("AttackAction"))
        {
            std::shared_ptr<EnemyAttackAction> pAttackAction(new EnemyAttackAction);

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

            assert(pAttackAction->animation != "" && pAttackAction->attackDamageType == DamageType_MeleeAttack);
            assert(pAttackAction->damage > 0);

            m_MeleeAttackActions.push_back(pAttackAction);
        }
    }

    assert(!m_MeleeAttackActions.empty());

    return true;
}

void MeleeAttackAIStateComponent::VPostInit()
{
    BaseEnemyAIStateComponent::VPostInit();

    m_pAnimationComponent->AddObserver(this);
}

void MeleeAttackAIStateComponent::VUpdate(uint32 msDiff)
{
    if (!m_IsActive)
    {
        return;
    }
}

void MeleeAttackAIStateComponent::VOnStateEnter()
{
    m_IsActive = true;
    ExecuteMeleeAttack();
}

void MeleeAttackAIStateComponent::VOnStateLeave()
{
    m_IsActive = false;
}

void MeleeAttackAIStateComponent::VOnAnimationLooped(Animation* pAnimation)
{
    if (!m_IsActive)
    {
        return;
    }

    // TODO: Support melee actions consiting of multiple animations ?
    m_pEnemyAIComponent->OnStateCanFinish();
    ExecuteMeleeAttack();
}

void MeleeAttackAIStateComponent::ExecuteMeleeAttack()
{
    if (!m_IsActive)
    {
        return;
    }

    Point closestEnemyOffset = m_pEnemyAIComponent->FindClosestHostileActorOffset();
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

    m_pAnimationComponent->SetAnimation(m_MeleeAttackActions[0]->animation);
}

void MeleeAttackAIStateComponent::VOnAnimationFrameChanged(
    Animation* pAnimation, 
    AnimationFrame* pLastFrame, 
    AnimationFrame* pNewFrame)
{
    if (!m_IsActive)
    {
        return;
    }

    if (m_MeleeAttackActions[0]->attackAnimFrameIdx == pNewFrame->idx)
    {
        std::shared_ptr<EnemyAttackAction> pAttack = m_MeleeAttackActions[0];

        Direction dir = Direction_Left;
        Point offset = pAttack->attackSpawnPositionOffset;
        if (m_pRenderComponent->IsMirrored())
        {
            dir = Direction_Right;
            offset = Point(offset.x * -1.0, offset.y);
        }

        ActorTemplates::CreateAreaDamage(
            m_pPositionComponent->GetPosition() + offset,
            pAttack->attackAreaSize,
            pAttack->damage,
            CollisionFlag_EnemyAIAttack,
            "Rectangle");

        // Play melee attack sound
        Util::PlayRandomSoundFromList(m_pEnemyAIComponent->GetMeleeAttackSounds());
    }
}

//=====================================================================================================================
// RangedAttackAIStateComponent
//=====================================================================================================================

RangedAttackAIStateComponent::RangedAttackAIStateComponent()
    :
    BaseEnemyAIStateComponent("RangedAttackState")
{

}

RangedAttackAIStateComponent::~RangedAttackAIStateComponent()
{

}

bool RangedAttackAIStateComponent::VDelegateInit(TiXmlElement* pData)
{
    assert(pData);

    if (TiXmlElement* pRangedAttacksElem = pData->FirstChildElement("Attacks"))
    {
        for (TiXmlElement* pElem = pRangedAttacksElem->FirstChildElement("AttackAction");
            pElem != NULL; pElem = pElem->NextSiblingElement("AttackAction"))
        {
            std::shared_ptr<EnemyAttackAction> pAttackAction(new EnemyAttackAction);

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
            if (TiXmlElement* pAttackFxImageSet = pElem->FirstChildElement("AttackFxImageSet"))
            {
                pAttackAction->attackFxImageSet = pAttackFxImageSet->GetText();
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

            assert(pAttackAction->animation != "" && 
                (pAttackAction->attackDamageType == DamageType_Bullet ||
                pAttackAction->attackDamageType == DamageType_Explosion));
            assert(pAttackAction->damage > 0);

            m_RangedAttackActions.push_back(pAttackAction);
        }
    }

    assert(!m_RangedAttackActions.empty());

    return true;
}

void RangedAttackAIStateComponent::VPostInit()
{
    BaseEnemyAIStateComponent::VPostInit();

    m_pAnimationComponent->AddObserver(this);
}

void RangedAttackAIStateComponent::VUpdate(uint32 msDiff)
{
    if (!m_IsActive)
    {
        return;
    }
}

void RangedAttackAIStateComponent::VOnStateEnter()
{
    m_IsActive = true;
    ExecuteRangedAttack();
}

void RangedAttackAIStateComponent::VOnStateLeave()
{
    m_IsActive = false;
}

void RangedAttackAIStateComponent::VOnAnimationLooped(Animation* pAnimation)
{
    if (!m_IsActive)
    {
        return;
    }

    // TODO: Support melee actions consiting of multiple animations ?
    m_pEnemyAIComponent->OnStateCanFinish();
    ExecuteRangedAttack();
}

void RangedAttackAIStateComponent::ExecuteRangedAttack()
{
    if (!m_IsActive)
    {
        return;
    }

    Point closestEnemyOffset = m_pEnemyAIComponent->FindClosestHostileActorOffset();
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

    m_pAnimationComponent->SetAnimation(m_RangedAttackActions[0]->animation);
}

void RangedAttackAIStateComponent::VOnAnimationFrameChanged(
    Animation* pAnimation,
    AnimationFrame* pLastFrame,
    AnimationFrame* pNewFrame)
{
    if (!m_IsActive)
    {
        return;
    }

    if (m_RangedAttackActions[0]->attackAnimFrameIdx == pNewFrame->idx)
    {
        std::shared_ptr<EnemyAttackAction> pAttack = m_RangedAttackActions[0];

        Direction dir = Direction_Left;
        Point offset = pAttack->attackSpawnPositionOffset;
        if (m_pRenderComponent->IsMirrored())
        {
            dir = Direction_Right;
            offset = Point(offset.x * -1.0, offset.y);
        }

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
}