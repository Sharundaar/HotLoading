#pragma once

#define DLLEXPORT __declspec(dllexport)

#include <windows.h>
#include "appdata.h"

extern "C" 
{
    DLLEXPORT void      set_dll_appdata( Appdata* appdata );
    DLLEXPORT Appdata&  get_dll_appdata();
    DLLEXPORT TypeStore get_dll_types();

    DLLEXPORT void loop( Appdata& appdata );
}
