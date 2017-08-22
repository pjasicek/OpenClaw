#ifndef __FOLLOWABLE_COMPONENT__
#define __FOLLOWABLE_COMPONENT__

#include "../ActorComponent.h"

//=====================================================================================================================
// FollowableComponent
//=====================================================================================================================

typedef std::map<std::string, Actor*> FollowingActorsMap;

class PositionComponent;
class ActorRenderComponent;
class FollowableComponent : public ActorComponent
{
public:
    FollowableComponent();
    virtual ~FollowableComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual void VPostInit() override;
    virtual TiXmlElement* VGenerateXml() override;

    virtual void VUpdate(uint32 msDiff) override;

    bool IsActive() { return m_CurrentMsDuration > 0; }
    void Activate(int msDuration);
    void Deactivate();

private:
    // XML Data members
    Point m_Offset;
    std::string m_ImageSet;
    std::string m_AnimationPath;

    // Internal members
    PositionComponent* m_pPositionComponent;

    PositionComponent* m_pTargetPositionComponent;
    ActorRenderComponent* m_pTargetRenderComponent;
    Actor* m_pFollowingActor;

    int m_MsDuration;
    int m_CurrentMsDuration;
};

#endif