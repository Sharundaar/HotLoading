#pragma once

#include <string>
#include "basic_types.h"
#include "size.h"
#include "resource.h"
#include "memory_pool.h"

struct Texture : public Resource
{
    GENERATE_BODY( Texture );

    unsigned char* data = nullptr;
    Size size = { 0, 0 };
    i32 channels = 0;

    uint buffer = 0;
};

Texture* find_texture( MemoryPool<Texture>& texture_pool, const char* name );
void upload_texture( Texture* texture );
Texture* load_texture( MemoryPool<Texture>& texture_pool, const std::string& source_file );
void cleanup_texture( Texture& texture );
