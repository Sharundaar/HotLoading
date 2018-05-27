#include <windows.h>

#include "appdata.h"
#include "basics.h"
#include "store.h"

bool dll_need_reload( Appdata& appdata )
{
    if( appdata.dll_info.instance == nullptr )
        return true;

    HANDLE dll_handle = CreateFile( 
        "HotLoadingDLL.dll",
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if( dll_handle == nullptr)
        return false;

    FILETIME last_write_time;
    if( GetFileTime( dll_handle, nullptr, nullptr, &last_write_time ) == 0 )
        return false;

    CloseHandle( dll_handle );
    dll_handle = nullptr;

    if( CompareFileTime( &appdata.dll_info.last_write_time, &last_write_time ) >= 0 )
        return false;
    
    appdata.dll_info.last_write_time = last_write_time;

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

    CopyFile( "HotLoadingDLL.dll", "HotLoadingDLL_loaded.dll", false );
    CopyFile( "HotLoadingDLL.pdb", "HotLoadingDLL_loaded.pdb", false );

    appdata.dll_info.instance = LoadLibrary( (LPCSTR)"HotLoadingDLL_loaded.dll" );
    assert( appdata.dll_info.instance != nullptr, "Failed to load HotLoadingDLL_loaded." );

    appdata.dll_info.loop_func = GetProcAddress( appdata.dll_info.instance, "loop" );
    assert( appdata.dll_info.loop_func != nullptr, "Failed to load loop function." );

    appdata.dll_info.reload_count++;
}

int main(int, char**)
{
    Appdata appdata;

    appdata.running = true;
    while( appdata.running )
    {
        if( dll_need_reload( appdata ) ) reload_dll( appdata );

        ((void (*)(Appdata&))(appdata.dll_info.loop_func))( appdata );
    }


    return 0;
}