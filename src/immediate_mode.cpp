#include "immediate_mode.h"

#include "basics.h"
#include "resource_pool.h"

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
    uint vao     = 0;
    
    uint draw_type = GL_TRIANGLES;
    Matrix4 world_matrix = Matrix4::Identity();
    Matrix4 projection_matrix = Matrix4::Identity();
    Matrix4 view_matrix = Matrix4::Identity();
    float depth = 0.0f;

    bool depth_test = true;
    bool alpha_blending = true;
    bool face_culling = true;

    bool scissoring = false;
    Vector4 scissor_window = {};

    const Shader* shader = nullptr;
    const Texture* texture = nullptr;
};

namespace
{
    Shader* immediate_shader;
    ImmediateContext immediate_context;

    void Initialize_ImmediateContext()
    {
        glGenBuffers( 4, &immediate_context.vbo_vertices );
        glGenVertexArrays( 1, &immediate_context.vao );
    }
}

void init_immediate()
{
    Initialize_ImmediateContext();
}

void immediate_set_default_shader( Shader* shader )
{
    immediate_shader = shader;
}

void cleanup_immediate()
{
    immediate_clear();
    glDeleteBuffers( 4, &immediate_context.vbo_vertices );
    glDeleteVertexArrays( 1, &immediate_context.vao );
    immediate_shader = nullptr;
}

void immediate_clear()
{
    immediate_context.vertex_count = 0;
    immediate_context.index_count  = 0;
    immediate_set_shader( *immediate_shader );
    immediate_set_texture( nullptr );
    immediate_context.world_matrix = Matrix4::Identity();
    immediate_context.draw_type = GL_TRIANGLES;

    immediate_context.depth_test = true;
    immediate_context.alpha_blending = true;
    immediate_context.face_culling = true;

    immediate_context.scissoring = false;
    immediate_context.scissor_window = {};
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

void immediate_set_texture( const Texture* texture )
{
    immediate_context.texture = texture;
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

void immediate_enable_depth_test( bool enabled ) { immediate_context.depth_test = enabled; }
void immediate_enable_blend( bool enabled ) { immediate_context.alpha_blending = enabled; }
void immediate_enable_face_cull( bool enabled ) { immediate_context.face_culling = enabled; }

void immediate_set_scissor_window( Vector2 pos, Size size ) { immediate_context.scissor_window = { pos.x, pos.y, size.width, size.height }; immediate_context.scissoring = true; }

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

    if( immediate_context.depth_test )
        glEnable( GL_DEPTH_TEST );
    else
        glDisable( GL_DEPTH_TEST );

    if( immediate_context.face_culling )
        glEnable( GL_CULL_FACE );
    else
        glDisable( GL_CULL_FACE );

    if( immediate_context.scissoring )
    {
        glEnable( GL_SCISSOR_TEST );
        glScissor((int)immediate_context.scissor_window.w, 
                  (int)immediate_context.scissor_window.x, 
                  (int)immediate_context.scissor_window.y,
                  (int)immediate_context.scissor_window.z);
    }
    else
    {
        glDisable( GL_SCISSOR_TEST );
    }

    if( immediate_context.alpha_blending )
    {
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable( GL_BLEND );
    }
    else
    {
        glDisable( GL_BLEND );
    }

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
                if( immediate_context.texture )
                {
                    glActiveTexture( GL_TEXTURE0 );
                    glBindTexture( GL_TEXTURE_2D, immediate_context.texture->buffer );
                    glUniform1i( param.location, 0 );
                }
                break;
            default:
                assert(false, "Error: Unhandled param type.");
                break;
            }
            break;

        default:
            assert(false, "Error: Unhandled param usage.");
            break;
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

void immediate_draw_triangle(  const Vector3& p1, const Color& c1, const Vector2& uv1,
                                const Vector3& p2, const Color& c2, const Vector2& uv2,
                                const Vector3& p3, const Color& c3, const Vector2& uv3 )
{
    auto& vertex_count = immediate_context.vertex_count;
    auto& index_count  = immediate_context.index_count;
    auto& vertices     = immediate_context.vertices;
    auto& colors       = immediate_context.colors;
    auto& uvws         = immediate_context.uvws;
    auto& indices      = immediate_context.indices;

    assert(vertex_count + 3 <= IMMEDIATE_VERTEX_COUNT, "No vertices left to make an immediate triangle.");
    assert(index_count  + 3 <= IMMEDIATE_INDEX_COUNT,  "No indices left to make an immediate triangle.");

    vertices[vertex_count] = p1;
    colors  [vertex_count] = c1;
    uvws    [vertex_count] = uv1;
    indices[index_count++] = vertex_count;
    vertex_count++;
    vertices[vertex_count] = p2;
    colors  [vertex_count] = c2;
    uvws    [vertex_count] = uv2;
    indices[index_count++] = vertex_count;
    vertex_count++;
    vertices[vertex_count] = p3;
    colors  [vertex_count] = c3;
    uvws    [vertex_count] = uv3;
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
    auto& ic_uvs          = immediate_context.uvws;
    auto& ic_indices      = immediate_context.indices;

    for( uint i=0; i < mesh->vertex_count; ++i )
    {
        ic_vertices[ic_vertex_count + i] = mesh->vertices[i].position;
        ic_colors  [ic_vertex_count + i] = mesh->vertices[i].color;
        ic_uvs     [ic_vertex_count + i] = mesh->vertices[i].uv;
    }
    ic_vertex_count += mesh->vertex_count;

    for( uint i=0; i < mesh->index_count; ++i )
    {
        ic_indices[ic_index_count + i] = mesh->indices[i];
    }
    ic_index_count += mesh->index_count;
}
