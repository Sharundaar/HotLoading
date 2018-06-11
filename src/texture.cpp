#include "texture.h"

#include <GLAD/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_ASSERT
#include <stb_image.h>

Texture init_texture()
{
    Texture tex;
    return tex;
}

bool load_texture( const std::string& source_file, Texture& texture )
{
    i32 width, height, channels;
    texture.data = stbi_load(source_file.c_str(), &width, &height, &channels, 0);
    texture.source_file = source_file;
    texture.size = {
        (f32) width,
        (f32) height
    };
    texture.channels = channels;
    
    return true;
}

void cleanup_texture( Texture& texture )
{
    if( texture.data != nullptr )
    {
        stbi_image_free( texture.data );
    }
}