#pragma once

#include "vector.h"
#include "quaternion.h"
#include "size.h"

struct Matrix3
{
    union
    {
        struct
        {
            float m11, m12, m13;
            float m21, m22, m23;
            float m31, m32, m33;
        };
        float m[3][3];
    };

    void Transpose();

    Matrix3 operator*(const Matrix3& other) const;
    
    Vector2 operator*(const Vector2& target) const;
    Vector2 Apply    (const Vector2& target) const;

    static Matrix3 Identity();
    static Matrix3 TranslationMatrix(const Vector2& translation);
    static Matrix3 ScaleMatrix      (const Vector2& scale);
    static Matrix3 RotationMatrix   (float angle_in_degree);
    static Matrix3 TSRMatrix        (const Vector2& translation, const Vector2& scale, float angle_in_degree);
    static Matrix3 ViewMatrix       (const Vector2& position, const Vector2& scale, float angle_in_degree, const Size& viewport);
};

struct Matrix4
{
    union
    {
        struct
        {
            float m11, m12, m13, m14;
            float m21, m22, m23, m24;
            float m31, m32, m33, m34;
            float m41, m42, m43, m44;
        };
        float m[4][4];
    };

    void Transpose();
    Matrix4 Transposed();

    Matrix4 operator*(const Matrix4& other) const;
    
    Vector3 operator*(const Vector3& target) const;
    Vector3 Apply    (const Vector3& target) const;

    static Matrix4 Transpose( const Matrix4& matrix );
    static Matrix4 Identity();
    static Matrix4 TranslationMatrix( const Vector3& translation );
    static Matrix4 ScaleMatrix      ( const Vector3& scale );
    static Matrix4 RotationMatrix   ( float angle_in_degree, const Vector3& axis );
    static Matrix4 OpenGLProjectionMatrix ( float fov, float aspect, float near, float far );

    static Matrix4 FromQuaternion( const Quaternion& q );
    static Matrix4 RotationTranslation( const Vector3& translation, const Quaternion& quaternion );
};

#ifdef MATHLIB_IMPLEMENTATION

#include "basics.h"

Matrix3 Matrix3::operator*(const Matrix3& other) const
{
    Matrix3 result;

    for(int i=0; i < 3; ++i)
    {
        for(int j=0; j < 3; ++j)
        {
            result.m[i][j] = 0;
            for(int k=0; k < 3; ++k)
            {
                result.m[i][j] += m[k][j] * other.m[i][k];
            }
        }
    }

    return result;
}

Vector2 Matrix3::operator*(const Vector2& target) const
{
    return Apply( target );
}

Vector2 Matrix3::Apply(const Vector2& target) const
{
    Vector2 result = {};
    result.x = m[0][0] * target.x + m[0][1] * target.y + m[0][2];
    result.y = m[1][0] * target.x + m[1][1] * target.y + m[1][2];
    float w  = m[2][0] * target.x + m[2][1] * target.y + m[2][2];
    ML_ASSERT(!AlmostZero(w), "Matrix projection leads to division by 0 !");
    result.x /= w;
    result.y /= w;
    return result;
}

Matrix3 Matrix3::Identity()
{
    Matrix3 id;
    id.m[0][0] = 1; id.m[0][1] = 0; id.m[0][2] = 0;
    id.m[1][0] = 0; id.m[1][1] = 1; id.m[1][2] = 0;
    id.m[2][0] = 0; id.m[2][1] = 0; id.m[2][2] = 1;
    return id;
}

Matrix3 Matrix3::TranslationMatrix(const Vector2& translation)
{
    Matrix3 t;
    t.m[0][0] = 1; t.m[0][1] = 0; t.m[0][2] = translation.x;
    t.m[1][0] = 0; t.m[1][1] = 1; t.m[1][2] = translation.y;
    t.m[2][0] = 0; t.m[2][1] = 0; t.m[2][2] = 1;
    return t;
}

Matrix3 Matrix3::RotationMatrix(float angle_in_degree)
{
    Matrix3 r;
    float cosine = std::cosf(DEG2RAD * angle_in_degree);
    float sine   = std::sinf(DEG2RAD * angle_in_degree);
    r.m[0][0] = cosine; r.m[0][1] =  -sine; r.m[0][2] = 0;
    r.m[1][0] =   sine; r.m[1][1] = cosine; r.m[1][2] = 0;
    r.m[2][0] =      0; r.m[2][1] =      0; r.m[2][2] = 1;
    return r;
}

Matrix3 Matrix3::ScaleMatrix(const Vector2& scale)
{
    Matrix3 r;
    r.m[0][0] = scale.x; r.m[0][1] =       0; r.m[0][2] = 0;
    r.m[1][0] =       0; r.m[1][1] = scale.y; r.m[1][2] = 0;
    r.m[2][0] =       0; r.m[2][1] =       0; r.m[2][2] = 1;
    return r;
}

Matrix3 Matrix3::TSRMatrix(const Vector2& translation, const Vector2& scale, float angle_in_degree)
{
    Matrix3 r;
    float cosine = std::cosf(DEG2RAD * angle_in_degree);
    float sine   = std::sinf(DEG2RAD * angle_in_degree);
    r.m[0][0] = scale.x * cosine; r.m[0][1] = scale.y *  -sine; r.m[0][2] = translation.x;
    r.m[1][0] = scale.x *   sine; r.m[1][1] = scale.y * cosine; r.m[1][2] = translation.y;
    r.m[2][0] =                0; r.m[2][1] =                0; r.m[2][2] =             1;
    return r;
}

Matrix3 Matrix3::ViewMatrix (const Vector2& position, const Vector2& scale, float angle_in_degree, const Size& viewport)
{
    ML_ASSERT( viewport.width != 0 && viewport.height != 0, "viewport parameter must be non 0.");

    return TranslationMatrix( -1.0f * position ) * RotationMatrix( -angle_in_degree ) * ScaleMatrix( { 2.0f / viewport.width, 2.0f / viewport.height } );
}

Matrix4 Matrix4::operator*(const Matrix4& other) const
{
    Matrix4 result;

    for(int i=0; i < 4; ++i)
    {
        for(int j=0; j < 4; ++j)
        {
            result.m[i][j] = 0;
            for(int k=0; k < 4; ++k)
            {
                result.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
    }

    return result;
}

Vector3 Matrix4::operator*(const Vector3& target) const
{
    return Apply( target );
}

Vector3 Matrix4::Apply(const Vector3& target) const
{
    Vector3 result;
    result.x = m[0][0] * target.x + m[0][1] * target.y + m[0][2] * target.z + m[0][3];
    result.y = m[1][0] * target.x + m[1][1] * target.y + m[1][2] * target.z + m[1][3];
    result.z = m[2][0] * target.x + m[2][1] * target.y + m[2][2] * target.z + m[2][3];
    return result;
}

Matrix4 Matrix4::Identity()
{
    Matrix4 id;
    
    id.m[0][0] = 1; id.m[0][1] = 0; id.m[0][2] = 0; id.m[0][3] = 0;
    id.m[1][0] = 0; id.m[1][1] = 1; id.m[1][2] = 0; id.m[1][3] = 0;
    id.m[2][0] = 0; id.m[2][1] = 0; id.m[2][2] = 1; id.m[2][3] = 0;
    id.m[3][0] = 0; id.m[3][1] = 0; id.m[3][2] = 0; id.m[3][3] = 1;
    
    return id;
}

void Matrix4::Transpose()
{
    Matrix4 result;

    for(int i=0; i<4; ++i)
    {
        for(int j=0; j<4; ++j)
        {
            result.m[i][j] = this->m[j][i];
        }
    }

    *this = result;
}

Matrix4 Matrix4::Transposed()
{
    Matrix4 result;

    for(int i=0; i<4; ++i)
    {
        for(int j=0; j<4; ++j)
        {
            result.m[i][j] = this->m[j][i];
        }
    }

    return result;
}

Matrix4 Matrix4::TranslationMatrix(const Vector3& translation)
{
    Matrix4 t;
    
    t.m[0][0] = 1; t.m[0][1] = 0; t.m[0][2] = 0; t.m[0][3] = translation.x;
    t.m[1][0] = 0; t.m[1][1] = 1; t.m[1][2] = 0; t.m[1][3] = translation.y;
    t.m[2][0] = 0; t.m[2][1] = 0; t.m[2][2] = 1; t.m[2][3] = translation.z;
    t.m[3][0] = 0; t.m[3][1] = 0; t.m[3][2] = 0; t.m[3][3] =             1;

    return t;
}

Matrix4 Matrix4::RotationMatrix(float angle_in_degree, const Vector3& axis)
{
    Matrix4 r;
    float cosine = std::cos(DEG2RAD * angle_in_degree);
    float sine   = std::sin(DEG2RAD * angle_in_degree);
    
    r.m11 = cosine + axis.x*axis.x * ( 1 - cosine );
    r.m12 = axis.x*axis.y*(1 - cosine) - axis.z * sine;
    r.m13 = axis.x*axis.z*(1 - cosine) + axis.y * sine;
    r.m14 = 0;

    r.m21 = axis.y*axis.x*(1 - cosine) + axis.z * sine;
    r.m22 = cosine + axis.y * axis.y * (1 - cosine);
    r.m23 = axis.y * axis.z * (1 - cosine) - axis.x * sine;
    r.m24 = 0;
    
    r.m31 = axis.z * axis.x * (1 - cosine) - axis.y * sine;
    r.m32 = axis.z * axis.y * (1 - cosine) + axis.x * sine;
    r.m33 = cosine + axis.z * axis.z * (1 - cosine);
    r.m34 = 0;

    r.m41 = 0;
    r.m42 = 0;
    r.m43 = 0;
    r.m44 = 1;

    return r;
}

Matrix4 Matrix4::ScaleMatrix(const Vector3& scale)
{
    Matrix4 r;

    r.m[0][0] = scale.x; r.m[0][1] =       0; r.m[0][2] =       0; r.m[0][3] = 0;
    r.m[1][0] =       0; r.m[1][1] = scale.y; r.m[1][2] =       0; r.m[1][3] = 0;
    r.m[2][0] =       0; r.m[2][1] =       0; r.m[2][2] = scale.z; r.m[2][3] = 0;
    r.m[3][0] =       0; r.m[3][1] =       0; r.m[3][2] =       0; r.m[3][3] = 1;
    
    return r;
}

Matrix4 Matrix4::OpenGLProjectionMatrix ( float fov, float aspect, float _near, float _far )
{
    Matrix4 p;
    float tanHalfFov = tanf( fov * DEG2RAD / 2.0f );

    p.m11 = 1.0f / ( aspect * tanHalfFov );
    p.m22 = 1.0f / tanHalfFov;
    p.m33 = ( _far + _near ) / ( _far - _near );
    p.m43 = 1.0f;
    p.m34 = -2.0f * _near * _far / ( _far - _near );

    p.m31 = 0; p.m32 = 0;
    p.m41 = 0; p.m42 = 0;
    p.m44 = 0.0f;

    p.m21 = 0;
    p.m23 = 0; p.m24 = 0;
    p.m12 = 0; p.m13 = 0; p.m14 = 0;

    return p;
}

Matrix4 Matrix4::FromQuaternion( const Quaternion& q )
{
    Matrix4 m;

    m.m11 = 1 - 2 * q.y * q.y - 2 * q.z * q.z;
    m.m12 = 2 * q.x * q.y - 2 * q.z * q.w;
    m.m13 = 2 * q.x * q.z + 2 * q.y * q.w;

    m.m21 = 2 * q.x * q.y + 2 * q.z * q.w;
    m.m22 = 1 - 2 * q.x * q.x - 2 * q.z * q.z;
    m.m23 = 2 * q.y * q.z - 2 * q.x * q.w;

    m.m31 = 2 * q.x * q.z - 2 * q.y * q.w;
    m.m32 = 2 * q.y * q.z + 2 * q.x * q.w;
    m.m33 = 1 - 2 * q.x * q.x - 2 * q.y * q.y;

    m.m14 = m.m24 = m.m34 = m.m41 = m.m42 = m.m43 = 0;
    m.m44 = 1.0f;

    return m;
}

Matrix4 Matrix4::RotationTranslation( const Vector3& t, const Quaternion& q )
{
    Matrix4 rot   = FromQuaternion( q );
    Matrix4 trans = TranslationMatrix( t );

    return rot * trans;
}

#endif