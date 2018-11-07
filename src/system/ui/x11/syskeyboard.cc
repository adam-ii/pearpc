/* 
 *	PearPC
 *	keyboard.cc - keyboard access functions for X11
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
#include <cassert>

#include "system/systhread.h"
#include "sysx11.h"

#include "system/display.h"
#include "system/keyboard.h"
#include "tools/snprintf.h"

extern "C" {
	#include "x_keymap.h"
}

namespace pearpc {

class X11SystemKeyboard: public SystemKeyboard {
public:
	X11SystemKeyboard()
	{
		assert(gX11Display);
		m_x11_to_qkeycode_map = qemu_xkeymap_mapping_table(gX11Display, &m_x11_to_qkeycode_len);
	}

	virtual int getKeybLEDs()
	{
		return 0;
	}

	virtual void setKeybLEDs(int leds)
	{
	}

	virtual bool handleEvent(const SystemEvent &ev)
	{
		return SystemKeyboard::handleEvent(ev);
	}

	QKeyCode convertKeycodeToQKeyCode(int keycode)
	{
		if (m_x11_to_qkeycode_len <= keycode) {
			ht_printf("<Warning> unknown keycode 0x%x\n", keycode);
			return Q_KEY_CODE_UNMAPPED;
		}
		return static_cast<QKeyCode>(m_x11_to_qkeycode_map[keycode]);
	}

private:
	const uint16_t *m_x11_to_qkeycode_map;
	size_t m_x11_to_qkeycode_len;
};

SystemKeyboard *allocSystemKeyboard()
{
	if (gKeyboard) return NULL;
	return new X11SystemKeyboard();
}

}
