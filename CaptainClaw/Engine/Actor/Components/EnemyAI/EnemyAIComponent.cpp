#include "EnemyAIComponent.h"
#include "EnemyAIStateComponent.h"
#include "../RenderComponent.h"
#include "../PositionComponent.h"
#include "../ControllerComponents/HealthComponent.h"
#include "../PhysicsComponent.h"

#include "../../../GameApp/BaseGameApp.h"
#include "../../../UserInterface/HumanView.h"
#include "../../../Scene/SceneNodes.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"

const char* EnemyAIComponent::g_Name = "EnemyAIComponent";

EnemyAIComponent::EnemyAIComponent()
    :
    m_bInitialized(false),
    m_bDead(false)
{

}

EnemyAIComponent::~EnemyAIComponent()
{

}

bool EnemyAIComponent::VInit(TiXmlElement* pData)
{
    assert(pData);



    return true;
}

void EnemyAIComponent::VPostInit()
{
    m_pRenderComponent = MakeStrongPtr(_owner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name));
    m_pPositionComponent = MakeStrongPtr(_owner->GetComponent<PositionComponent>(PositionComponent::g_Name));
    assert(m_pRenderComponent);
    assert(m_pPositionComponent);

    shared_ptr<HealthComponent> pHealthComp = 
        MakeStrongPtr(_owner->GetComponent<HealthComponent>(HealthComponent::g_Name));
    assert(pHealthComp);

    pHealthComp->AddObserver(this);
}

void EnemyAIComponent::VUpdate(uint32 msDiff)
{
    if (!m_bInitialized)
    {
        LOG_WARNING("");
        assert(!m_StateMap.empty());
        auto findIt = m_StateMap.find("PatrolState");
        assert(findIt != m_StateMap.end());

        findIt->second->VOnStateEnter();

        m_bInitialized = true;
    }

    if (m_bDead)
    {


        // I want it to disappear after ~900ms
        Point moveDelta(-(800 / 900.0 * msDiff), -(800 / 900.0f * msDiff));
        m_pPositionComponent->SetPosition(m_pPositionComponent->GetX() + moveDelta.x, m_pPositionComponent->GetY() + moveDelta.y);

        // This feels like a hack
        if (HumanView* pHumanView = g_pApp->GetHumanView())
        {
            shared_ptr<CameraNode> pCamera = pHumanView->GetCamera();
            if (pCamera)
            {
                shared_ptr<EventData_Move_Actor> pEvent(new EventData_Move_Actor(_owner->GetGUID(), m_pPositionComponent->GetPosition()));
                IEventMgr::Get()->VTriggerEvent(pEvent);

                SDL_Rect dummy;
                SDL_Rect renderRect = m_pRenderComponent->VGetPositionRect();
                SDL_Rect cameraRect = pCamera->GetCameraRect();
                if (!SDL_IntersectRect(&renderRect, &cameraRect, &dummy))
                {
                    shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(_owner->GetGUID()));
                    IEventMgr::Get()->VQueueEvent(pEvent);
                }
            }
            else
            {
                LOG_ERROR("Could not retrieve camera");
            }
        }
    }
}

void EnemyAIComponent::VOnHealthBelowZero()
{
    m_bDead = true;
    for (auto stateComponentIter : m_StateMap)
    {
        stateComponentIter.second->VOnStateLeave();
    }

    shared_ptr<PhysicsComponent> pPhysicsComponent =
        MakeStrongPtr(_owner->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
    assert(pPhysicsComponent);

    pPhysicsComponent->Destroy();
}