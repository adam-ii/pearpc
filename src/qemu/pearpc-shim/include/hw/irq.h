/*
 *	PearPC
 *	include/hw/irq.h
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

#ifndef QEMU_IRQ_H
#define QEMU_IRQ_H

#include "io/pic/pic.h"

///	\class IRQState
///	\brief Interfaces QEMU irq to PearPC pic_* functions taking an integer interrupt number
typedef struct IRQState {
	int n;
} *qemu_irq;

static inline void qemu_irq_raise(qemu_irq irq) {
	pic_raise_interrupt(irq->n);
}

static inline void qemu_irq_lower(qemu_irq irq) {
	pic_cancel_interrupt(irq->n);
}

#endif
