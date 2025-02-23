#ifndef KORE_INPUT_KEYBOARD_HEADER
#define KORE_INPUT_KEYBOARD_HEADER

#include <kore3/global.h>

/*! \file keyboard.h
    \brief Provides keyboard-support.
*/

#ifdef __cplusplus
extern "C" {
#endif

#define KORE_KEY_UNKNOWN 0
#define KORE_KEY_BACK 1 // Android
#define KORE_KEY_CANCEL 3
#define KORE_KEY_HELP 6
#define KORE_KEY_BACKSPACE 8
#define KORE_KEY_TAB 9
#define KORE_KEY_CLEAR 12
#define KORE_KEY_RETURN 13
#define KORE_KEY_SHIFT 16
#define KORE_KEY_CONTROL 17
#define KORE_KEY_ALT 18
#define KORE_KEY_PAUSE 19
#define KORE_KEY_CAPS_LOCK 20
#define KORE_KEY_KANA 21
#define KORE_KEY_HANGUL 21
#define KORE_KEY_EISU 22
#define KORE_KEY_JUNJA 23
#define KORE_KEY_FINAL 24
#define KORE_KEY_HANJA 25
#define KORE_KEY_KANJI 25
#define KORE_KEY_ESCAPE 27
#define KORE_KEY_CONVERT 28
#define KORE_KEY_NON_CONVERT 29
#define KORE_KEY_ACCEPT 30
#define KORE_KEY_MODE_CHANGE 31
#define KORE_KEY_SPACE 32
#define KORE_KEY_PAGE_UP 33
#define KORE_KEY_PAGE_DOWN 34
#define KORE_KEY_END 35
#define KORE_KEY_HOME 36
#define KORE_KEY_LEFT 37
#define KORE_KEY_UP 38
#define KORE_KEY_RIGHT 39
#define KORE_KEY_DOWN 40
#define KORE_KEY_SELECT 41
#define KORE_KEY_PRINT 42
#define KORE_KEY_EXECUTE 43
#define KORE_KEY_PRINT_SCREEN 44
#define KORE_KEY_INSERT 45
#define KORE_KEY_DELETE 46
#define KORE_KEY_0 48
#define KORE_KEY_1 49
#define KORE_KEY_2 50
#define KORE_KEY_3 51
#define KORE_KEY_4 52
#define KORE_KEY_5 53
#define KORE_KEY_6 54
#define KORE_KEY_7 55
#define KORE_KEY_8 56
#define KORE_KEY_9 57
#define KORE_KEY_COLON 58
#define KORE_KEY_SEMICOLON 59
#define KORE_KEY_LESS_THAN 60
#define KORE_KEY_EQUALS 61
#define KORE_KEY_GREATER_THAN 62
#define KORE_KEY_QUESTIONMARK 63
#define KORE_KEY_AT 64
#define KORE_KEY_A 65
#define KORE_KEY_B 66
#define KORE_KEY_C 67
#define KORE_KEY_D 68
#define KORE_KEY_E 69
#define KORE_KEY_F 70
#define KORE_KEY_G 71
#define KORE_KEY_H 72
#define KORE_KEY_I 73
#define KORE_KEY_J 74
#define KORE_KEY_K 75
#define KORE_KEY_L 76
#define KORE_KEY_M 77
#define KORE_KEY_N 78
#define KORE_KEY_O 79
#define KORE_KEY_P 80
#define KORE_KEY_Q 81
#define KORE_KEY_R 82
#define KORE_KEY_S 83
#define KORE_KEY_T 84
#define KORE_KEY_U 85
#define KORE_KEY_V 86
#define KORE_KEY_W 87
#define KORE_KEY_X 88
#define KORE_KEY_Y 89
#define KORE_KEY_Z 90
#define KORE_KEY_WIN 91
#define KORE_KEY_CONTEXT_MENU 93
#define KORE_KEY_SLEEP 95
#define KORE_KEY_NUMPAD_0 96
#define KORE_KEY_NUMPAD_1 97
#define KORE_KEY_NUMPAD_2 98
#define KORE_KEY_NUMPAD_3 99
#define KORE_KEY_NUMPAD_4 100
#define KORE_KEY_NUMPAD_5 101
#define KORE_KEY_NUMPAD_6 102
#define KORE_KEY_NUMPAD_7 103
#define KORE_KEY_NUMPAD_8 104
#define KORE_KEY_NUMPAD_9 105
#define KORE_KEY_MULTIPLY 106
#define KORE_KEY_ADD 107
#define KORE_KEY_SEPARATOR 108
#define KORE_KEY_SUBTRACT 109
#define KORE_KEY_DECIMAL 110
#define KORE_KEY_DIVIDE 111
#define KORE_KEY_F1 112
#define KORE_KEY_F2 113
#define KORE_KEY_F3 114
#define KORE_KEY_F4 115
#define KORE_KEY_F5 116
#define KORE_KEY_F6 117
#define KORE_KEY_F7 118
#define KORE_KEY_F8 119
#define KORE_KEY_F9 120
#define KORE_KEY_F10 121
#define KORE_KEY_F11 122
#define KORE_KEY_F12 123
#define KORE_KEY_F13 124
#define KORE_KEY_F14 125
#define KORE_KEY_F15 126
#define KORE_KEY_F16 127
#define KORE_KEY_F17 128
#define KORE_KEY_F18 129
#define KORE_KEY_F19 130
#define KORE_KEY_F20 131
#define KORE_KEY_F21 132
#define KORE_KEY_F22 133
#define KORE_KEY_F23 134
#define KORE_KEY_F24 135
#define KORE_KEY_NUM_LOCK 144
#define KORE_KEY_SCROLL_LOCK 145
#define KORE_KEY_WIN_OEM_FJ_JISHO 146
#define KORE_KEY_WIN_OEM_FJ_MASSHOU 147
#define KORE_KEY_WIN_OEM_FJ_TOUROKU 148
#define KORE_KEY_WIN_OEM_FJ_LOYA 149
#define KORE_KEY_WIN_OEM_FJ_ROYA 150
#define KORE_KEY_CIRCUMFLEX 160
#define KORE_KEY_EXCLAMATION 161
#define KORE_KEY_DOUBLE_QUOTE 162
#define KORE_KEY_HASH 163
#define KORE_KEY_DOLLAR 164
#define KORE_KEY_PERCENT 165
#define KORE_KEY_AMPERSAND 166
#define KORE_KEY_UNDERSCORE 167
#define KORE_KEY_OPEN_PAREN 168
#define KORE_KEY_CLOSE_PAREN 169
#define KORE_KEY_ASTERISK 170
#define KORE_KEY_PLUS 171
#define KORE_KEY_PIPE 172
#define KORE_KEY_HYPHEN_MINUS 173
#define KORE_KEY_OPEN_CURLY_BRACKET 174
#define KORE_KEY_CLOSE_CURLY_BRACKET 175
#define KORE_KEY_TILDE 176
#define KORE_KEY_VOLUME_MUTE 181
#define KORE_KEY_VOLUME_DOWN 182
#define KORE_KEY_VOLUME_UP 183
#define KORE_KEY_COMMA 188
#define KORE_KEY_PERIOD 190
#define KORE_KEY_SLASH 191
#define KORE_KEY_BACK_QUOTE 192
#define KORE_KEY_OPEN_BRACKET 219
#define KORE_KEY_BACK_SLASH 220
#define KORE_KEY_CLOSE_BRACKET 221
#define KORE_KEY_QUOTE 222
#define KORE_KEY_META 224
#define KORE_KEY_ALT_GR 225
#define KORE_KEY_WIN_ICO_HELP 227
#define KORE_KEY_WIN_ICO_00 228
#define KORE_KEY_WIN_ICO_CLEAR 230
#define KORE_KEY_WIN_OEM_RESET 233
#define KORE_KEY_WIN_OEM_JUMP 234
#define KORE_KEY_WIN_OEM_PA1 235
#define KORE_KEY_WIN_OEM_PA2 236
#define KORE_KEY_WIN_OEM_PA3 237
#define KORE_KEY_WIN_OEM_WSCTRL 238
#define KORE_KEY_WIN_OEM_CUSEL 239
#define KORE_KEY_WIN_OEM_ATTN 240
#define KORE_KEY_WIN_OEM_FINISH 241
#define KORE_KEY_WIN_OEM_COPY 242
#define KORE_KEY_WIN_OEM_AUTO 243
#define KORE_KEY_WIN_OEM_ENLW 244
#define KORE_KEY_WIN_OEM_BACK_TAB 245
#define KORE_KEY_ATTN 246
#define KORE_KEY_CRSEL 247
#define KORE_KEY_EXSEL 248
#define KORE_KEY_EREOF 249
#define KORE_KEY_PLAY 250
#define KORE_KEY_ZOOM 251
#define KORE_KEY_PA1 253
#define KORE_KEY_WIN_OEM_CLEAR 254

/// <summary>
/// Show the keyboard if the system is using a software-keyboard.
/// </summary>
KORE_FUNC void kore_keyboard_show(void);

/// <summary>
/// Hide the keyboard if the system is using a software-keyboard.
/// </summary>
KORE_FUNC void kore_keyboard_hide(void);

/// <summary>
/// Figure out whether the keyboard is currently shown if the system is using a software-keyboard.
/// </summary>
/// <returns>Whether the keyboard is currently shown</returns>
KORE_FUNC bool kore_keyboard_active(void);

/// <summary>
/// Sets the keyboard-key-down-callback which is called with a key-code when a key goes down. Do not use this for text-input, that's what the key-press-callback
/// is here for.
/// </summary>
/// <param name="value">The callback</param>
KORE_FUNC void kore_keyboard_set_key_down_callback(void (*value)(int /*key_code*/, void * /*data*/), void *data);

/// <summary>
/// Sets the keyboard-key-up-callback which is called with a key-code when a key goes up. Do not use this for text-input, that's what the key-press-callback is
/// here for.
/// </summary>
/// <param name="value">The callback</param>
KORE_FUNC void kore_keyboard_set_key_up_callback(void (*value)(int /*key_code*/, void * /*data*/), void *data);

/// <summary>
/// Sets the keyboard-key-press-callback which is called when the system decides that a character came in via the keyboard. Use this for text-input.
/// </summary>
/// <param name="value">The callback</param>
KORE_FUNC void kore_keyboard_set_key_press_callback(void (*value)(unsigned /*character*/, void * /*data*/), void *data);

void kore_internal_keyboard_trigger_key_down(int key_code);
void kore_internal_keyboard_trigger_key_up(int key_code);
void kore_internal_keyboard_trigger_key_press(unsigned character);

#ifdef __cplusplus
}
#endif

#endif