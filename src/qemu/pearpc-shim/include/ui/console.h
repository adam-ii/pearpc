/*
 *	PearPC
 *	include/ui/console.h
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

#ifndef CONSOLE_H
#define CONSOLE_H

#define MOUSE_EVENT_LBUTTON 0x01
#define MOUSE_EVENT_RBUTTON 0x02
#define MOUSE_EVENT_MBUTTON 0x04
#define MOUSE_EVENT_WHEELUP 0x08
#define MOUSE_EVENT_WHEELDN 0x10

typedef void QEMUPutKBDEvent(void *opaque, int keycode);
typedef void QEMUPutLEDEvent(void *opaque, int ledstate);
typedef void QEMUPutMouseEvent(void *opaque, int dx, int dy, int dz, int buttons_state);

typedef struct QEMUPutMouseEntry QEMUPutMouseEntry;
typedef struct QEMUPutKbdEntry QEMUPutKbdEntry;
typedef struct QEMUPutLEDEntry QEMUPutLEDEntry;

QEMUPutKbdEntry *qemu_add_kbd_event_handler(QEMUPutKBDEvent *func,
											void *opaque);

QEMUPutMouseEntry *qemu_add_mouse_event_handler(QEMUPutMouseEvent *func,
												void *opaque, int absolute,
												const char *name);

struct QemuConsole;

#endif
