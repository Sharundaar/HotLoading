#include "shader.h"

#include "basics.h"
#include "file_parser.h"

#include <SDL.h>
#include <glad/glad.h>

#include <fstream>
#include <vector>

#include "dll.h"

static std::unordered_map<MaterialParamType, std::string> s_materialParamTypeDictionary = {
    { MaterialParamType::FLOAT,     "float" },
    { MaterialParamType::VECTOR2,   "vec2" },
    { MaterialParamType::VECTOR3,   "vec3" },
    { MaterialParamType::VECTOR4,   "vec4" },
    { MaterialParamType::MATRIX3,   "mat3" },
    { MaterialParamType::MATRIX4,   "mat4" },
    { MaterialParamType::TEXTURE2D, "sampler2d" },
};

const std::string& to_string( MaterialParamType type )
{
    assert( (ushort)type < s_materialParamTypeDictionary.size(), "ERROR: Unkown material param type." );
    return s_materialParamTypeDictionary[ type ];
}

static std::unordered_map<MaterialParamUsage, std::string> s_materialParamUsageDictionary = {
    { MaterialParamUsage::WORLD,      "world" },
    { MaterialParamUsage::VIEW,       "view" },
    { MaterialParamUsage::PROJECTION, "projection" },
    { MaterialParamUsage::POSITION,   "position" },
    { MaterialParamUsage::COLOR,      "color" },
    { MaterialParamUsage::NORMAL,     "normal" },
    { MaterialParamUsage::UV,         "uv" },
    { MaterialParamUsage::CUSTOM,     "custom" },
};

const std::string& to_string( MaterialParamUsage usage )
{
    assert( (ushort)usage < s_materialParamUsageDictionary.size(), "ERROR: Unkown material param usage." );
    return s_materialParamUsageDictionary[ usage ];
}

static std::unordered_map<MaterialParamUsage, MaterialParamType> s_materialParamTypeFromUsage = {
    { MaterialParamUsage::WORLD,      MaterialParamType::MATRIX4 },
    { MaterialParamUsage::VIEW,       MaterialParamType::MATRIX4 },
    { MaterialParamUsage::PROJECTION, MaterialParamType::MATRIX4 },
    { MaterialParamUsage::POSITION,   MaterialParamType::VECTOR3 },
    { MaterialParamUsage::COLOR,      MaterialParamType::VECTOR4 },
    { MaterialParamUsage::NORMAL,     MaterialParamType::VECTOR3 },
    { MaterialParamUsage::UV,         MaterialParamType::VECTOR2 },
};

MaterialParamType get_material_type_from_usage( MaterialParamUsage usage )
{
    return s_materialParamTypeFromUsage[ usage ];
}

// @Cleanup: Clean this mess
static MaterialDef* make_material_definition( uint program, const RFBlock* block )
{
    if( block == nullptr )
        return make_material_definition( program, std::string() );
    else
        return make_material_definition( program, block->content );
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
    shader->exported_material = make_material_definition( shader->program, params_block );
    shader->material          = get_equivalent_material( shader->exported_material );

    assert(shader->material != nullptr, "Error: material must be initialized at this point !");

    return shader;
}

MaterialDef* get_equivalent_material( MaterialDef* def )
{
    return def->equivalent != nullptr ? def->equivalent : def;
}

MaterialDef* find_equivalent_material( const MaterialDef* def )
{
    auto& material_pool = get_dll_appdata().global_store.material_pool;
    for( auto it = material_pool.begin(); it != material_pool.end(); ++it )
    {
        if( def == *it )
            continue;
        if( are_material_equivalent( def, *it ) )
        {
            if( it->equivalent != nullptr )
                return it->equivalent;
            else
                return *it;
        }
    }

    return nullptr;
}

MaterialParam make_material_param( const char* name, uint location,
                                    MaterialParamType type, MaterialParamUsage usage )
{
    MaterialParam param;

    param.name     = name;
    param.location = location;
    param.type     = type;
    param.usage    = usage;

    return param;
}

MaterialParam make_material_param( const char* name, uint location,
                                    const char* type, const char* usage )
{
    MaterialParam param;

    param.name     = name;
    param.location = location;

    // @Improvement: This is slooooow !!
    for( auto pair : s_materialParamTypeDictionary )
    {
        if( pair.second == type )
        {
            param.type = pair.first;
            break;
        }
    }

    if( usage != nullptr && !is_eof( *usage ) )
    {
        // @Improvement: This is slooooow !!
        for( auto pair : s_materialParamUsageDictionary )
        {
            if( pair.second == usage )
            {
                param.usage = pair.first;
            }
        }
    }
    else
    {
        param.usage = MaterialParamUsage::CUSTOM;
    }

    return param;
}

struct BuiltInMaterialParam
{
    std::string name;
    MaterialParamUsage usage;
    bool is_attrib;
};

static std::vector<BuiltInMaterialParam> s_builtInMaterialParams = {
    { "World",      MaterialParamUsage::WORLD,      false },
    { "View",       MaterialParamUsage::VIEW,       false },
    { "Projection", MaterialParamUsage::PROJECTION, false },
    { "color",      MaterialParamUsage::COLOR,      true  },
    { "position",   MaterialParamUsage::POSITION,   true  },
    { "normal",     MaterialParamUsage::NORMAL,     true  },
    { "uv",         MaterialParamUsage::UV,         true  },
};

MaterialDef* make_material_definition( uint program, const std::string& params )
{
    auto& material_pool = get_dll_appdata().global_store.material_pool;
    MaterialDef* def = material_pool.Instantiate();

    int location = -1;

    // check for builtin
    for( auto param : s_builtInMaterialParams )
    {
        if( param.is_attrib )
            location = glGetAttribLocation( program, param.name.c_str() );
        else
            location = glGetUniformLocation( program, param.name.c_str() );
        if( location > -1 )
        {
            def->params.emplace_back( make_material_param(
                param.name.c_str(), location,
                get_material_type_from_usage( param.usage ),
                param.usage
            ) );
        }
    }

    // check custom params
    const char* params_ptr = params.c_str();
    if( !params.empty() ) while( true )
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
        def->params.emplace_back( 
            make_material_param( param_name.c_str(), glGetUniformLocation( program, param_name.c_str() ), 
                                    type_token.c_str(), usage_token.c_str() ) 
        );

        assert( is_new_line( *params_ptr ), "ERROR: Must be newline at this point." );
        params_ptr++;
    }

    MaterialDef* equivalent_material = find_equivalent_material( def );
    def->equivalent = equivalent_material;

    return def;
}

bool are_material_equivalent( const MaterialDef* def1, const MaterialDef* def2 )
{
    if( ( def1 == nullptr || def2 == nullptr ) || ( def1->params.size() != def2->params.size() ) )
        return false;

    bool equivalent = true;
    int param_idx = 0;
    for( uint param1 = 0; param1 < def1->params.size(); ++param1 )
    {
        bool paramFound = false;
        for( uint param2 = 0; param2 < def2->params.size(); ++param2 )
        {
            if( def1->params[param1].type == def2->params[param2].type 
                && def1->params[param1].location == def2->params[param2].location )
                {
                    paramFound = true;
                    break;
                }
        }

        if( !paramFound )
            return false;
    }

    return true;
}