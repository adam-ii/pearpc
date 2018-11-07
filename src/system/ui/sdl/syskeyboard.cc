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

#include "system/display.h"
#include "system/keyboard.h"
#include "tools/snprintf.h"

extern "C" const uint16_t sdl_to_qcode_map[];
extern "C" const size_t sdl_to_qcode_map_len;

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
		if (sdl_to_qcode_map_len <= keycode) {
			ht_printf("<Warning> unknown keycode 0x%x\n", keycode);
			return Q_KEY_CODE_UNMAPPED;
		}
		return static_cast<QKeyCode>(sdl_to_qcode_map[keycode]);
	}

};

SystemKeyboard *allocSystemKeyboard()
{
	if (gKeyboard) return NULL;
	return new SDLSystemKeyboard();
}

}
