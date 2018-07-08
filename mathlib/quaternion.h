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

    static Quaternion FromEulerAngle( float yaw, float pitch, float roll );
    static Vector3 ToEulerAngle( const Quaternion& quat );
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
    ML_ASSERT( !AlmostZero( axis.LengthSqr() ), "Error: Axis must be non zero." );

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

Quaternion Quaternion::FromEulerAngle( float yaw, float pitch, float roll )
{
    Quaternion q;
        // Abbreviations for the various angular functions
	float cy = cosf(yaw * 0.5f);
	float sy = sinf(yaw * 0.5f);
	float cr = cosf(roll * 0.5f);
	float sr = sinf(roll * 0.5f);
	float cp = cosf(pitch * 0.5f);
	float sp = sinf(pitch * 0.5f);

	q.w = cy * cr * cp + sy * sr * sp;
	q.x = cy * sr * cp - sy * cr * sp;
	q.y = cy * cr * sp + sy * sr * cp;
	q.z = sy * cr * cp - cy * sr * sp;
	return q;
}

Vector3 Quaternion::ToEulerAngle( const Quaternion& q )
{
    Vector3 ypr;

    // roll (x-axis rotation)
	float sinr = 2.0f * (q.w * q.x + q.y * q.z);
	float cosr = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	ypr.m[2] = atan2(sinr, cosr);

	// pitch (y-axis rotation)
	float sinp = 2.0f * (q.w * q.y - q.z * q.x);
	if (fabs(sinp) >= 1.0f)
		ypr.m[1] = (float)copysign(M_PI / 2.0f, sinp); // use 90 degrees if out of range
	else
		ypr.m[1] = asin(sinp);

	// yaw (z-axis rotation)
	float siny = 2.0f * (q.w * q.z + q.x * q.y);
	float cosy = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);  
	ypr.m[0] = atan2(siny, cosy);

    return ypr;
}

#endif