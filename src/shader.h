#pragma once

#include <array>
#include <unordered_map>
#include <vector>
#include <string>

#include "basic_types.h"
#include "variant_type.h"
#include "basics.h"
#include "resource.h"
#include "memory_pool.h"

enum class ShaderParamType : ushort
{
    UNKNOWN,   // unknown type

    FLOAT,     // float
    VECTOR2,   // vec2
    VECTOR3,   // vec3
    VECTOR4,   // vec4
    MATRIX3,   // mat3
    MATRIX4,   // mat4
    TEXTURE2D, // sampler2d

    Count,
};
const char* to_string( ShaderParamType type );

enum class ShaderParamUsage : ushort
{
    CUSTOM,
    WORLD,
    VIEW,
    PROJECTION,
    POSITION,
    COLOR,
    NORMAL,
    UV,

    Count,
};
const char* to_string( ShaderParamUsage usage );

struct ShaderParam
{
    std::string name;
    uint        location;
    ShaderParamType  type;
    ShaderParamUsage usage;
};

struct Shader : public Resource
{
    GENERATE_BODY( Shader );

    uint program = 0;
    std::vector<ShaderParam> params;
};

struct MaterialParam
{
    const char* name = nullptr;
    uint location = 0;
    ShaderParamType type = ShaderParamType::UNKNOWN;
    Variant value;
};

struct Material
{
    Shader* shader;
    std::vector<MaterialParam> param_instances;
};

Material* create_material( MemoryPool<Material>& material_pool, Shader* shader );
void set_material_param( Material* material, const char* param_name, Variant value );

char* extract_shader_name( const char* file, char* buffer, uint buffer_length );
Shader* load_shader( MemoryPool<Shader>& shader_pool, const char* source_file );
ShaderParamType get_shader_param_type_from_usage( ShaderParamUsage usage );
