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
#include "Util/XmlUtil.h"
#include "ActorDefinitions.h"

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
    else if (fixtureTypeStr == "EnemyAIDuckMeleeSensor") { fixtureType = FixtureType_EnemyAIDuckMeleeSensor; }
    else if (fixtureTypeStr == "EnemyAIRangedSensor") { fixtureType = FixtureType_EnemyAIRangedSensor; }
    else if (fixtureTypeStr == "EnemyAIDuckRangedSensor") { fixtureType = FixtureType_EnemyAIDuckRangedSensor; }
    else if (fixtureTypeStr == "DamageAura") { fixtureType = FixtureType_DamageAura; }
    else
    {
        assert(false && "Unknown body type");
    }

    return fixtureType;
}

inline DamageType StringToDamageTypeEnum(const std::string& str)
{
    static const std::map<std::string, DamageType> s_StringToDamageTypeEnumMap =
    {
        { "DamageType_None", DamageType_None },
        { "DamageType_MeleeAttack", DamageType_MeleeAttack },
        { "DamageType_Bullet", DamageType_Bullet },
        { "DamageType_Magic", DamageType_Magic },
        { "DamageType_Explosion", DamageType_Explosion },
        { "DamageType_FireSword", DamageType_FireSword },
        { "DamageType_LightningSword", DamageType_LightningSword },
        { "DamageType_FrostSword", DamageType_FrostSword }
    };

    auto findIt = s_StringToDamageTypeEnumMap.find(str);
    if (findIt == s_StringToDamageTypeEnumMap.end())
    {
        LOG_ERROR("Could not find DamageType: " + str);
        assert(false);
    }

    return findIt->second;
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

template
<typename KeyType, typename ValueType>
ValueType GetValueFromMap(KeyType _key, const std::map<KeyType, ValueType>& _map)
{
    auto findIt = _map.find(_key);
    assert(findIt != _map.end() && "Failed to locate value from map");

    return findIt->second;
}



#endif
