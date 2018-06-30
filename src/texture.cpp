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

Texture* instantiate_empty_texture( MemoryPool<Texture>& texture_pool )
{
    return texture_pool.Instantiate();
}

void upload_texture( Texture* texture )
{
    glGenTextures( 1, &texture->buffer );

    glBindTexture( GL_TEXTURE_2D, texture->buffer );

    uint tex_format;
    switch( texture->channels )
    {
        case 3:
            tex_format = GL_RGB;
            break;
        case 4:
            tex_format = GL_RGBA;
            break;
        default:
            tex_format = GL_RGB;
            break;
    }
    glTexImage2D( GL_TEXTURE_2D, 0, tex_format, (GLsizei) texture->size.width, (GLsizei) texture->size.height, 0, tex_format, GL_UNSIGNED_BYTE, texture->data );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );

    glGenerateMipmap( GL_TEXTURE_2D );
}

Texture* get_texture( MemoryPool<Texture>& texture_pool, const char* source_file )
{
    for( auto texture : texture_pool )
    {
        if( texture->source->file == source_file )
            return texture;
    }
    return nullptr;
}

Texture* load_texture( MemoryPool<Texture>& texture_pool, const std::string& source_file )
{
    auto texture = instantiate_empty_texture( texture_pool );
    setup_resource( texture, source_file.c_str(), source_file.c_str() );

    i32 width, height, channels;
    texture->data = stbi_load(source_file.c_str(), &width, &height, &channels, 0);
    texture->size = {
        (f32) width,
        (f32) height
    };
    texture->channels = channels;
    upload_texture( texture );

    return texture;
}

void cleanup_texture( Texture& texture )
{
    if( texture.data != nullptr )
    {
        stbi_image_free( texture.data );
    }
}