#pragma once

#include <SDL.h>
#include <windows.h>
#include <vector>
#include "store.h"

struct Appdata;
struct DLLInfo;

struct DLLInfo
{
    HINSTANCE instance  = nullptr;

    FARPROC   loop_func            = nullptr;
    FARPROC   set_dll_appdata_func = nullptr;
    FARPROC   reload_func          = nullptr;
    FARPROC   unload_func          = nullptr;

    FILETIME  last_dll_write_time;
    FILETIME  last_pdb_write_time;

    u32 reload_count = 0;
};

struct SDLInfo
{
    SDL_Window* window = nullptr;
    SDL_GLContext opengl_context = nullptr;
    float width  = 0;
    float height = 0;
};

struct Metadata
{
    TypeDefaultAllocator allocator1 = {};
    TypeDefaultAllocator allocator2 = {};

    TypeDefaultAllocator* current_allocator = nullptr;

    std::vector<const TypeInfo*> type_infos = {};
};

struct Appdata
{
    TypeStore type_store = {};
    DLLInfo   dll_info = {};
    SDLInfo   sdl_info = {};
    Metadata  metadata = {};

    bool      running = false;
};