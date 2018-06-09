#pragma once

struct Vector2;

struct Size
{
    Size() = default;
    Size( float _width, float _height );
    Size( const Vector2& _src );

    float width;
    float height;

    float AspectRatio() const;
};

#ifdef MATHLIB_IMPLEMENTATION

#include "vector.h"

Size::Size( float _width, float _height )
    : width(_width), height(_height)
{}

Size::Size(const Vector2& vec)
    : width(vec.x), height(vec.y)
{}

float Size::AspectRatio() const
{
    return width / height;
}

#endif