#pragma once

#include "basics.h"
#include "types.h"
#include "memory_pool.h"

#define MAX_RPOOL_COUNT 8

struct Resource;
struct ResourceHandle;
typedef u32 ResourceId;

struct ResourceHandle
{
    TypeId type;
    ResourceId id;
};

struct ResourcePoolHandle
{
    TypeId type          = INVALID_TYPE_ID;
    MemoryPoolBase* pool = nullptr;

    operator bool();
};

struct ResourcePool
{
    ResourcePoolHandle pools[MAX_RPOOL_COUNT] = {};
};

void init_resource_pool( ResourcePool& resource_pool, TypeId type );
MemoryPoolBase* get_resource_pool( ResourcePool& resource_pool, TypeId type );
Resource* get_resource( ResourceHandle handle );

template<typename T>
void init_resource_pool( ResourcePool& resource_pool )
{
    return init_resource_pool( resource_pool, type_id<T>() );
}

template<typename T>
MemoryPool<T>& get_resource_pool( ResourcePool& resource_pool )
{
    return static_cast<MemoryPool<T>&>( *get_resource_pool( resource_pool, type_id<T>() ) );
}

template<typename T>
T* get_resource( ResourcePool& resource_pool, ResourceHandle handle )
{
    assert( type_id<T>() == handle.type, "Handle and requested type don't match." );
    return static_cast<T*>( get_resource( resource_pool, handle ) );
}