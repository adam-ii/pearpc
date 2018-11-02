/*
 *	PearPC
 *	hw/core/sysbus.cc
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
 *	System (CPU) Bus device support code
 *
 *	Copyright (c) 2009 CodeSourcery
 *
 */

#include "qemu/osdep.h"
#include "qom/object.h"
#include "hw/sysbus.h"

void sysbus_init_child_obj(Object *parent, const char *childname, void *child,
						   size_t childsize, const char *childtype) {
	Object *object = reinterpret_cast<Object *>(child);
	qemu::initObject(object, qemu::findTypeInfo(childtype));
	object->setContainer(parent);
}

void sysbus_init_mmio(SysBusDevice *dev, MemoryRegion *memory) {}

void sysbus_init_irq(SysBusDevice *dev, qemu_irq *p) {}

void sysbus_pass_irq(SysBusDevice *dev, SysBusDevice *target) {}
