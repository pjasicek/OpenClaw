#ifndef SHAREDDEFINES_H_
#define SHAREDDEFINES_H_

#include <stdint.h>
#include <string>
#include <memory>
#include <assert.h>
#include <vector>
#include <list>
#include <map>
#include <Tinyxml/tinyxml.h>
#include <algorithm>

#include "Logger/Logger.h"
#include "Util/StringUtil.h"
#include "Util/Util.h"
#include "Util/Profilers.h"
#include "Interfaces.h"
#include "Events/EventMgr.h"
#include "XmlMacros.h"

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
    Direction_Left,
    Direction_Right
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
};

inline Point operator-(const Point& left, const Point& right) { Point temp(left); temp -= right; return temp; }

#define NOMINMAX
#include <Windows.h>
#include <iostream>

#define START_QUERY_PERFORMANCE_TIMER \
    LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds; \
    LARGE_INTEGER Frequency; \
    QueryPerformanceFrequency(&Frequency); \
    QueryPerformanceCounter(&StartingTime); \

#define END_QUERY_PERFORMANCE_TIMER \
    QueryPerformanceCounter(&EndingTime); \
    ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart; \
    ElapsedMicroseconds.QuadPart *= 1000000; \
    ElapsedMicroseconds.QuadPart /= Frequency.QuadPart; \
    std::cout << "Elapsed microseconds: " << ElapsedMicroseconds.QuadPart << std::endl; \

#endif