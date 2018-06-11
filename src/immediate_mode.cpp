#include "immediate_mode.h"

#include "basics.h"

#include <SDL.h>
#include <glad/glad.h>

#define IMMEDIATE_VERTEX_COUNT 65536
#define IMMEDIATE_INDEX_COUNT 65536

struct ImmediateContext
{
    typedef std::array<Vector3, IMMEDIATE_VERTEX_COUNT> Vertices;
    typedef std::array<Color,   IMMEDIATE_VERTEX_COUNT> Colors;
    typedef std::array<Vector2, IMMEDIATE_VERTEX_COUNT> UVW;
    typedef std::array<uint,    IMMEDIATE_INDEX_COUNT>  Indices;    

    Vertices vertices     = {};
    Colors   colors       = {};
    UVW      uvws         = {};
    uint     vertex_count = 0;

    Indices indices     = {};
    uint    index_count = 0;

    // keep these in order ! (check glGenBuffers)
    uint vbo_vertices = 0;
    uint vbo_colors   = 0;
    uint vbo_uvws     = 0;
    uint vbo_indices  = 0;

    uint texture = 0;
    uint vao     = 0;
    
    uint draw_type = GL_TRIANGLES;
    Matrix4 world_matrix = Matrix4::Identity();
    Matrix4 projection_matrix = Matrix4::Identity();
    Matrix4 view_matrix = Matrix4::Identity();
    float depth = 0.0f;

    const Shader* shader = nullptr;
};

namespace
{
    Shader* immediate_shader;
    ImmediateContext immediate_context;

    void Initialize_ImmediateContext()
    {
        glGenBuffers( 4, &immediate_context.vbo_vertices );
        glGenTextures( 1, &immediate_context.texture );
        glGenVertexArrays( 1, &immediate_context.vao );
    
        immediate_shader = load_shader( "datas/shaders/immediate_shader.glsl" );
        immediate_set_shader( *immediate_shader );
    }
}

void init_immediate()
{
    Initialize_ImmediateContext();
}

void cleanup_immediate()
{
    immediate_clear();
    glDeleteProgram( immediate_shader->program );
    immediate_shader->program = NULL;
}

void immediate_clear()
{
    immediate_context.vertex_count = 0;
    immediate_context.index_count  = 0;
    immediate_set_shader( *immediate_shader );
    immediate_context.world_matrix = Matrix4::Identity();
    immediate_context.draw_type = GL_TRIANGLES;
}

void immediate_set_world_matrix( const Matrix4& w )
{
    immediate_context.world_matrix = w;
}

void immediate_set_view_matrix( const Matrix4& v )
{
    immediate_context.view_matrix = v;
}

void immediate_set_projection_matrix( const Matrix4& p )
{
    immediate_context.projection_matrix = p;
}

void immediate_set_texture( const Texture& texture )
{
    glBindTexture( GL_TEXTURE_2D, immediate_context.texture );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, (GLsizei) texture.size.width, (GLsizei) texture.size.height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture.data );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );

    glGenerateMipmap( GL_TEXTURE_2D );
}

void immediate_set_draw_type( uint type )
{
    assert( type >= GL_POINTS && type <= GL_QUADS, "type must be between GL_POINTS and GL_QUADS.");
    immediate_context.draw_type = type;
}

void immediate_set_depth( float depth )
{
    immediate_context.depth = depth;
}

void immediate_flush()
{
    auto& context = immediate_context;

    if(context.vertex_count == 0 || context.index_count == 0)
    {
        immediate_clear();
        return;
    }

    assert(context.shader != nullptr, "Need to set a shader before flushing immediate mode.");

    const Shader& shader = *immediate_context.shader;
    glUseProgram( shader.program );

    const MaterialDef& def = *shader.material;
    for( int i=0; i<def.params.size(); ++i )
    {
        const MaterialParam& param = def.params[i];
        switch( param.usage )
        {
        case MaterialParamUsage::WORLD:
            glUniformMatrix4fv( param.location, 1, GL_TRUE, context.world_matrix.m[0] );
            break;
        case MaterialParamUsage::VIEW:
            glUniformMatrix4fv( param.location, 1, GL_TRUE, context.view_matrix.m[0] );
            break;
        case MaterialParamUsage::PROJECTION:
            glUniformMatrix4fv( param.location, 1, GL_TRUE, context.projection_matrix.m[0] );
            break;
        
        case MaterialParamUsage::POSITION:
            glBindBuffer( GL_ARRAY_BUFFER, context.vbo_vertices );
            glBufferData( GL_ARRAY_BUFFER, context.vertex_count * sizeof(Vector3), 
                            context.vertices.data(), GL_DYNAMIC_DRAW );
            break;
        case MaterialParamUsage::COLOR:
            glBindBuffer( GL_ARRAY_BUFFER, context.vbo_colors );
            glBufferData( GL_ARRAY_BUFFER, context.vertex_count * sizeof(context.colors[0]), 
                            context.colors.data(), GL_DYNAMIC_DRAW );
            break;
        case MaterialParamUsage::UV:
            glBindBuffer( GL_ARRAY_BUFFER, context.vbo_uvws );
            glBufferData( GL_ARRAY_BUFFER, context.vertex_count * sizeof(context.uvws[0]), 
                            context.uvws.data(), GL_DYNAMIC_DRAW );
            break;
        
        case MaterialParamUsage::CUSTOM:
            switch( param.type )
            {
            case MaterialParamType::TEXTURE2D:
                glActiveTexture( GL_TEXTURE0 );
                glBindTexture( GL_TEXTURE_2D, immediate_context.texture );
                glUniform1i( param.location, 0 );
            default:
                assert(false, "Error: Unhandled param type.");
            }

        default:
            assert(false, "Error: Unhandled param usage.");
        }
    }

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, context.vbo_indices );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, context.index_count * sizeof(context.indices[0]), context.indices.data(), GL_DYNAMIC_DRAW );

    glBindVertexArray( context.vao );
    glDrawElements( immediate_context.draw_type, context.index_count, GL_UNSIGNED_INT, (void*)0 );

    glBindVertexArray( NULL );

    immediate_clear();
}

static void immediate_setup_buffers()
{
    if( immediate_context.shader == nullptr )
        return;

    glBindVertexArray( immediate_context.vao );
    
    const Shader& shader = *immediate_context.shader;
    const MaterialDef& def = *shader.material;
    
    for( int i=0; i<def.params.size(); ++i )
    {
        const MaterialParam& param = def.params[i];
        switch( param.usage )
        {
        case MaterialParamUsage::POSITION:
            glBindBuffer( GL_ARRAY_BUFFER, immediate_context.vbo_vertices );
            glVertexAttribPointer( param.location, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr );
            glEnableVertexAttribArray( param.location );
            break;
        case MaterialParamUsage::COLOR:
            glBindBuffer( GL_ARRAY_BUFFER, immediate_context.vbo_colors );
            glVertexAttribPointer( param.location, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
            glEnableVertexAttribArray( param.location );
            break;
        case MaterialParamUsage::UV:
            glBindBuffer( GL_ARRAY_BUFFER, immediate_context.vbo_uvws );
            glVertexAttribPointer( param.location, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
            glEnableVertexAttribArray( param.location );
            break;
        }
    }

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, immediate_context.vbo_indices );

    glBindVertexArray( NULL );
}

void immediate_set_shader( const Shader& shader )
{
    immediate_context.shader = &shader;
    immediate_setup_buffers();
}

void immediate_draw_triangle(
    const Vector3& p1, const Color& c1,
    const Vector3& p2, const Color& c2,
    const Vector3& p3, const Color& c3 )
{
    auto& vertex_count = immediate_context.vertex_count;
    auto& index_count  = immediate_context.index_count;
    auto& vertices     = immediate_context.vertices;
    auto& colors       = immediate_context.colors;
    auto& indices      = immediate_context.indices;

    assert(vertex_count + 3 <= IMMEDIATE_VERTEX_COUNT, "No vertices left to make an immediate triangle.");
    assert(index_count  + 3 <= IMMEDIATE_INDEX_COUNT,  "No indices left to make an immediate triangle.");

    vertices[vertex_count] = p1;
    colors  [vertex_count] = c1;
    indices[index_count++] = vertex_count;
    vertex_count++;
    vertices[vertex_count] = p2;
    colors  [vertex_count] = c2;
    indices[index_count++] = vertex_count;
    vertex_count++;
    vertices[vertex_count] = p3;
    colors  [vertex_count] = c3;
    indices[index_count++] = vertex_count;
    vertex_count++;
}

void immediate_draw_line(
    const Vector3& p1, const Color& c1,
    const Vector3& p2, const Color& c2
)
{
    auto& vertex_count = immediate_context.vertex_count;
    auto& index_count  = immediate_context.index_count;
    auto& vertices     = immediate_context.vertices;
    auto& colors       = immediate_context.colors;
    auto& indices      = immediate_context.indices;

    assert(vertex_count + 2 <= IMMEDIATE_VERTEX_COUNT, "No vertices left to make an immediate line.");
    assert(index_count  + 2 <= IMMEDIATE_INDEX_COUNT,  "No indices left to make an immediate line.");

    uint idx1 = vertex_count;
    vertices[vertex_count] = p1;
    colors  [vertex_count] = c1;
    vertex_count++;

    uint idx2 = vertex_count;
    vertices[vertex_count] = p2;
    colors  [vertex_count] = c2;
    vertex_count++;

    indices[index_count++] = idx1;
    indices[index_count++] = idx2;
}

void immediate_draw_quad(  const Vector3& p1, const Vector2& uv1,
                            const Vector3& p2, const Vector2& uv2,
                            const Vector3& p3, const Vector2& uv3,
                            const Vector3& p4, const Vector2& uv4 )
{
    auto& vertex_count = immediate_context.vertex_count;
    auto& index_count  = immediate_context.index_count;
    auto& vertices     = immediate_context.vertices;
    auto& uvws         = immediate_context.uvws;
    auto& indices      = immediate_context.indices;

    assert(vertex_count + 4 <= IMMEDIATE_VERTEX_COUNT, "No vertices left to make an immediate quad.");
    assert(index_count  + 6 <= IMMEDIATE_INDEX_COUNT,  "No indices left to make an immediate quad.");

    uint idx1 = vertex_count;
    vertices[vertex_count] = p1;
    uvws  [vertex_count] = uv1;
    vertex_count++;
    uint idx2 = vertex_count;
    vertices[vertex_count] = p2;
    uvws  [vertex_count] = uv2;
    vertex_count++;
    uint idx3 = vertex_count;
    vertices[vertex_count] = p3;
    uvws  [vertex_count] = uv3;
    vertex_count++;
    uint idx4 = vertex_count;
    vertices[vertex_count] = p4;
    uvws  [vertex_count] = uv4;
    vertex_count++;

    indices[index_count++] = idx1;
    indices[index_count++] = idx2;
    indices[index_count++] = idx3;
    indices[index_count++] = idx3;
    indices[index_count++] = idx2;
    indices[index_count++] = idx4;
}

void immediate_draw_quad(
    const Vector3& p1, const Color& c1,
    const Vector3& p2, const Color& c2,
    const Vector3& p3, const Color& c3,
    const Vector3& p4, const Color& c4 )
{
    auto& vertex_count = immediate_context.vertex_count;
    auto& index_count  = immediate_context.index_count;
    auto& vertices     = immediate_context.vertices;
    auto& colors       = immediate_context.colors;
    auto& indices      = immediate_context.indices;

    assert(vertex_count + 4 <= IMMEDIATE_VERTEX_COUNT, "No vertices left to make an immediate quad.");
    assert(index_count  + 6 <= IMMEDIATE_INDEX_COUNT,  "No indices left to make an immediate quad.");

    uint idx1 = vertex_count;
    vertices[vertex_count] = p1;
    colors  [vertex_count] = c1;
    vertex_count++;
    uint idx2 = vertex_count;
    vertices[vertex_count] = p2;
    colors  [vertex_count] = c2;
    vertex_count++;
    uint idx3 = vertex_count;
    vertices[vertex_count] = p3;
    colors  [vertex_count] = c3;
    vertex_count++;
    uint idx4 = vertex_count;
    vertices[vertex_count] = p4;
    colors  [vertex_count] = c4;
    vertex_count++;

    indices[index_count++] = idx1;
    indices[index_count++] = idx2;
    indices[index_count++] = idx3;
    indices[index_count++] = idx3;
    indices[index_count++] = idx2;
    indices[index_count++] = idx4;
}

void immediate_draw_mesh( const MeshDef* mesh )
{
    auto& ic_vertex_count = immediate_context.vertex_count;
    auto& ic_index_count  = immediate_context.index_count;
    auto& ic_vertices     = immediate_context.vertices;
    auto& ic_colors       = immediate_context.colors;
    auto& ic_indices      = immediate_context.indices;

    for( uint i=0; i < mesh->vertex_count; ++i )
    {
        ic_vertices[ic_vertex_count + i] = mesh->vertices[i].position;
        ic_colors  [ic_vertex_count + i] = mesh->vertices[i].color;
    }
    ic_vertex_count += mesh->vertex_count;

    for( uint i=0; i < mesh->index_count; ++i )
    {
        ic_indices[ic_index_count + i] = mesh->indices[i];
    }
    ic_index_count += mesh->index_count;
}
