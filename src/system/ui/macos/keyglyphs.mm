/*
 *	PearPC
 *	keyglyphs.mm
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
 */

#import "keyglyphs.h"

namespace pearpc {

	// With thanks to https://gist.github.com/Zenexer/c5243c4216f1f8cd2251
	NSDictionary* kMacOSKeyGlpyhs = @{
		@(Q_KEY_CODE_SHIFT) : @"⇧",
		@(Q_KEY_CODE_SHIFT_R) : @"⇧",
		@(Q_KEY_CODE_ALT) : @"⌥",
		@(Q_KEY_CODE_ALT_R) : @"⌥",
		@(Q_KEY_CODE_CTRL) : @"⌃",
		@(Q_KEY_CODE_CTRL_R) : @"⌃",
		@(Q_KEY_CODE_ESC) : @"⎋",
		@(Q_KEY_CODE_BACKSPACE) : @"⌫",
		@(Q_KEY_CODE_TAB) : @"⇥",
		@(Q_KEY_CODE_RET) : @"↩",
		@(Q_KEY_CODE_CAPS_LOCK) : @"⇪",
		@(Q_KEY_CODE_F1) : @"F1",
		@(Q_KEY_CODE_F2) : @"F2",
		@(Q_KEY_CODE_F3) : @"F3",
		@(Q_KEY_CODE_F4) : @"F4",
		@(Q_KEY_CODE_F5) : @"F5",
		@(Q_KEY_CODE_F6) : @"F6",
		@(Q_KEY_CODE_F7) : @"F7",
		@(Q_KEY_CODE_F8) : @"F8",
		@(Q_KEY_CODE_F9) : @"F9",
		@(Q_KEY_CODE_F10) : @"F10",
		@(Q_KEY_CODE_SCROLL_LOCK) : @"F14",
		@(Q_KEY_CODE_F11) : @"F11",
		@(Q_KEY_CODE_F12) : @"F12",
		@(Q_KEY_CODE_PRINT) : @"F13",
		@(Q_KEY_CODE_HOME) : @"↖",
		@(Q_KEY_CODE_PGUP) : @"⇞",
		@(Q_KEY_CODE_PGDN) : @"⇟",
		@(Q_KEY_CODE_END) : @"↘",
		@(Q_KEY_CODE_LEFT) : @"←",
		@(Q_KEY_CODE_UP) : @"↑",
		@(Q_KEY_CODE_DOWN) : @"↓",
		@(Q_KEY_CODE_RIGHT) : @"→",
		@(Q_KEY_CODE_DELETE) : @"⌦",
		@(Q_KEY_CODE_PAUSE) : @"F15",
	};


	NSString* keyGlyphFromQKeyCode(QKeyCode k) {
		return [kMacOSKeyGlpyhs objectForKey:@(k)];
	}
	
}
