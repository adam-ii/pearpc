/* 
 *	PearPC
 *	keyboard.cc
 *
 *	Copyright (C) 2004 Stefan Weyergraf
 *	Copyright (C) 2003,2004 Sebastian Biallas (sb@biallas.net)
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

#include "tools/snprintf.h"
#include "keyboard.h"
#include "display.h"

namespace pearpc {

SystemKeyboard *gKeyboard = NULL;


SystemKeyboard::SystemKeyboard()
{
	reset();
}

bool SystemKeyboard::handleEvent(const SystemEvent &ev)
{
	if (ev.type != sysevKey) return false;
	
	if (ev.key.qkeycode == Q_KEY_CODE_CTRL || ev.key.qkeycode == Q_KEY_CODE_CTRL_R) {
		mCtrl = ev.key.pressed ? KEYCODE_MASK_CTRL : 0;
	}
	if (ev.key.qkeycode == Q_KEY_CODE_META_L || ev.key.qkeycode == Q_KEY_CODE_META_R) {
		mLAlt = ev.key.pressed ? KEYCODE_MASK_META : 0;
	}
	if (ev.key.qkeycode == Q_KEY_CODE_ALT || ev.key.qkeycode == Q_KEY_CODE_ALT_R) {
		mRAlt = ev.key.pressed ? KEYCODE_MASK_ALT : 0;
	}
	if (ev.key.qkeycode == Q_KEY_CODE_SHIFT || ev.key.qkeycode == Q_KEY_CODE_SHIFT_R) {
		mShift = ev.key.pressed ? KEYCODE_MASK_SHIFT : 0;
	}
	
	const KeyBinding currentState(ev.key.qkeycode, mCtrl | mRAlt | mLAlt | mShift);
	
	if (currentState == keyConfig.key_toggle_mouse_grab) {
		if (ev.key.pressed) gDisplay->setMouseGrab(!gDisplay->isMouseGrabbed());
		return true;
	} else if (currentState == keyConfig.key_toggle_full_screen) {
		if (ev.key.pressed) gDisplay->setFullscreenMode(!gDisplay->mFullscreen);
		/*
		 *	Changing fullscreen/windowed mode confuses
		 *	some well-known OS (key release events are dropped).
		 */
		reset();
		return true;
	} else if (currentState == keyConfig.key_compose_dialog) {
		if (ev.key.pressed) gDisplay->composeKeyDialog();
		return true;
	} else {
		return SystemDevice::handleEvent(ev);
	}
}

void SystemKeyboard::reset()
{
	mCtrl = false;
	mLAlt = false;
	mRAlt = false;
	mShift = false;
}

bool SystemKeyboard::convertKeyBindingToString(String &result, const KeyBinding &bind)
{
	if (bind.qkeycode == Q_KEY_CODE_UNMAPPED || bind.qkeycode >= Q_KEY_CODE__MAX)  {
		return false;
	}
	
	result = "";
	if (bind.modifiers & KEYCODE_MASK_CTRL) {
		result += qemu::stringFromQKeyCode(Q_KEY_CODE_CTRL, true);
		result += "+";
	}
	if (bind.modifiers & KEYCODE_MASK_META) {
		result += qemu::stringFromQKeyCode(Q_KEY_CODE_META_L, true);
		result += "+";
	}
	if (bind.modifiers & KEYCODE_MASK_ALT) {
		result += qemu::stringFromQKeyCode(Q_KEY_CODE_ALT, true);
		result += "+";
	}
	if (bind.modifiers & KEYCODE_MASK_SHIFT) {
		result += qemu::stringFromQKeyCode(Q_KEY_CODE_SHIFT, true);
		result += "+";
	}
	result += qemu::stringFromQKeyCode(bind.qkeycode, true);
	return true;
}

bool SystemKeyboard::convertStringToKeyBinding(KeyBinding &bind, const String &s)
{
	bind = KeyBinding();
	
	if (s == "none") {
		return true;
	}
	
	String k = s;
	bool cont = true;
	
	while (cont) {
		String key, rem;
		cont = k.leftSplit('+', key, rem);
		k = rem;
		
		const QKeyCode qkey = qemu::stringToQKeyCode(key.contentChar(), true);

		switch (qkey) {
		case Q_KEY_CODE_UNMAPPED:
			ht_printf("%y not found\n", &key);
			return false;
		case Q_KEY_CODE_CTRL:
		case Q_KEY_CODE_CTRL_R:
			if (bind.modifiers & KEYCODE_MASK_CTRL) return false;
			bind.modifiers |= KEYCODE_MASK_CTRL;
			break;
		case Q_KEY_CODE_META_L:
		case Q_KEY_CODE_META_R:
			if (bind.modifiers & KEYCODE_MASK_META) return false;
			bind.modifiers |= KEYCODE_MASK_META;
			break;
		case Q_KEY_CODE_ALT:
		case Q_KEY_CODE_ALT_R:
			if (bind.modifiers & KEYCODE_MASK_ALT) return false;
			bind.modifiers |= KEYCODE_MASK_ALT;
			break;
		case Q_KEY_CODE_SHIFT:
		case Q_KEY_CODE_SHIFT_R:
			if (bind.modifiers & KEYCODE_MASK_SHIFT) return false;
			bind.modifiers |= KEYCODE_MASK_SHIFT;
			break;
		default:
			if (bind.qkeycode != Q_KEY_CODE_UNMAPPED) return false;
			bind.qkeycode = qkey;
		}
	}
	return true;
}

bool SystemKeyboard::adbKeyToAscii(char &chr, int adbcode)
{
	// DEPRECATED: only used by prom
	return false;
}

bool SystemKeyboard::setKeyConfig(KeyboardCharacteristics keycon)
{
	keyConfig = keycon;
	return true;
}

const KeyboardCharacteristics &SystemKeyboard::getKeyConfig()
{
	return keyConfig;
}

}
