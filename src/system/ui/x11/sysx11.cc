/* 
 *	PearPC
 *	sysx11.cc
 *
 *	Copyright (C) 1999-2002 Stefan Weyergraf
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

#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <sys/time.h>

#include <csignal>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <cstring>

#include "system/sysclk.h"
#include "system/display.h"
#include "system/keyboard.h"
#include "system/mouse.h"
#include "clientconfig.h"

#include "tools/snprintf.h"

#include "sysx11.h"

namespace pearpc {

sys_mutex	gX11Mutex;
Display *	gX11Display = NULL;
Window		gX11Window;

static void handleX11Event(const XEvent &event)
{
	static bool visible = true;
	static bool mapped = true;
	static bool mouseButton[3] = {false, false, false};

	switch (event.type) {
	case GraphicsExpose:
	case Expose:
		damageFrameBufferAll();
		gDisplay->displayShow();
		break;
	case KeyRelease: {
		char buffer[4];
		SystemEvent ev;
		XComposeStatus compose;
		KeySym keysym;

		ev.key.qkeycode = gKeyboard->convertKeycodeToQKeyCode(event.xkey.keycode);
		ev.type = sysevKey;
		ev.key.pressed = false;

		sys_lock_mutex(gX11Mutex);
		XLookupString((XKeyEvent*)&event, buffer, sizeof buffer, &keysym, &compose);
		sys_unlock_mutex(gX11Mutex);
		ev.key.chr = buffer[0];

		gKeyboard->handleEvent(ev);
		break;
	}
	case KeyPress: {
		char buffer[4];
		XComposeStatus compose;
		KeySym keysym;

		SystemEvent ev;
		ev.key.qkeycode = gKeyboard->convertKeycodeToQKeyCode(event.xkey.keycode);
		ev.type = sysevKey;
		ev.key.pressed = true;

		sys_lock_mutex(gX11Mutex);
		XLookupString((XKeyEvent*)&event, buffer, sizeof buffer, &keysym, &compose);
		sys_unlock_mutex(gX11Mutex);
		ev.key.chr = buffer[0];

		gKeyboard->handleEvent(ev);
		break;
	}
	case ButtonPress: {
		SystemEvent ev;
		ev.type = sysevMouse;
		switch (((XButtonEvent *)&event)->button) {
		case Button1:
			mouseButton[0] = true;
			break;
		case Button2:
			mouseButton[2] = true;
			break;
		case Button3:
			mouseButton[1] = true;
			break;
		}
		ev.mouse.type = sme_buttonPressed;
		ev.mouse.button1 = mouseButton[0];
		ev.mouse.button2 = mouseButton[1];
		ev.mouse.button3 = mouseButton[2];
		ev.mouse.x = gDisplay->mCurMouseX;
		ev.mouse.y = gDisplay->mCurMouseY;
		ev.mouse.relx = 0;
		ev.mouse.rely = 0;

		gMouse->handleEvent(ev);
		break;
	}
	case ButtonRelease: {
		SystemEvent ev;	
		ev.type = sysevMouse;
		switch (((XButtonEvent *)&event)->button) {
		case Button1:
			mouseButton[0] = false;
			break;
		case Button2:
			mouseButton[2] = false;
			break;
		case Button3:
			mouseButton[1] = false;
			break;
		}
		ev.mouse.type = sme_buttonReleased;
		ev.mouse.button1 = mouseButton[0];
		ev.mouse.button2 = mouseButton[1];
		ev.mouse.button3 = mouseButton[2];
		ev.mouse.x = gDisplay->mCurMouseX;
		ev.mouse.y = gDisplay->mCurMouseY;
		ev.mouse.relx = 0;
		ev.mouse.rely = 0;

		gMouse->handleEvent(ev);
		break;
	}
	case MotionNotify: {
		SystemEvent ev;	
		gDisplay->mCurMouseX = ev.mouse.x = ((XPointerMovedEvent *)&event)->x;
		gDisplay->mCurMouseY = ev.mouse.y = ((XPointerMovedEvent *)&event)->y;
		if (gDisplay->mCurMouseX == gDisplay->mHomeMouseX && gDisplay->mCurMouseY == gDisplay->mHomeMouseY) break;
		if (gDisplay->mCurMouseX == -1) break;
		ev.type = sysevMouse;
		ev.mouse.type = sme_motionNotify;
		ev.mouse.button1 = mouseButton[0];
		ev.mouse.button2 = mouseButton[1];
		ev.mouse.button3 = mouseButton[2];
		ev.mouse.relx = gDisplay->mCurMouseX - gDisplay->mHomeMouseX;
		ev.mouse.rely = gDisplay->mCurMouseY - gDisplay->mHomeMouseY;
		if (gDisplay->isMouseGrabbed()) {
			sys_lock_mutex(gX11Mutex);
			XWarpPointer(gX11Display, gX11Window, gX11Window, 0, 0, 0, 0, gDisplay->mHomeMouseX, gDisplay->mHomeMouseY);
			sys_unlock_mutex(gX11Mutex);
		}

		gMouse->handleEvent(ev);
		break;
	}
	case EnterNotify:
		gDisplay->mCurMouseX = ((XEnterWindowEvent *)&event)->x;
		gDisplay->mCurMouseY = ((XEnterWindowEvent *)&event)->y;
		break;
	case LeaveNotify:
		gDisplay->mCurMouseX = gDisplay->mCurMouseY = -1;
		break;
	case FocusOut:
		if (gDisplay->isMouseGrabbed()) gDisplay->setMouseGrab(false);
		break;
	case MapNotify:
		mapped = true;
		gDisplay->setExposed(visible);
		break;
	case UnmapNotify:
		mapped = false;
		gDisplay->setExposed(false);
		break;
	case VisibilityNotify:
		visible = (event.xvisibility.state != VisibilityFullyObscured);
		gDisplay->setExposed(mapped && visible);
		break;
	}
}

static inline bool checkHandleX11Event()
{
	XEvent event;
	uint xevmask = KeyPressMask | KeyReleaseMask | ExposureMask
		| ButtonPressMask | ButtonReleaseMask | PointerMotionMask
		| EnterWindowMask | LeaveWindowMask | StructureNotifyMask
		| VisibilityChangeMask;

	sys_lock_mutex(gX11Mutex);
	if (!XCheckWindowEvent(gX11Display, gX11Window, xevmask, &event)) {
		sys_unlock_mutex(gX11Mutex);
		return false;
	}
	sys_unlock_mutex(gX11Mutex);

	handleX11Event(event);
	return true;
}

extern SystemDisplay *allocSystemDisplay(const char *title, const DisplayCharacteristics &chr, int redraw_ms, bool fullscreen);
extern SystemMouse *allocSystemMouse();
extern SystemKeyboard *allocSystemKeyboard();

void initUI(const char *title, const ClientConfig& clientConfig)
{
	// connect to X server
	const char *display = getenv("DISPLAY");
	if (display == NULL) {
		display = ":0.0";
	}
	gX11Display = XOpenDisplay(display);
	if (!gX11Display) {
		ht_printf("can't open X11 display (%s)!\n", display);
		exit(1);
	}

	sys_create_mutex(&gX11Mutex);

	gDisplay = allocSystemDisplay(title, clientConfig.getDisplayConfig(), clientConfig.getRedrawInterval(), clientConfig.getFullScreen());
	gMouse = allocSystemMouse();
	gKeyboard = allocSystemKeyboard();
	if(!gKeyboard->setKeyConfig(clientConfig.getKeyConfig())) {
		ht_printf("no keyConfig, or is empty");
		exit(1);
	}
	gDisplay->updateTitle();
}

void mainLoopUI(const std::function<bool ()> &exitLoop)
{
	int fd = ConnectionNumber(gX11Display);
	
	int redraw_interval_msec = gDisplay->mRedraw_ms;
	uint64 redraw_interval_clk = redraw_interval_msec*sys_get_hiresclk_ticks_per_second()/1000;
	uint64 clk_per_sec = sys_get_hiresclk_ticks_per_second();
	uint64 next_redraw_clk = sys_get_hiresclk_ticks();

	while (1) {
		while (1) {
			uint64 clk = sys_get_hiresclk_ticks();

			if (clk >= next_redraw_clk) {
				uint64 d = clk - next_redraw_clk;
				// We may have missed some scheduled display
				// redraws. We'll just ignore this and
				// keep drawing every 'redraw_interval_msec' msecs
				d %= redraw_interval_clk;
				next_redraw_clk = clk + redraw_interval_clk - d;
				gDisplay->displayShow();
			}
			struct timeval tm;
			fd_set zerofds;
			fd_set xfds;

			FD_ZERO(&zerofds);
			FD_ZERO(&xfds);
			FD_SET(fd, &xfds);

			uint64 x = (next_redraw_clk - clk) * 1000000 / clk_per_sec;
			tm.tv_sec = 0;
			tm.tv_usec = x;

			if (checkHandleX11Event()) break;
			if (select(fd+1, &xfds, &zerofds, &zerofds, &tm)) break;
		}

		// kind of limit the number of X events we handle to give the above
		// code a chance
		for (int i=0; i<500; i++) {
			if (!checkHandleX11Event()) break;
		}
		
		if (exitLoop()) {
			break;
		}
	}
}

void doneUI()
{
	XCloseDisplay(gX11Display);
}

}
