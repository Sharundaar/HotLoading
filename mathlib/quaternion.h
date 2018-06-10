#pragma once

struct Vector3;

struct Quaternion
{
    union
    {
        struct{ float w, x, y, z; };
        float m[4];
    };

    Quaternion operator*( const Quaternion& other ) const;

    float  Length()    const;
    float  LengthSqr() const;
    void Normalize();

    static void Normalize( const Quaternion& quat );
    static Quaternion Invert( const Quaternion& quat );

    static Quaternion FromAxisAngle( float angle_in_degree, const Vector3& axis );
    static Quaternion Identity();
};

#ifdef MATHLIB_IMPLEMENTATION

#include "basics.h"
#include "math_helper.h"
#include "vector.h"

Quaternion Quaternion::operator*( const Quaternion& other ) const
{
    return Quaternion {
        this->w * other.w - this->x * other.x - this->y * other.y - this->z * other.z,
        this->w * other.x + this->x * other.w + this->y * other.z - this->z * other.y,
        this->w * other.y + this->y * other.w + this->z * other.x - this->x * other.z,
        this->w * other.z + this->z * other.w + this->x * other.y - this->y * other.x,
    };
}

void Quaternion::Normalize()
{
    float l = Length();
    w /= l;
    x /= l;
    y /= l;
    z /= l;
}

float Quaternion::Length() const
{
    return sqrt( LengthSqr() );
}

float Quaternion::LengthSqr() const
{
    return w * w + x * x + y * y + z * z;
}

Quaternion Quaternion::Invert( const Quaternion& quat )
{
    return {
         quat.w,
        -quat.x,
        -quat.y,
        -quat.z,
    };
}

Quaternion Quaternion::FromAxisAngle( float angle_in_degree, const Vector3& axis )
{
    assert( !AlmostZero( axis.LengthSqr() ), "Error: Axis must be non zero." );

    float cosine = cosf( DEG2RAD * angle_in_degree / 2.0f );
    float sine   = sinf( DEG2RAD * angle_in_degree / 2.0f );

    Vector3 axis_normalized = Vector3::Normalize( axis );
    return Quaternion {
        cosine,
        axis_normalized.x * sine,
        axis_normalized.y * sine,
        axis_normalized.z * sine,
    };
}

Quaternion Quaternion::Identity()
{
    return Quaternion {
        1.0f,
        0.0f,
        0.0f,
        0.0f,
    };
}

#endif