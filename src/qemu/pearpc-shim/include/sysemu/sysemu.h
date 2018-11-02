/*
 *	PearPC
 *	include/sysemu/sysemu.h
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

#ifndef SYSEMU_H
#define SYSEMU_H

#include "qemu/timer.h"
#include "qom/object.h"

// PearPC
#include "cpu/cpu.h"

typedef enum ShutdownCause {
	SHUTDOWN_CAUSE_GUEST_SHUTDOWN,
	SHUTDOWN_CAUSE_GUEST_RESET,
} ShutdownCause;

inline void qemu_system_shutdown_request(ShutdownCause reason) {
	ppc_cpu_stop();
}

inline void qemu_system_reset_request(ShutdownCause reason) {
	ppc_cpu_stop();
}

#endif
