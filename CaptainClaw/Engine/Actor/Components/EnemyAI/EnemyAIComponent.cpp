#include "EnemyAIComponent.h"

const char* EnemyAIComponent::g_Name = "EnemyAIComponent";

EnemyAIComponent::EnemyAIComponent()
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