#include <string>

#include <windows.h>

#include "appdata.h"
#include "basics.h"
#include "store.h"

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
    assert( appdata.dll_info.instance, "Failed to load HotLoadingDLL_loaded." );

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

    appdata.running = true;
    while( appdata.running )
    {
        if( dll_need_reload( appdata ) ) reload_dll( appdata );
        reinterpret_cast< void(*)() >( appdata.dll_info.loop_func )();
    }

    unload_dll( appdata, true );
    return 0;
}