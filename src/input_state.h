#pragma once
#include "mathlib.h"
#include <SDL.h>

enum InputKey
{
    IK_UNKNOWN,

    IK_A,
    IK_B,
    IK_C,
    IK_D,
    IK_E,
    IK_F,
    IK_G,
    IK_H,
    IK_I,
    IK_J,
    IK_K,
    IK_L,
    IK_M,
    IK_N,
    IK_O,
    IK_P,
    IK_Q,
    IK_R,
    IK_S,
    IK_T,
    IK_U,
    IK_V,
    IK_W,
    IK_X,
    IK_Y,
    IK_Z,

    IK_0,
    IK_1,
    IK_2,
    IK_3,
    IK_4,
    IK_5,
    IK_6,
    IK_7,
    IK_8,
    IK_9,

    IK_F1,
    IK_F2,
    IK_F3,
    IK_F4,
    IK_F5,
    IK_F6,
    IK_F7,
    IK_F8,
    IK_F9,
    IK_F10,
    IK_F11,
    IK_F12,

    IK_LSHIFT,
    IK_LCTRL,
    IK_LALT,

    IK_RSHIFT,
    IK_RCTRL,
    IK_RALT,

    IK_LEFTARROW,
    IK_UPARROW,
    IK_RIGHTARROW,
    IK_DOWNARROW,

    IK_HOME,
    IK_END,
    IK_INSERT,
    IK_DELETE,
    IK_PAGEUP,
    IK_PAGEDOWN,

    IK_SPACE,
    IK_ESCAPE,
    IK_ENTER,
    IK_BACKSPACE,
    IK_TAB,

    IK_MOUSE1,
    IK_MOUSE2,
    IK_MOUSE3,
    IK_MOUSE4,
    IK_MOUSE5,

    IK_COUNT
};

struct InputState
{
    Vector2 mouse_position = {};
    Vector2 mouse_wheel = {};

    bool key_down[IK_COUNT]   = {};
    bool key_change[IK_COUNT] = {};
    bool key_repeat[IK_COUNT] = {};

    bool input_text_enabled = false;
    bool input_char_ready = false;
    char input_chars[128];

    bool is_key_down( InputKey key );
    bool is_key_up( InputKey key );
    bool is_key_down_this_frame( InputKey key );
    bool is_key_up_this_frame( InputKey key );
    bool is_key_down_this_frame_or_repeat( InputKey key );

    void enable_text_input( bool enabled );

    void frame_start();
};

void init_input_state( InputState& input_state );

bool handle_inputs( InputState& input_state, const SDL_Event& evt );
InputKey get_inputkey_from_sdl_keycode( SDL_Keycode keycode );
InputKey get_inputkey_from_sdl_scancode( SDL_Scancode scancode );
