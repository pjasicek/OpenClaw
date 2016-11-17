#ifndef __PHYSICSCONTACTLISTENER_H__
#define __PHYSICSCONTACTLISTENER_H__

#include <Box2D/Box2D.h>

class PhysicsContactListener : public b2ContactListener
{
public:
    virtual void BeginContact(b2Contact* pContact) override;
    virtual void EndContact(b2Contact* pContact) override;
};

#endif