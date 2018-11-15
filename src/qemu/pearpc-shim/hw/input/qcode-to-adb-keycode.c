/*
 * QEMU ADB keyboard support
 *
 * Copyright (c) 2004 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "system/qkeycode.h"
#include "qemu/include/hw/input/adb-keys.h"

// From hw/input/adb-kbd.c

/* The adb keyboard doesn't have every key imaginable */
#define NO_KEY 0xff

const int qcode_to_adb_keycode[] = {
	/* Make sure future additions are automatically set to NO_KEY */
	[0 ... 0xff]               = NO_KEY,
	
	[Q_KEY_CODE_SHIFT]         = ADB_KEY_LEFT_SHIFT,
	[Q_KEY_CODE_SHIFT_R]       = ADB_KEY_RIGHT_SHIFT,
	[Q_KEY_CODE_ALT]           = ADB_KEY_LEFT_OPTION,
	[Q_KEY_CODE_ALT_R]         = ADB_KEY_RIGHT_OPTION,
	[Q_KEY_CODE_CTRL]          = ADB_KEY_LEFT_CONTROL,
	[Q_KEY_CODE_CTRL_R]        = ADB_KEY_RIGHT_CONTROL,
	[Q_KEY_CODE_META_L]        = ADB_KEY_COMMAND,
	[Q_KEY_CODE_META_R]        = ADB_KEY_COMMAND,
	[Q_KEY_CODE_SPC]           = ADB_KEY_SPACEBAR,
	
	[Q_KEY_CODE_ESC]           = ADB_KEY_ESC,
	[Q_KEY_CODE_1]             = ADB_KEY_1,
	[Q_KEY_CODE_2]             = ADB_KEY_2,
	[Q_KEY_CODE_3]             = ADB_KEY_3,
	[Q_KEY_CODE_4]             = ADB_KEY_4,
	[Q_KEY_CODE_5]             = ADB_KEY_5,
	[Q_KEY_CODE_6]             = ADB_KEY_6,
	[Q_KEY_CODE_7]             = ADB_KEY_7,
	[Q_KEY_CODE_8]             = ADB_KEY_8,
	[Q_KEY_CODE_9]             = ADB_KEY_9,
	[Q_KEY_CODE_0]             = ADB_KEY_0,
	[Q_KEY_CODE_MINUS]         = ADB_KEY_MINUS,
	[Q_KEY_CODE_EQUAL]         = ADB_KEY_EQUAL,
	[Q_KEY_CODE_BACKSPACE]     = ADB_KEY_DELETE,
	[Q_KEY_CODE_TAB]           = ADB_KEY_TAB,
	[Q_KEY_CODE_Q]             = ADB_KEY_Q,
	[Q_KEY_CODE_W]             = ADB_KEY_W,
	[Q_KEY_CODE_E]             = ADB_KEY_E,
	[Q_KEY_CODE_R]             = ADB_KEY_R,
	[Q_KEY_CODE_T]             = ADB_KEY_T,
	[Q_KEY_CODE_Y]             = ADB_KEY_Y,
	[Q_KEY_CODE_U]             = ADB_KEY_U,
	[Q_KEY_CODE_I]             = ADB_KEY_I,
	[Q_KEY_CODE_O]             = ADB_KEY_O,
	[Q_KEY_CODE_P]             = ADB_KEY_P,
	[Q_KEY_CODE_BRACKET_LEFT]  = ADB_KEY_LEFT_BRACKET,
	[Q_KEY_CODE_BRACKET_RIGHT] = ADB_KEY_RIGHT_BRACKET,
	[Q_KEY_CODE_RET]           = ADB_KEY_RETURN,
	[Q_KEY_CODE_A]             = ADB_KEY_A,
	[Q_KEY_CODE_S]             = ADB_KEY_S,
	[Q_KEY_CODE_D]             = ADB_KEY_D,
	[Q_KEY_CODE_F]             = ADB_KEY_F,
	[Q_KEY_CODE_G]             = ADB_KEY_G,
	[Q_KEY_CODE_H]             = ADB_KEY_H,
	[Q_KEY_CODE_J]             = ADB_KEY_J,
	[Q_KEY_CODE_K]             = ADB_KEY_K,
	[Q_KEY_CODE_L]             = ADB_KEY_L,
	[Q_KEY_CODE_SEMICOLON]     = ADB_KEY_SEMICOLON,
	[Q_KEY_CODE_APOSTROPHE]    = ADB_KEY_APOSTROPHE,
	[Q_KEY_CODE_GRAVE_ACCENT]  = ADB_KEY_GRAVE_ACCENT,
	[Q_KEY_CODE_BACKSLASH]     = ADB_KEY_BACKSLASH,
	[Q_KEY_CODE_Z]             = ADB_KEY_Z,
	[Q_KEY_CODE_X]             = ADB_KEY_X,
	[Q_KEY_CODE_C]             = ADB_KEY_C,
	[Q_KEY_CODE_V]             = ADB_KEY_V,
	[Q_KEY_CODE_B]             = ADB_KEY_B,
	[Q_KEY_CODE_N]             = ADB_KEY_N,
	[Q_KEY_CODE_M]             = ADB_KEY_M,
	[Q_KEY_CODE_COMMA]         = ADB_KEY_COMMA,
	[Q_KEY_CODE_DOT]           = ADB_KEY_PERIOD,
	[Q_KEY_CODE_SLASH]         = ADB_KEY_FORWARD_SLASH,
	[Q_KEY_CODE_ASTERISK]      = ADB_KEY_KP_MULTIPLY,
	[Q_KEY_CODE_CAPS_LOCK]     = ADB_KEY_CAPS_LOCK,
	
	[Q_KEY_CODE_F1]            = ADB_KEY_F1,
	[Q_KEY_CODE_F2]            = ADB_KEY_F2,
	[Q_KEY_CODE_F3]            = ADB_KEY_F3,
	[Q_KEY_CODE_F4]            = ADB_KEY_F4,
	[Q_KEY_CODE_F5]            = ADB_KEY_F5,
	[Q_KEY_CODE_F6]            = ADB_KEY_F6,
	[Q_KEY_CODE_F7]            = ADB_KEY_F7,
	[Q_KEY_CODE_F8]            = ADB_KEY_F8,
	[Q_KEY_CODE_F9]            = ADB_KEY_F9,
	[Q_KEY_CODE_F10]           = ADB_KEY_F10,
	[Q_KEY_CODE_F11]           = ADB_KEY_F11,
	[Q_KEY_CODE_F12]           = ADB_KEY_F12,
	[Q_KEY_CODE_PRINT]         = ADB_KEY_F13,
	[Q_KEY_CODE_SYSRQ]         = ADB_KEY_F13,
	[Q_KEY_CODE_SCROLL_LOCK]   = ADB_KEY_F14,
	[Q_KEY_CODE_PAUSE]         = ADB_KEY_F15,
	
	[Q_KEY_CODE_NUM_LOCK]      = ADB_KEY_KP_CLEAR,
	[Q_KEY_CODE_KP_EQUALS]     = ADB_KEY_KP_EQUAL,
	[Q_KEY_CODE_KP_DIVIDE]     = ADB_KEY_KP_DIVIDE,
	[Q_KEY_CODE_KP_MULTIPLY]   = ADB_KEY_KP_MULTIPLY,
	[Q_KEY_CODE_KP_SUBTRACT]   = ADB_KEY_KP_SUBTRACT,
	[Q_KEY_CODE_KP_ADD]        = ADB_KEY_KP_PLUS,
	[Q_KEY_CODE_KP_ENTER]      = ADB_KEY_KP_ENTER,
	[Q_KEY_CODE_KP_DECIMAL]    = ADB_KEY_KP_PERIOD,
	[Q_KEY_CODE_KP_0]          = ADB_KEY_KP_0,
	[Q_KEY_CODE_KP_1]          = ADB_KEY_KP_1,
	[Q_KEY_CODE_KP_2]          = ADB_KEY_KP_2,
	[Q_KEY_CODE_KP_3]          = ADB_KEY_KP_3,
	[Q_KEY_CODE_KP_4]          = ADB_KEY_KP_4,
	[Q_KEY_CODE_KP_5]          = ADB_KEY_KP_5,
	[Q_KEY_CODE_KP_6]          = ADB_KEY_KP_6,
	[Q_KEY_CODE_KP_7]          = ADB_KEY_KP_7,
	[Q_KEY_CODE_KP_8]          = ADB_KEY_KP_8,
	[Q_KEY_CODE_KP_9]          = ADB_KEY_KP_9,
	
	[Q_KEY_CODE_UP]            = ADB_KEY_UP,
	[Q_KEY_CODE_DOWN]          = ADB_KEY_DOWN,
	[Q_KEY_CODE_LEFT]          = ADB_KEY_LEFT,
	[Q_KEY_CODE_RIGHT]         = ADB_KEY_RIGHT,
	
	[Q_KEY_CODE_HELP]          = ADB_KEY_HELP,
	[Q_KEY_CODE_INSERT]        = ADB_KEY_HELP,
	[Q_KEY_CODE_DELETE]        = ADB_KEY_FORWARD_DELETE,
	[Q_KEY_CODE_HOME]          = ADB_KEY_HOME,
	[Q_KEY_CODE_END]           = ADB_KEY_END,
	[Q_KEY_CODE_PGUP]          = ADB_KEY_PAGE_UP,
	[Q_KEY_CODE_PGDN]          = ADB_KEY_PAGE_DOWN,
	
	[Q_KEY_CODE_POWER]         = ADB_KEY_POWER
};

const size_t qcode_to_adb_keycode_len = sizeof(qcode_to_adb_keycode) / sizeof(qcode_to_adb_keycode[0]);
