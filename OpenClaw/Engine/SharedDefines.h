#ifndef SHAREDDEFINES_H_
#define SHAREDDEFINES_H_

#ifdef _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define MYDEBUG_NEW   new( _NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define MYDEBUG_NEW
#endif // _DEBUG

#ifdef _DEBUG
#define new MYDEBUG_NEW
#endif
#endif

#include <stdint.h>
#include <string>
#include <memory>
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
#include "Util/CustomAssert.h"
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

template
<typename KeyType, typename ValueType>
ValueType GetValueFromMap(KeyType _key, const std::map<KeyType, ValueType>& _map)
{
    auto findIt = _map.find(_key);
    assert(findIt != _map.end() && "Failed to locate value from map");

    return findIt->second;
}

#endif
