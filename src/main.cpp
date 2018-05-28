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

    appdata.dll_info.reload_count++;

    reinterpret_cast< void(*)( Appdata* appdata ) >( appdata.dll_info.set_dll_appdata_func )( &appdata );
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

    return 0;
}