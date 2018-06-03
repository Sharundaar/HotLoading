#include <string>

#include <windows.h>

#include <SDL.h>
#include <GLAD/glad.h>

#include "appdata.h"
#include "basics.h"
#include "store.h"

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

void init_graphics( Appdata& appdata )
{
    // init SDL
    print("Initializing SDL...");
    assert(SDL_Init(SDL_INIT_EVERYTHING) >= 0, format("Failed to init the sdl: %", SDL_GetError()));
    println(" done.");

    setup_opengl_attributes();
    
    create_sdl_window( appdata );

    setup_opengl_context( appdata );
}

void cleanup_graphics( Appdata& appdata )
{
    SDL_GL_DeleteContext( appdata.sdl_info.opengl_context );
    SDL_DestroyWindow( appdata.sdl_info.window );
}

struct RAIIHandle
{
    ~RAIIHandle() { if( handle ) CloseHandle( handle ); }
    HANDLE handle = NULL;

    operator bool() { return handle != NULL; }
    operator HANDLE() { return handle; }
};

bool dll_need_reload( Appdata& appdata )
{
    if( appdata.dll_info.instance == nullptr )
        return true;

    FILETIME last_dll_write_time;
    FILETIME last_pdb_write_time;

    {
        RAIIHandle dll_handle = { CreateFileW( 
            L"HotLoadingDLL.dll",
            GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        ) };

        if( !dll_handle )
            return false;

        RAIIHandle pdb_handle = { CreateFileW( 
            L"HotLoadingDLL.pdb",
            GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        ) };

        if( !pdb_handle )
            return false;

        if( GetFileTime( dll_handle, nullptr, nullptr, &last_dll_write_time ) == 0 )
            return false;
        if( GetFileTime( pdb_handle, nullptr, nullptr, &last_pdb_write_time ) == 0 )
            return false;
    }

    if( CompareFileTime( &appdata.dll_info.last_dll_write_time, &last_dll_write_time ) >= 0 )
        return false;

    if( CompareFileTime( &appdata.dll_info.last_pdb_write_time, &last_pdb_write_time ) >= 0 )
        return false;
    
    appdata.dll_info.last_dll_write_time = last_dll_write_time;
    appdata.dll_info.last_pdb_write_time = last_pdb_write_time;

    return true;
}

void reload_dll( Appdata& appdata )
{
    // cleanup old instance
    if( appdata.dll_info.instance != nullptr )
    {
        FreeLibrary( appdata.dll_info.instance );
        appdata.dll_info.instance = nullptr;
    }

    CopyFileW( L"HotLoadingDLL.dll", L"HotLoadingDLL_loaded.dll", false );
    CopyFileW( L"HotLoadingDLL.pdb", L"HotLoadingDLL_loaded.pdb", false );

    appdata.dll_info.instance = LoadLibraryW( L"HotLoadingDLL_loaded.dll" );
    assert( appdata.dll_info.instance != nullptr, "Failed to load HotLoadingDLL_loaded." );

    appdata.dll_info.loop_func = GetProcAddress( appdata.dll_info.instance, "loop" );
    assert( appdata.dll_info.loop_func != nullptr, "Failed to load loop function." );

    appdata.dll_info.set_dll_appdata_func = GetProcAddress( appdata.dll_info.instance, "set_dll_appdata" );
    assert( appdata.dll_info.set_dll_appdata_func != nullptr, "Failed to load set_dll_appdata function." );

    appdata.dll_info.reload_func = GetProcAddress( appdata.dll_info.instance, "reload" );
    assert( appdata.dll_info.reload_func != nullptr, "Failed to load reload function." );

    appdata.dll_info.reload_count++;

    reinterpret_cast< void(*)( Appdata* appdata ) >( appdata.dll_info.set_dll_appdata_func )( &appdata );
    reinterpret_cast< void(*)() >( appdata.dll_info.reload_func )();
}

int main(int, char**)
{
    Appdata appdata;

    init_graphics( appdata );

    appdata.running = true;
    while( appdata.running )
    {
        if( dll_need_reload( appdata ) ) reload_dll( appdata );
        reinterpret_cast< void(*)() >( appdata.dll_info.loop_func )();
    }

    cleanup_graphics( appdata );
    return 0;
}