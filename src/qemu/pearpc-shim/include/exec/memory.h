/*
 *	PearPC
 *	include/exec/memory.h
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
 * Physical memory management API
 *
 * Copyright 2011 Red Hat, Inc. and/or its affiliates
 *
 * Authors:
 *  Avi Kivity <avi@redhat.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 *
 */

#ifndef MEMORY_H
#define MEMORY_H

#include "exec/hwaddr.h"
#include "qom/object.h"
#include "qemu/timer.h"
#include "hw/qdev-core.h"

// From include/exec/cpu-common.h
enum device_endian {
	DEVICE_NATIVE_ENDIAN,
	DEVICE_BIG_ENDIAN,
	DEVICE_LITTLE_ENDIAN,
};


///	\class MemoryRegion
///	\brief Placeholder for QEMU MemoryRegion
struct MemoryRegion {};


///	\class MemoryRegion
///	\brief Simplified MemoryRegionOps from QEMU
struct MemoryRegionOps {
	/* Read from the memory region. @addr is relative to @mr; @size is
	 * in bytes. */
	uint64_t (*read)(void *opaque,
					 hwaddr addr,
					 unsigned size);
	/* Write to the memory region. @addr is relative to @mr; @size is
	 * in bytes. */
	void (*write)(void *opaque,
				  hwaddr addr,
				  uint64_t data,
				  unsigned size);
	
	enum device_endian endianness;
	/* Guest-visible constraints: */
	struct {
		/* If nonzero, specify bounds on access sizes beyond which a machine
		 * check is thrown.
		 */
		unsigned min_access_size;
		unsigned max_access_size;
	} valid;
};

inline void memory_region_init_io(MemoryRegion *mr,
								  Object *owner,
								  const MemoryRegionOps *ops,
								  void *opaque,
								  const char *name,
								  uint64_t size) {
	if (auto dev = dynamic_cast<DeviceState *>(owner)) {
		dev->m_memRead = ops->read;
		dev->m_memWrite = ops->write;
	}
}

#endif
