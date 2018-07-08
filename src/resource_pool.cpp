#include "resource_pool.h"

#include "type_db.h"
#include "dll.h"

static MemoryPoolBase* create_pool( TypeId type )
{
    switch( (LocalTypeId)type )
    {
        case LocalTypeId::Shader_id:
            return new MemoryPool<Shader>();
        case LocalTypeId::MeshDef_id:
            return new MemoryPool<MeshDef>();
        case LocalTypeId::Texture_id:
            return new MemoryPool<Texture>();
    }

    assert_fmt( false, "Can't create pool from % type_id", type );
    return nullptr;
}

void init_resource_pool( ResourcePool& resource_pool, TypeId type )
{
    const Metadata& metadata = get_dll_appdata().metadata;

    const TypeInfo* resource_type = metadata.type_infos[type];

    // check that type inherit Resource
    const TypeInfo* resource_check = resource_type;
    if( resource_check->type != TypeInfoType::Struct )
        resource_check = nullptr;
    while( resource_check && strcmp( resource_check->name, "Resource" ) != 0 )
        resource_check = resource_check->struct_info.parent;
    assert_fmt( resource_check, "Type % is not a Resource.", resource_type->name );

    int pool_index = 0;
    while( pool_index < MAX_RPOOL_COUNT && resource_pool.pools[pool_index] ) 
        pool_index++;

    resource_pool.pools[pool_index] = { type, create_pool( type ) };
}

MemoryPoolBase* get_resource_pool( ResourcePool& resource_pool, TypeId type )
{
    uint pool_index = 0;
    while( pool_index < MAX_RPOOL_COUNT && resource_pool.pools[pool_index] && resource_pool.pools[pool_index].type != type ) 
        pool_index++;

    if( pool_index < MAX_RPOOL_COUNT )
        return resource_pool.pools[pool_index].pool;
    else
        return nullptr;
}

Resource* get_resource( ResourcePool& resource_pool, ResourceHandle handle )
{
    auto pool = get_resource_pool( resource_pool, handle.type );
    return static_cast<Resource*>( pool->GetRaw( handle.id ) );
}

ResourcePoolHandle::operator bool()
{
    return type != INVALID_TYPE_ID && pool != nullptr;
}