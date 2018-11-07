/*
 *	PearPC
 *	qkeycode.cc
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

#include "qkeycode.h"

namespace qemu {

	// Generated from QEMU qapi/ui.json using PearPC tools/qkeycode.py
	struct QKeyCodeNames { const char *qemu; const char *pearpc; } qkeycode_names[Q_KEY_CODE__MAX] = {
		{ "unmapped"        , NULL           }, // Q_KEY_CODE_UNMAPPED
		{ "shift"           , NULL           }, // Q_KEY_CODE_SHIFT
		{ "shift_r"         , NULL           }, // Q_KEY_CODE_SHIFT_R
		{ "alt"             , NULL           }, // Q_KEY_CODE_ALT
		{ "alt_r"           , NULL           }, // Q_KEY_CODE_ALT_R
		{ "ctrl"            , NULL           }, // Q_KEY_CODE_CTRL
		{ "ctrl_r"          , NULL           }, // Q_KEY_CODE_CTRL_R
		{ "menu"            , NULL           }, // Q_KEY_CODE_MENU
		{ "esc"             , "Escape"       }, // Q_KEY_CODE_ESC
		{ "1"               , NULL           }, // Q_KEY_CODE_1
		{ "2"               , NULL           }, // Q_KEY_CODE_2
		{ "3"               , NULL           }, // Q_KEY_CODE_3
		{ "4"               , NULL           }, // Q_KEY_CODE_4
		{ "5"               , NULL           }, // Q_KEY_CODE_5
		{ "6"               , NULL           }, // Q_KEY_CODE_6
		{ "7"               , NULL           }, // Q_KEY_CODE_7
		{ "8"               , NULL           }, // Q_KEY_CODE_8
		{ "9"               , NULL           }, // Q_KEY_CODE_9
		{ "0"               , NULL           }, // Q_KEY_CODE_0
		{ "minus"           , "-"            }, // Q_KEY_CODE_MINUS
		{ "equal"           , "="            }, // Q_KEY_CODE_EQUAL
		{ "backspace"       , NULL           }, // Q_KEY_CODE_BACKSPACE
		{ "tab"             , NULL           }, // Q_KEY_CODE_TAB
		{ "q"               , NULL           }, // Q_KEY_CODE_Q
		{ "w"               , NULL           }, // Q_KEY_CODE_W
		{ "e"               , NULL           }, // Q_KEY_CODE_E
		{ "r"               , NULL           }, // Q_KEY_CODE_R
		{ "t"               , NULL           }, // Q_KEY_CODE_T
		{ "y"               , NULL           }, // Q_KEY_CODE_Y
		{ "u"               , NULL           }, // Q_KEY_CODE_U
		{ "i"               , NULL           }, // Q_KEY_CODE_I
		{ "o"               , NULL           }, // Q_KEY_CODE_O
		{ "p"               , NULL           }, // Q_KEY_CODE_P
		{ "bracket_left"    , "["            }, // Q_KEY_CODE_BRACKET_LEFT
		{ "bracket_right"   , "]"            }, // Q_KEY_CODE_BRACKET_RIGHT
		{ "ret"             , "Return"       }, // Q_KEY_CODE_RET
		{ "a"               , NULL           }, // Q_KEY_CODE_A
		{ "s"               , NULL           }, // Q_KEY_CODE_S
		{ "d"               , NULL           }, // Q_KEY_CODE_D
		{ "f"               , NULL           }, // Q_KEY_CODE_F
		{ "g"               , NULL           }, // Q_KEY_CODE_G
		{ "h"               , NULL           }, // Q_KEY_CODE_H
		{ "j"               , NULL           }, // Q_KEY_CODE_J
		{ "k"               , NULL           }, // Q_KEY_CODE_K
		{ "l"               , NULL           }, // Q_KEY_CODE_L
		{ "semicolon"       , ";"            }, // Q_KEY_CODE_SEMICOLON
		{ "apostrophe"      , "\'"           }, // Q_KEY_CODE_APOSTROPHE
		{ "grave_accent"    , "`"            }, // Q_KEY_CODE_GRAVE_ACCENT
		{ "backslash"       , "\\"           }, // Q_KEY_CODE_BACKSLASH
		{ "z"               , NULL           }, // Q_KEY_CODE_Z
		{ "x"               , NULL           }, // Q_KEY_CODE_X
		{ "c"               , NULL           }, // Q_KEY_CODE_C
		{ "v"               , NULL           }, // Q_KEY_CODE_V
		{ "b"               , NULL           }, // Q_KEY_CODE_B
		{ "n"               , NULL           }, // Q_KEY_CODE_N
		{ "m"               , NULL           }, // Q_KEY_CODE_M
		{ "comma"           , ","            }, // Q_KEY_CODE_COMMA
		{ "dot"             , "."            }, // Q_KEY_CODE_DOT
		{ "slash"           , "/"            }, // Q_KEY_CODE_SLASH
		{ "asterisk"        , NULL           }, // Q_KEY_CODE_ASTERISK
		{ "spc"             , "Space"        }, // Q_KEY_CODE_SPC
		{ "caps_lock"       , "Caps-Lock"    }, // Q_KEY_CODE_CAPS_LOCK
		{ "f1"              , NULL           }, // Q_KEY_CODE_F1
		{ "f2"              , NULL           }, // Q_KEY_CODE_F2
		{ "f3"              , NULL           }, // Q_KEY_CODE_F3
		{ "f4"              , NULL           }, // Q_KEY_CODE_F4
		{ "f5"              , NULL           }, // Q_KEY_CODE_F5
		{ "f6"              , NULL           }, // Q_KEY_CODE_F6
		{ "f7"              , NULL           }, // Q_KEY_CODE_F7
		{ "f8"              , NULL           }, // Q_KEY_CODE_F8
		{ "f9"              , NULL           }, // Q_KEY_CODE_F9
		{ "f10"             , NULL           }, // Q_KEY_CODE_F10
		{ "num_lock"        , "Numlock"      }, // Q_KEY_CODE_NUM_LOCK
		{ "scroll_lock"     , "Scrolllock"   }, // Q_KEY_CODE_SCROLL_LOCK
		{ "kp_divide"       , "Keypad-/"     }, // Q_KEY_CODE_KP_DIVIDE
		{ "kp_multiply"     , "Keypad-*"     }, // Q_KEY_CODE_KP_MULTIPLY
		{ "kp_subtract"     , "Keypad--"     }, // Q_KEY_CODE_KP_SUBTRACT
		{ "kp_add"          , "Keypad-+"     }, // Q_KEY_CODE_KP_ADD
		{ "kp_enter"        , "Keypad-Enter" }, // Q_KEY_CODE_KP_ENTER
		{ "kp_decimal"      , "Keypad-."     }, // Q_KEY_CODE_KP_DECIMAL
		{ "sysrq"           , NULL           }, // Q_KEY_CODE_SYSRQ
		{ "kp_0"            , "Keypad-0"     }, // Q_KEY_CODE_KP_0
		{ "kp_1"            , "Keypad-1"     }, // Q_KEY_CODE_KP_1
		{ "kp_2"            , "Keypad-2"     }, // Q_KEY_CODE_KP_2
		{ "kp_3"            , "Keypad-3"     }, // Q_KEY_CODE_KP_3
		{ "kp_4"            , "Keypad-4"     }, // Q_KEY_CODE_KP_4
		{ "kp_5"            , "Keypad-5"     }, // Q_KEY_CODE_KP_5
		{ "kp_6"            , "Keypad-6"     }, // Q_KEY_CODE_KP_6
		{ "kp_7"            , "Keypad-7"     }, // Q_KEY_CODE_KP_7
		{ "kp_8"            , "Keypad-8"     }, // Q_KEY_CODE_KP_8
		{ "kp_9"            , "Keypad-9"     }, // Q_KEY_CODE_KP_9
		{ "less"            , NULL           }, // Q_KEY_CODE_LESS
		{ "f11"             , NULL           }, // Q_KEY_CODE_F11
		{ "f12"             , NULL           }, // Q_KEY_CODE_F12
		{ "print"           , NULL           }, // Q_KEY_CODE_PRINT
		{ "home"            , NULL           }, // Q_KEY_CODE_HOME
		{ "pgup"            , "Pageup"       }, // Q_KEY_CODE_PGUP
		{ "pgdn"            , "Pagedown"     }, // Q_KEY_CODE_PGDN
		{ "end"             , NULL           }, // Q_KEY_CODE_END
		{ "left"            , NULL           }, // Q_KEY_CODE_LEFT
		{ "up"              , NULL           }, // Q_KEY_CODE_UP
		{ "down"            , NULL           }, // Q_KEY_CODE_DOWN
		{ "right"           , NULL           }, // Q_KEY_CODE_RIGHT
		{ "insert"          , NULL           }, // Q_KEY_CODE_INSERT
		{ "delete"          , NULL           }, // Q_KEY_CODE_DELETE
		{ "stop"            , NULL           }, // Q_KEY_CODE_STOP
		{ "again"           , NULL           }, // Q_KEY_CODE_AGAIN
		{ "props"           , NULL           }, // Q_KEY_CODE_PROPS
		{ "undo"            , NULL           }, // Q_KEY_CODE_UNDO
		{ "front"           , NULL           }, // Q_KEY_CODE_FRONT
		{ "copy"            , NULL           }, // Q_KEY_CODE_COPY
		{ "open"            , NULL           }, // Q_KEY_CODE_OPEN
		{ "paste"           , NULL           }, // Q_KEY_CODE_PASTE
		{ "find"            , NULL           }, // Q_KEY_CODE_FIND
		{ "cut"             , NULL           }, // Q_KEY_CODE_CUT
		{ "lf"              , NULL           }, // Q_KEY_CODE_LF
		{ "help"            , NULL           }, // Q_KEY_CODE_HELP
		{ "meta_l"          , "Right-Alt"    }, // Q_KEY_CODE_META_L
		{ "meta_r"          , NULL           }, // Q_KEY_CODE_META_R
		{ "compose"         , NULL           }, // Q_KEY_CODE_COMPOSE
		{ "pause"           , NULL           }, // Q_KEY_CODE_PAUSE
		{ "ro"              , NULL           }, // Q_KEY_CODE_RO
		{ "hiragana"        , NULL           }, // Q_KEY_CODE_HIRAGANA
		{ "henkan"          , NULL           }, // Q_KEY_CODE_HENKAN
		{ "yen"             , NULL           }, // Q_KEY_CODE_YEN
		{ "muhenkan"        , NULL           }, // Q_KEY_CODE_MUHENKAN
		{ "katakanahiragana", NULL           }, // Q_KEY_CODE_KATAKANAHIRAGANA
		{ "kp_comma"        , NULL           }, // Q_KEY_CODE_KP_COMMA
		{ "kp_equals"       , NULL           }, // Q_KEY_CODE_KP_EQUALS
		{ "power"           , NULL           }, // Q_KEY_CODE_POWER
		{ "sleep"           , NULL           }, // Q_KEY_CODE_SLEEP
		{ "wake"            , NULL           }, // Q_KEY_CODE_WAKE
		{ "audionext"       , NULL           }, // Q_KEY_CODE_AUDIONEXT
		{ "audioprev"       , NULL           }, // Q_KEY_CODE_AUDIOPREV
		{ "audiostop"       , NULL           }, // Q_KEY_CODE_AUDIOSTOP
		{ "audioplay"       , NULL           }, // Q_KEY_CODE_AUDIOPLAY
		{ "audiomute"       , NULL           }, // Q_KEY_CODE_AUDIOMUTE
		{ "volumeup"        , NULL           }, // Q_KEY_CODE_VOLUMEUP
		{ "volumedown"      , NULL           }, // Q_KEY_CODE_VOLUMEDOWN
		{ "mediaselect"     , NULL           }, // Q_KEY_CODE_MEDIASELECT
		{ "mail"            , NULL           }, // Q_KEY_CODE_MAIL
		{ "calculator"      , NULL           }, // Q_KEY_CODE_CALCULATOR
		{ "computer"        , NULL           }, // Q_KEY_CODE_COMPUTER
		{ "ac_home"         , NULL           }, // Q_KEY_CODE_AC_HOME
		{ "ac_back"         , NULL           }, // Q_KEY_CODE_AC_BACK
		{ "ac_forward"      , NULL           }, // Q_KEY_CODE_AC_FORWARD
		{ "ac_refresh"      , NULL           }, // Q_KEY_CODE_AC_REFRESH
		{ "ac_bookmarks"    , NULL           }, // Q_KEY_CODE_AC_BOOKMARKS
	};
	
	const size_t qkeycode_names_len = sizeof(qkeycode_names) / sizeof(qkeycode_names[0]);

	QKeyCode stringToQKeyCode(const char *s, bool pearpcNames) {
		for (size_t i = 0; i < qkeycode_names_len; ++i) {
			if (strcasecmp(s, qkeycode_names[i].qemu) == 0) {
				return static_cast<QKeyCode>(i);
			}
			
			if (pearpcNames) {
				if (qkeycode_names[i].pearpc && strcasecmp(s, qkeycode_names[i].pearpc) == 0) {
					return static_cast<QKeyCode>(i);
				}
			}
		}
		return Q_KEY_CODE_UNMAPPED;
	}
	
	const char *stringFromQKeyCode(QKeyCode k, bool pearpcNames) {
		if (k >= 0 && k < qkeycode_names_len) {
			if (pearpcNames && qkeycode_names[k].pearpc) {
				return qkeycode_names[k].pearpc;
			}
			return qkeycode_names[k].qemu;
		}
		return NULL;
	}
			
}
