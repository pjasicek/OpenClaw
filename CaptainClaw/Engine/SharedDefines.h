#ifndef SHAREDDEFINES_H_
#define SHAREDDEFINES_H_

#include <stdint.h>
#include <string>
#include <memory>
#include <assert.h>
#include <vector>
#include <list>
#include <map>
#include <tinyxml.h>
#include <Box2D/Box2D.h>
#include <algorithm>
#include <cmath>

#include "Logger/Logger.h"
#include "Util/StringUtil.h"
#include "Util/Util.h"
#include "Util/Profilers.h"
#include "Interfaces.h"
#include "Events/EventMgr.h"
#include "XmlMacros.h"
#include "Interfaces.h"
#include "SoundStrings.h"

using std::shared_ptr;
using std::unique_ptr;
using std::weak_ptr;
using std::static_pointer_cast;

typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;
typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;

#ifndef SAFE_DELETE
#define SAFE_DELETE(ptr)       { if (ptr) { delete (ptr);     (ptr)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(ptr) { if (ptr) { delete[] (ptr);   (ptr)=NULL; } }
#endif

#ifndef PROFILE_CPU
#define PROFILE_CPU(tag) CPU_PROFILER _CPU_PROFILER_(tag);
#endif

#ifndef PROFILE_MEMORY
#define PROFILE_MEMORY(tag) MEMORY_PROFILER _MEMORY_PROFILER_(tag);
#endif

#ifndef max
#define max(a, b) ((a) < (b) ? (b) : (a))
#endif

// Resource caches
// Hardcoding this saves A LOT of headaches
#define ORIGINAL_RESOURCE "CLAW_REZ"
#define CUSTOM_RESOURCE   "ASSETS_ZIP"

const uint32 INVALID_ACTOR_ID = 0;
const uint32 INVALID_GAME_VIEW_ID = 0xFFFFFFFF;

struct Position2D
{
    Position2D(int32 posX, int32 posY) : x(posX), y(posY) { }
    Position2D() { x = 0; y = 0; }

    int32 x;
    int32 y;
};

enum Orientation
{
    Orientation_Left,
    Orientation_Right
};

template<class T>
struct SortBy_SharedPtr_Content
{
    bool operator()(const shared_ptr<T> &lhs, const shared_ptr<T> &rhs) const
    {
        return *lhs < *rhs;
    }
};

template <class Type>
inline std::shared_ptr<Type> MakeStrongPtr(std::weak_ptr<Type> pWeakPtr)
{
    if (!pWeakPtr.expired())
        return std::shared_ptr<Type>(pWeakPtr);
    else
        return std::shared_ptr<Type>();
}

enum Direction
{
    Direction_None,
    Direction_Left,
    Direction_Right,
};

//---------------------------------------------------------------------------------------------------------------------
// This class represents a single point in 2D space
//---------------------------------------------------------------------------------------------------------------------
class Point
{
public:
    double x, y;

    // construction
    Point(void) { x = y = 0; }
    Point(const double newX, const double newY) { x = newX; y = newY; }
    Point(const Point& newPoint) { x = newPoint.x; y = newPoint.y; }
    Point(const Point* pNewPoint) { x = pNewPoint->x; y = pNewPoint->y; }

    // assignment
    Point& operator=(const Point& newPoint) { x = newPoint.x; y = newPoint.y; return (*this); }
    Point& operator=(const Point* pNewPoint) { x = pNewPoint->x; y = pNewPoint->y; return (*this); }

    // addition/subtraction
    Point& operator+=(const Point& newPoint) { x += newPoint.x; y += newPoint.y; return (*this); }
    Point& operator-=(const Point& newPoint) { x -= newPoint.x; y -= newPoint.y; return (*this); }
    Point& operator+=(const Point* pNewPoint) { x += pNewPoint->x; y += pNewPoint->y; return (*this); }
    Point& operator-=(const Point* pNewPoint) { x -= pNewPoint->x; y -= pNewPoint->y; return (*this); }
    Point operator+(const Point& other) { Point temp(this); temp += other; return temp; }
    Point operator-(const Point& other) { Point temp(this); temp -= other; return temp; }

    // comparison
    bool operator==(const Point& other) const { return ((x == other.x) && (y == other.y)); }
    bool operator!=(const Point& other) const { return (!((x == other.x) && (y == other.y))); }

    // accessors (needed for Lua)
    double GetX() const { return x; }
    double GetY() const { return y; }
    void SetX(const double newX) { x = newX; }
    void SetY(const double newY) { y = newY; }
    void Set(const double newX, const double newY) { x = newX; y = newY; }

    // somewhat hacky vector emulation (maybe I should just write my own vector class)
    float Length() const { return sqrt((float)(x*x + y*y)); }

    bool IsZero() { return (std::fabs(x) < DBL_EPSILON || std::fabs(y) < DBL_EPSILON); }
    bool IsZeroXY() { return (std::fabs(x) < DBL_EPSILON && std::fabs(y) < DBL_EPSILON); }

    std::string ToString() { return ("[X: " + ToStr(x) + ", Y: " + ToStr(y) + "]"); }
};

inline Point operator-(const Point& left, const Point& right) { Point temp(left); temp -= right; return temp; }

struct ActorFixtureDef
{
    ActorFixtureDef()
    {
        fixtureType = FixtureType_None;
        collisionShape = "Rectangle";
        isSensor = false;
        size = Point(0, 0);
        offset = Point(0, 0);

        friction = 0.0f;
        density = 0.0f;
        float restitution = 0.0f;

        collisionFlag = CollisionFlag_None;
        collisionMask = 0x0;
    }

    FixtureType fixtureType;
    std::string collisionShape;
    bool isSensor;
    Point size;
    Point offset;

    float friction;
    float density;
    float restitution;

    CollisionFlag collisionFlag;
    uint32 collisionMask;
};

struct ActorBodyDef
{
    ActorBodyDef()
    {
        bodyType = b2_dynamicBody;
        addFootSensor = false;
        makeCapsule = false;
        makeBullet = false;
        makeSensor = true;
        fixtureType = FixtureType_None;
        position = Point(0, 0);
        positionOffset = Point(0, 0);
        collisionShape = "Rectangle";
        size = Point(0, 0);
        gravityScale = 1.0f;
        setInitialSpeed = false;
        setInitialImpulse = false;
        initialSpeed = Point(0, 0);
        collisionFlag = CollisionFlag_None;
        collisionMask = 0x0;
        prefabType = "";

        friction = 0.0f;
        density = 0.0f;
        restitution = 0.0f;

        isActive = true;
    }

    WeakActorPtr pActor;
    b2BodyType bodyType;
    bool addFootSensor;
    bool makeCapsule;
    bool makeBullet;
    bool makeSensor;
    FixtureType fixtureType;
    Point position;
    Point positionOffset;
    std::string collisionShape;
    Point size;
    float gravityScale;
    bool setInitialSpeed;
    bool setInitialImpulse;
    Point initialSpeed;
    CollisionFlag collisionFlag;
    uint32 collisionMask;
    float friction;
    float density;
    float restitution;
    std::string prefabType;
    // TODO: This is a bit hacky - used for ducking 
    bool isActive;

    std::vector<ActorFixtureDef> fixtureList;
};

struct EnemyAttackAction
{
    EnemyAttackAction()
    {
        attackAnimFrameIdx = 0;
        attackDamageType = DamageType_None;
        attackFxImageSet = "NONE";
    }

    std::string animation;
    uint32 attackAnimFrameIdx;
    std::string attackFxImageSet;

    DamageType attackDamageType;

    // Used to spawn area damage

    // Considering enemy is looking left
    Point attackSpawnPositionOffset;
    
    // Applicable only to melee attacks
    Point attackAreaSize;

    // Damage which the attack will deal to enemies
    uint32 damage;
};

struct PredefinedMove
{
    PredefinedMove()
    {
        msDuration = 0;
        pixelsPerSecond = Point(0, 0);
        soundToPlay = "";
    }

    uint32 msDuration;
    Point pixelsPerSecond;
    std::string soundToPlay;
};

inline void SetBoolIfDefined(bool *dest, TiXmlElement* elem)
{
    if (elem && elem->GetText() == NULL)
        return;

    std::string opt = elem->GetText();
    if (opt == "true")
    {
        *dest = true;
    }
    else if (opt == "false")
    {
        *dest = false;
    }
}

inline void SetUintIfDefined(unsigned* dest, TiXmlElement* elem)
{
    if (elem && elem->GetText())
    {
        *dest = std::stoi(elem->GetText());
    }
}

inline void SetIntIfDefined(int* dest, TiXmlElement* elem)
{
    if (elem && elem->GetText())
    {
        *dest = std::stoi(elem->GetText());
    }
}

inline void SetStringIfDefined(std::string* dest, TiXmlElement* elem)
{
    if (elem && elem->GetText())
    {
        *dest = elem->GetText();
    }
}

inline void SetDoubleIfDefined(double* dest, TiXmlElement* elem)
{
    if (elem && elem->GetText())
    {
        *dest = std::stod(elem->GetText());
    }
}

inline void SetFloatIfDefined(float* dest, TiXmlElement* elem)
{
    if (elem && elem->GetText())
    {
        *dest = std::stof(elem->GetText());
    }
}

// Rather overload it than 10 different functions ?

inline bool ParseValueFromXmlElem(bool* pDest, TiXmlElement* pElemSource)
{
    if (pElemSource && pElemSource->GetText())
    {
        std::string opt = pElemSource->GetText();
        if (opt == "true")
        {
            *pDest = true;
        }
        else if (opt == "false")
        {
            *pDest = false;
        }
        return true;
    }

    return false;
}

inline bool ParseValueFromXmlElem(unsigned* pDest, TiXmlElement* pElemSource)
{
    if (pElemSource && pElemSource->GetText())
    {
        *pDest = std::stoi(pElemSource->GetText());
        return true;
    }

    return false;
}

inline bool ParseValueFromXmlElem(unsigned long* pDest, TiXmlElement* pElemSource)
{
    if (pElemSource && pElemSource->GetText())
    {
        *pDest = std::stoul(pElemSource->GetText());
        return true;
    }

    return false;
}

inline bool ParseValueFromXmlElem(long* pDest, TiXmlElement* pElemSource)
{
    if (pElemSource && pElemSource->GetText())
    {
        *pDest = std::stol(pElemSource->GetText());
        return true;
    }

    return false;
}

inline bool ParseValueFromXmlElem(int* pDest, TiXmlElement* pElemSource)
{
    if (pElemSource && pElemSource->GetText())
    {
        *pDest = std::stoi(pElemSource->GetText());
        return true;
    }

    return false;
}

inline bool ParseValueFromXmlElem(float* pDest, TiXmlElement* pElemSource)
{
    if (pElemSource && pElemSource->GetText())
    {
        *pDest = std::stof(pElemSource->GetText());
        return true;
    }

    return false;
}

inline bool ParseValueFromXmlElem(double* pDest, TiXmlElement* pElemSource)
{
    if (pElemSource && pElemSource->GetText())
    {
        *pDest = std::stof(pElemSource->GetText());
        return true;
    }

    return false;
}

inline bool ParseValueFromXmlElem(std::string* pDest, TiXmlElement* pElemSource)
{
    if (pElemSource && pElemSource->GetText())
    {
        *pDest = std::string(pElemSource->GetText());
        return true;
    }

    return false;
}

inline void SetPointIfDefined(Point* pDest, TiXmlElement* pElem, const char* elemAttrNameX, const char* elemAttrNameY)
{
    if (pElem)
    {
        pElem->Attribute(elemAttrNameX, &pDest->x);
        pElem->Attribute(elemAttrNameY, &pDest->y);
    }
}

#define SetEnumIfDefined(pDest, pElem, enumType) \
    if (pElem && pElem->GetText()) \
    { \
        *pDest = enumType(std::stoi(pElem->GetText())); \
    } \

inline FixtureType FixtureTypeStringToEnum(std::string fixtureTypeStr)
{
    FixtureType fixtureType = FixtureType_None;

    if (fixtureTypeStr == "Solid") { fixtureType = FixtureType_Solid; }
    else if (fixtureTypeStr == "Ground") { fixtureType = FixtureType_Ground; }
    else if (fixtureTypeStr == "Climb") { fixtureType = FixtureType_Climb; }
    else if (fixtureTypeStr == "Death") { fixtureType = FixtureType_Death; }
    else if (fixtureTypeStr == "Trigger") { fixtureType = FixtureType_Trigger; }
    else if (fixtureTypeStr == "Projectile") { fixtureType = FixtureType_Projectile; }
    else if (fixtureTypeStr == "Crate") { fixtureType = FixtureType_Crate; }
    else if (fixtureTypeStr == "Pickup") { fixtureType = FixtureType_Pickup; }
    else if (fixtureTypeStr == "Trigger") { fixtureType = FixtureType_Trigger; }
    else if (fixtureTypeStr == "Controller") { fixtureType = FixtureType_Controller; }
    else if (fixtureTypeStr == "PowderKeg") { fixtureType = FixtureType_PowderKeg; }
    else if (fixtureTypeStr == "Explosion") { fixtureType = FixtureType_Explosion; }
    else if (fixtureTypeStr == "EnemyAI") { fixtureType = FixtureType_EnemyAI; }
    else if (fixtureTypeStr == "EnemyAIMeleeSensor") { fixtureType = FixtureType_EnemyAIMeleeSensor; }
    else if (fixtureTypeStr == "EnemyAIRangedSensor") { fixtureType = FixtureType_EnemyAIRangedSensor; }
    else if (fixtureTypeStr == "DamageAura") { fixtureType = FixtureType_DamageAura; }
    else
    {
        assert(false && "Unknown body type");
    }

    return fixtureType;
}

inline b2BodyType BodyTypeStringToEnum(std::string bodyTypeStr)
{
    b2BodyType bodyType;

    if (bodyTypeStr == "Static") { bodyType = b2_staticBody; }
    else if (bodyTypeStr == "Kinematic") { bodyType = b2_kinematicBody; }
    else if (bodyTypeStr == "Dynamic") { bodyType = b2_dynamicBody; }
    else
    {
        assert(false && "Unknown body type");
    }

    return bodyType;
}

#endif
