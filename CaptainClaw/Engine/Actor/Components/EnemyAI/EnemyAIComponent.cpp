#include "EnemyAIComponent.h"
#include "EnemyAIStateComponent.h"

const char* EnemyAIComponent::g_Name = "EnemyAIComponent";

EnemyAIComponent::EnemyAIComponent()
    :
    m_bInitialized(false)
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
}