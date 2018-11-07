/*
 *	PearPC
 *	sysmacos.h
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

#ifndef __SYSMACOS_H__
#define __SYSMACOS_H__

#include "system/display.h"
#include "system/keyboard.h"
#include "system/mouse.h"
#include <memory>
#include <vector>

namespace pearpc {
	
	class RenderMetal;
	
	class MetalSystemDisplay: public SystemDisplay {
	public:
		MetalSystemDisplay(const char *title, const DisplayCharacteristics &chr, int redraw_ms, const std::shared_ptr<RenderMetal>& renderer);
		~MetalSystemDisplay();
		
		virtual void finishMenu() {}
		virtual void updateTitle() {}
		virtual void displayShow() {}
		virtual void convertCharacteristicsToHost(DisplayCharacteristics &aHostChar, const DisplayCharacteristics &aClientChar);
		virtual bool changeResolution(const DisplayCharacteristics &aCharacteristics);
		virtual void getHostCharacteristics(Container &modes) {}
		
	private:
		std::shared_ptr<RenderMetal> m_renderer;
	};
	
	class MacSystemKeyboard: public SystemKeyboard {
	public:
		virtual int getKeybLEDs();
		virtual void setKeybLEDs(int leds);
		
		virtual QKeyCode convertKeycodeToQKeyCode(int keycode);
	};
	
	class MacSystemMouse: public SystemMouse {
	};
	
}

#endif
