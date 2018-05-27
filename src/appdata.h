#pragma once

#include <windows.h>
#include "store.h"

struct Appdata;
struct DLLInfo;

struct DLLInfo
{
    HINSTANCE instance;
    FARPROC   loop_func;
};

struct Appdata
{
    ~Appdata()
    {
        cleanup_type_store( this->type_store );
    }

    TypeStore type_store = {};
    DLLInfo dll_info;
};