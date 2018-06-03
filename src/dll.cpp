#include <SDL.h>
#include <glad/glad.h>

#include "dll.h"

#include "basics.h"

#define TYPES_IMPLEMENTATION
#include "types.h"
#undef TYPES_IMPLEMENTATION

#include "type_db.h"

Appdata  s_default_appdata = {};
Appdata* s_appdata = nullptr;

void set_dll_appdata( Appdata* appdata )
{
    s_appdata = appdata;
}

Appdata& get_dll_appdata()
{
    if( s_appdata )
        return *s_appdata;
    else
        return s_default_appdata;
}

static void setup_opengl_attributes()
{
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

    SDL_GL_SetAttribute( SDL_GL_RED_SIZE,      8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE,     8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE,    8 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE,   16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER,  1 );
}

static void setup_opengl_context( Appdata& appdata )
{
    appdata.sdl_info.opengl_context = SDL_GL_CreateContext( appdata.sdl_info.window );
    SDL_GL_SetSwapInterval( 0 );

    // init GLAD AFTER the GL Context
    print("Initialing GLAD...");
    assert(gladLoadGLLoader( &SDL_GL_GetProcAddress ), "Failed to load GL functions with GLAD.");
    println(" done.");

    println("Vendor: %",   (const char*) glGetString(GL_VENDOR));
    println("Renderer: %", (const char*) glGetString(GL_RENDERER));
    println("Version: %",  (const char*) glGetString(GL_VERSION));

    glViewport( 0, 0, (int) appdata.sdl_info.width, (int) appdata.sdl_info.height );
    glEnable( GL_DEPTH_TEST );
}

void reload()
{
    setup_opengl_attributes();
    setup_opengl_context( get_dll_appdata() );
}

void loop( )
{
    auto& appdata = get_dll_appdata();

    SDL_Event evt;
    while( SDL_PollEvent( &evt ) )
    {
        switch( evt.type )
        {
            case SDL_QUIT:
                appdata.running = false;
                break;
            default:
                break;
        }
    }

    glClearColor( 0.00f, 1.67f, 0.88f, 1 );
    glClear( GL_COLOR_BUFFER_BIT );

    glClearColor( 0, 0, 0, 0 );
    glClear( GL_DEPTH_BUFFER_BIT );

    SDL_GL_SwapWindow( appdata.sdl_info.window );
}

TypeStore get_dll_types()
{
    TypeStore store = {};

    u32 capacity = 0x7000;
    store.buffer = new u8[capacity];
    store.buffer_capacity = capacity;

    auto alloc_type = []( void* param ) -> TypeInfo& {
        auto store = (TypeStore*)param;
        assert( store->buffer_size + sizeof(TypeInfo) <= store->buffer_capacity, "store capacity is too low." );
        auto alloc = new (store->buffer + store->buffer_size) TypeInfo();
        store->buffer_size += sizeof(TypeInfo);
        store->type_count++;
        return *alloc;
    };

    auto alloc_data = []( void* param, uint32_t size ) -> void* {
        auto store = (TypeStore*)param;
        assert( store->buffer_size + size <= store->buffer_capacity, "store capacity is too low." );
        auto alloc = (void*)(store->buffer + store->buffer_size);
        store->buffer_size += size;
        return alloc;
    };

    register_types( alloc_type, &store, alloc_data, &store );

    store.all_types = new const TypeInfo*[(u32)LocalTypeId::COUNT];
    for(u32 idx = 0; idx < store.type_count; ++idx)
    {
        auto type = (TypeInfo*) (store.buffer+(idx*sizeof(TypeInfo)));
        if( type->type_id.local_type != INVALID_TYPE_ID )
            store.all_types[type->type_id.local_type] = type;
    }

    return store;
}