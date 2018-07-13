#pragma once

#include "basic_types.h"

enum class VariantType
{
    NIL,
    i32,
    f32,
    u32,
};

#define VARIANT_CONSTRUCTOR( T ) Variant( T value ) { value_##T = value; type = VariantType::##T; }
#define VARIANT_CAST( T ) operator T() const { return value_##T; }

struct Variant
{
    Variant() { type = VariantType::NIL; }
    VARIANT_CONSTRUCTOR( i32 )
    VARIANT_CONSTRUCTOR( f32 )
    VARIANT_CONSTRUCTOR( u32 )

    VariantType type;
    union
    {
        i32 value_i32;
        u32 value_u32;
        f32 value_f32;
    };

    VARIANT_CAST( i32 )
    VARIANT_CAST( u32 )
    VARIANT_CAST( f32 )
};
