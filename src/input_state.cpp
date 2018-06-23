#include "input_state.h"

#include "basics.h"
#include <cstring>

InputKey sdlscancode_to_inputkey_map[SDL_NUM_SCANCODES] = {
    IK_UNKNOWN, // SDL_SCANCODE_UNKNOWN
    IK_UNKNOWN,
    IK_UNKNOWN,
    IK_UNKNOWN,
    IK_A,       // SDL_SCANCODE_A
    IK_B,       // SDL_SCANCODE_B
    IK_C,       // SDL_SCANCODE_C
    IK_D,       // SDL_SCANCODE_D
    IK_E,       // SDL_SCANCODE_E
    IK_F,       // SDL_SCANCODE_F
    IK_G,       // SDL_SCANCODE_G
    IK_H,       // SDL_SCANCODE_H
    IK_I,       // SDL_SCANCODE_I
    IK_J,       // SDL_SCANCODE_J
    IK_K,       // SDL_SCANCODE_K
    IK_L,       // SDL_SCANCODE_L
    IK_M,       // SDL_SCANCODE_M
    IK_N,       // SDL_SCANCODE_N
    IK_O,       // SDL_SCANCODE_O
    IK_P,       // SDL_SCANCODE_P
    IK_Q,       // SDL_SCANCODE_Q
    IK_R,       // SDL_SCANCODE_R
    IK_S,       // SDL_SCANCODE_S
    IK_T,       // SDL_SCANCODE_T
    IK_U,       // SDL_SCANCODE_U
    IK_V,       // SDL_SCANCODE_V
    IK_W,       // SDL_SCANCODE_W
    IK_X,       // SDL_SCANCODE_X
    IK_Y,       // SDL_SCANCODE_Y
    IK_Z,       // SDL_SCANCODE_Z

    IK_1, // SDL_SCANCODE_1
    IK_2, // SDL_SCANCODE_2
    IK_3, // SDL_SCANCODE_3
    IK_4, // SDL_SCANCODE_4
    IK_5, // SDL_SCANCODE_5
    IK_6, // SDL_SCANCODE_6
    IK_7, // SDL_SCANCODE_7
    IK_8, // SDL_SCANCODE_8
    IK_9, // SDL_SCANCODE_9
    IK_0, // SDL_SCANCODE_0

    IK_UNKNOWN,   // SDL_SCANCODE_RETURN
    IK_ESCAPE,    // SDL_SCANCODE_ESCAPE
    IK_BACKSPACE, // SDL_SCANCODE_BACKSPACE
    IK_TAB,       // SDL_SCANCODE_TAB
    IK_SPACE,     // SDL_SCANCODE_SPACE

    IK_UNKNOWN,  // SDL_SCANCODE_MINUS
    IK_UNKNOWN,  // SDL_SCANCODE_EQUALS
    IK_UNKNOWN,  // SDL_SCANCODE_LEFTBRACKET
    IK_UNKNOWN,  // SDL_SCANCODE_RIGHTBRACKET
    IK_UNKNOWN,  // SDL_SCANCODE_BACKSLASH

    IK_UNKNOWN,  // SDL_SCANCODE_NONUSHASH

    IK_UNKNOWN,  // SDL_SCANCODE_COMMA
    IK_UNKNOWN,  // SDL_SCANCODE_PERIOD
    IK_UNKNOWN,  // SDL_SCANCODE_SLASH

    IK_UNKNOWN, // SDL_SCANCODE_COMMA
    IK_UNKNOWN, // SDL_SCANCODE_PERIOD
    IK_UNKNOWN, // SDL_SCANCODE_SLASH

    IK_UNKNOWN,  // SDL_SCANCODE_CAPSLOCK

    IK_F1,   //SDL_SCANCODE_F1
    IK_F2,   //SDL_SCANCODE_F2
    IK_F3,   //SDL_SCANCODE_F3
    IK_F4,   //SDL_SCANCODE_F4
    IK_F5,   //SDL_SCANCODE_F5
    IK_F6,   //SDL_SCANCODE_F6
    IK_F7,   //SDL_SCANCODE_F7
    IK_F8,   //SDL_SCANCODE_F8
    IK_F9,   //SDL_SCANCODE_F9
    IK_F10,  //SDL_SCANCODE_F10
    IK_F11,  //SDL_SCANCODE_F11
    IK_F12,  //SDL_SCANCODE_F12

    IK_UNKNOWN, //     SDL_SCANCODE_PRINTSCREEN
    IK_UNKNOWN, //     SDL_SCANCODE_SCROLLLOCK
    IK_UNKNOWN, //     SDL_SCANCODE_PAUSE
    IK_UNKNOWN, //     SDL_SCANCODE_INSERT
    IK_UNKNOWN, //     SDL_SCANCODE_HOME
    IK_UNKNOWN, //     SDL_SCANCODE_PAGEUP
    IK_UNKNOWN, //     SDL_SCANCODE_DELETE
    IK_UNKNOWN, //     SDL_SCANCODE_END
    IK_UNKNOWN, //     SDL_SCANCODE_PAGEDOWN
    IK_UNKNOWN, //     SDL_SCANCODE_RIGHT
    IK_UNKNOWN, //     SDL_SCANCODE_LEFT
    IK_UNKNOWN, //     SDL_SCANCODE_DOWN
    IK_UNKNOWN, //     SDL_SCANCODE_UP

    IK_UNKNOWN, //     SDL_SCANCODE_NUMLOCKCLEAR
    IK_UNKNOWN, //     SDL_SCANCODE_KP_DIVIDE
    IK_UNKNOWN, //     SDL_SCANCODE_KP_MULTIPLY
    IK_UNKNOWN, //     SDL_SCANCODE_KP_MINUS
    IK_UNKNOWN, //     SDL_SCANCODE_KP_PLUS
    IK_UNKNOWN, //     SDL_SCANCODE_KP_ENTER
    IK_UNKNOWN, //     SDL_SCANCODE_KP_1
    IK_UNKNOWN, //     SDL_SCANCODE_KP_2
    IK_UNKNOWN, //     SDL_SCANCODE_KP_3
    IK_UNKNOWN, //     SDL_SCANCODE_KP_4
    IK_UNKNOWN, //     SDL_SCANCODE_KP_5
    IK_UNKNOWN, //     SDL_SCANCODE_KP_6
    IK_UNKNOWN, //     SDL_SCANCODE_KP_7
    IK_UNKNOWN, //     SDL_SCANCODE_KP_8
    IK_UNKNOWN, //     SDL_SCANCODE_KP_9
    IK_UNKNOWN, //     SDL_SCANCODE_KP_0
    IK_UNKNOWN, //     SDL_SCANCODE_KP_PERIOD

    IK_UNKNOWN, //     SDL_SCANCODE_NONUSBACKSLASH
    IK_UNKNOWN, //     SDL_SCANCODE_APPLICATION
    IK_UNKNOWN, //     SDL_SCANCODE_POWER
    IK_UNKNOWN, //     SDL_SCANCODE_KP_EQUALS
    IK_UNKNOWN, //     SDL_SCANCODE_F13
    IK_UNKNOWN, //     SDL_SCANCODE_F14
    IK_UNKNOWN, //     SDL_SCANCODE_F15
    IK_UNKNOWN, //     SDL_SCANCODE_F16
    IK_UNKNOWN, //     SDL_SCANCODE_F17
    IK_UNKNOWN, //     SDL_SCANCODE_F18
    IK_UNKNOWN, //     SDL_SCANCODE_F19
    IK_UNKNOWN, //     SDL_SCANCODE_F20
    IK_UNKNOWN, //     SDL_SCANCODE_F21
    IK_UNKNOWN, //     SDL_SCANCODE_F22
    IK_UNKNOWN, //     SDL_SCANCODE_F23
    IK_UNKNOWN, //     SDL_SCANCODE_F24
    IK_UNKNOWN, //     SDL_SCANCODE_EXECUTE
    IK_UNKNOWN, //     SDL_SCANCODE_HELP
    IK_UNKNOWN, //     SDL_SCANCODE_MENU
    IK_UNKNOWN, //     SDL_SCANCODE_SELECT
    IK_UNKNOWN, //     SDL_SCANCODE_STOP
    IK_UNKNOWN, //     SDL_SCANCODE_AGAIN
    IK_UNKNOWN, //     SDL_SCANCODE_UNDO
    IK_UNKNOWN, //     SDL_SCANCODE_CUT
    IK_UNKNOWN, //     SDL_SCANCODE_COPY
    IK_UNKNOWN, //     SDL_SCANCODE_PASTE
    IK_UNKNOWN, //     SDL_SCANCODE_FIND
    IK_UNKNOWN, //     SDL_SCANCODE_MUTE
    IK_UNKNOWN, //     SDL_SCANCODE_VOLUMEUP
    IK_UNKNOWN, //     SDL_SCANCODE_VOLUMEDOWN
    IK_UNKNOWN, //     SDL_SCANCODE_LOCKINGCAPSLOCK
    IK_UNKNOWN, //     SDL_SCANCODE_LOCKINGNUMLOCK
    IK_UNKNOWN, //     SDL_SCANCODE_LOCKINGSCROLLLOCK
    IK_UNKNOWN, //     SDL_SCANCODE_KP_COMMA
    IK_UNKNOWN, //     SDL_SCANCODE_KP_EQUALSAS400

    IK_UNKNOWN, //     SDL_SCANCODE_INTERNATIONAL1
    IK_UNKNOWN, //     SDL_SCANCODE_INTERNATIONAL2
    IK_UNKNOWN, //     SDL_SCANCODE_INTERNATIONAL3
    IK_UNKNOWN, //     SDL_SCANCODE_INTERNATIONAL4
    IK_UNKNOWN, //     SDL_SCANCODE_INTERNATIONAL5
    IK_UNKNOWN, //     SDL_SCANCODE_INTERNATIONAL6
    IK_UNKNOWN, //     SDL_SCANCODE_INTERNATIONAL7
    IK_UNKNOWN, //     SDL_SCANCODE_INTERNATIONAL8
    IK_UNKNOWN, //     SDL_SCANCODE_INTERNATIONAL9
    IK_UNKNOWN, //     SDL_SCANCODE_LANG1
    IK_UNKNOWN, //     SDL_SCANCODE_LANG2
    IK_UNKNOWN, //     SDL_SCANCODE_LANG3
    IK_UNKNOWN, //     SDL_SCANCODE_LANG4
    IK_UNKNOWN, //     SDL_SCANCODE_LANG5
    IK_UNKNOWN, //     SDL_SCANCODE_LANG6
    IK_UNKNOWN, //     SDL_SCANCODE_LANG7
    IK_UNKNOWN, //     SDL_SCANCODE_LANG8
    IK_UNKNOWN, //     SDL_SCANCODE_LANG9

    IK_UNKNOWN, //     SDL_SCANCODE_ALTERASE
    IK_UNKNOWN, //     SDL_SCANCODE_SYSREQ
    IK_UNKNOWN, //     SDL_SCANCODE_CANCEL
    IK_UNKNOWN, //     SDL_SCANCODE_CLEAR
    IK_UNKNOWN, //     SDL_SCANCODE_PRIOR
    IK_UNKNOWN, //     SDL_SCANCODE_RETURN2
    IK_UNKNOWN, //     SDL_SCANCODE_SEPARATOR
    IK_UNKNOWN, //     SDL_SCANCODE_OUT
    IK_UNKNOWN, //     SDL_SCANCODE_OPER
    IK_UNKNOWN, //     SDL_SCANCODE_CLEARAGAIN
    IK_UNKNOWN, //     SDL_SCANCODE_CRSEL
    IK_UNKNOWN, //     SDL_SCANCODE_EXSEL

    IK_UNKNOWN, //     SDL_SCANCODE_KP_00
    IK_UNKNOWN, //     SDL_SCANCODE_KP_000
    IK_UNKNOWN, //     SDL_SCANCODE_THOUSANDSSEPARATOR
    IK_UNKNOWN, //     SDL_SCANCODE_DECIMALSEPARATOR
    IK_UNKNOWN, //     SDL_SCANCODE_CURRENCYUNIT
    IK_UNKNOWN, //     SDL_SCANCODE_CURRENCYSUBUNIT
    IK_UNKNOWN, //     SDL_SCANCODE_KP_LEFTPAREN
    IK_UNKNOWN, //     SDL_SCANCODE_KP_RIGHTPAREN
    IK_UNKNOWN, //     SDL_SCANCODE_KP_LEFTBRACE
    IK_UNKNOWN, //     SDL_SCANCODE_KP_RIGHTBRACE
    IK_UNKNOWN, //     SDL_SCANCODE_KP_TAB
    IK_UNKNOWN, //     SDL_SCANCODE_KP_BACKSPACE
    IK_UNKNOWN, //     SDL_SCANCODE_KP_A
    IK_UNKNOWN, //     SDL_SCANCODE_KP_B
    IK_UNKNOWN, //     SDL_SCANCODE_KP_C
    IK_UNKNOWN, //     SDL_SCANCODE_KP_D
    IK_UNKNOWN, //     SDL_SCANCODE_KP_E
    IK_UNKNOWN, //     SDL_SCANCODE_KP_F
    IK_UNKNOWN, //     SDL_SCANCODE_KP_XOR
    IK_UNKNOWN, //     SDL_SCANCODE_KP_POWER
    IK_UNKNOWN, //     SDL_SCANCODE_KP_PERCENT
    IK_UNKNOWN, //     SDL_SCANCODE_KP_LESS
    IK_UNKNOWN, //     SDL_SCANCODE_KP_GREATER
    IK_UNKNOWN, //     SDL_SCANCODE_KP_AMPERSAND
    IK_UNKNOWN, //     SDL_SCANCODE_KP_DBLAMPERSAND
    IK_UNKNOWN, //     SDL_SCANCODE_KP_VERTICALBAR
    IK_UNKNOWN, //     SDL_SCANCODE_KP_DBLVERTICALBAR
    IK_UNKNOWN, //     SDL_SCANCODE_KP_COLON
    IK_UNKNOWN, //     SDL_SCANCODE_KP_HASH
    IK_UNKNOWN, //     SDL_SCANCODE_KP_SPACE
    IK_UNKNOWN, //     SDL_SCANCODE_KP_AT
    IK_UNKNOWN, //     SDL_SCANCODE_KP_EXCLAM
    IK_UNKNOWN, //     SDL_SCANCODE_KP_MEMSTORE
    IK_UNKNOWN, //     SDL_SCANCODE_KP_MEMRECALL
    IK_UNKNOWN, //     SDL_SCANCODE_KP_MEMCLEAR
    IK_UNKNOWN, //     SDL_SCANCODE_KP_MEMADD
    IK_UNKNOWN, //     SDL_SCANCODE_KP_MEMSUBTRACT
    IK_UNKNOWN, //     SDL_SCANCODE_KP_MEMMULTIPLY
    IK_UNKNOWN, //     SDL_SCANCODE_KP_MEMDIVIDE
    IK_UNKNOWN, //     SDL_SCANCODE_KP_PLUSMINUS
    IK_UNKNOWN, //     SDL_SCANCODE_KP_CLEAR
    IK_UNKNOWN, //     SDL_SCANCODE_KP_CLEARENTRY
    IK_UNKNOWN, //     SDL_SCANCODE_KP_BINARY
    IK_UNKNOWN, //     SDL_SCANCODE_KP_OCTAL
    IK_UNKNOWN, //     SDL_SCANCODE_KP_DECIMAL
    IK_UNKNOWN, //     SDL_SCANCODE_KP_HEXADECIMAL

    IK_LCTRL,   // SDL_SCANCODE_LCTRL
    IK_LSHIFT,  // SDL_SCANCODE_LSHIFT
    IK_LALT,    // SDL_SCANCODE_LALT
    IK_UNKNOWN, // SDL_SCANCODE_LGUI
    IK_RCTRL,   // SDL_SCANCODE_RCTRL
    IK_RSHIFT,  // SDL_SCANCODE_RSHIFT
    IK_RALT,    // SDL_SCANCODE_RALT
    IK_UNKNOWN, // SDL_SCANCODE_RGUI

    IK_UNKNOWN, //     SDL_SCANCODE_MODE

    IK_UNKNOWN, //     SDL_SCANCODE_AUDIONEXT
    IK_UNKNOWN, //     SDL_SCANCODE_AUDIOPREV
    IK_UNKNOWN, //     SDL_SCANCODE_AUDIOSTOP
    IK_UNKNOWN, //     SDL_SCANCODE_AUDIOPLAY
    IK_UNKNOWN, //     SDL_SCANCODE_AUDIOMUTE
    IK_UNKNOWN, //     SDL_SCANCODE_MEDIASELECT
    IK_UNKNOWN, //     SDL_SCANCODE_WWW
    IK_UNKNOWN, //     SDL_SCANCODE_MAIL
    IK_UNKNOWN, //     SDL_SCANCODE_CALCULATOR
    IK_UNKNOWN, //     SDL_SCANCODE_COMPUTER
    IK_UNKNOWN, //     SDL_SCANCODE_AC_SEARCH
    IK_UNKNOWN, //     SDL_SCANCODE_AC_HOME
    IK_UNKNOWN, //     SDL_SCANCODE_AC_BACK
    IK_UNKNOWN, //     SDL_SCANCODE_AC_FORWARD
    IK_UNKNOWN, //     SDL_SCANCODE_AC_STOP
    IK_UNKNOWN, //     SDL_SCANCODE_AC_REFRESH
    IK_UNKNOWN, //     SDL_SCANCODE_AC_BOOKMARKS

    IK_UNKNOWN, //     SDL_SCANCODE_BRIGHTNESSDOWN
    IK_UNKNOWN, //     SDL_SCANCODE_BRIGHTNESSUP
    IK_UNKNOWN, //     SDL_SCANCODE_DISPLAYSWITCH
    IK_UNKNOWN, //     SDL_SCANCODE_KBDILLUMTOGGLE
    IK_UNKNOWN, //     SDL_SCANCODE_KBDILLUMDOWN
    IK_UNKNOWN, //     SDL_SCANCODE_KBDILLUMUP
    IK_UNKNOWN, //     SDL_SCANCODE_EJECT
    IK_UNKNOWN, //     SDL_SCANCODE_SLEEP

    IK_UNKNOWN, //     SDL_SCANCODE_APP1
    IK_UNKNOWN, //     SDL_SCANCODE_APP2
};

InputKey get_inputkey_from_sdl_keycode( SDL_Keycode keycode )
{
    return sdlscancode_to_inputkey_map[ keycode & ~SDLK_SCANCODE_MASK ];
}

InputKey get_inputkey_from_sdl_scancode( SDL_Scancode scancode )
{
    return sdlscancode_to_inputkey_map[ scancode ];
}

bool handle_inputs( InputState& input_state, const SDL_Event& evt )
{
    input_state.mouse_wheel = {};
    switch( evt.type )
    {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
            auto key = get_inputkey_from_sdl_scancode( evt.key.keysym.scancode );
            if( key != IK_UNKNOWN && evt.key.repeat == 0 )
            {
                input_state.key_down[key] = evt.key.state == SDL_PRESSED;
                input_state.key_change[key] = true;
            }
            break;
        }
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        {
            auto key = IK_UNKNOWN;
            switch(evt.button.button)
            {
                case SDL_BUTTON_LEFT:
                    key = IK_MOUSE1;
                    break;
                case SDL_BUTTON_RIGHT:
                    key = IK_MOUSE2;
                    break;
                case SDL_BUTTON_MIDDLE:
                    key = IK_MOUSE3;
                    break;
                case SDL_BUTTON_X1:
                    key = IK_MOUSE4;
                    break;
                case SDL_BUTTON_X2:
                    key = IK_MOUSE5;
                    break;
            }
            input_state.key_down[key] = evt.button.state == SDL_PRESSED;
            input_state.key_change[key] = true;
            break;
        }
        case SDL_MOUSEWHEEL:
            input_state.mouse_wheel.x = (float)evt.wheel.x;
            input_state.mouse_wheel.y = (float)evt.wheel.y;
            break;
        case SDL_MOUSEMOTION:
            input_state.mouse_position.x = (float)evt.motion.x;
            input_state.mouse_position.y = (float)evt.motion.y;
            break;
        default:
            return false;
            break;
    }

    return true;
}

void init_input_state( InputState& input_state )
{
    input_state = {};
}

bool InputState::is_key_down( InputKey key )
{
    return key_down[key];
}

bool InputState::is_key_up( InputKey key )
{
    return !key_down[key];
}

bool InputState::is_key_down_this_frame( InputKey key )
{
    return key_change[key] && key_down[key];
}

bool InputState::is_key_up_this_frame( InputKey key )
{
    return key_change[key] && !key_down[key];
}

void InputState::frame_start()
{
    memset( key_change, 0, sizeof(bool)*IK_COUNT );
}
