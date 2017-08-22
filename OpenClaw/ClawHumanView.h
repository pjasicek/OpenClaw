#ifndef __CLAWHUMANVIEW_H__
#define __CLAWHUMANVIEW_H__

#include "Engine/SharedDefines.h"
#include "Engine/UserInterface/HumanView.h"
#include "Engine/UserInterface/MovementController.h"

class ActorController;
class ClawHumanView : public HumanView
{
public:
    ClawHumanView(SDL_Renderer* renderer);
    virtual ~ClawHumanView();

    virtual bool VOnEvent(SDL_Event& event) override;
    virtual void VOnUpdate(uint32 msDiff) override;
    virtual void VOnAttach(uint32 gameViewId, uint32 actorId) override;

    virtual void VSetControlledActor(uint32 actorId) override;

    virtual bool VLoadGameDelegate(TiXmlElement* pLevelXmlElem, LevelData* pLevelData) override;

private:
    void RegisterAllDelegates();
    void RemoveAllDelegates();

    shared_ptr<MovementController> m_pFreeCameraController;
    shared_ptr<ActorController> m_pActorController;

    shared_ptr<SceneNode> m_pControlledActor;
};

#endif
