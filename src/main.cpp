#include <windows.h>

#include "appdata.h"
#include "basics.h"
#include "store.h"

bool dll_need_reload()
{
    return true;
}

void reload_dll()
{

}

int main(int, char**)
{
    Appdata appdata;

    appdata.dll_info.instance = LoadLibrary( (LPCSTR)"HotLoadingDLL.dll" );
    assert( appdata.dll_info.instance != nullptr, "Failed to load HotLoadingDLL." );

    appdata.dll_info.loop_func = GetProcAddress( appdata.dll_info.instance, "loop" );
    assert( appdata.dll_info.loop_func != nullptr, "Failed to load loop function." );

    ((void (*)())(appdata.dll_info.loop_func))();

    return 0;
}