/*
 *	PearPC
 *	keyglyphs.h
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

#include "system/qkeycode.h"
#include <Foundation/Foundation.h>

namespace pearpc {

	/// Maps a QKeyCode to a Mac OSX keyboard glyph, or nil
	/// if no specific glyph is defined for the key
	NSString* keyGlyphFromQKeyCode(QKeyCode k);
	
}
