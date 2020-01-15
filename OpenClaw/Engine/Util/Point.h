#ifndef __POINT_H__
#define __POINT_H__

#include <string>
#include <cmath>
#include <float.h>

#include "StringUtil.h"

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
inline Point operator+(const Point& left, const Point& right) { Point temp(left); temp += right; return temp; }

#endif