#pragma once

#include <windows.h>
#include "store.h"

struct Appdata;
struct DLLInfo;

struct DLLInfo
{
    HINSTANCE instance  = nullptr;
    FARPROC   loop_func = nullptr;
    FARPROC   set_dll_appdata_func = nullptr;

    FILETIME  last_dll_write_time;
    FILETIME  last_pdb_write_time;

    u32 reload_count = 0;
};

struct Appdata
{
    TypeStore type_store = {};
    DLLInfo   dll_info = {};
    bool      running = false;
};