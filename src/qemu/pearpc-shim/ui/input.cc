/*
 *	PearPC
 *	ui/input.cc
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
 *
 */

/*
 * QEMU System Emulator
 *
 * Copyright (c) 2003-2008 Fabrice Bellard
 */

#include "ui/console.h"
#include "ui/input.h"
#include "system/mouse.h"
#include "system/keyboard.h"
#include <memory>
#include <list>
#include <string>

using std::shared_ptr;

// ui/input-legacy.c
struct QEMUPutMouseEntry
{
	QEMUPutMouseEntry(const char* name, QEMUPutMouseEvent *func, void *opaque)
	: m_name(name)
	, m_func(func)
	, m_opaque(opaque)
	{}
	
	std::string m_name;
	QEMUPutMouseEvent *m_func;
	void *m_opaque;
};

// ui/input.c
struct QemuInputHandlerState
{
	DeviceState       *dev;
	QemuInputHandler  *handler;
};

namespace {
	
	std::list<shared_ptr<QEMUPutMouseEntry>> s_mouseHandlers;
	std::list<shared_ptr<QemuInputHandlerState>> s_inputHandlers;

	bool mouseEventHandler(const pearpc::SystemEvent &ev) {
		for (const auto& handler : s_mouseHandlers) {
			int buttons = ((ev.mouse.button1 ? MOUSE_EVENT_LBUTTON : 0) |
						   (ev.mouse.button2 ? MOUSE_EVENT_RBUTTON : 0) |
						   (ev.mouse.button3 ? MOUSE_EVENT_MBUTTON : 0));
			
			handler->m_func(handler->m_opaque, ev.mouse.relx, ev.mouse.rely, 0, buttons);
		}
		return true;
	}
	
	bool inputEventHandler(const pearpc::SystemEvent &ev) {
		switch (ev.type) {
			case pearpc::sysevKey:
			{
				KeyValue qemuKey;
				qemuKey.type = KEY_VALUE_KIND_QCODE;
				qemuKey.u.qcode.data = ev.key.qkeycode;
				
				InputKeyEvent qemuKeyEvent;
				qemuKeyEvent.down = ev.key.pressed;
				qemuKeyEvent.key = &qemuKey;
				
				InputEvent qemuEvent;
				qemuEvent.type = INPUT_EVENT_KIND_KEY;
				qemuEvent.u.key.data = &qemuKeyEvent;
				
				for (const auto& handler : s_inputHandlers) {
					handler->handler->event(handler->dev, nullptr, &qemuEvent);
				}
				return true;
			}
				
			case pearpc::sysevMouse:
			case pearpc::sysevNone:
				return false;
		}
	}

	void addMouseHandler(const shared_ptr<QEMUPutMouseEntry> &entry) {
		s_mouseHandlers.push_back(entry);

		// Attach one pearpc event handler
		static bool s_attached = false;
		if (!s_attached) {
			s_attached = true;
			pearpc::gMouse->attachEventHandler(mouseEventHandler);
		}
	}
	
	void addKeyboardHandler(const shared_ptr<QemuInputHandlerState> &entry) {
		s_inputHandlers.push_back(entry);
		
		// Attach one pearpc event handler
		static bool s_attached = false;
		if (!s_attached) {
			s_attached = true;
			pearpc::gKeyboard->attachEventHandler(inputEventHandler);
		}
	}
}

QEMUPutMouseEntry *qemu_add_mouse_event_handler(QEMUPutMouseEvent *func,
												void *opaque, int absolute,
												const char *name) {
	auto s = std::make_shared<QEMUPutMouseEntry>(name, func, opaque);
	addMouseHandler(s);
	return s.get();
}

QemuInputHandlerState *qemu_input_handler_register(DeviceState *dev,
												   QemuInputHandler *handler) {
	auto s = std::make_shared<QemuInputHandlerState>();
	s->dev = dev;
	s->handler = handler;
	addKeyboardHandler(s);
	return s.get();
}

int qemu_input_key_value_to_qcode(const KeyValue *value) {
	return value->u.qcode.data;
}
