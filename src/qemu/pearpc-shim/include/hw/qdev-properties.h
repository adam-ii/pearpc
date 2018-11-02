/*
 *	PearPC
 *	include/hw/qdev-properties.h
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

#ifndef QEMU_QDEV_PROPERTIES_H
#define QEMU_QDEV_PROPERTIES_H

// Consume DEFINE_PROP_*

#define DEFINE_PROP_BOOL(_n, _s, _f, _d) Property()

#define DEFINE_PROP_UINT8(_n, _s, _f, _d) Property()
#define DEFINE_PROP_UINT16(_n, _s, _f, _d) Property()
#define DEFINE_PROP_UINT32(_n, _s, _f, _d) Property()
#define DEFINE_PROP_INT32(_n, _s, _f, _d) Property()
#define DEFINE_PROP_UINT64(_n, _s, _f, _d) Property()

#define DEFINE_PROP_END_OF_LIST() Property()

#endif
