#include <SDL.h>
#include <glad/glad.h>
#include <imgui.h>
#include <string>

#include "basics.h"

#define TYPES_IMPLEMENTATION
#include "types.h"

#define MATHLIB_IMPLEMENTATION
#define ML_ASSERT(EXPR, MSG) assert(EXPR, MSG)
#include "mathlib.h"

#include "dll.h"
#include "type_db.h"
#include "object.h"
#include "immediate_mode.h"

Appdata  s_default_appdata = {};
Appdata* s_appdata = nullptr;

void set_dll_appdata( Appdata* appdata )
{
    s_appdata = appdata;
}

Appdata& get_dll_appdata()
{
    if( s_appdata )
        return *s_appdata;
    else
        return s_default_appdata;
}

static void setup_opengl_attributes()
{
    SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5 );

    SDL_GL_SetAttribute( SDL_GL_RED_SIZE,      8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE,     8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE,    8 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE,   16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER,  1 );
}

static void setup_opengl_context( Appdata& appdata )
{
    appdata.sdl_info.opengl_context = SDL_GL_CreateContext( appdata.sdl_info.window );
    SDL_GL_SetSwapInterval( 0 );

    // init GLAD AFTER the GL Context
    print("Initialing GLAD...");
    assert(gladLoadGLLoader( &SDL_GL_GetProcAddress ), "Failed to load GL functions with GLAD.");
    println(" done.");

    println("Vendor: %",   (const char*) glGetString(GL_VENDOR));
    println("Renderer: %", (const char*) glGetString(GL_RENDERER));
    println("Version: %",  (const char*) glGetString(GL_VERSION));

    glViewport( 0, 0, (int) appdata.sdl_info.width, (int) appdata.sdl_info.height );
}

static void create_sdl_window( Appdata& appdata )
{
    std::string title = "Hello, World!";
    appdata.sdl_info.width = 1600;
    appdata.sdl_info.height = 900;

    appdata.sdl_info.window = SDL_CreateWindow( title.c_str(), 
                    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                    (int)appdata.sdl_info.width, (int)appdata.sdl_info.height,
                    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

    assert( appdata.sdl_info.window != nullptr, "Failed to create the window.");
}

static void init_graphics( Appdata& appdata )
{
    // init SDL
    print("Initializing SDL...");
    assert(SDL_Init(SDL_INIT_EVERYTHING) >= 0, format("Failed to init the sdl: %", SDL_GetError()));
    println(" done.");

    setup_opengl_attributes();
    create_sdl_window( appdata );
    setup_opengl_context( appdata );
}

static void reload_metadata( Appdata& appdata )
{
    auto previous_allocator = appdata.metadata.current_allocator;

    if( appdata.metadata.current_allocator == nullptr ) 
        appdata.metadata.current_allocator = &appdata.metadata.allocator1;
    else
        appdata.metadata.current_allocator = appdata.metadata.current_allocator == &appdata.metadata.allocator1 ? &appdata.metadata.allocator2 : &appdata.metadata.allocator1;

    auto current_allocator = appdata.metadata.current_allocator;

    current_allocator->type_size = 0;
    current_allocator->data_size = 0;

    if( current_allocator->type_capacity == 0 )
    {
        const size_t capacity = 0x5000; // @TODO: Allocate according to generated information
        current_allocator->type_buffer   = new uint8_t[capacity];
        current_allocator->type_capacity = capacity; 
    }

    if( current_allocator->data_capacity == 0 )
    {
        const size_t capacity = 0x7000; // @TODO: Allocate according to generated information
        current_allocator->data_buffer   = new uint8_t[capacity];
        current_allocator->data_capacity = capacity; 
    }

    register_types( default_type_allocator, current_allocator, default_data_allocator, current_allocator );

    appdata.metadata.type_infos.clear();
    appdata.metadata.type_infos.resize( (size_t)LocalTypeId::COUNT );
    const auto* type_array = (TypeInfo*) current_allocator->type_buffer;
    for( u32 type_idx=0; type_idx < (u32)LocalTypeId::COUNT; ++type_idx)
    {
        auto type = &type_array[type_idx];
        if( type->type_id != INVALID_TYPE_ID )
            appdata.metadata.type_infos[type->type_id] = type;
    }
}

static void report_types( Appdata& appdata )
{
    println( "Declared types:" );
    for( auto type: appdata.metadata.type_infos )
    {
        println( "    %", type->name );
    }
}

static void init_imgui( Appdata& appdata )
{
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = appdata.sdl_info.width;
    io.DisplaySize.y = appdata.sdl_info.height;
    
    auto imgui_texture = get_texture( "imgui_texture" );

    if( imgui_texture == nullptr )
    {
        imgui_texture = appdata.global_store.texture_pool.Instantiate();
        setup_resource( imgui_texture, "imgui_texture", "imgui_texture" );
    }

    int width, height;
    io.Fonts->GetTexDataAsRGBA32( &imgui_texture->data,
                                &width, 
                                &height );
    imgui_texture->size = { (float)width, (float)height };
    imgui_texture->channels = 4;
    upload_texture( imgui_texture );
    io.Fonts->TexID = (void*)imgui_texture;
    io.KeyMap[ImGuiKey_Tab]        = IK_TAB;
    io.KeyMap[ImGuiKey_LeftArrow]  = IK_LEFTARROW;
    io.KeyMap[ImGuiKey_RightArrow] = IK_RIGHTARROW;
    io.KeyMap[ImGuiKey_UpArrow]    = IK_UPARROW;
    io.KeyMap[ImGuiKey_DownArrow]  = IK_DOWNARROW;
    io.KeyMap[ImGuiKey_PageUp]     = IK_PAGEUP;
    io.KeyMap[ImGuiKey_PageDown]   = IK_PAGEDOWN;
    io.KeyMap[ImGuiKey_Home]       = IK_HOME;
    io.KeyMap[ImGuiKey_End]        = IK_END;
    io.KeyMap[ImGuiKey_Insert]     = IK_INSERT;
    io.KeyMap[ImGuiKey_Delete]     = IK_DELETE;
    io.KeyMap[ImGuiKey_Backspace]  = IK_BACKSPACE;
    io.KeyMap[ImGuiKey_Space]      = IK_SPACE;
    io.KeyMap[ImGuiKey_Enter]      = IK_ENTER;
    io.KeyMap[ImGuiKey_Escape]     = IK_ESCAPE;
    io.KeyMap[ImGuiKey_A]          = IK_A;
    io.KeyMap[ImGuiKey_C]          = IK_C;
    io.KeyMap[ImGuiKey_V]          = IK_V;
    io.KeyMap[ImGuiKey_X]          = IK_X;
    io.KeyMap[ImGuiKey_Y]          = IK_Y;
    io.KeyMap[ImGuiKey_Z]          = IK_Z;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    appdata.imgui_info.texture = imgui_texture;
    appdata.imgui_info.shader = load_shader( "datas/shaders/imgui_shader.glsl" );
}

void reload_dll()
{
    println( "[INFO]: DLL reloaded." );
    auto& appdata = get_dll_appdata();
    if( !appdata.sdl_info.window )
    {
        init_graphics( appdata );

        appdata.test_data.checkerboard_texture = load_texture( "datas/textures/checkerboard.png" );
        appdata.test_data.texture_shader = load_shader( "datas/shaders/transformed_texture.glsl" );
    }
    else
    {
        // this needs to be done on reload since it's loading function pointers
        assert(gladLoadGLLoader( &SDL_GL_GetProcAddress ), "Failed to load GL functions with GLAD.");
    }

    init_imgui( appdata );
    init_immediate();
    reload_metadata( appdata );
    report_types( appdata );
    init_input_state( appdata.input_state );
}

void unload_dll( bool last_time )
{
    auto& appdata = get_dll_appdata();

    ImGui::DestroyContext();
    cleanup_immediate();

    if( last_time )
    {
        if( appdata.sdl_info.window )
        {
            cleanup_texture( *appdata.test_data.checkerboard_texture );
            appdata.test_data.checkerboard_texture = nullptr;
            
            SDL_GL_DeleteContext( appdata.sdl_info.opengl_context );
            SDL_DestroyWindow( appdata.sdl_info.window );
            appdata.sdl_info.window = nullptr;
        }
    }

    println( "[INFO]: DLL unloaded." );
}

static void render_imgui_data( const ImDrawData* draw_data )
{
    auto& imgui_info = get_dll_appdata().imgui_info;
    auto& sdl_info   = get_dll_appdata().sdl_info;

    immediate_clear();

    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        auto cmd_list = draw_data->CmdLists[n];
        const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;  // vertex buffer generated by ImGui
        const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;   // index buffer generated by ImGui
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                immediate_enable_blend( true );
                immediate_enable_depth_test( false );
                immediate_enable_face_cull( false );

                immediate_set_scissor_window( {
                    pcmd->ClipRect.x,
                    sdl_info.height - pcmd->ClipRect.w
                }, 
                {
                    pcmd->ClipRect.z - pcmd->ClipRect.x,
                    pcmd->ClipRect.w - pcmd->ClipRect.y,
                } );

                immediate_set_shader( *imgui_info.shader );
                immediate_set_projection_matrix( Matrix4::OrthographicProjection( sdl_info.width, 0, 0, sdl_info.height, -1, 1 ) );

                // The texture for the draw call is specified by pcmd->TextureId.
                // The vast majority of draw calls with use the imgui texture atlas, which value you have set yourself during initialization.
                immediate_set_texture( (Texture*)pcmd->TextureId );

                // We are using scissoring to clip some objects. All low-level graphics API supports it.
                // If your engine doesn't support scissoring yet, you may ignore this at first. You will get some small glitches
                // (some elements visible outside their bounds) but you can fix that once everywhere else works!
                // MyEngineScissor((int)pcmd->ClipRect.x, (int)pcmd->ClipRect.y, (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));

                // Render 'pcmd->ElemCount/3' indexed triangles.
                // By default the indices ImDrawIdx are 16-bits, you can change them to 32-bits if your engine doesn't support 16-bits indices.
                // MyEngineDrawIndexedTriangles(pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer, vtx_buffer);

                for( uint idx = 0; idx < pcmd->ElemCount; idx += 3 )
                {
                    const auto& vtx1 = vtx_buffer[idx_buffer[idx + 0]];
                    auto& vtx2 = vtx_buffer[idx_buffer[idx + 1]];
                    auto& vtx3 = vtx_buffer[idx_buffer[idx + 2]];

                    immediate_draw_triangle( { vtx1.pos.x, vtx1.pos.y, 0 }, Color::From32ARGB( vtx1.col ), { vtx1.uv.x, vtx1.uv.y },
                                             { vtx2.pos.x, vtx2.pos.y, 0 }, Color::From32ARGB( vtx2.col ), { vtx2.uv.x, vtx2.uv.y }, 
                                             { vtx3.pos.x, vtx3.pos.y, 0 }, Color::From32ARGB( vtx3.col ), { vtx3.uv.x, vtx3.uv.y } );
                }

                immediate_flush();
            }
            idx_buffer += pcmd->ElemCount;
        }
    }

}

void handle_events( InputState& input_state, AppState& app_state )
{
    input_state.mouse_wheel = {};

    SDL_Event evt;
    while( SDL_PollEvent( &evt ) )
    {
        if( handle_inputs( input_state, evt ) )
            continue;

        switch( evt.type )
        {
            case SDL_QUIT:
                app_state.running = false;
                break;
            default:
                break;
        }
    }
}

static std::string string_buf;
void loop_dll( )
{
    auto& appdata = get_dll_appdata();

    appdata.app_state.global_timer.Tick();
    appdata.app_state.global_frame_count++;

    appdata.input_state.frame_start();
    handle_events( appdata.input_state, appdata.app_state );

    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = (float) appdata.app_state.global_timer.Elapsed();
    io.MousePos = { appdata.input_state.mouse_position.x, appdata.input_state.mouse_position.y };
    for(int i=0; i<5; ++i)
        io.MouseDown[i] = appdata.input_state.is_key_down((InputKey)(IK_MOUSE1+i));
    for(int i=0; i<IK_COUNT; ++i)
        io.KeysDown[i] = appdata.input_state.is_key_down( (InputKey)i );
    io.KeyAlt = appdata.input_state.is_key_down( IK_LALT ) || appdata.input_state.is_key_down( IK_RALT );
    io.KeyCtrl = appdata.input_state.is_key_down( IK_LCTRL ) || appdata.input_state.is_key_down( IK_RCTRL );
    io.KeyShift = appdata.input_state.is_key_down( IK_LSHIFT ) || appdata.input_state.is_key_down( IK_RSHIFT );

    io.MouseWheel = (float)appdata.input_state.mouse_wheel.y;
    io.MouseWheelH = (float)appdata.input_state.mouse_wheel.x;

    for( int i=IK_A; i<=IK_Z; ++i )
    {
        if( appdata.input_state.is_key_down_this_frame_or_repeat( (InputKey)i ) )
            io.AddInputCharacter( (char) i-IK_A + 'a' );
    }

    for( int i=IK_0; i<=IK_9; ++i )
    {
        if( appdata.input_state.is_key_down_this_frame_or_repeat( (InputKey)i ) )
            io.AddInputCharacter( (char) i-IK_0 + '0' );
    }

    if( appdata.input_state.is_key_down_this_frame(IK_ESCAPE) )
        appdata.app_state.running = false;
    if( appdata.input_state.is_key_down_this_frame(IK_F9) )
        appdata.app_state.debug_open = !appdata.app_state.debug_open;
    if( appdata.input_state.is_key_down_this_frame(IK_F10) )
        appdata.app_state.demo_window_open = !appdata.app_state.demo_window_open;

    ImGui::NewFrame();

    glDisable( GL_SCISSOR_TEST );

    glClearColor( 0.00f, 1.67f, 0.88f, 1 );
    glClear( GL_COLOR_BUFFER_BIT );

    glClearColor( 0, 0, 0, 0 );
    glClear( GL_DEPTH_BUFFER_BIT );

    immediate_clear();

    immediate_set_view_matrix( Matrix4::RotationTranslation( { 0, 0, 1 } , Quaternion::Identity() ) ); 
    immediate_set_projection_matrix( Matrix4::OpenGLProjectionMatrix( 90.0f, appdata.sdl_info.width / appdata.sdl_info.height, 0.001f, 1000.0f ) );

    immediate_set_texture( appdata.test_data.checkerboard_texture );
    immediate_set_shader( *appdata.test_data.texture_shader );
    immediate_draw_quad( Vector3{ -1, -1, 0 }, Vector2{ 0, 0 },
                         Vector3{ -1,  1, 0 }, Vector2{ 0, 1 },
                         Vector3{  1, -1, 0 }, Vector2{ 1, 0 },
                         Vector3{  1,  1, 0 }, Vector2{ 1, 1 } );
    immediate_enable_face_cull( false );

    immediate_flush();

    if( appdata.app_state.debug_open )
    {
        ImGui::Begin( "Debug", &appdata.app_state.debug_open, ImGuiWindowFlags_NoCollapse );
            const float child_height = 146.0f;
            ImGui::BeginChild( "Types", ImVec2( 0.30f * ImGui::GetWindowWidth(), child_height ) );
                ImGui::Text( "Types: " );
                auto get_selected_type_lambda = [](void* data, int idx, const char** out_text) -> bool {
                    auto type_infos = (const TypeInfo**) data;
                    *out_text = type_infos[idx]->name;
                    return true;
                };
                ImGui::PushItemWidth(-1);
                ImGui::ListBox( "##empty", &appdata.app_state.type_list_current_item, get_selected_type_lambda, appdata.metadata.type_infos.data(), (int)appdata.metadata.type_infos.size() );
            ImGui::EndChild();
            ImGui::SameLine();
            ImGui::BeginChild( "TypeData", ImVec2( 0, child_height ) );
                if( appdata.app_state.type_list_current_item >= 0 && appdata.app_state.type_list_current_item < appdata.metadata.type_infos.size() )
                {
                    auto selected_type = appdata.metadata.type_infos[ appdata.app_state.type_list_current_item ];
                    auto type_name = appdata.metadata.type_infos[type_id<TypeInfoType>()]->enum_info.enum_values[(int)selected_type->type].name;
                    ImGui::Text( "%s %s", type_name, selected_type->name );
                    switch( selected_type->type )
                    {
                        case TypeInfoType::Enum:
                        {
                            ImGui::Text( "    underlying_type: %s", selected_type->enum_info.underlying_type ? selected_type->enum_info.underlying_type->name : "(unknown)" );
                            ImGui::BeginChild( "EnumValues", ImVec2( -36 , 0), true );
                                for( uint enum_idx = 0; enum_idx < selected_type->enum_info.enum_value_count; ++enum_idx )
                                    ImGui::Text( "%i: %s", selected_type->enum_info.enum_values[enum_idx].value, selected_type->enum_info.enum_values[enum_idx].name );
                            ImGui::EndChild();
                            break;
                        }
                        case TypeInfoType::Function:
                            break;
                        case TypeInfoType::Scalar:
                        {
                            const auto& scalar_info = selected_type->scalar_info;
                            auto scalar_type_name = appdata.metadata.type_infos[type_id<ScalarInfoType>()]->enum_info.enum_values[(int)scalar_info.scalar_type].name;
                            ImGui::Text("    kind: %s", scalar_type_name);
                            ImGui::Text("    size: %i", scalar_info.size);
                            break;
                        }
                        case TypeInfoType::Struct:
                        {
                            const auto& struct_info = selected_type->struct_info;
                            ImGui::Text("    parent: %s", struct_info.parent ? struct_info.parent->name : "null");
                            ImGui::Text("    size: %i", struct_info.size);
                            ImGui::BeginChild( "StructFields", ImVec2( -36, 0 ), true );
                                for( uint field_idx = 0; field_idx < struct_info.field_count; ++field_idx )
                                    ImGui::Text( "%s%s%s %s", struct_info.fields[field_idx].modifier & FieldInfoModifier::CONSTANT ? "const " : "", struct_info.fields[field_idx].type ? struct_info.fields[field_idx].type->name : "(unknown)", struct_info.fields[field_idx].modifier & FieldInfoModifier::POINTER ? "*" : "", struct_info.fields[field_idx].name );
                            ImGui::EndChild();
                            break;
                        }
                        case TypeInfoType::TemplateDef:
                        {
                            const auto& templatedef_info = selected_type->template_def_info;
                            ImGui::Text("instances: ");
                            ImGui::BeginChild( "TemplateInstances", ImVec2( -36, 0 ), true );
                                for( uint instance_idx = 0; instance_idx < templatedef_info.instance_count ; ++instance_idx )
                                {
                                    const TemplateInstInfo& inst = templatedef_info.instances[instance_idx];
                                    string_buf.clear();
                                    for( uint param_idx = 0; param_idx < inst.param_count; ++param_idx )
                                        string_buf += inst.params[param_idx].info.type ? inst.params[param_idx].info.type->name : "(unknown) ";
                                    ImGui::Text( "%s", string_buf.c_str() );
                                }
                            ImGui::EndChild();
                            break;
                        }
                        case TypeInfoType::Typedef:
                        {
                            const auto& typedef_info = selected_type->typedef_info;
                            ImGui::Text("    base: %s%s%s", typedef_info.info.modifier & FieldInfoModifier::CONSTANT ? "const " : "", typedef_info.info.type ? typedef_info.info.type->name : "(unknown)", typedef_info.info.modifier & FieldInfoModifier::POINTER ? "*" : "");
                            break;
                        }
                        default:
                            break;
                    }

                }
            ImGui::EndChild();

            ImGui::Text("Frame count: %i", appdata.app_state.global_frame_count);
            ImGui::Text("Frame rate: %f", 1.0 / appdata.app_state.global_timer.Elapsed());

            if(ImGui::Button("Quit")) appdata.app_state.running = false;
        ImGui::End();
    }

    if( appdata.app_state.demo_window_open )
        ImGui::ShowDemoWindow(&appdata.app_state.demo_window_open);

    ImGui::Render();
    render_imgui_data( ImGui::GetDrawData() );

    SDL_GL_SwapWindow( appdata.sdl_info.window );
}

 const TypeInfo* Object::get_type() const { return get_dll_appdata().metadata.type_infos[m_type_id]; }
 