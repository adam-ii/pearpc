/*
 *	PearPC
 *	sysmacos.mm
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

#import "sysmacos.h"
#import "rendermetal.h"
#import "clientconfig.h"
#import "PearPCViewController.h"
#import <Cocoa/Cocoa.h>
#import <cstring>

namespace pearpc {
	
	MetalSystemDisplay::MetalSystemDisplay(const char *title, const DisplayCharacteristics &chr, int redraw_ms, const std::shared_ptr<RenderMetal>& renderer)
	: SystemDisplay(chr, redraw_ms)
	, m_renderer(renderer)
	{
		gFrameBuffer = m_renderer->createFrameBuffer(mClientChar);
		damageFrameBufferAll();
	}
	
	MetalSystemDisplay::~MetalSystemDisplay()
	{
		m_renderer->destroyFrameBuffer(gFrameBuffer);
		gFrameBuffer = NULL;
	}
	
	void MetalSystemDisplay::convertCharacteristicsToHost(DisplayCharacteristics &aHostChar, const DisplayCharacteristics &aClientChar)
	{
		aHostChar = aClientChar;
	}
	
	bool MetalSystemDisplay::changeResolution(const DisplayCharacteristics &aCharacteristics)
	{
		// Release our reference to frame buffer
		m_renderer->destroyFrameBuffer(gFrameBuffer);

		convertCharacteristicsToHost(mClientChar, aCharacteristics);
		gFrameBuffer = m_renderer->createFrameBuffer(mClientChar);
		
		m_renderer->clientResolutionChanged(mClientChar);
		return true;
	}
	
	void MetalSystemDisplay::setMouseGrab(bool mouseGrab)
	{
		if (m_mouseGrabWillChange) {
			if (!m_mouseGrabWillChange(mouseGrab)) {
				return;
			}
		}
		
		SystemDisplay::setMouseGrab(mouseGrab);
		
		if (m_mouseGrabDidChange) {
			m_mouseGrabDidChange(mouseGrab);
		}
	}
	
	int MacSystemKeyboard::getKeybLEDs()
	{
		return 0;
	}
	
	void MacSystemKeyboard::setKeybLEDs(int leds)
	{
	}
	
	
	void initUI(const char *title, const ClientConfig& clientConfig)
	{
	}
	
	void mainLoopUI(const std::function<bool ()> &exitLoop)
	{
	}
	
	void doneUI()
	{
		delete gDisplay;
		gDisplay = NULL;
		delete gMouse;
		gMouse = NULL;
		delete gKeyboard;
		gKeyboard = NULL;
	}
	
}
