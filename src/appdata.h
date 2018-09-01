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
#include "timer.h"
#include "input_state.h"
#include "resource_pool.h"
#include "entity.h"

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
    ResourcePool               resource_pool = {};
};

struct ImguiInfo
{
    Texture* texture = nullptr;
    Shader* shader  = nullptr;
};

struct TestData
{
    Texture* checkerboard_texture = nullptr;
    Texture* flower_texture = nullptr;
    Shader* texture_shader = nullptr;
    Shader* mix_texture_shader = nullptr;

    Entity checkerboard_entity = {};

    float mix_amount = 0.5f;

    std::string some_name;
};

struct AppState
{
    bool debug_open = true;
    bool type_info_open = false;
    bool demo_window_open = false;
    bool inspector_open = false;

    bool running = false;
    
    int type_list_current_item = 0;

    Timer global_timer = {};
    int   global_frame_count = 0;

    Shader* immediate_default_shader = nullptr;
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