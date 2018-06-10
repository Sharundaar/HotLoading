#pragma once

#define DLLEXPORT __declspec(dllexport)

#include <windows.h>
#include "appdata.h"

extern "C" 
{
    DLLEXPORT void      set_dll_appdata( Appdata* appdata );

    DLLEXPORT void loop_dll();   // called reapetedly
    DLLEXPORT void reload_dll(); // called after reload
    DLLEXPORT void unload_dll( bool last_time ); // called before reload
}

Appdata& get_dll_appdata();