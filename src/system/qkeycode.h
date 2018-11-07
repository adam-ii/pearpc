/*
 *	PearPC
 *	qkeycode.h
 *
 *	Copyright (C) 2018 Adam Iarossi
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License version 2 as
 *	published by the Free Software Foundation.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/*
 * QEMU System Emulator
 *
 * Copyright (c) 2003-2008 Fabrice Bellard
 */

#ifndef __QKEYCODE_H__
#define __QKEYCODE_H__

#include <string.h>

// Generated from QEMU qapi/ui.json using PearPC tools/qkeycode.py
typedef enum QKeyCode {
	Q_KEY_CODE_UNMAPPED         = 0,
	Q_KEY_CODE_SHIFT            = 1,
	Q_KEY_CODE_SHIFT_R          = 2,
	Q_KEY_CODE_ALT              = 3,
	Q_KEY_CODE_ALT_R            = 4,
	Q_KEY_CODE_CTRL             = 5,
	Q_KEY_CODE_CTRL_R           = 6,
	Q_KEY_CODE_MENU             = 7,
	Q_KEY_CODE_ESC              = 8,
	Q_KEY_CODE_1                = 9,
	Q_KEY_CODE_2                = 10,
	Q_KEY_CODE_3                = 11,
	Q_KEY_CODE_4                = 12,
	Q_KEY_CODE_5                = 13,
	Q_KEY_CODE_6                = 14,
	Q_KEY_CODE_7                = 15,
	Q_KEY_CODE_8                = 16,
	Q_KEY_CODE_9                = 17,
	Q_KEY_CODE_0                = 18,
	Q_KEY_CODE_MINUS            = 19,
	Q_KEY_CODE_EQUAL            = 20,
	Q_KEY_CODE_BACKSPACE        = 21,
	Q_KEY_CODE_TAB              = 22,
	Q_KEY_CODE_Q                = 23,
	Q_KEY_CODE_W                = 24,
	Q_KEY_CODE_E                = 25,
	Q_KEY_CODE_R                = 26,
	Q_KEY_CODE_T                = 27,
	Q_KEY_CODE_Y                = 28,
	Q_KEY_CODE_U                = 29,
	Q_KEY_CODE_I                = 30,
	Q_KEY_CODE_O                = 31,
	Q_KEY_CODE_P                = 32,
	Q_KEY_CODE_BRACKET_LEFT     = 33,
	Q_KEY_CODE_BRACKET_RIGHT    = 34,
	Q_KEY_CODE_RET              = 35,
	Q_KEY_CODE_A                = 36,
	Q_KEY_CODE_S                = 37,
	Q_KEY_CODE_D                = 38,
	Q_KEY_CODE_F                = 39,
	Q_KEY_CODE_G                = 40,
	Q_KEY_CODE_H                = 41,
	Q_KEY_CODE_J                = 42,
	Q_KEY_CODE_K                = 43,
	Q_KEY_CODE_L                = 44,
	Q_KEY_CODE_SEMICOLON        = 45,
	Q_KEY_CODE_APOSTROPHE       = 46,
	Q_KEY_CODE_GRAVE_ACCENT     = 47,
	Q_KEY_CODE_BACKSLASH        = 48,
	Q_KEY_CODE_Z                = 49,
	Q_KEY_CODE_X                = 50,
	Q_KEY_CODE_C                = 51,
	Q_KEY_CODE_V                = 52,
	Q_KEY_CODE_B                = 53,
	Q_KEY_CODE_N                = 54,
	Q_KEY_CODE_M                = 55,
	Q_KEY_CODE_COMMA            = 56,
	Q_KEY_CODE_DOT              = 57,
	Q_KEY_CODE_SLASH            = 58,
	Q_KEY_CODE_ASTERISK         = 59,
	Q_KEY_CODE_SPC              = 60,
	Q_KEY_CODE_CAPS_LOCK        = 61,
	Q_KEY_CODE_F1               = 62,
	Q_KEY_CODE_F2               = 63,
	Q_KEY_CODE_F3               = 64,
	Q_KEY_CODE_F4               = 65,
	Q_KEY_CODE_F5               = 66,
	Q_KEY_CODE_F6               = 67,
	Q_KEY_CODE_F7               = 68,
	Q_KEY_CODE_F8               = 69,
	Q_KEY_CODE_F9               = 70,
	Q_KEY_CODE_F10              = 71,
	Q_KEY_CODE_NUM_LOCK         = 72,
	Q_KEY_CODE_SCROLL_LOCK      = 73,
	Q_KEY_CODE_KP_DIVIDE        = 74,
	Q_KEY_CODE_KP_MULTIPLY      = 75,
	Q_KEY_CODE_KP_SUBTRACT      = 76,
	Q_KEY_CODE_KP_ADD           = 77,
	Q_KEY_CODE_KP_ENTER         = 78,
	Q_KEY_CODE_KP_DECIMAL       = 79,
	Q_KEY_CODE_SYSRQ            = 80,
	Q_KEY_CODE_KP_0             = 81,
	Q_KEY_CODE_KP_1             = 82,
	Q_KEY_CODE_KP_2             = 83,
	Q_KEY_CODE_KP_3             = 84,
	Q_KEY_CODE_KP_4             = 85,
	Q_KEY_CODE_KP_5             = 86,
	Q_KEY_CODE_KP_6             = 87,
	Q_KEY_CODE_KP_7             = 88,
	Q_KEY_CODE_KP_8             = 89,
	Q_KEY_CODE_KP_9             = 90,
	Q_KEY_CODE_LESS             = 91,
	Q_KEY_CODE_F11              = 92,
	Q_KEY_CODE_F12              = 93,
	Q_KEY_CODE_PRINT            = 94,
	Q_KEY_CODE_HOME             = 95,
	Q_KEY_CODE_PGUP             = 96,
	Q_KEY_CODE_PGDN             = 97,
	Q_KEY_CODE_END              = 98,
	Q_KEY_CODE_LEFT             = 99,
	Q_KEY_CODE_UP               = 100,
	Q_KEY_CODE_DOWN             = 101,
	Q_KEY_CODE_RIGHT            = 102,
	Q_KEY_CODE_INSERT           = 103,
	Q_KEY_CODE_DELETE           = 104,
	Q_KEY_CODE_STOP             = 105,
	Q_KEY_CODE_AGAIN            = 106,
	Q_KEY_CODE_PROPS            = 107,
	Q_KEY_CODE_UNDO             = 108,
	Q_KEY_CODE_FRONT            = 109,
	Q_KEY_CODE_COPY             = 110,
	Q_KEY_CODE_OPEN             = 111,
	Q_KEY_CODE_PASTE            = 112,
	Q_KEY_CODE_FIND             = 113,
	Q_KEY_CODE_CUT              = 114,
	Q_KEY_CODE_LF               = 115,
	Q_KEY_CODE_HELP             = 116,
	Q_KEY_CODE_META_L           = 117,
	Q_KEY_CODE_META_R           = 118,
	Q_KEY_CODE_COMPOSE          = 119,
	Q_KEY_CODE_PAUSE            = 120,
	Q_KEY_CODE_RO               = 121,
	Q_KEY_CODE_HIRAGANA         = 122,
	Q_KEY_CODE_HENKAN           = 123,
	Q_KEY_CODE_YEN              = 124,
	Q_KEY_CODE_MUHENKAN         = 125,
	Q_KEY_CODE_KATAKANAHIRAGANA = 126,
	Q_KEY_CODE_KP_COMMA         = 127,
	Q_KEY_CODE_KP_EQUALS        = 128,
	Q_KEY_CODE_POWER            = 129,
	Q_KEY_CODE_SLEEP            = 130,
	Q_KEY_CODE_WAKE             = 131,
	Q_KEY_CODE_AUDIONEXT        = 132,
	Q_KEY_CODE_AUDIOPREV        = 133,
	Q_KEY_CODE_AUDIOSTOP        = 134,
	Q_KEY_CODE_AUDIOPLAY        = 135,
	Q_KEY_CODE_AUDIOMUTE        = 136,
	Q_KEY_CODE_VOLUMEUP         = 137,
	Q_KEY_CODE_VOLUMEDOWN       = 138,
	Q_KEY_CODE_MEDIASELECT      = 139,
	Q_KEY_CODE_MAIL             = 140,
	Q_KEY_CODE_CALCULATOR       = 141,
	Q_KEY_CODE_COMPUTER         = 142,
	Q_KEY_CODE_AC_HOME          = 143,
	Q_KEY_CODE_AC_BACK          = 144,
	Q_KEY_CODE_AC_FORWARD       = 145,
	Q_KEY_CODE_AC_REFRESH       = 146,
	Q_KEY_CODE_AC_BOOKMARKS     = 147,
	Q_KEY_CODE__MAX             = 148
} QKeyCode;


#ifdef __cplusplus
namespace qemu {
	
	/// Convert a QKeyCode name to a QKeyCode enumeration, optionally checking against PearPC key names
	QKeyCode stringToQKeyCode(const char *s, bool pearpcNames);
	
	const char *stringFromQKeyCode(QKeyCode k, bool pearpcNames);
	
}
#endif

#endif
