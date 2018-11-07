/*
 *	PearPC
 *	keyboard.h
 *
 *	Copyright (C) 2004 Stefan Weyergraf
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

#ifndef __SYSTEM_KEYBOARD_H__
#define __SYSTEM_KEYBOARD_H__

#include "system/types.h"

#include "system/device.h"
#include "system/event.h"

#include "tools/data.h"
#include "tools/str.h"

#include "system/qkeycode.h"

namespace pearpc {
	
const uint32_t KEYCODE_MASK_CTRL = 0x100;
const uint32_t KEYCODE_MASK_META = 0x200;
const uint32_t KEYCODE_MASK_ALT = 0x400;
const uint32_t KEYCODE_MASK_SHIFT = 0x800;
const uint32_t KEYCODE_MASK_ALL = KEYCODE_MASK_CTRL | KEYCODE_MASK_META | KEYCODE_MASK_ALT | KEYCODE_MASK_SHIFT;

struct KeyBinding {
	KeyBinding(QKeyCode qkey = Q_KEY_CODE_UNMAPPED, uint32_t mod = 0)
	: qkeycode(qkey)
	, modifiers(mod)
	{}
	
	bool operator==(const KeyBinding &rhs) const { return qkeycode == rhs.qkeycode && modifiers == rhs.modifiers; }
	
	QKeyCode qkeycode;
	uint32_t modifiers;
};
	
struct KeyboardCharacteristics
{
	KeyBinding key_compose_dialog;
	KeyBinding key_toggle_mouse_grab;
	KeyBinding key_toggle_full_screen;
};

/* system-dependent (implementation in ui / $MYUI / *.cc) */
class SystemKeyboard: public SystemDevice {
	int mShift;
	int mLAlt;
	int mRAlt;
	int mCtrl;
public:
			SystemKeyboard();
	virtual int	getKeybLEDs() = 0;
	virtual void	setKeybLEDs(int leds) = 0;
	virtual bool	handleEvent(const SystemEvent &ev);
	virtual void	reset();
	virtual	bool	adbKeyToAscii(char &chr, int adbcode);
	virtual QKeyCode convertKeycodeToQKeyCode(int keycode) = 0;
	static	bool	convertKeyBindingToString(String &result, const KeyBinding &keycode);
	static	bool	convertStringToKeyBinding(KeyBinding &keycode, const String &s);
	bool		setKeyConfig(KeyboardCharacteristics keyConfig);
	const KeyboardCharacteristics &getKeyConfig();
private:
	KeyboardCharacteristics keyConfig;
};

/* system-independent (implementation in keyboard.cc) */
extern SystemKeyboard *gKeyboard;

}

#endif /* __SYSTEM_KEYBOARD_H__ */
