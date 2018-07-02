#include "resource.h"
#include "dll.h"

#include <unordered_map>
#include <string.h>

void setup_resource( Resource* resource, const char* source_file, const char* name )
{
    resource->name = name;
    resource->loaded = false;

    add_resource_to_source( resource, source_file );
}

void clear_resource( Resource* resource )
{
    remove_resource_from_source( resource );
}

ResourceSource* find_source( MemoryPool<ResourceSource>& pool, const char* source )
{
    for( auto s : pool )
        if ( s->source == source )
            return s;
    return nullptr;
}

ResourceSource* create_source( MemoryPool<ResourceSource>& pool, const char* source )
{
    auto& appdata = get_dll_appdata();
    
    auto resource_source = appdata.global_store.resource_sources_pool.Instantiate();
    resource_source->source = source;
    return resource_source;
}

ResourceSource* get_source( const char* source, bool add_if_needed )
{
    auto& pool = get_dll_appdata().global_store.resource_sources_pool;
    auto resource_source = find_source( pool, source );
    if( !resource_source && add_if_needed )
        resource_source = create_source( pool, source );
    return resource_source;
}

void add_resource_to_source( Resource* resource, const char* source_file )
{
    if( source_file == nullptr )
    {
        resource->source = nullptr;
    }
    else
    {
        auto source = get_source( source_file, true );
        resource->source = source;
    }
}

void remove_resource_from_source( Resource* resource )
{
    if( resource->source != nullptr )
    {
        resource->source = nullptr;
    }
}
