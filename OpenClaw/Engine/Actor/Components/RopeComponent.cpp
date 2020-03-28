#include "RopeComponent.h"
#include "PositionComponent.h"
#include "ControllableComponent.h"
#include "PhysicsComponent.h"

#include "../../Events/EventMgr.h"
#include "../../Events/Events.h"

const char* RopeComponent::g_Name = "RopeComponent";

static Point GetRopeEndFramePosition(const Point& initialPosition, int frameIdx)
{
    assert(frameIdx >= 0 && frameIdx <= 119);

    static std::map<int, Point> s_RopeFrameIndexToRopeHandleOffsetMap =
    {
        { 0, Point(-175, 173) },
        { 1, Point(-174, 174) },
        { 2, Point(-174, 175) },
        { 3, Point(-172, 178) },
        { 4, Point(-171, 180) },
        { 5, Point(-170, 182) },
        { 6, Point(-168, 186) },
        { 7, Point(-166, 190) },
        { 8, Point(-165, 195) },
        { 9, Point(-164, 198) },

        { 10, Point(-163, 203) },
        { 11, Point(-157, 208) },
        { 12, Point(-151, 214) },
        { 13, Point(-146, 222) },
        { 14, Point(-141, 227) },
        { 15, Point(-134, 234) },
        { 16, Point(-123, 239) },
        { 17, Point(-113, 244) },
        { 18, Point(-102, 249) },
        { 19, Point(-91, 254) },

        { 20, Point(-80, 260) },
        { 21, Point(-68, 262) },
        { 22, Point(-56, 264) },
        { 23, Point(-44, 267) },
        { 24, Point(-32, 269) },
        { 25, Point(-20, 271) },
        { 26, Point(-7, 271) },
        { 27, Point(6, 271) },
        { 28, Point(18, 271) },
        { 29, Point(31, 270) },

        { 30, Point(43, 270) },
        { 31, Point(56, 267) },
        { 32, Point(69, 264) },
        { 33, Point(82, 261) },
        { 34, Point(94, 258) },
        { 35, Point(106, 253) },
        { 36, Point(116, 249) },
        { 37, Point(125, 243) },
        { 38, Point(134, 237) },
        { 39, Point(143, 232) },

        { 40, Point(151, 226) },
        { 41, Point(156, 221) },
        { 42, Point(161, 215) },
        { 43, Point(166, 209) },
        { 44, Point(170, 204) },
        { 45, Point(175, 199) },
        { 46, Point(176, 197) },
        { 47, Point(177, 195) },
        { 48, Point(178, 194) },
        { 49, Point(178, 192) },

        { 50, Point(178, 191) },
        { 51, Point(178, 189) },
        { 52, Point(178, 188) },
        { 53, Point(178, 187) },
        { 54, Point(178, 185) },
        { 55, Point(178, 184) },
        { 56, Point(178, 181) },
        { 57, Point(178, 179) },
        { 58, Point(178, 177) },
        { 59, Point(177, 175) },

        { 60, Point(177, 173) },
        { 61, Point(175, 176) },
        { 62, Point(174, 177) },
        { 63, Point(173, 180) },
        { 64, Point(171, 182) },
        { 65, Point(169, 184) },
        { 66, Point(168, 188) },
        { 67, Point(166, 192) },
        { 68, Point(165, 195) },
        { 69, Point(164, 200) },

        { 70, Point(163, 204) },
        { 71, Point(157, 210) },
        { 72, Point(151, 216) },
        { 73, Point(146, 222) },
        { 74, Point(140, 228) },
        { 75, Point(134, 234) },
        { 76, Point(122, 239) },
        { 77, Point(112, 244) },
        { 78, Point(101, 249) },
        { 79, Point(90, 254) },

        { 80, Point(79, 259) },
        { 81, Point(67, 261) },
        { 82, Point(54, 263) },
        { 83, Point(42, 265) },
        { 84, Point(31, 268) },
        { 85, Point(19, 270) },
        { 86, Point(6, 270) },
        { 87, Point(-7, 270) },
        { 88, Point(-20, 269) },
        { 89, Point(-31, 269) },

        { 90, Point(-44, 269) },
        { 91, Point(-56, 268) },
        { 92, Point(-69, 262) },
        { 93, Point(-81, 259) },
        { 94, Point(-93, 256) },
        { 95, Point(-106, 252) },
        { 96, Point(-115, 247) },
        { 97, Point(-124, 242) },
        { 98, Point(-133, 236) },
        { 99, Point(-142, 230) },

        { 100, Point(-151, 225) },
        { 101, Point(-155, 220) },
        { 102, Point(-160, 214) },
        { 103, Point(-165, 209) },
        { 104, Point(-170, 203) },
        { 105, Point(-175, 198) },
        { 106, Point(-175, 197) },
        { 107, Point(-176, 195) },
        { 108, Point(-177, 194) },
        { 109, Point(-178, 191) },

        { 110, Point(-178, 190) },
        { 111, Point(-178, 188) },
        { 112, Point(-178, 187) },
        { 113, Point(-178, 185) },
        { 114, Point(-178, 184) },
        { 115, Point(-178, 183) },
        { 116, Point(-178, 181) },
        { 117, Point(-178, 178) },
        { 118, Point(-176, 176) },
        { 119, Point(-175, 174) },
    };

    return (initialPosition + s_RopeFrameIndexToRopeHandleOffsetMap[frameIdx] + Point(2, -151));
}

RopeComponent::RopeComponent()
    :
    m_TimeStanceAttach(0),
    m_pAttachedActor(NULL),
    m_pRopeEndTriggerActor(NULL)
{

}

bool RopeComponent::VInit(TiXmlElement* data)
{
    assert(data != NULL);

    return true;
}

void RopeComponent::VPostInit()
{
    shared_ptr<AnimationComponent>pAnimationComponent =
        MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>());
    assert(pAnimationComponent);

    pAnimationComponent->AddObserver(this);
}

void RopeComponent::VPostPostInit()
{
    shared_ptr<AnimationComponent>pAnimationComponent =
        MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>());
    //pAnimationComponent->SetDelay(1000000);

    ActorBodyDef triggerDef;
    triggerDef.bodyType = b2_dynamicBody;
    triggerDef.makeSensor = false;
    triggerDef.fixtureType = FixtureType_Trigger;
    triggerDef.gravityScale = 0.0;
    triggerDef.collisionFlag = CollisionFlag_Rope;
    triggerDef.collisionMask = CollisionFlag_RopeSensor;
    triggerDef.size = Point(50, 50);

    m_pRopeEndTriggerActor = ActorTemplates::CreateActor_Trigger(
        triggerDef, 
        m_pOwner->GetPositionComponent()->GetPosition()).get();
    assert(m_pRopeEndTriggerActor != NULL);

    shared_ptr<TriggerComponent> pTrigger = MakeStrongPtr(m_pRopeEndTriggerActor->GetComponent<TriggerComponent>());
    assert(pTrigger != nullptr);

    pTrigger->AddObserver(this);
}

void RopeComponent::VUpdate(uint32 msDiff)
{
    m_TimeStanceAttach += msDiff;
}

void RopeComponent::VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    Point newPosition = GetRopeEndFramePosition(m_pOwner->GetPositionComponent()->GetPosition(), pNewFrame->idx);
    if (pNewFrame->idx > 60)
    {
        IEventMgr::Get()->VQueueEvent(IEventDataPtr(new EventData_Teleport_Actor(
            m_pRopeEndTriggerActor->GetGUID(),
            newPosition)));
    }
    else
    {
        IEventMgr::Get()->VQueueEvent(IEventDataPtr(new EventData_Teleport_Actor(
            m_pRopeEndTriggerActor->GetGUID(),
            newPosition)));
    }

    if (m_pAttachedActor != NULL)
    {
        shared_ptr<ClawControllableComponent> pClawComponent =
            MakeStrongPtr(m_pAttachedActor->GetComponent<ClawControllableComponent>());
        assert(pClawComponent != nullptr);

        // Check he actor detached himself
        if (!pClawComponent->VIsAttachedToRope())
        {
            DetachActor();
            return;
        }

        UpdateAttachedActorPosition(newPosition);

        shared_ptr<PhysicsComponent> pRopeActorPhysicsComponent = m_pAttachedActor->GetPhysicsComponent();
        assert(pRopeActorPhysicsComponent != nullptr);

        if (pNewFrame->idx > 60)
        {
            pRopeActorPhysicsComponent->SetDirection(Direction_Left);
        }
        else
        {
            pRopeActorPhysicsComponent->SetDirection(Direction_Right);
        }
    }
}

void RopeComponent::VOnActorEnteredTrigger(Actor* pActorWhoEntered, FixtureType triggerType)
{
    if (m_TimeStanceAttach < 250)
    {
        return;
    }
    m_TimeStanceAttach = 0;

    shared_ptr<ClawControllableComponent> pClawComponent =
        MakeStrongPtr(pActorWhoEntered->GetComponent<ClawControllableComponent>());
    assert(pClawComponent != nullptr);

    if (pClawComponent->IsHoldingRope())
    {
        return;
    }
    
    shared_ptr<HealthComponent> pHealthComponent =
        MakeStrongPtr(pActorWhoEntered->GetComponent<HealthComponent>(HealthComponent::g_Name));
     
    if ((int)pHealthComponent->GetCurrentHealth() <= 0)
    {
        return;
    }


    assert(m_pAttachedActor == NULL);
    m_pAttachedActor = pActorWhoEntered;

    pClawComponent->VOnAttachedToRope();

    UpdateAttachedActorPosition(m_pRopeEndTriggerActor->GetPositionComponent()->GetPosition());
}

void RopeComponent::VOnActorLeftTrigger(Actor* pActorWhoLeft, FixtureType triggerType)
{
    // Claw has to detach himself on his own so it is very probably that this will be NULL
    if (m_pAttachedActor == NULL)
    {
        return;
    }

    m_pAttachedActor = NULL;

    shared_ptr<ClawControllableComponent> pClawComponent =
        MakeStrongPtr(pActorWhoLeft->GetComponent<ClawControllableComponent>());
    assert(pClawComponent != nullptr);

    pClawComponent->VDetachFromRope();
}

void RopeComponent::UpdateAttachedActorPosition(const Point& newPosition)
{
    assert(m_pAttachedActor != NULL);

    IEventMgr::Get()->VQueueEvent(IEventDataPtr(new EventData_Teleport_Actor(
        m_pAttachedActor->GetGUID(),
        newPosition)));
}

void RopeComponent::DetachActor()
{
    // Sanity check
    assert(m_pAttachedActor != NULL);

    m_pAttachedActor = NULL;
}
