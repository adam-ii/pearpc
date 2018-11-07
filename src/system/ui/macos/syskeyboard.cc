/* 
 *	PearPC
 *	keyboard.cc - keyboardaccess functions for macOS
 *
 *	Copyright (C) 1999-2004 Stefan Weyergraf
 *	Copyright (C) 1999-2004 Sebastian Biallas (sb@biallas.net)
 *	Copyright (C) 2018 Adam iarossi
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
 */

/*
 * QEMU Cocoa CG display driver
 *
 * Copyright (c) 2008 Mike Kronenberg
 *
 */

#include "sysmacos.h"
#include "system/keyboard.h"
#include "tools/snprintf.h"
#include <Carbon/Carbon.h>

namespace pearpc {

	namespace {
		
		// From QEMU ui/cocoa.m
		const int mac_to_qkeycode_map[] = {
			[kVK_ANSI_A] = Q_KEY_CODE_A,
			[kVK_ANSI_B] = Q_KEY_CODE_B,
			[kVK_ANSI_C] = Q_KEY_CODE_C,
			[kVK_ANSI_D] = Q_KEY_CODE_D,
			[kVK_ANSI_E] = Q_KEY_CODE_E,
			[kVK_ANSI_F] = Q_KEY_CODE_F,
			[kVK_ANSI_G] = Q_KEY_CODE_G,
			[kVK_ANSI_H] = Q_KEY_CODE_H,
			[kVK_ANSI_I] = Q_KEY_CODE_I,
			[kVK_ANSI_J] = Q_KEY_CODE_J,
			[kVK_ANSI_K] = Q_KEY_CODE_K,
			[kVK_ANSI_L] = Q_KEY_CODE_L,
			[kVK_ANSI_M] = Q_KEY_CODE_M,
			[kVK_ANSI_N] = Q_KEY_CODE_N,
			[kVK_ANSI_O] = Q_KEY_CODE_O,
			[kVK_ANSI_P] = Q_KEY_CODE_P,
			[kVK_ANSI_Q] = Q_KEY_CODE_Q,
			[kVK_ANSI_R] = Q_KEY_CODE_R,
			[kVK_ANSI_S] = Q_KEY_CODE_S,
			[kVK_ANSI_T] = Q_KEY_CODE_T,
			[kVK_ANSI_U] = Q_KEY_CODE_U,
			[kVK_ANSI_V] = Q_KEY_CODE_V,
			[kVK_ANSI_W] = Q_KEY_CODE_W,
			[kVK_ANSI_X] = Q_KEY_CODE_X,
			[kVK_ANSI_Y] = Q_KEY_CODE_Y,
			[kVK_ANSI_Z] = Q_KEY_CODE_Z,
			
			[kVK_ANSI_0] = Q_KEY_CODE_0,
			[kVK_ANSI_1] = Q_KEY_CODE_1,
			[kVK_ANSI_2] = Q_KEY_CODE_2,
			[kVK_ANSI_3] = Q_KEY_CODE_3,
			[kVK_ANSI_4] = Q_KEY_CODE_4,
			[kVK_ANSI_5] = Q_KEY_CODE_5,
			[kVK_ANSI_6] = Q_KEY_CODE_6,
			[kVK_ANSI_7] = Q_KEY_CODE_7,
			[kVK_ANSI_8] = Q_KEY_CODE_8,
			[kVK_ANSI_9] = Q_KEY_CODE_9,
			
			[kVK_ANSI_Grave] = Q_KEY_CODE_GRAVE_ACCENT,
			[kVK_ANSI_Minus] = Q_KEY_CODE_MINUS,
			[kVK_ANSI_Equal] = Q_KEY_CODE_EQUAL,
			[kVK_Delete] = Q_KEY_CODE_BACKSPACE,
			[kVK_CapsLock] = Q_KEY_CODE_CAPS_LOCK,
			[kVK_Tab] = Q_KEY_CODE_TAB,
			[kVK_Return] = Q_KEY_CODE_RET,
			[kVK_ANSI_LeftBracket] = Q_KEY_CODE_BRACKET_LEFT,
			[kVK_ANSI_RightBracket] = Q_KEY_CODE_BRACKET_RIGHT,
			[kVK_ANSI_Backslash] = Q_KEY_CODE_BACKSLASH,
			[kVK_ANSI_Semicolon] = Q_KEY_CODE_SEMICOLON,
			[kVK_ANSI_Quote] = Q_KEY_CODE_APOSTROPHE,
			[kVK_ANSI_Comma] = Q_KEY_CODE_COMMA,
			[kVK_ANSI_Period] = Q_KEY_CODE_DOT,
			[kVK_ANSI_Slash] = Q_KEY_CODE_SLASH,
			[kVK_Shift] = Q_KEY_CODE_SHIFT,
			[kVK_RightShift] = Q_KEY_CODE_SHIFT_R,
			[kVK_Control] = Q_KEY_CODE_CTRL,
			[kVK_RightControl] = Q_KEY_CODE_CTRL_R,
			[kVK_Option] = Q_KEY_CODE_ALT,
			[kVK_RightOption] = Q_KEY_CODE_ALT_R,
			[kVK_Command] = Q_KEY_CODE_META_L,
			[0x36] = Q_KEY_CODE_META_R, /* There is no kVK_RightCommand */
			[kVK_Space] = Q_KEY_CODE_SPC,
			
			[kVK_ANSI_Keypad0] = Q_KEY_CODE_KP_0,
			[kVK_ANSI_Keypad1] = Q_KEY_CODE_KP_1,
			[kVK_ANSI_Keypad2] = Q_KEY_CODE_KP_2,
			[kVK_ANSI_Keypad3] = Q_KEY_CODE_KP_3,
			[kVK_ANSI_Keypad4] = Q_KEY_CODE_KP_4,
			[kVK_ANSI_Keypad5] = Q_KEY_CODE_KP_5,
			[kVK_ANSI_Keypad6] = Q_KEY_CODE_KP_6,
			[kVK_ANSI_Keypad7] = Q_KEY_CODE_KP_7,
			[kVK_ANSI_Keypad8] = Q_KEY_CODE_KP_8,
			[kVK_ANSI_Keypad9] = Q_KEY_CODE_KP_9,
			[kVK_ANSI_KeypadDecimal] = Q_KEY_CODE_KP_DECIMAL,
			[kVK_ANSI_KeypadEnter] = Q_KEY_CODE_KP_ENTER,
			[kVK_ANSI_KeypadPlus] = Q_KEY_CODE_KP_ADD,
			[kVK_ANSI_KeypadMinus] = Q_KEY_CODE_KP_SUBTRACT,
			[kVK_ANSI_KeypadMultiply] = Q_KEY_CODE_KP_MULTIPLY,
			[kVK_ANSI_KeypadDivide] = Q_KEY_CODE_KP_DIVIDE,
			[kVK_ANSI_KeypadEquals] = Q_KEY_CODE_KP_EQUALS,
			[kVK_ANSI_KeypadClear] = Q_KEY_CODE_NUM_LOCK,
			
			[kVK_UpArrow] = Q_KEY_CODE_UP,
			[kVK_DownArrow] = Q_KEY_CODE_DOWN,
			[kVK_LeftArrow] = Q_KEY_CODE_LEFT,
			[kVK_RightArrow] = Q_KEY_CODE_RIGHT,
			
			[kVK_Help] = Q_KEY_CODE_INSERT,
			[kVK_Home] = Q_KEY_CODE_HOME,
			[kVK_PageUp] = Q_KEY_CODE_PGUP,
			[kVK_PageDown] = Q_KEY_CODE_PGDN,
			[kVK_End] = Q_KEY_CODE_END,
			[kVK_ForwardDelete] = Q_KEY_CODE_DELETE,
			
			[kVK_Escape] = Q_KEY_CODE_ESC,
			
			/* The Power key can't be used directly because the operating system uses
			 * it. This key can be emulated by using it in place of another key such as
			 * F1. Don't forget to disable the real key binding.
			 */
			/* [kVK_F1] = Q_KEY_CODE_POWER, */
			
			[kVK_F1] = Q_KEY_CODE_F1,
			[kVK_F2] = Q_KEY_CODE_F2,
			[kVK_F3] = Q_KEY_CODE_F3,
			[kVK_F4] = Q_KEY_CODE_F4,
			[kVK_F5] = Q_KEY_CODE_F5,
			[kVK_F6] = Q_KEY_CODE_F6,
			[kVK_F7] = Q_KEY_CODE_F7,
			[kVK_F8] = Q_KEY_CODE_F8,
			[kVK_F9] = Q_KEY_CODE_F9,
			[kVK_F10] = Q_KEY_CODE_F10,
			[kVK_F11] = Q_KEY_CODE_F11,
			[kVK_F12] = Q_KEY_CODE_F12,
			[kVK_F13] = Q_KEY_CODE_PRINT,
			[kVK_F14] = Q_KEY_CODE_SCROLL_LOCK,
			[kVK_F15] = Q_KEY_CODE_PAUSE,
			
			/*
			 * The eject and volume keys can't be used here because they are handled at
			 * a lower level than what an Application can see.
			 */
		};
		
	}

	
	QKeyCode MacSystemKeyboard::convertKeycodeToQKeyCode(int keycode) {
		if ((sizeof(mac_to_qkeycode_map) / sizeof(mac_to_qkeycode_map[0])) <= keycode) {
			ht_printf("<Warning> unknown keycode 0x%x\n", keycode);
			return Q_KEY_CODE_UNMAPPED;
		}
		return static_cast<QKeyCode>(mac_to_qkeycode_map[keycode]);
	}

}
