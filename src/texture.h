#pragma once

#include <string>
#include "basic_types.h"
#include "size.h"

struct Texture
{
    std::string source_file;
    unsigned char* data = nullptr;
    Size size = { 0, 0 };
    i32 channels = 0;
};

bool load_texture( const std::string& source_file, Texture& texture );
void cleanup_texture( Texture& texture );
