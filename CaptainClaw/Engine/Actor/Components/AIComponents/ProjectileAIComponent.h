#ifndef __PROJECTILE_AI_COMPONENT_H__
#define __PROJECTILE_AI_COMPONENT_H__

#include "../../../SharedDefines.h"
#include "../../ActorComponent.h"


class ProjectileAIComponent : public ActorComponent
{
public:
    ProjectileAIComponent();
    virtual ~ProjectileAIComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }
    virtual void VPostInit() override;

    virtual void VUpdate(uint32 msDiff) override;

    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;

    void OnCollidedWithSolidTile();
    void OnCollidedWithActor(Actor* pActorWhoWasShot);

private:
    void Detonate();

    // XML Data
    int32 m_Damage;
    DamageType m_DamageType;
    Point m_ProjectileSpeed;
    int m_SourceActorId;
    int m_DetonationTime;
    int m_NumSparkles;

    // Internal state
    shared_ptr<IGamePhysics> m_pPhysics;
    bool m_IsActive;
    std::vector<StrongActorPtr> m_PowerupSparkles;
    bool m_bHasDetonationTime;
};

#endif