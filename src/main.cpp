#include <string>

#include <windows.h>

#define ML_ASSERT( COND, MSG )
#define MATHLIB_IMPLEMENTATION
#define TYPES_IMPLEMENTATION // @Todo: Remove that when appdata is entirely in the DLL
#include "appdata.h"
#include "basics.h"

struct RAIIHandle
{
    ~RAIIHandle() { if( handle ) CloseHandle( handle ); }
    HANDLE handle = NULL;

    operator bool() { return handle != NULL; }
    operator HANDLE() { return handle; }
};

bool dll_need_reload( Appdata& appdata )
{
    FILETIME last_dll_write_time;
    FILETIME last_pdb_write_time;

    {
        WIN32_FILE_ATTRIBUTE_DATA dll_info;
        WIN32_FILE_ATTRIBUTE_DATA pdb_info;

        if( !GetFileAttributesExW( L"HotLoadingDLL.dll", GetFileExInfoStandard, &dll_info ) )
            return false;

        if( !GetFileAttributesExW( L"HotLoadingDLL.pdb", GetFileExInfoStandard, &pdb_info ) )
            return false;

        last_dll_write_time = dll_info.ftLastWriteTime;
        last_pdb_write_time = pdb_info.ftLastWriteTime;
    }

    if( appdata.dll_info.instance != nullptr )
    {
        if( CompareFileTime( &appdata.dll_info.last_dll_write_time, &last_dll_write_time ) >= 0 )
            return false;

        if( CompareFileTime( &appdata.dll_info.last_pdb_write_time, &last_pdb_write_time ) >= 0 )
            return false;
    }
    
    appdata.dll_info.last_dll_write_time = last_dll_write_time;
    appdata.dll_info.last_pdb_write_time = last_pdb_write_time;

    return true;
}

void unload_dll( Appdata& appdata, bool last_time )
{
    if( appdata.dll_info.instance != nullptr )
    {
        reinterpret_cast< void(*)(bool) >( appdata.dll_info.unload_func )( last_time );
        FreeLibrary( appdata.dll_info.instance );
        appdata.dll_info.instance = nullptr;
    }
}

void reload_dll( Appdata& appdata )
{
    // cleanup old instance
    unload_dll( appdata, false );

    CopyFileW( L"HotLoadingDLL.dll", L"HotLoadingDLL_loaded.dll", false );
    CopyFileW( L"HotLoadingDLL.pdb", L"HotLoadingDLL_loaded.pdb", false );

    appdata.dll_info.instance = LoadLibraryW( L"HotLoadingDLL_loaded.dll" );
    assert_fmt( appdata.dll_info.instance, "Failed to load HotLoadingDLL_loaded. ( error code: % )", GetLastError() );

    appdata.dll_info.loop_func = GetProcAddress( appdata.dll_info.instance, "loop_dll" );
    assert( appdata.dll_info.loop_func, "Failed to load loop_dll function." );

    appdata.dll_info.set_dll_appdata_func = GetProcAddress( appdata.dll_info.instance, "set_dll_appdata" );
    assert( appdata.dll_info.set_dll_appdata_func, "Failed to load set_dll_appdata function." );

    appdata.dll_info.reload_func = GetProcAddress( appdata.dll_info.instance, "reload_dll" );
    assert( appdata.dll_info.reload_func, "Failed to load reload function." );

    appdata.dll_info.unload_func = GetProcAddress( appdata.dll_info.instance, "unload_dll" );
    assert( appdata.dll_info.unload_func, "Failed to load unload_dll function." );

    appdata.dll_info.reload_count++;

    reinterpret_cast< void(*)( Appdata* appdata ) >( appdata.dll_info.set_dll_appdata_func )( &appdata );
    reinterpret_cast< void(*)() >( appdata.dll_info.reload_func )();
}

int main(int, char**)
{
    Appdata appdata;

    appdata.app_state.running = true;
    while( appdata.app_state.running )
    {
        if( dll_need_reload( appdata ) ) reload_dll( appdata );
        reinterpret_cast< void(*)() >( appdata.dll_info.loop_func )();
    }

    unload_dll( appdata, true );
    return 0;
}