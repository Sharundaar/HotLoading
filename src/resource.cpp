#include "resource.h"
#include "dll.h"

#include <unordered_map>
#include <string.h>

std::unordered_map<const char*, ResourceSource*> resource_sources;

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

ResourceSource* add_source( const char* file )
{
    if( resource_sources.find( file ) == resource_sources.cend() )
    {
        auto source = get_dll_appdata().global_store.resource_sources_pool.Instantiate();
        source->file = file;
        resource_sources[ file ] = source;
        return source;
    }

    return resource_sources[ file ];
}

ResourceSource* get_source( const char* file, bool add_if_needed )
{
    auto it = resource_sources.find( file );
    if( it == resource_sources.cend() )
    {
        if( add_if_needed )
            return add_source( file );
        else
            return nullptr;
        
        return resource_sources[ file ];
    }
    return resource_sources[ file ];
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
        source->associated_resources.insert( resource );
        resource->source = source;
    }
}

void remove_resource_from_source( Resource* resource )
{
    if( resource->source != nullptr )
    {
        resource->source->associated_resources.erase( resource );
        resource->source = nullptr;
    }
}
