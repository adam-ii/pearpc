/*
 *	PearPC
 *	include/ui/input.h
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

#ifndef INPUT_H
#define INPUT_H

#include "qemu/osdep.h"
#include "hw/qdev.h"
#include "ui/console.h"

// From {build dir}/qapi/qapi-types-ui.h
typedef enum InputEventKind {
	INPUT_EVENT_KIND_KEY = 0,
	INPUT_EVENT_KIND_BTN = 1,
	INPUT_EVENT_KIND_REL = 2,
	INPUT_EVENT_KIND_ABS = 3,
	INPUT_EVENT_KIND__MAX = 4,
} InputEventKind;

typedef enum KeyValueKind {
	KEY_VALUE_KIND_NUMBER = 0,
	KEY_VALUE_KIND_QCODE = 1,
	KEY_VALUE_KIND__MAX = 2,
} KeyValueKind;

struct q_obj_int_wrapper {
	int64_t data;
};
struct KeyValue {
	KeyValueKind type;
	union {
		q_obj_int_wrapper number;
	} u;
};

struct InputKeyEvent {
	KeyValue *key;
	bool down;
};

struct InputEvent {
	InputEventKind type;
	union {
		struct {
			InputKeyEvent *data;
		} key;
	} u;
};

// From include/ui/input.h
#define INPUT_EVENT_MASK_KEY   (1<<INPUT_EVENT_KIND_KEY)
#define INPUT_EVENT_MASK_BTN   (1<<INPUT_EVENT_KIND_BTN)
#define INPUT_EVENT_MASK_REL   (1<<INPUT_EVENT_KIND_REL)
#define INPUT_EVENT_MASK_ABS   (1<<INPUT_EVENT_KIND_ABS)

typedef void (*QemuInputHandlerEvent)(DeviceState *dev, QemuConsole *src,
									  InputEvent *evt);
typedef void (*QemuInputHandlerSync)(DeviceState *dev);

struct QemuInputHandler {
	const char             *name;
	uint32_t               mask;
	QemuInputHandlerEvent  event;
	QemuInputHandlerSync   sync;
};

struct QemuInputHandlerState *qemu_input_handler_register(DeviceState *dev,
												   QemuInputHandler *handler);

#endif
