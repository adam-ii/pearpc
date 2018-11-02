/*
 *	PearPC
 *	include/qemu/cutils.h
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

#ifndef QEMU_CUTILS_H
#define QEMU_CUTILS_H

// From util/cutils.c
inline time_t mktimegm(struct tm *tm)
{
	time_t t;
	int y = tm->tm_year + 1900, m = tm->tm_mon + 1, d = tm->tm_mday;
	if (m < 3) {
		m += 12;
		y--;
	}
	t = 86400ULL * (d + (153 * m - 457) / 5 + 365 * y + y / 4 - y / 100 +
					y / 400 - 719469);
	t += 3600 * tm->tm_hour + 60 * tm->tm_min + tm->tm_sec;
	return t;
}

#endif
