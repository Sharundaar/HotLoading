#include "texture.h"
#include "file_parser.h"

#include <GLAD/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_ASSERT
#include <stb_image.h>

void upload_texture( Texture* texture )
{
    if( texture->buffer == 0 )
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

Texture* find_texture( MemoryPool<Texture>& texture_pool, const char* name )
{
    for( auto texture : texture_pool )
    {
        if( texture->name == name )
            return texture;
    }
    return nullptr;
}

Texture* load_texture( MemoryPool<Texture>& texture_pool, const std::string& source_file )
{
    char buffer[256];
    extract_file_name( source_file.c_str(), buffer, 256 );
    auto texture = find_texture( texture_pool, buffer );
    if( !texture )
    {
        texture = texture_pool.Instantiate();
        setup_resource( texture, buffer, source_file.c_str() );
    }
    else
    {
        cleanup_texture( *texture );
    }

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

    if( texture.buffer != 0 )
    {
        glDeleteTextures( 1, &texture.buffer );
    }

    texture = {};
}