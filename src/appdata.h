#pragma once

#include <SDL.h>
#include <windows.h>
#include <vector>

#include "mathlib.h"
#include "memory_pool.h"
#include "basic_types.h"
#include "types.h"
#include "resource.h"
#include "shader.h"
#include "mesh.h"
#include "texture.h"

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

struct GlobalStore
{
    MemoryPool<ResourceSource> resource_sources_pool = {};
    MemoryPool<Shader>         shader_pool = {};
    MemoryPool<MaterialDef>    material_pool = {};
    MemoryPool<MeshDef>        mesh_pool = {};
    MemoryPool<Texture>        texture_pool = {};
};

struct InputState
{
    Vector2 mouse_position = {};
    bool lmouse_down = false;
    bool rmouse_down = false;
};

struct ImguiInfo
{
    Texture* texture = {};
    Shader* shader  = nullptr;
};

struct TestData
{
    Texture* checkerboard_texture = nullptr;
    Shader* texture_shader = nullptr;
};

struct AppState
{
    bool debug_open = false;
    bool running = false;
    
    int type_list_current_item = 0;
};

struct Appdata
{
    DLLInfo   dll_info = {};
    SDLInfo   sdl_info = {};
    ImguiInfo imgui_info = {};
    Metadata  metadata = {};

    GlobalStore global_store = {};
    InputState  input_state = {};

    TestData    test_data = {};
    AppState    app_state = {};
};