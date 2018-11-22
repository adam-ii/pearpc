/*
 *	PearPC
 *	include/qemu/timer.h
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

#ifndef QEMU_TIMER_H
#define QEMU_TIMER_H

#include <sys/time.h>
#include "system/sysclk.h"
#include "system/timer.h"

// From include/qemu/timer.h
inline void qemu_get_timedate(struct tm *tm, int offset)
{
	time_t ti = time(NULL);
	
	ti += offset;
	gmtime_r(&ti, tm);
}

// From include/qemu/host-utils.h
inline uint64_t muldiv64(uint64_t a, uint32_t b, uint32_t c)
{
	#ifdef CONFIG_INT128
	return (__int128_t)a * b / c;
	#else
	union {
		uint64_t ll;
		struct {
			#ifdef HOST_WORDS_BIGENDIAN
			uint32_t high, low;
			#else
			uint32_t low, high;
			#endif
		} l;
	} u, res;
	uint64_t rl, rh;
	
	u.ll = a;
	rl = (uint64_t)u.l.low * (uint64_t)b;
	rh = (uint64_t)u.l.high * (uint64_t)b;
	rh += (rl >> 32);
	res.l.high = rh / c;
	res.l.low = (((rh % c) << 32) + (rl & 0xffffffff)) / c;
	return res.ll;
	#endif
}

#define NANOSECONDS_PER_SECOND 1000000000LL

#define SCALE_MS 1000000
#define SCALE_US 1000
#define SCALE_NS 1

typedef enum {
	QEMU_CLOCK_VIRTUAL,
} QEMUClockType;

typedef void QEMUTimerCB(void *opaque);


///	\class QEMUTimer
///	\brief Interfaces QEMUTimer to pearpc::Timer
struct QEMUTimer {
	QEMUTimer(const pearpc::Timer &timer)
	: m_timer(timer)
	{}
	
	pearpc::Timer m_timer;
};


inline uint64_t qemu_clock_get_ns(QEMUClockType) {
	return sys_get_elapsed_ns();
}

inline QEMUTimer *timer_new_ns(QEMUClockType type, QEMUTimerCB *cb, void *opaque) {
	auto t = pearpc::createTimer([cb, opaque] {
		cb(opaque);
	});
	
	return new QEMUTimer(t);
}

inline void timer_del(QEMUTimer* t) {
	pearpc::cancelTimer(t->m_timer);
}

inline void timer_mod(QEMUTimer *t, uint64_t expires) {
	pearpc::modifyTimer(t->m_timer, expires);
}

#endif
