/* 
 *	PearPC
 *	syssdl.cc
 *
 *	Copyright (C)      2004 Jens v.d. Heydt (mailme@vdh-webservice.de)
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

#include <SDL.h>
#include <SDL_thread.h>

#include <csignal>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <cstring>

// for stopping the CPU
#include "cpu/cpu.h"

#include "system/sysclk.h"
#include "system/display.h"
#include "system/keyboard.h"
#include "system/mouse.h"
#include "system/systhread.h"
#include "system/systimer.h"
#include "clientconfig.h"

#include "tools/snprintf.h"

#include "syssdl.h"

namespace pearpc {

SDL_Surface *	gSDLScreen;
static bool	gSDLVideoExposePending = false;
SDL_TimerID SDL_RedrawTimerID;

SDLSystemDisplay *sd;

static bool handleSDLEvent(const SDL_Event &event)
{
	static bool mouseButton[3] = {false, false, false};
	
	SystemEvent ev;
	switch (event.type) {
	case SDL_USEREVENT:
		if (event.user.code == 1) {  // helper for changeResolution
			//ht_printf("got forward event\n");
			sd->mChangeResRet = sd->changeResolutionREAL(sd->mSDLChartemp);
			SDL_CondSignal(sd->mWaitcondition); // Signal, that condition is over.
		}
		return true;
	case SDL_VIDEOEXPOSE:
		gDisplay->displayShow();
		gSDLVideoExposePending = false;
		return true;
	case SDL_KEYUP:
		ev.key.qkeycode = gKeyboard->convertKeycodeToQKeyCode(event.key.keysym.sym);
		ev.type = sysevKey;
		ev.key.pressed = false;
		gKeyboard->handleEvent(ev);
		return true;
	case SDL_KEYDOWN:
		ev.key.qkeycode = gKeyboard->convertKeycodeToQKeyCode(event.key.keysym.sym);
		ev.type = sysevKey;
		ev.key.pressed = true;
		gKeyboard->handleEvent(ev);
		return true;
	case SDL_MOUSEBUTTONDOWN:
		ev.type = sysevMouse;
		ev.mouse.type = sme_buttonPressed;
		mouseButton[0] = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1);
		mouseButton[1] = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(2);
		mouseButton[2] = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(3);
		ev.mouse.button1 = mouseButton[0];
		ev.mouse.button2 = mouseButton[1];
		ev.mouse.button3 = mouseButton[2];
		ev.mouse.x = gDisplay->mCurMouseX;
		ev.mouse.y = gDisplay->mCurMouseY;
		ev.mouse.relx = 0;
		ev.mouse.rely = 0;
		gMouse->handleEvent(ev);
		return true;
	case SDL_MOUSEBUTTONUP:
		ev.type = sysevMouse;
		ev.mouse.type = sme_buttonReleased;
		mouseButton[0] = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1);
		mouseButton[1] = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(2);
		mouseButton[2] = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(3);
		ev.mouse.button1 = mouseButton[0];
		ev.mouse.button2 = mouseButton[1];
		ev.mouse.button3 = mouseButton[2];
		ev.mouse.x = gDisplay->mCurMouseX;
		ev.mouse.y = gDisplay->mCurMouseY;
		ev.mouse.relx = 0;
		ev.mouse.rely = 0;
		gMouse->handleEvent(ev);
		return true;
	case SDL_MOUSEMOTION:
		ev.type = sysevMouse;
		ev.mouse.type = sme_motionNotify;
		ev.mouse.button1 = mouseButton[0];
		ev.mouse.button2 = mouseButton[1];
		ev.mouse.button3 = mouseButton[2];
		ev.mouse.x = event.motion.y;
		ev.mouse.y = event.motion.x;
		ev.mouse.relx = event.motion.xrel;
		ev.mouse.rely = event.motion.yrel;
		gMouse->handleEvent(ev);
		return true;
	case SDL_ACTIVEEVENT:
		if (event.active.state & SDL_APPACTIVE) {
			gDisplay->setExposed(event.active.gain);
		}
		if (event.active.state & SDL_APPINPUTFOCUS) {
			if (!event.active.gain) {
				gDisplay->setMouseGrab(false);			
			}
		}
		return true;
	case SDL_QUIT:		
		gDisplay->setFullscreenMode(false);
		return false;
	}
	return true;
}

static Uint32 SDL_redrawCallback(Uint32 interval, void *param)
{
	SDL_Event event;

	if (!gSDLVideoExposePending) {
		event.type = SDL_VIDEOEXPOSE;
		// according to the docs, "You may always call SDL_PushEvent" in an SDL
		// timer callback function
		SDL_PushEvent(&event);
		gSDLVideoExposePending = true;
	}
	return interval;
}

sys_timer gSDLRedrawTimer;

SystemDisplay *allocSystemDisplay(const char *title, const DisplayCharacteristics &chr, int redraw_ms);
SystemKeyboard *allocSystemKeyboard();
SystemMouse *allocSystemMouse();

void initUI(const char *title, const ClientConfig& clientConfig)
{
#if 0
	createSDLToADBKeytable();
#endif

	gDisplay = allocSystemDisplay(title, clientConfig.getDisplayConfig(), clientConfig.getRedrawInterval());
	gMouse = allocSystemMouse();
	gKeyboard = allocSystemKeyboard();
	if (!gKeyboard->setKeyConfig(clientConfig.getKeyConfig())) {
		ht_printf("no keyConfig, or is empty");
		exit(1);
	}

	gDisplay->mFullscreen = clientConfig.getFullScreen();

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE) < 0) {
		ht_printf("SDL: Unable to init: %s\n", SDL_GetError());
		exit(1);
	}
	
	atexit(SDL_Quit); // give SDl a chance to clean up before exit!
	sd = (SDLSystemDisplay*)gDisplay;
	
	sd->initCursor();
	
	sd->updateTitle();
	sd->mEventThreadID = SDL_ThreadID();

	SDL_WM_GrabInput(SDL_GRAB_OFF);
	
	sd->changeResolution(sd->mClientChar);
	sd->setExposed(true);
	
	gSDLVideoExposePending = false;
	SDL_RedrawTimerID = SDL_AddTimer(gDisplay->mRedraw_ms, SDL_redrawCallback, NULL);
	
	sd->setFullscreenMode(clientConfig.getFullScreen());
}

void mainLoopUI(const std::function<bool ()> &exitLoop)
{
	SDL_Event event;
	do {
		if (exitLoop()) {
			break;
		}
		
		SDL_WaitEvent(&event);
	} while (handleSDLEvent(event));
	
	ppc_cpu_stop();
}

void doneUI()
{
	gDisplay->setMouseGrab(false);
	
	if (SDL_RedrawTimerID)
		SDL_RemoveTimer(SDL_RedrawTimerID);
	
	SDL_Quit();
}

}
