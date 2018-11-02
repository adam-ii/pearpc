/*
 *	PearPC
 *	include/qemu/osdep.h
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

#ifndef QEMU_OSDEP_H
#define QEMU_OSDEP_H

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <sys/time.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

// From include/qemu/typedefs.h
struct Error;

// From include/qemu/compiler.h
#define container_of(ptr, type, member) \
    dynamic_cast<type*>((Object *)((char *)(ptr) - qemu::get_member_offset(&type::member)))

#define container_of_parent_obj(ptr, type) \
    dynamic_cast<type*>(ptr)

namespace qemu {
    // C++11 compliant method of retrieving offsetof a class member
    template <typename T>
    intptr_t get_member_offset(T t) {
        return *(intptr_t *)(&t);
    }
}

#endif
