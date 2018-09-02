#include "shader.h"

#include "basics.h"
#include "file_parser.h"

#include <SDL.h>
#include <glad/glad.h>

#include <fstream>
#include <vector>

#include "dll.h"

static ShaderParamType s_ShaderParamUsageToTypeTable[] = {
    ShaderParamType::UNKNOWN,
    ShaderParamType::MATRIX4,
    ShaderParamType::MATRIX4,
    ShaderParamType::MATRIX4,
    ShaderParamType::VECTOR3,
    ShaderParamType::VECTOR4,
    ShaderParamType::VECTOR3,
    ShaderParamType::VECTOR2,
};
static_assert( ARRAY_SIZE( s_ShaderParamUsageToTypeTable ) == (uint)ShaderParamUsage::Count, "Update s_ShaderParamUsageToTypeTable if you update ShaderParamUsage enum." );

ShaderParamType get_shader_param_type_from_usage( ShaderParamUsage usage )
{
    return s_ShaderParamUsageToTypeTable[ (uint)usage ];
}

static const char* s_ShaderParamTypeStringTable[] = {
    "unknown",
    "float",
    "vec2",
    "vec3",
    "vec4",
    "mat3",
    "mat4",
    "sampler2D"
};
static_assert( ARRAY_SIZE( s_ShaderParamTypeStringTable ) == (int)ShaderParamType::Count, "Update s_ShaderParamTypeStringTable if you updat ShaderParamType enum." );

const char* to_string( ShaderParamType type )
{
    return s_ShaderParamTypeStringTable[(uint)type];
}

static const char* s_ShaderParamUsageStringTable[] = {
    "world",
    "view",
    "projection",
    "position",
    "color",
    "normal",
    "uv",
    "custom",
};
static_assert( ARRAY_SIZE( s_ShaderParamUsageStringTable ) == (int)ShaderParamUsage::Count, "Update s_ShaderParamUsageStringTable if you update ShaderParamUsage enum." );

struct BuiltInShaderParam
{
    const char* name;
    ShaderParamUsage usage;
    bool is_attrib;
};

static BuiltInShaderParam s_BuiltInShaderParams[] = {
    { "World",      ShaderParamUsage::WORLD,      false },
    { "View",       ShaderParamUsage::VIEW,       false },
    { "Projection", ShaderParamUsage::PROJECTION, false },
    { "color",      ShaderParamUsage::COLOR,      true  },
    { "position",   ShaderParamUsage::POSITION,   true  },
    { "normal",     ShaderParamUsage::NORMAL,     true  },
    { "uv",         ShaderParamUsage::UV,         true  },
};

const char* to_string( ShaderParamUsage usage )
{
    return s_ShaderParamUsageStringTable[(uint)usage];
}

static ShaderParam make_shader_param( const char* name, uint location,
                               const char* type, const char* usage )
{
    ShaderParam param;

    param.name = name;
    param.location = location;

    for(uint i=0; i<ARRAY_SIZE(s_ShaderParamTypeStringTable); ++i)
    {
        auto type_str = s_ShaderParamTypeStringTable[i];
        if( strcmp(type_str, type) == 0 )
        {
            param.type = (ShaderParamType)i;
            break;
        }
    }

    if( !is_eof_or_nil(usage) )
    {
        for(uint i=0; i<ARRAY_SIZE(s_ShaderParamUsageStringTable); ++i)
        {
            auto usage_str = s_ShaderParamUsageStringTable[i];
            if( strcmp(usage_str, usage) == 0 )
            {
                param.usage = (ShaderParamUsage)i;
                break;
            }
        }
    }
    else
    {
        param.usage = ShaderParamUsage::CUSTOM;
    }

    return param;
}


static void extract_shader_params( uint program, std::vector<ShaderParam>& params, const char* param_block )
{
    int location = -1;

    // check for builtin
    for( auto& param : s_BuiltInShaderParams )
    {
        if( param.is_attrib )
            location = glGetAttribLocation( program, param.name );
        else
            location = glGetUniformLocation( program, param.name );
        if( location > -1 )
        {
            params.emplace_back( ShaderParam {
                param.name, (uint)location,
                get_shader_param_type_from_usage( param.usage ),
                param.usage
             } );
        }
    }

    // check custom params
    const char* params_ptr = param_block;
    if( !is_eof_or_nil(params_ptr) ) while( true )
    {
        params_ptr = chomp_empty_space( params_ptr );
        if( is_eof( *params_ptr ) )
            break;

        if( is_new_line( *params_ptr ) )
        {
            ++params_ptr;
            continue;
        }

        const char* token_end = params_ptr;
        token_end = chomp_token(token_end);
        std::string type_token(params_ptr, token_end);
        
        params_ptr = chomp_empty_space( token_end );
        token_end  = chomp_token( params_ptr );
        std::string param_name( params_ptr, token_end );

        // @Cleanup: NO SANITY CHECKS ARE DONE !!!
        // @Cleanup: NO SANITY CHECKS ARE DONE !!!
        // @Cleanup: NO SANITY CHECKS ARE DONE !!!

        params_ptr = chomp_empty_space( token_end );
        std::string usage_token;
        if( !is_new_line( *params_ptr ) )
        {
            params_ptr = chomp_empty_space( params_ptr );
            
            if( *params_ptr == ':' )
            {
                params_ptr++;
                params_ptr = chomp_empty_space( params_ptr );
                token_end = chomp_token( params_ptr );
                usage_token.assign( params_ptr, token_end );
            }

            params_ptr = chomp_empty_space( params_ptr );
        }

        // @Cleanup: can location be an attrib ?
        params.emplace_back( 
            make_shader_param ( 
                param_name.c_str(),
                glGetUniformLocation( program, param_name.c_str() ), 
                type_token.c_str(),
                usage_token.c_str() 
            )
        );

        assert( is_new_line( *params_ptr ), "ERROR: Must be newline at this point." );
        params_ptr++;
    }
}

char* extract_shader_name( const char* file, char* buffer, uint buffer_length )
{
    extract_file_name( file, buffer, buffer_length );
    return buffer;
}

static Shader* find_shader( MemoryPool<Shader>& shader_pool, const char* name )
{
    for(auto shader : shader_pool)
    {
        if( shader->name == name )
            return shader;
    }
    return nullptr;
}

Shader* load_shader( MemoryPool<Shader>& shader_pool, const char* source_file )
{
    std::string VERTEX_SHADER_TOKEN = "vertex";
    std::string FRAGMENT_SHADER_TOKEN = "fragment";
    std::string PARAMS_TOKEN = "params";

    ResourceFile file = parse_resource_file( source_file );
    if( !file.is_valid )
    {
        println( "Error: Unable to read the shader %.", source_file );
        return nullptr;
    }

    RFBlock* vertex_shader_block   = nullptr;
    RFBlock* fragment_shader_block = nullptr;
    RFBlock* params_block  = nullptr;

    for( int i=0; i<file.blocks.size(); ++i )
    {
        if( file.blocks[i].name == PARAMS_TOKEN )
            params_block = &file.blocks[i];
        else if( file.blocks[i].name == VERTEX_SHADER_TOKEN )
            vertex_shader_block = &file.blocks[i];
        else if( file.blocks[i].name == FRAGMENT_SHADER_TOKEN )
            fragment_shader_block = &file.blocks[i];
    }

    if(vertex_shader_block == nullptr || vertex_shader_block->content.size() == 0)
    {
        println("Haven't found any vertex shader in file %.", source_file);
        return nullptr;
    }

    if(fragment_shader_block == nullptr || fragment_shader_block->content.size() == 0)
    {
        println("Haven't found any fragment shader in file %.", source_file);
        return nullptr;
    }

    bool shader_compile_error = false, shader_link_error = false;
    int success;
    std::vector<const char*> source_array(1);
    char info_log[512];

    uint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const char* source_ptr = vertex_shader_block->content.c_str();
    glShaderSource(vertex_shader, 1, &source_ptr, nullptr);
    glCompileShader(vertex_shader);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
        println("ERROR: Compilation of vertex shader failed. File: %. Reason: \n%", source_file, info_log);
        shader_compile_error = true;
    }

    uint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    source_ptr = fragment_shader_block->content.c_str();
    glShaderSource(fragment_shader, 1, &source_ptr, nullptr);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
        println("ERROR: Compilition of fragment shader failed. File: %. Reason: \n%", source_file, info_log);
        shader_compile_error = true;
    }

    if(shader_compile_error)
    {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return nullptr;
    }

    uint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shader_program, 512, nullptr, &info_log[0]);
        println("ERROR: Linking of program shader failed. File: %. Reason: \n%", source_file, info_log);
        shader_link_error = true;
    }

    glDetachShader(shader_program, vertex_shader);
    glDetachShader(shader_program, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    if(shader_link_error)
    {
        glDeleteProgram(shader_program);
        return nullptr;
    }

    char shader_name[512];
    extract_shader_name( source_file, shader_name, 512 );
    Shader* shader = find_shader( shader_pool, shader_name );
    if( shader ) glDeleteProgram( shader->program );
    else shader = shader_pool.Instantiate();
    assert( shader != nullptr, "Allocation error." );

    setup_resource( shader, source_file, shader_name );
    shader->program           = shader_program;
    extract_shader_params( shader->program, shader->params, params_block ? params_block->content.c_str() : nullptr );

    return shader;
}

static Variant variant_from_shader_type( ShaderParamType type )
{
    switch( type )
    {
    case ShaderParamType::FLOAT:
        return 0.0f;
    case ShaderParamType::TEXTURE2D:
        return (u32)-1;
    default:
        return Variant();
    }
}

static MaterialParam material_param_from_shader_param( ShaderParam& param )
{
    return MaterialParam {
        param.name.c_str(),
        param.location,
        param.type,
        variant_from_shader_type( param.type ),
    };
}

Material* create_material( MemoryPool<Material>& material_pool, Shader* shader )
{
    assert( shader != nullptr, "Shader must have a value." );

    auto mat = material_pool.Instantiate();
    mat->shader = shader;

    uint shader_param_size = (uint)shader->params.size();
    uint custom_param_count = 0;
    int custom_param_begin = -1;
    for( uint i = 0; i < shader_param_size; ++i )
    {
        if( shader->params[i].usage == ShaderParamUsage::CUSTOM )
        {
            custom_param_count = shader_param_size - i;
            custom_param_begin = i;
            break;
        }
    }

    if( custom_param_count > 0 )
    {
        mat->param_instances.reserve( custom_param_count );
        for( uint i = custom_param_begin; i < shader_param_size; ++i )
            mat->param_instances.emplace_back( material_param_from_shader_param( shader->params[i] ) );
    }

    return mat;
}

void set_material_param( Material* material, const char* param_name, Variant value )
{
    for( auto& param : material->param_instances )
    {
        if( strcmp( param.name, param_name ) == 0 )
        {
            if( value.type != param.value.type )
                println( "Tried to set material param with the wrong variant type, received: %, expected: %.", value.type, param.value.type );
            else
                param.value = value;
            break;
        }
    }
}
