#pragma once

#include "resource.h"
#include "mathlib.h"

struct Vertex 
{
    Vector3 position;
    Color   color;
    Vector3 normal;
    Vector2 uv;
};

struct MeshDef : public Resource
{
    GENERATE_BODY( MeshDef );

    Vertex* vertices     = nullptr;
    uint    vertex_count = 0;

    uint*   indices     = nullptr;
    uint    index_count = 0;
};

MeshDef  make_meshdef( uint vertex_count, uint index_count );
void     destroy_meshdef( MeshDef* meshdef );

MeshDef* load_mesh_from_data( const char* name, const std::vector<Vertex>& vertices, const std::vector<uint>& indices );
MeshDef* load_mesh( const char* file );


/* @Improvements: Might reuse this if we want to pack things better...
struct Vertex {};

struct SimpleVertex : public Vertex
{
    Vector3 position;
    Color   color;
    Vector3 normal;
    Vector2 uv;
};

struct PosVertex : public Vertex
{
    Vector3 position;
};

struct PosColVertex : public Vertex
{
    Vector3 position;
    Color   color;
};

enum class VertexKind
{
    SimpleVertex,
};
*/