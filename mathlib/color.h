#pragma once

struct Color
{
    float r, g, b, a;

    static Color From32ARGB( unsigned int argb );
};

#ifdef MATHLIB_IMPLEMENTATION

Color Color::From32ARGB( unsigned int argb )
{
    Color color;

    color.a = (float)(( argb & 0xFF000000 ) >> 24) / 255.0f;
    color.r = (float)(( argb & 0x00FF0000 ) >> 16) / 255.0f;
    color.g = (float)(( argb & 0x0000FF00 ) >> 8)  / 255.0f;
    color.b = (float)(( argb & 0x000000FF ))       / 255.0f;

    return color;
}

#endif