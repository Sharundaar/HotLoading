#pragma once

#include <vector>
#include <string>
#include <set>

#include "basic_types.h"
#include "object.h"

struct Resource;
struct ResourceSource;

struct Resource : public Object
{
    GENERATE_BODY( Resource );

    // source
    ResourceSource* source = nullptr;

    // resource data
    bool dirty = false;
    std::string name;

};

struct ResourceSource
{
    std::string file;                               // file that instantiated the associated_resources
    std::set<Resource*> associated_resources;       // resource associated with this source
    std::vector<std::string> errors;                // errors found by reading file, can be populated by multiple sources
};

void setup_resource( Resource* resource, const char* source_file, const char* name );
void clear_resource( Resource* resource );

ResourceSource* get_source( const char* file, bool add_if_needed = true );
ResourceSource* add_source( const char* file );

void remove_resource_from_source( Resource* resource );
void add_resource_to_source( Resource* resource, const char* source_file );
