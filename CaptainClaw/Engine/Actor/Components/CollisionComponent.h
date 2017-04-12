#ifndef COLLISIONCOMPONENT_H_
#define COLLISIONCOMPONENT_H_

#include "../../SharedDefines.h"
#include "../ActorComponent.h"

class CollisionComponent : public ActorComponent
{
public:
    CollisionComponent() { }

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;

    void GetCollisionBodySize(int32* collisionWidth, int32* collisionHeight)
    {
        (*collisionWidth) = _collisionWidth;
        (*collisionHeight) = _collisionHeight;
    }
    int32 GetCollisionBodyWidth() const { return _collisionWidth; }
    int32 GetCollisionBodyHeight() const { return _collisionHeight; }

    void SetCollisionBodySize(int32 collisionWidth, int32 collisionHeight)
    {
        _collisionWidth = collisionWidth;
        _collisionHeight = collisionHeight;
    }
    void SetCollisionBodyWidth(int32 collisionWidth) { _collisionWidth = collisionWidth; }
    void SetCollisionBodyHeight(int32 collisionHeight) { _collisionHeight = collisionHeight; }

private:
    int32 _collisionWidth;
    int32 _collisionHeight;
};

#endif
