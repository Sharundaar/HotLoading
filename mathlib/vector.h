#pragma once

struct Vector2
{
    union
    {
        struct{ float x, y; };
        struct{ float r, g; };
        struct{ float u, b; };
        float m[2];
    };

    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-(const Vector2& other) const;
    Vector2 operator*( float scale ) const;
    Vector2 operator/( float scale ) const;
    void operator+=(const Vector2& other);
    void operator-=(const Vector2& other);
    void operator*=( float scale );
    void operator/=( float scale );

    float Length() const;
    float LengthSqr() const;

    void Rotate(float degrees);
    void Normalize();

    static Vector2 Multiply(const Vector2& v1, const Vector2& v2);

    static float Dot(const Vector2& a, const Vector2& b);
    static void Rotate(const Vector2& v, float degrees);
    static Vector2 Normalize( const Vector2& v );

    static Vector2 Up()    { return Vector2 {  0,  1 };  }
    static Vector2 Down()  { return Vector2 {  0, -1 };  }
    static Vector2 Left()  { return Vector2 { -1,  0 };  }
    static Vector2 Right() { return Vector2 {  1,  0 };  }
    static Vector2 Zero()  { return Vector2 {  0,  0 };  }
    static Vector2 One()   { return Vector2 {  1,  1 };  }
};

Vector2 operator*( float scale, const Vector2& vect );

struct Vector3
{
    union
    {
        struct{ float x, y, z; };
        struct{ float r, g, b; };
        struct{ float u, v, w; };
        float m[3];
    };

    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator*(const Vector3& other) const;
    Vector3 operator-() const;
    Vector3 operator*( float scale ) const;
    Vector3 operator/( float scale ) const;
    void operator+=(const Vector3& other);
    void operator-=(const Vector3& other);
    void operator*=(const Vector3& other);
    void operator*=( float scale );
    void operator/=( float scale );

    float Length() const;
    float LengthSqr() const;

    void Normalize();

    static Vector3 Multiply(const Vector3& v1, const Vector3& v2);

    static float Dot(const Vector3& a, const Vector3& b);
    static void Rotate(const Vector3& v, float degrees);
    static Vector3 Normalize( const Vector3& v );

    static Vector3 Up()      { return Vector3 {  0,  1,  0 };  }
    static Vector3 Down()    { return Vector3 {  0, -1,  0 };  }
    static Vector3 Left()    { return Vector3 { -1,  0,  0 };  }
    static Vector3 Right()   { return Vector3 {  1,  0,  0 };  }
    static Vector3 Forward() { return Vector3 {  0,  0,  1 };  }
    static Vector3 Back()    { return Vector3 {  0,  0, -1 };  }
    static Vector3 Zero()    { return Vector3 {  0,  0,  0 };  }
    static Vector3 One()     { return Vector3 {  1,  1,  1 };  }
};

Vector3 operator*( float scale, const Vector3& vect );


struct Vector4
{
    union
    {
        struct{ float w, x, y, z; };
        struct{ float r, g, b, a; };
        float m[4];
    };

    Vector4 operator+(const Vector4& other) const;
    Vector4 operator-(const Vector4& other) const;
    Vector4 operator-() const;
    Vector4 operator*( float scale ) const;
    Vector4 operator/( float scale ) const;
    void operator+=(const Vector4& other);
    void operator-=(const Vector4& other);
    void operator*=( float scale );
    void operator/=( float scale );

    float Length() const;
    float LengthSqr() const;

    void Normalize();

    static Vector4 Multiply(const Vector4& v1, const Vector4& v2);

    static float Dot(const Vector4& a, const Vector4& b);
    static void Rotate(const Vector4& v, float degrees);
    static Vector4 Normalize( const Vector4& v );
};

Vector3 operator*( float scale, const Vector3& vect );

#ifdef MATHLIB_IMPLEMENTATION

#include "math_helper.h"

Vector2 Vector2::operator+( const Vector2& other ) const
{
    return { this->x + other.x, this->y + other.y };
}

Vector2 Vector2::operator-( const Vector2& other ) const
{
    return { this->x - other.x, this->y - other.y };
}

Vector2 Vector2::operator*( float scale ) const
{
    return { scale * x, scale * y };
}

Vector2 Vector2::operator/( float scale ) const
{
    float invert = 1.0f / scale;
    return { invert * x, invert * y };
}

void Vector2::operator+=( const Vector2& other )
{
    this->x += other.x;
    this->y += other.y;
}

void Vector2::operator-=( const Vector2& other )
{
    this->x -= other.x;
    this->y -= other.y;
}

void Vector2::operator*=( float scale )
{
    x *= scale;
    y *= scale;
}

void Vector2::operator/=( float scale )
{
    float invert = 1.0f / scale;
    x *= invert;
    y *= invert;
}

float Vector2::Length() const
{
    return sqrt( x * x + y * y );
}

float Vector2::LengthSqr() const
{
    return x * x + y * y;
}

Vector2 Vector2::Multiply(const Vector2& v1, const Vector2& v2)
{
    return {
        v1.x*v2.x,
        v1.y*v2.y,
    };
}

float Vector2::Dot( const Vector2& a, const Vector2& b )
{
    return a.x * b.x + a.y * b.y;
}

void Vector2::Rotate( float degrees )
{
    float newX = x;
    float newY = y;
    float rad = degrees * DEG2RAD;
    newX = x * cos( rad ) + y * sin( rad );
    newY = x * sin( rad ) + y * cos( rad );
    x = newX;
    y = newY;
}

void Vector2::Normalize()
{
    float l = Length();
    x = x / l;
    y = y / l;
}

Vector2 operator*( float scale, const Vector2& vect )
{
    return vect * scale;
}

Vector3 Vector3::operator+( const Vector3& other ) const
{
    return { this->x + other.x, this->y + other.y, this->z + other.z };
}

Vector3 Vector3::operator-( const Vector3& other ) const
{
    return { this->x - other.x, this->y - other.y, this->z - other.z };
}

Vector3 Vector3::operator*(const Vector3& other) const
{
    return { this->x * other.x, this->y * other.y, this->z * other.z };
}

Vector3 Vector3::operator-() const
{
    return { -this->x, -this->y, -this->z };
}

Vector3 Vector3::operator*( float scale ) const
{
    return { scale * x, scale * y, scale * z };
}

Vector3 Vector3::operator/( float scale ) const
{
    float invert = 1.0f / scale;
    return { invert * x, invert * y, invert * z };
}

void Vector3::operator+=( const Vector3& other )
{
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;
}

void Vector3::operator-=( const Vector3& other )
{
    this->x -= other.x;
    this->y -= other.y;
    this->z -= other.z;
}

void Vector3::operator*=( float scale )
{
    this->x *= scale;
    this->y *= scale;
    this->z *= scale;
}

void Vector3::operator/=( float scale )
{
    float invert = 1.0f / scale;
    this->x *= invert;
    this->y *= invert;
    this->z *= invert;
}

float Vector3::Length() const
{
    return sqrt( x * x + y * y + z * z );
}

float Vector3::LengthSqr() const
{
    return x * x + y * y + z * z;
}

Vector3 Vector3::Multiply(const Vector3& v1, const Vector3& v2)
{
    return {
        v1.x*v2.x,
        v1.y*v2.y,
        v1.z*v2.z,
    };
}

float Vector3::Dot( const Vector3& a, const Vector3& b )
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

void Vector3::Normalize()
{
    float l = Length();

    if( AlmostZero( l ) )
    {
        x = y = z = 0;
    }
    else
    {
        x = x / l;
        y = y / l;
        z = z / l;
    }
}

Vector3 Vector3::Normalize( const Vector3& v )
{
    Vector3 result = v;
    result.Normalize();
    return result;
}

Vector3 operator*( float scale, const Vector3& vect )
{
    return vect * scale;
}

#endif