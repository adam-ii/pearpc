/* 
 *	PearPC
 *	keyboard.cc - keyboardaccess functions for POSIX
 *
 *	Copyright (C) 1999-2004 Stefan Weyergraf
 *	Copyright (C) 1999-2004 Sebastian Biallas (sb@biallas.net)
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

#include <cstdlib>

#include <SDL.h>
#include <map>

#include "system/display.h"
#include "system/keyboard.h"
#include "tools/snprintf.h"

namespace {
	
	const std::map<SDLKey, QKeyCode> kSDLToQKeyCode = {
		{ SDLK_UNKNOWN, Q_KEY_CODE_UNMAPPED },
		{ SDLK_LSHIFT, Q_KEY_CODE_SHIFT },
		{ SDLK_RSHIFT, Q_KEY_CODE_SHIFT_R },
		{ SDLK_LALT, Q_KEY_CODE_ALT },
		{ SDLK_RALT, Q_KEY_CODE_ALT_R },
		{ SDLK_LCTRL, Q_KEY_CODE_CTRL },
		{ SDLK_RCTRL, Q_KEY_CODE_CTRL_R },
		{ SDLK_MENU, Q_KEY_CODE_MENU },
		{ SDLK_ESCAPE, Q_KEY_CODE_ESC },
		{ SDLK_1, Q_KEY_CODE_1 },
		{ SDLK_2, Q_KEY_CODE_2 },
		{ SDLK_3, Q_KEY_CODE_3 },
		{ SDLK_4, Q_KEY_CODE_4 },
		{ SDLK_5, Q_KEY_CODE_5 },
		{ SDLK_6, Q_KEY_CODE_6 },
		{ SDLK_7, Q_KEY_CODE_7 },
		{ SDLK_8, Q_KEY_CODE_8 },
		{ SDLK_9, Q_KEY_CODE_9 },
		{ SDLK_0, Q_KEY_CODE_0 },
		{ SDLK_MINUS, Q_KEY_CODE_MINUS },
		{ SDLK_EQUALS, Q_KEY_CODE_EQUAL },
		{ SDLK_BACKSPACE, Q_KEY_CODE_BACKSPACE },
		{ SDLK_TAB, Q_KEY_CODE_TAB },
		{ SDLK_q, Q_KEY_CODE_Q },
		{ SDLK_w, Q_KEY_CODE_W },
		{ SDLK_e, Q_KEY_CODE_E },
		{ SDLK_r, Q_KEY_CODE_R },
		{ SDLK_t, Q_KEY_CODE_T },
		{ SDLK_y, Q_KEY_CODE_Y },
		{ SDLK_u, Q_KEY_CODE_U },
		{ SDLK_i, Q_KEY_CODE_I },
		{ SDLK_o, Q_KEY_CODE_O },
		{ SDLK_p, Q_KEY_CODE_P },
		{ SDLK_LEFTBRACKET, Q_KEY_CODE_BRACKET_LEFT },
		{ SDLK_RIGHTBRACKET, Q_KEY_CODE_BRACKET_RIGHT },
		{ SDLK_RETURN, Q_KEY_CODE_RET },
		{ SDLK_a, Q_KEY_CODE_A },
		{ SDLK_s, Q_KEY_CODE_S },
		{ SDLK_d, Q_KEY_CODE_D },
		{ SDLK_f, Q_KEY_CODE_F },
		{ SDLK_g, Q_KEY_CODE_G },
		{ SDLK_h, Q_KEY_CODE_H },
		{ SDLK_j, Q_KEY_CODE_J },
		{ SDLK_k, Q_KEY_CODE_K },
		{ SDLK_l, Q_KEY_CODE_L },
		{ SDLK_SEMICOLON, Q_KEY_CODE_SEMICOLON },
		{ SDLK_QUOTE, Q_KEY_CODE_APOSTROPHE },
		{ SDLK_BACKQUOTE, Q_KEY_CODE_GRAVE_ACCENT },
		{ SDLK_BACKSLASH, Q_KEY_CODE_BACKSLASH },
		{ SDLK_z, Q_KEY_CODE_Z },
		{ SDLK_x, Q_KEY_CODE_X },
		{ SDLK_c, Q_KEY_CODE_C },
		{ SDLK_v, Q_KEY_CODE_V },
		{ SDLK_b, Q_KEY_CODE_B },
		{ SDLK_n, Q_KEY_CODE_N },
		{ SDLK_m, Q_KEY_CODE_M },
		{ SDLK_COMMA, Q_KEY_CODE_COMMA },
		{ SDLK_PERIOD, Q_KEY_CODE_DOT },
		{ SDLK_SLASH, Q_KEY_CODE_SLASH },
		{ SDLK_ASTERISK, Q_KEY_CODE_ASTERISK },
		{ SDLK_SPACE, Q_KEY_CODE_SPC },
		{ SDLK_CAPSLOCK, Q_KEY_CODE_CAPS_LOCK },
		{ SDLK_F1, Q_KEY_CODE_F1 },
		{ SDLK_F2, Q_KEY_CODE_F2 },
		{ SDLK_F3, Q_KEY_CODE_F3 },
		{ SDLK_F4, Q_KEY_CODE_F4 },
		{ SDLK_F5, Q_KEY_CODE_F5 },
		{ SDLK_F6, Q_KEY_CODE_F6 },
		{ SDLK_F7, Q_KEY_CODE_F7 },
		{ SDLK_F8, Q_KEY_CODE_F8 },
		{ SDLK_F9, Q_KEY_CODE_F9 },
		{ SDLK_F10, Q_KEY_CODE_F10 },
		{ SDLK_NUMLOCK, Q_KEY_CODE_NUM_LOCK },
		{ SDLK_SCROLLOCK, Q_KEY_CODE_SCROLL_LOCK },
		{ SDLK_KP_DIVIDE, Q_KEY_CODE_KP_DIVIDE },
		{ SDLK_KP_MULTIPLY, Q_KEY_CODE_KP_MULTIPLY },
		{ SDLK_KP_MINUS, Q_KEY_CODE_KP_SUBTRACT },
		{ SDLK_KP_PLUS, Q_KEY_CODE_KP_ADD },
		{ SDLK_KP_ENTER, Q_KEY_CODE_KP_ENTER },
		{ SDLK_KP_PERIOD, Q_KEY_CODE_KP_DECIMAL },
		{ SDLK_SYSREQ, Q_KEY_CODE_SYSRQ },
		{ SDLK_KP0, Q_KEY_CODE_KP_0 },
		{ SDLK_KP1, Q_KEY_CODE_KP_1 },
		{ SDLK_KP2, Q_KEY_CODE_KP_2 },
		{ SDLK_KP3, Q_KEY_CODE_KP_3 },
		{ SDLK_KP4, Q_KEY_CODE_KP_4 },
		{ SDLK_KP5, Q_KEY_CODE_KP_5 },
		{ SDLK_KP6, Q_KEY_CODE_KP_6 },
		{ SDLK_KP7, Q_KEY_CODE_KP_7 },
		{ SDLK_KP8, Q_KEY_CODE_KP_8 },
		{ SDLK_KP9, Q_KEY_CODE_KP_9 },
		{ SDLK_LESS, Q_KEY_CODE_LESS },
		{ SDLK_F11, Q_KEY_CODE_F11 },
		{ SDLK_F12, Q_KEY_CODE_F12 },
		{ SDLK_PRINT, Q_KEY_CODE_PRINT },
		{ SDLK_HOME, Q_KEY_CODE_HOME },
		{ SDLK_PAGEUP, Q_KEY_CODE_PGUP },
		{ SDLK_PAGEDOWN, Q_KEY_CODE_PGDN },
		{ SDLK_END, Q_KEY_CODE_END },
		{ SDLK_LEFT, Q_KEY_CODE_LEFT },
		{ SDLK_UP, Q_KEY_CODE_UP },
		{ SDLK_DOWN, Q_KEY_CODE_DOWN },
		{ SDLK_RIGHT, Q_KEY_CODE_RIGHT },
		{ SDLK_INSERT, Q_KEY_CODE_INSERT },
		{ SDLK_DELETE, Q_KEY_CODE_DELETE },
		{ SDLK_UNDO, Q_KEY_CODE_UNDO },
		{ SDLK_HELP, Q_KEY_CODE_HELP },
		{ SDLK_LMETA, Q_KEY_CODE_META_L },
		{ SDLK_RMETA, Q_KEY_CODE_META_R },
		{ SDLK_COMPOSE, Q_KEY_CODE_COMPOSE },
		{ SDLK_PAUSE, Q_KEY_CODE_PAUSE },
		{ SDLK_KP_EQUALS, Q_KEY_CODE_KP_EQUALS },
		{ SDLK_POWER, Q_KEY_CODE_POWER },
	};

}

namespace pearpc {

class SDLSystemKeyboard: public SystemKeyboard {
public:
	virtual int getKeybLEDs()
	{
		int r = 0;
		SDLMod keyMods = SDL_GetModState();
		if (keyMods & KMOD_NUM)
			r |= KEYB_LED_NUM;
		if (keyMods & KMOD_CAPS)
			r |= KEYB_LED_CAPS;
		/*
		if (keyMods & SDLK_SCROLLOCK)
			r |= KEYB_LED_SCROLL;*/
		return r;
	}

	void setKeybLEDs(int leds)
	{
#if 0
		int r = getKeybLEDs() ^ leds;
		SDLMod keyMods = SDL_GetModState();

		if (r & KEYB_LED_NUM && leds & KEYB_LED_NUM)
			(int)keyMods |= KMOD_NUM;
		else
			(int)keyMods &= KMOD_NUM;

		if (r & KEYB_LED_CAPS && leds & KEYB_LED_CAPS)
			(int)keyMods |= KMOD_CAPS;
		else
			(int)keyMods &= KMOD_CAPS;
		/*
		if (r & KEYB_LED_SCROLL && leds & KEYB_LED_SCROLL)
			keyMods |= SDLK_SCROLLOCK;
		else
			keyMods &= SDLK_SCROLLOCK;
		*/
		SDL_SetModState(keyMods);
#endif
	}

	virtual bool handleEvent(const SystemEvent &ev)
	{
		return SystemKeyboard::handleEvent(ev);
	}

	QKeyCode convertKeycodeToQKeyCode(int keycode)
	{
		auto itr = kSDLToQKeyCode.find(static_cast<SDLKey>(keycode));
		if (itr == kSDLToQKeyCode.end()) {
			ht_printf("<Warning> unknown keycode 0x%x\n", keycode);
			return Q_KEY_CODE_UNMAPPED;
		}
		return itr->second;
	}

};

SystemKeyboard *allocSystemKeyboard()
{
	if (gKeyboard) return NULL;
	return new SDLSystemKeyboard();
}

}
