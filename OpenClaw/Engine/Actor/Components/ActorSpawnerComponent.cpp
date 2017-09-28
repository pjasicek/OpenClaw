#include "ActorSpawnerComponent.h"
#include "PositionComponent.h"

#include "../../GameApp/BaseGameApp.h"
#include "../../GameApp/BaseGameLogic.h"

//=====================================================================================================================
//
// ActorSpawnerComponent Implementation
//
//=====================================================================================================================

const char* ActorSpawnerComponent::g_Name = "ActorSpawnerComponent";

ActorSpawnerComponent::ActorSpawnerComponent()
{ 

}

bool ActorSpawnerComponent::VInit(TiXmlElement* pData)
{
    assert(pData);

    for (TiXmlElement* pActorSpawnInfoElem = pData->FirstChildElement("ActorSpawnInfo");
        pActorSpawnInfoElem != NULL;
        pActorSpawnInfoElem = pActorSpawnInfoElem->NextSiblingElement("ActorSpawnInfo"))
    {
        ActorSpawnInfo spawnInfo;

        std::string actorProtoStr;
        ParseValueFromXmlElem(&actorProtoStr, pActorSpawnInfoElem->FirstChildElement("ActorPrototype"));
        spawnInfo.actorProto = StringToEnum_ActorPrototype(actorProtoStr);

        ParseValueFromXmlElem(&spawnInfo.spawnPositionOffset, pActorSpawnInfoElem->FirstChildElement("SpawnPositionOffset"), "x", "y");
        ParseValueFromXmlElem(&spawnInfo.initialVelocity, pActorSpawnInfoElem->FirstChildElement("InitialVelocity"), "x", "y");

        m_ActorSpawnInfoList.push_back(spawnInfo);
    }

    ParseValueFromXmlElem(&m_SpawnAnimation, pData->FirstChildElement("SpawnAnimation"));

    assert(!m_ActorSpawnInfoList.empty());

    return true;
}

void ActorSpawnerComponent::VPostInit()
{
    m_pTriggerComponent = MakeStrongPtr(m_pOwner->GetComponent<TriggerComponent>()).get();
    assert(m_pTriggerComponent);

    m_pTriggerComponent->AddObserver(this);

    if (!m_SpawnAnimation.empty())
    {
        auto pAC = MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>());
        if (pAC)
        {
            pAC->AddObserver(this);
        }
    }
}

void ActorSpawnerComponent::VOnActorEnteredTrigger(Actor* pActorWhoPickedThis, FixtureType triggerType)
{
    if (triggerType != FixtureType_Trigger_SpawnArea)
    {
        return;
    }

    for (const ActorSpawnInfo& actorSpawnInfo : m_ActorSpawnInfoList)
    {
        Point spawnPosition = m_pOwner->GetPositionComponent()->GetPosition() + actorSpawnInfo.spawnPositionOffset;

        StrongActorPtr pSpawnedActor =
            ActorTemplates::CreateActor(actorSpawnInfo.actorProto, spawnPosition);

        g_pApp->GetGameLogic()->VGetGamePhysics()->VApplyForce(pSpawnedActor->GetGUID(), actorSpawnInfo.initialVelocity);
    }

    if (!m_SpawnAnimation.empty())
    {
        auto pAC = MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>());
        if (pAC)
        {
            pAC->SetAnimation(m_SpawnAnimation);
            pAC->ResumeAnimation();
        }
    }

    m_pTriggerComponent->Deactivate(FixtureType_Trigger_SpawnArea);
}

void ActorSpawnerComponent::VOnAnimationLooped(Animation* pAnimation)
{
    auto pAC = MakeStrongPtr(m_pOwner->GetComponent<AnimationComponent>());
    pAC->PauseAnimation();
}