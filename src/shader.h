#pragma once

#include <array>
#include <unordered_map>
#include <vector>
#include <string>

#include "basic_types.h"
#include "basics.h"
#include "resource.h"
#include "memory_pool.h"

enum class MaterialParamType : ushort
{
    FLOAT,     // float
    VECTOR2,   // vec2
    VECTOR3,   // vec3
    VECTOR4,   // vec4
    MATRIX3,   // mat3
    MATRIX4,   // mat4
    TEXTURE2D, // sampler2d
};
const std::string& to_string( MaterialParamType type );

enum class MaterialParamUsage : ushort
{
    CUSTOM,
    WORLD,
    VIEW,
    PROJECTION,
    POSITION,
    COLOR,
    NORMAL,
    UV,
};
const std::string& to_string( MaterialParamUsage usage );

struct MaterialParam
{
    std::string name;
    uint        location;
    MaterialParamType  type;
    MaterialParamUsage usage;
};

struct MaterialDef
{
    MaterialDef* equivalent;
    std::vector<MaterialParam> params;
};

struct Shader : public Resource
{
    GENERATE_BODY( Shader );

    uint program = 0;

    MaterialDef* material;
    MaterialDef* exported_material;
};


char* extract_shader_name( const char* file, char* buffer, uint buffer_length );
Shader* load_shader( MemoryPool<Shader>& shader_pool, const char* source_file );
bool are_material_equivalent( const MaterialDef* def1, const MaterialDef* def2 );
MaterialDef* make_material_definition( uint program, const std::string& params );
MaterialDef* find_equivalent_material( const MaterialDef* def );
MaterialDef* get_equivalent_material( MaterialDef* def );
MaterialParamType get_material_type_from_usage( MaterialParamUsage usage );
