#pragma once

#include "mathlib.h"
#include "texture.h"
#include "shader.h"
#include "mesh.h"
#include "variant_type.h"


void init_immediate();
void cleanup_immediate();

// API for other systems
void immediate_clear();
void immediate_flush();

void immediate_set_default_shader( Shader* shader );
void immediate_set_shader       ( const Shader& shader );
void immediate_set_world_matrix ( const Matrix4& w );
void immediate_set_view_matrix  ( const Matrix4& v );
void immediate_set_projection_matrix( const Matrix4& p );
void immediate_set_texture( const Texture* texture );
void immediate_set_draw_type    ( uint type );
void immediate_set_depth        ( float depth );
void immediate_set_material     ( const Material* material );

void immediate_set_custom_param_value( const char* param_name, Variant value );

void immediate_enable_depth_test( bool enabled );
void immediate_enable_blend( bool enabled );
void immediate_enable_face_cull( bool enabled );

void immediate_set_scissor_window( Vector2 pos, Size size );

void immediate_draw_line(  const Vector3& p1, const Color& c1,
                            const Vector3& p2, const Color& c2);

void immediate_draw_triangle(  const Vector3& p1, const Color& c1,
                                const Vector3& p2, const Color& c2,
                                const Vector3& p3, const Color& c3 );

void immediate_draw_triangle(  const Vector3& p1, const Color& c1, const Vector2& uv1,
                                const Vector3& p2, const Color& c2, const Vector2& uv2,
                                const Vector3& p3, const Color& c3, const Vector2& uv3 );

void immediate_draw_quad(  const Vector3& p1, const Vector2& uv1,
                            const Vector3& p2, const Vector2& uv2,
                            const Vector3& p3, const Vector2& uv3,
                            const Vector3& p4, const Vector2& uv4 );

void immediate_draw_quad(  const Vector3& p1, const Color& c1,
                            const Vector3& p2, const Color& c2,
                            const Vector3& p3, const Color& c3,
                            const Vector3& p4, const Color& c4 );

void immediate_draw_mesh( const MeshDef* mesh );