#pragma once

#include <windows.h>
#include "store.h"

struct Appdata;
struct DLLInfo;

struct DLLInfo
{
    HINSTANCE instance  = nullptr;
    FARPROC   loop_func = nullptr;
    FILETIME  last_write_time;

    u32 reload_count = 0;
};

struct Appdata
{
    TypeStore type_store = {};
    DLLInfo   dll_info = {};
    bool      running = false;
};