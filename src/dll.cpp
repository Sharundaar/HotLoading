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
    SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5 );

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

static void create_sdl_window( Appdata& appdata )
{
    std::string title = "Hello, World!";
    appdata.sdl_info.width = 1600;
    appdata.sdl_info.height = 900;

    appdata.sdl_info.window = SDL_CreateWindow( title.c_str(), 
                    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                    (int)appdata.sdl_info.width, (int)appdata.sdl_info.height,
                    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

    assert( appdata.sdl_info.window != nullptr, "Failed to create the window.");
}

static void init_graphics( Appdata& appdata )
{
    // init SDL
    print("Initializing SDL...");
    assert(SDL_Init(SDL_INIT_EVERYTHING) >= 0, format("Failed to init the sdl: %", SDL_GetError()));
    println(" done.");

    setup_opengl_attributes();
    create_sdl_window( appdata );
    setup_opengl_context( appdata );
}

static void reload_metadata( Appdata& appdata )
{
    auto previous_allocator = appdata.metadata.current_allocator;

    if( appdata.metadata.current_allocator == nullptr ) 
        appdata.metadata.current_allocator = &appdata.metadata.allocator1;
    else
        appdata.metadata.current_allocator = appdata.metadata.current_allocator == &appdata.metadata.allocator1 ? &appdata.metadata.allocator2 : &appdata.metadata.allocator1;

    auto current_allocator = appdata.metadata.current_allocator;

    current_allocator->type_size = 0;
    current_allocator->data_size = 0;

    if( current_allocator->type_capacity == 0 )
    {
        const size_t capacity = 0x5000; // @TODO: Allocate according to generated information
        current_allocator->type_buffer   = new uint8_t[capacity];
        current_allocator->type_capacity = capacity; 
    }

    if( current_allocator->data_capacity == 0 )
    {
        const size_t capacity = 0x5000; // @TODO: Allocate according to generated information
        current_allocator->data_buffer   = new uint8_t[capacity];
        current_allocator->data_capacity = capacity; 
    }

    register_types( default_type_allocator, current_allocator, default_data_allocator, current_allocator );

    appdata.metadata.type_infos.clear();
    appdata.metadata.type_infos.resize( (size_t)LocalTypeId::COUNT );
    const auto* type_array = (TypeInfo*) current_allocator->type_buffer;
    for( u32 type_idx=0; type_idx < (u32)LocalTypeId::COUNT; ++type_idx)
    {
        auto type = &type_array[type_idx];
        if( type->type_id.local_type != INVALID_TYPE_ID )
            appdata.metadata.type_infos[type->type_id.local_type] = type;
    }
}

static void report_types( Appdata& appdata )
{
    println( "Declared types:" );
    for( auto type: appdata.metadata.type_infos )
    {
        println( "    %", type->name );
    }
}

void reload_dll()
{
    println( "[INFO]: DLL reloaded." );
    auto& appdata = get_dll_appdata();
    if( !appdata.sdl_info.window )
        init_graphics( appdata );
    else
        assert(gladLoadGLLoader( &SDL_GL_GetProcAddress ), "Failed to load GL functions with GLAD.");

    reload_metadata( appdata );
    report_types( appdata );
}

void unload_dll( bool last_time )
{
    auto& appdata = get_dll_appdata();

    if( last_time && appdata.sdl_info.window )
    {
        SDL_GL_DeleteContext( appdata.sdl_info.opengl_context );
        SDL_DestroyWindow( appdata.sdl_info.window );
        appdata.sdl_info.window = nullptr;
    }

    println( "[INFO]: DLL unloaded." );
}

/*
     // Application init
     ImGui::CreateContext();
     ImGuiIO& io = ImGui::GetIO();
     io.DisplaySize.x = 1920.0f;
     io.DisplaySize.y = 1280.0f;
     // TODO: Fill others settings of the io structure later.

     // Load texture atlas (there is a default font so you don't need to care about choosing a font yet)
     unsigned char* pixels;
     int width, height;
     io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
     // TODO: At this points you've got the texture data and you need to upload that your your graphic system:
     MyTexture* texture = MyEngine::CreateTextureFromMemoryPixels(pixels, width, height, TEXTURE_TYPE_RGBA)
     // TODO: Store your texture pointer/identifier (whatever your engine uses) in 'io.Fonts->TexID'. This will be passed back to your via the renderer.
     io.Fonts->TexID = (void*)texture;

     // Application main loop
     while (true)
     {
        // Setup low-level inputs (e.g. on Win32, GetKeyboardState(), or write to those fields from your Windows message loop handlers, etc.)
        ImGuiIO& io = ImGui::GetIO();
        io.DeltaTime = 1.0f/60.0f;
        io.MousePos = mouse_pos;
        io.MouseDown[0] = mouse_button_0;
        io.MouseDown[1] = mouse_button_1;

        // Call NewFrame(), after this point you can use ImGui::* functions anytime
        ImGui::NewFrame();

        // Most of your application code here
        MyGameUpdate(); // may use any ImGui functions, e.g. ImGui::Begin("My window"); ImGui::Text("Hello, world!"); ImGui::End();
        MyGameRender(); // may use any ImGui functions as well!

        // Render & swap video buffers
        ImGui::Render();
        MyImGuiRenderFunction(ImGui::GetDrawData());
        SwapBuffers();
     }

     // Shutdown
     ImGui::DestroyContext();
*/

void loop_dll( )
{
    auto& appdata = get_dll_appdata();

    SDL_Event evt;
    while( SDL_PollEvent( &evt ) )
    {
        switch( evt.type )
        {
            case SDL_KEYDOWN:
            {
                switch( evt.key.keysym.sym )
                {
                    case SDLK_ESCAPE:
                        appdata.running = false;
                        break;
                }
            }
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