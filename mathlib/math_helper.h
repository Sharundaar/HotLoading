#pragma once

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define DEG2RAD ((float)M_PI) / 180.0f
#define RAD2DEG 180.0f / ((float)M_PI)

#define EPSILON 1e-6

#define SAME_SIGNS(a, b) ((a<0 && b<0) || (a>=0 && b>=0))

inline bool AlmostEqual( float a, float b )
{
    if ( std::abs( a - b ) < EPSILON )
        return true;
    return false;
}

inline bool AlmostZero( float a )
{
    return AlmostEqual( a, 0.0f );
}

