#include "mesh.h"

#include "basics.h"
#include "dll.h"

/* assimp include files. These three are usually needed. */
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static const struct aiMesh* find_first_mesh( const struct aiScene* scene, const struct aiNode* node )
{
    if( node == nullptr )
        return nullptr;

    if( node->mNumMeshes != 0 )
    {
        return scene->mMeshes[ node->mMeshes[0] ];
    }
    else
    {
        for( uint i=0; i<node->mNumChildren; ++i )
        {
            auto mesh = find_first_mesh( scene, node->mChildren[i] );
            if( mesh != nullptr )
                return mesh;
        }
        return nullptr;
    }
}

MeshDef make_meshdef( uint vertex_count, uint index_count )
{
    MeshDef def;
    def.vertices = new Vertex[vertex_count];
    def.indices  = new uint[index_count];

    assert_fmt( def.vertices != nullptr, "Failed to allocate % vertices.", vertex_count);
    assert_fmt( def.indices  != nullptr, "Failed to allocate % indices.", index_count);

    def.vertex_count = vertex_count;
    def.index_count  = index_count;

    return def;

}

void destroy_meshdef( MeshDef* meshdef )
{
    delete[] meshdef->indices;
    delete[] meshdef->vertices;

    meshdef->indices = nullptr;
    meshdef->index_count = 0;
    meshdef->vertices = nullptr;
    meshdef->vertex_count = 0;

    clear_resource( meshdef );
}

MeshDef* load_mesh_from_data( const char* name, const std::vector<Vertex>& vertices, const std::vector<uint>& indices )
{
    MeshDef* def = get_dll_appdata().global_store.mesh_pool.Instantiate();
    *def = make_meshdef( (uint)vertices.size(), (uint)indices.size() );
    setup_resource( def, nullptr, name );

    memcpy( def->vertices, vertices.data(), vertices.size() * sizeof(Vertex) );
    memcpy( def->indices, indices.data(), indices.size() * sizeof(uint) );

    return def;
}

MeshDef* load_mesh( const char* file_path )
{
    const struct aiScene* scene = aiImportFile( file_path, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate | aiProcess_MakeLeftHanded );

    if( scene == nullptr )
    {
        println("Error: Couldn't load file %", file_path);
        return nullptr;
    }

    // @TODO: For now we're only going to load the first mesh we find for conveniency... Maybe return a list of MeshDef at one point ?
    const struct aiMesh* mesh = find_first_mesh( scene, scene->mRootNode );
    if( mesh == nullptr )
    {
        println("Error: No mesh found in file %", file_path);
        return nullptr;
    }

    MeshDef* def = get_dll_appdata().global_store.mesh_pool.Instantiate();
    *def = make_meshdef( mesh->mNumVertices, mesh->mNumFaces * 3 );
    setup_resource( def, file_path, mesh->mName.C_Str() );

    auto blender_adaption_matrix = Matrix4::RotationMatrix( 90, Vector3::Right() );

    for( uint vertex = 0; vertex < mesh->mNumVertices; ++vertex )
    {
        def->vertices[vertex].position = { mesh->mVertices[vertex].x, mesh->mVertices[vertex].y, mesh->mVertices[vertex].z };
        def->vertices[vertex].position = blender_adaption_matrix.Apply( def->vertices[vertex].position );
        if( mesh->mColors[0] != nullptr )
            def->vertices[vertex].color = { mesh->mColors[0][vertex][0], mesh->mColors[0][vertex][1], mesh->mColors[0][vertex][2], mesh->mColors[0][vertex][3] };
        if( mesh->mNormals != nullptr )
            def->vertices[vertex].normal = { mesh->mNormals[vertex].x, mesh->mNormals[vertex].y, mesh->mNormals[vertex].z };
    }

    uint index  = 0;
    for( uint i=0; i<mesh->mNumFaces; ++i )
    {
        const struct aiFace& face = mesh->mFaces[i];
        assert( face.mNumIndices == 3, "Only support triangle or polygon faces." );
        for( uint j=0; j<3; ++j )
        {
            def->indices[index] = face.mIndices[j];
            ++index;
        }
    }

    return def;
}

